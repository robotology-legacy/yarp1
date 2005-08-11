
#include "cv.h"
#include "highgui.h"
#include <time.h>
#include <math.h>
#include <ctype.h>

#define HAVE_IPL
#include <yarp/YARPImage.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPImageFile.h>
#include <fstream>

using namespace std;

#include "MotionGroup.h"

// various tracking parameters (in seconds)
//const double MHI_DURATION = 1;
//const double MAX_TIME_DELTA = 0.5;
//const double MIN_TIME_DELTA = 0.05;
static const double MHI_DURATION = 1000;
static const double MAX_TIME_DELTA = 500;
static const double MIN_TIME_DELTA = 0.00001;
// number of cyclic frame buffer used for motion detection
// (should, probably, depend on FPS)
static const int N = 4;

// ring image buffer
static IplImage **buf = 0;
static int last = 0;

// temporary images
static IplImage *mhi = 0; // MHI
static IplImage *orient = 0; // orientation
static IplImage *mask = 0; // valid orientation mask
static IplImage *segmask = 0; // motion segmentation map
static CvMemStorage* storage = 0; // temporary storage

static int gx = 0, gy = 0;
static double gv = -1;

// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
void  update_mhi(IplImage *img, YARPImageOf<YarpPixelRGB>& dest, 
		 int diff_threshold )
{
  IplImage *dst = dest.GetIplPointer();
    double timestamp2 = YARPTime::GetTimeAsSeconds();
    static double start = timestamp2;
    timestamp2 = (timestamp2-start)*1000;
    double timestamp = clock()/1000.; // get current time in seconds
    //printf("%g %g\n", timestamp, timestamp2);
    timestamp = timestamp2;


    CvSize size = cvSize(img->width,img->height); // get current frame size
    int i, idx1 = last, idx2;
    IplImage* silh;
    CvSeq* seq;
    CvRect comp_rect;
    double count;
    double angle;
    CvPoint center;
    double magnitude;          
    int color;

    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if( !mhi || mhi->width != size.width || mhi->height != size.height ) {
        if( buf == 0 ) {
            buf = (IplImage**)malloc(N*sizeof(buf[0]));
            memset( buf, 0, N*sizeof(buf[0]));
        }
        
        for( i = 0; i < N; i++ ) {
            cvReleaseImage( &buf[i] );
            buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
            cvZero( buf[i] );
        }
        cvReleaseImage( &mhi );
        cvReleaseImage( &orient );
        cvReleaseImage( &segmask );
        cvReleaseImage( &mask );
        
        mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        cvZero( mhi ); // clear MHI at the beginning
        orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
    }

    cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale

    idx2 = (last + 1) % N; // index of (last - (N-1))th frame
    last = idx2;

    silh = buf[idx2];
    cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames
    
    cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); // and threshold it
    cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

    // convert MHI to blue 8u image
    cvCvtScale( mhi, mask, 255./MHI_DURATION,
                (MHI_DURATION - timestamp)*255./MHI_DURATION );
    cvZero( dst );
    cvCvtPlaneToPix( mask, 0, 0, 0, dst );


    double value = 0;
    int ct = 0;
    IMGFOR(dest,x,y) {
      value+=dest(x,y).r;
      value+=dest(x,y).g;
      value+=dest(x,y).b;
      ct++;
    }
    if (ct<1) ct = 1;
    value /= ct;

    // calculate motion gradient orientation and valid orientation mask
    cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );
    
    if( !storage )
        storage = cvCreateMemStorage(0);
    else
        cvClearMemStorage(storage);
    
    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)

    double now = YARPTime::GetTimeAsSeconds();
    static double last_wide = now;

    for( i = -1; i < seq->total; i++ ) {
      int wide = 0;

        if( i < 0 ) { // case of the whole image
            comp_rect = cvRect( 0, 0, size.width, size.height );
            color = CV_RGB(255,255,255);
            magnitude = 100;
	    wide = 1;
        }
        else { // i-th motion component
            comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i, 0 ))->rect;
            if( comp_rect.width + comp_rect.height < 150 ) // reject very small components
                continue;
            color = CV_RGB(255,0,0);
            magnitude = 30;
        }

        // select component ROI
        cvSetImageROI( silh, comp_rect );
        cvSetImageROI( mhi, comp_rect );
        cvSetImageROI( orient, comp_rect );
        cvSetImageROI( mask, comp_rect );

        // calculate orientation
        angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin
	//printf("Angle is %g\n", angle);

        count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

        cvResetImageROI( mhi );
        cvResetImageROI( orient );
        cvResetImageROI( mask );
        cvResetImageROI( silh );

        // check for the case of little motion
        if( count < comp_rect.width*comp_rect.height * 0.05 )
            continue;

        // draw a clock with arrow indicating the direction
        center = cvPoint( (comp_rect.x + comp_rect.width/2),
                          (comp_rect.y + comp_rect.height/2) );

	if (wide) {
	  last_wide = now;
	} else {
	  cvCircle( dst, center, cvRound(15*1.2), CV_RGB(0,255,0), 3 );
	  gx = (int)center.x;
	  gy = (int)center.y;
	  gv = value;
	}

        cvCircle( dst, center, cvRound(magnitude*1.2), color, 3 );
        cvLine( dst, center, 
		cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
                cvRound( center.y - magnitude*sin(angle*CV_PI/180))), 
		color, 3, 8 );
    }
}


