///
///
///	  YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///		       #original paulfitz, changed pasa#
///
///	"Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: objectspotter.cpp,v 1.2 2004-09-05 23:20:24 eshuy Exp $
///
///

#ifdef __LINUX__
#define WORK_FROM_SCRIPT
#endif

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>

#include <iostream>
#include <math.h>

#include <yarp/YARPTime.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPBottle.h>

#include <yarp/YARPFineOrientation.h>

#include "ImgTrack.h"
#include "find_flipper.h"

#ifdef WORK_FROM_SCRIPT
#include "FileProcessor.h"
#endif

// stupid windows
#define for if(1) for


YARPInputPortOf<YARPGenericImage> in_img;
YARPOutputPortOf<YARPGenericImage> out_img;

static int global_grasping = 0;
static YARPSemaphore global_mutex(1);

class MyBot : public YARPInputPortOf<YARPBottle> 
{
public:
  virtual void OnRead() 
  {
    if (Read()) 
      {
	YARPBottle& bot = Content();
	printf("Got a bottle!\n");
	bot.display();
	bot.rewind();
	// interested in:
	// KFStart
	// KFStop
	const char *buf = bot.tryReadText();
	YBVocab vocab;
	if (bot.tryReadVocab(vocab)) 
	  {
	    buf = vocab.c_str();
	  }

	if (buf!=NULL) 
	  {
	    printf("read text %s\n", buf);
	    if (strcmp(buf,"KFStart")==0) 
	      {
		global_mutex.Wait();
		global_grasping = 1;
		global_mutex.Post();
		printf("Beginning exploration\n");
	      }
	    if (strcmp(buf,"KFStop")==0) 
	      {
		global_mutex.Wait();
		global_grasping = 0;
		global_mutex.Post();
		printf("Ending exploration\n");
	      }
	  }
      }
  }
} in_bot;

YARPOutputPortOf<YARPBottle> out_bot;

// port prefix is trackercloned for obscure historical reasons 
const char *DEFAULT_NAME = "/trackercloned";

///
///
///
///
extern int flipper_main(int argc, char *argv[]);


class ObjectSpotter : public FileProcessor {
public:
  double train_time;
  double down_time;

  virtual void Apply(int argc, char *argv[]) {
    if (argc>=3) {
      const char *filename = argv[1];
      int grasping = atoi(argv[2]);
      printf("Dealing with image %s, grasping %d\n", filename, grasping);
      YARPImageOf<YarpPixelBGR> src;
      YARPImageOf<YarpPixelBGR> dest;
      YARPImageFile::Read(filename,src);
      //src.Resize(256,256);
      dest.PeerCopy(src);
      process(src,dest,grasping);

      char buf[256];
      char buf2[256];
      static int ct = 0;
      sprintf(buf,"out_%06d_%s",ct,filename);
      ct++;
      for (int i=0; i<strlen(buf); i++)
	{
	  if (buf[i]=='/')
	    {
	      buf[i] = 'Z';
	    }
	}
      sprintf(buf2,"/tmp/%s",buf);
      YARPImageFile::Write(buf2,dest);
    }
  }

  void process(YARPImageOf<YarpPixelBGR>& src,
	       YARPImageOf<YarpPixelBGR>& dest,
	       int grasping) {
    if (grasping) {
      process_training(src,dest);
    } else {
      process_normal(src,dest);
    }
  }

  void process_normal(YARPImageOf<YarpPixelBGR>& src,
		      YARPImageOf<YarpPixelBGR>& dest);

  void process_training(YARPImageOf<YarpPixelBGR>& src,
			YARPImageOf<YarpPixelBGR>& dest);

  int main(int argc, char *argv[]);
};

void ObjectSpotter::process_normal(YARPImageOf<YarpPixelBGR>& src,
				   YARPImageOf<YarpPixelBGR>& dest) {
  static YARPImageOf<YarpPixelBGR> remapped;

  static int msg1 = 1;
  if (msg1) { 
    printf("Got an image %dx%d\n",src.GetWidth(),src.GetHeight()); 
    msg1 = 0; 
  }

  SatisfySize(src,remapped);
  remapped.CastCopy(src);
  SatisfySize(remapped,dest);

  // sorry, currently have expectations about source image size
  assert(src.GetWidth()==256 && src.GetHeight()==256);
  int xx = 128, yy = 128, ff = 0;
  flipper_apply(remapped,dest, xx, yy, ff);

  static int at = 0;
  at = (at+1)%dest.GetHeight();

  printf("Writing image of size %d %d\n",
	 dest.GetWidth(),
	 dest.GetHeight());

  double now = YARPTime::GetTimeAsSeconds();
  static int fixct = 0;
  if (now-down_time<20 && now-down_time>3) 
    {
      fixct++;
      int go = 0;
      if (fixct==3) 
	{
	  go = 1;
	  down_time = -10000;
	} 
      else 
	{
	  down_time = now-1.5;
	}

      printf(">>> %d %d %d\n", xx, yy, go);
      if (xx>=0 && yy>=0 && xx<256 && yy<256) 
	{
	  out_bot.Content().rewind();
	  out_bot.Content().writeInt(xx);
	  out_bot.Content().writeInt(yy);
	  if (go) 
	    {
	      out_bot.Content().writeInt(255); //apparently any third number is enough to trigger grasping
	    }
	  out_bot.Write();
	}
    }

  if (now-train_time<3) 
    {
      fixct = 0;
      train_time = -100000;
      down_time = now;
      out_bot.Content().rewind();
      out_bot.Content().writeInt(128);
      out_bot.Content().writeInt(230);
      out_bot.Write();
    }
}