int foo_main(int argc, char** argv)
{
  fstream fin("list.txt");
  int capture = 1;
  //cvWaitKey(0);
  if( capture ) {
    IplImage* motion = 0;
    cvNamedWindow( "Motion", 1 );
        
    //IplImage* image;
	//image = cvCreateImage(cvSize(128,128),8,3);

	YARPImageOf<YarpPixelBGR> test_image,test2;
	test_image.Resize(256,256);

    for(;;)
      {

	IplImage* image = test_image.GetIplPointer();
	static int offset = 0;
	offset++;
	test_image.Zero();
	IMGFOR(test_image,x,y) {
	  test_image(x,y).r=128+x+y*10;
	  test_image(x,y).g=128+y;
	  test_image(x,y).b=128+x;
	}
	for (int i=0; i<80; i++) {
	  for (int j=0; j<80; j++) {
	    test_image.SafePixel(i+100-offset,j+100+offset).r = 255+i*40+j*97;
	    test_image.SafePixel(i+50-offset*3,j+100).r = 255+i*40+j*97;
	  }
	}
	char buf[256] = "foo";
	fin >> buf;
	YARPImageFile::Read(buf,test2);
	test_image.ScaledCopy(test2,256,256);

	
	if( image )
	  {
	    if( !motion )
	      {
		motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
		cvZero( motion );
		motion->origin = image->origin;
	      }
	  }
	
	//update_mhi( image, motion, 15 );
	//cvShowImage( "Motion", image );
	//cvShowImage( "Motion", motion );
	
	//if( cvWaitKey(10) >= 0 )
	//break;
      }
    cvDestroyWindow( "Motion" );
  }
  
  return 0;
}
           

static int capture = 0;
static IplImage* motion = 0;
static YARPImageOf<YarpPixelRGB> motion_image;

void MotionGroup::Apply(YARPImageOf<YarpPixelRGB>& src,
			YARPImageOf<YarpPixelRGB>& dest) {

  static int ct = 0;

  //foo_main(0,NULL);

  //for (;;) {
  ct++;
  gv = -1;
  if (ct>0) {
    //dest.CastCopy(src);
    
    //YARPImageOf<YarpPixelBGR> test_image,test2;
    //test_image.Resize(256,256);
    //test_image.Zero();
    IplImage* image = src.GetIplPointer();
    
    if( !capture ) {
      cvNamedWindow( "Motiony", CV_WINDOW_AUTOSIZE );
      capture = 1;
    }
    if( image )
      {
	if( !motion )
	  {
	    motion_image.Resize(image->width,image->height);
	    motion_image.Zero();
	    motion = motion_image.GetIplPointer();
	    //motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
	    //cvZero( motion );
	    //motion->origin = image->origin;
	    //printf("WIDTH %d\n", image->width);
	    //printf("HEIGHT %d\n", image->height);
	  }
      }
    
    update_mhi( image, motion_image, 30 );
    //cvShowImage( "Motiony", image );
    cvShowImage( "Motiony", motion );
    cvWaitKey(1);
  }

  this->x = gx;
  this->y = gy;
  this->v = gv;
}