void ObjectSpotter::process_training(YARPImageOf<YarpPixelBGR>& src,
				     YARPImageOf<YarpPixelBGR>& dest) {
  dest.CastCopy(src);
  static int phase = 0;
  if (phase) 
    {
      for (int j=-10; j<=10; j++) 
	{
	  for (int i=0; i<dest.GetWidth(); i++) 
	    {
	      dest.SafePixel(i,j+dest.GetHeight()/2).r = 255;
	    }
	}
    } 
  else 
    {
      for (int j=-10; j<=10; j++) 
	{
	  for (int i=0; i<dest.GetHeight(); i++) 
	    {
	      dest.SafePixel(j+dest.GetWidth()/2,i).r = 255;
	    }
	}
    }

  phase = !phase;
  // pick a foveal region, any foveal region
  // prep a mask, leave some of the boundary so orientations
  // can be computed reasonably

  int xc = src.GetWidth()/2;
  int yc = src.GetHeight()/2;
  int dx = src.GetWidth()/8;
  int dy = src.GetHeight()/8;
  int dx2 = dx*2;
  int dy2 = dy*2;

  YarpPixelBGR pix1(255,255,0);
  YarpPixelBGR pix2(0,255,255);
  AddRectangle(dest,pix2,xc,yc,dx+1,dy+1);
  AddRectangle(dest,pix2,xc,yc,dx-1,dy-1);
  AddRectangle(dest,pix2,xc,yc,dx2,dy2);

  static YARPImageOf<YarpPixelBGR> img,img2,fake_dest;
  static YARPImageOf<YarpPixelMono> mask,mask2;
  img.Resize(dx2*2,dy2*2);
  mask.Resize(dx2*2,dy2*2);
  img2.Resize(dx2*2,dy2*2);
  mask2.Resize(dx2*2,dy2*2);
  src.Crop(img,xc-dx2,yc-dy2);
  src.Crop(img2,xc-dx2,0);
  mask.Zero();
  mask2.Zero();
			
  for (int x=dx2-dx; x<=dx2+dx; x++) 
    {
      for (int y=dy2-dy; y<=dy2+dy; y++) 
	{
	  mask(x,y) = 255;
	}
    }
			
  IMGFOR(mask2,x,y) 
    {
      if (y<mask2.GetHeight()*0.75) 
	{
	  mask2(x,y) = 255;
	}
    }
			
  static int ct = 0;
  ct++;

  flipper_apply(img,mask,fake_dest);
  flipper_apply(img2,mask2,fake_dest,1);
  train_time = YARPTime::GetTimeAsSeconds();
}


int ObjectSpotter::main(int argc, char *argv[])
{
  ///set_yarp_debug(100,100);
  printf("Objectspotter starting up - this might take a little while.\n");
  YARPFineOrientation fine;

  char filename[256];
  sprintf (filename, "%s/conf/babybot/orient.txt", GetYarpRoot());
  ///fine.Init("y:/conf/babybot/orient.txt");
  fine.Init(filename);
	
  int ac = 1;
  char *av[] = { "main", "foo.txt", NULL };

  //flipper_main(argc,argv);
  flipper_main(ac,av);
  int use_wide = 1;

  YARPString name;
  YARPString network_i;
  YARPString network_o;
  char buf[256];

  if (!YARPParseParameters::parse(argc, argv, "-name", name))
    {
      name = DEFAULT_NAME;
    }

  if (!YARPParseParameters::parse(argc, argv, "-neti", network_i))
    {
      network_i = "default";
    }

  if (!YARPParseParameters::parse(argc, argv, "-neto", network_o))
    {
      network_o = "default";
    }

  /// images are coming from the input network.
  sprintf(buf, "%s/i:img", name.c_str());
  in_img.Register(buf, network_i.c_str());

  sprintf(buf, "%s/o:img", name.c_str());
  out_img.Register(buf, network_i.c_str());
  out_img.SetRequireAck(1);

  sprintf(buf, "%s/i:bot", name.c_str());
  in_bot.Register(buf, "default");

  sprintf(buf, "%s/o:bot", name.c_str());
  out_bot.Register(buf, "default");

  ///
  ///
  ///
  YARPImageOf<YarpPixelBGR> in;
  YARPImageOf<YarpPixelBGR> out;

  printf("\nObjectspotter active.\n\n");

  train_time = -10000;
  down_time = -10000;

#ifdef WORK_FROM_SCRIPT
  Process("objectspotter.txt");
#else
  while(1)
    {
      double now = YARPTime::GetTimeAsSeconds();
      static double first_time = now;
      int tick = (int)(now-first_time)/10;
      use_wide = (tick%2);
      use_wide = 1;

      static int msg0 = 1;
      if (msg0) { printf("waiting for first image.\n"); msg0=0; }

      global_mutex.Wait();
      int grasping = global_grasping;
      global_mutex.Post();

      in_img.Read();
      if (!grasping) 
	{
	  in_img.Read();
	  in_img.Read();
	  in_img.Read();
	}
      out_img.Content().SetID (YARP_PIXEL_BGR);
      in.Refer(in_img.Content());
      out.Refer(out_img.Content());
      process(in,out,grasping);
      out_img.Write ();

    } /* end while(1) */
#endif

  flipper_end();

  return 0;
}



int main(int argc, char *argv[])
{
  ///set_yarp_debug(100,100);

  ObjectSpotter os;

  os.main(argc,argv);
  
  return 0;
}
