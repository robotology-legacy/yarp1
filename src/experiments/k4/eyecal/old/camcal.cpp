//
// skin.cpp
//	- June 2001.
//	- Sept 2001.
//
#include <sys/kernel.h>
#include <iostream.h>

// YARP stuff first.
#include "YARPImage.h"
#include "YARPPort.h"
#include "YARPImagePort.h"
#include "YARPImageFile.h"
#include "YARPTime.h"
#include "YARPImageDraw.h"

#define for if(1) for

// still some functions to be updated.
//#include "temporary/YARPConverter.h"

YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;

char *my_name = "/camcal";
char in_img_name[256];
char out_img_name[256];
int duration = 60;

// parses command line.
void ParseCommandLine(int argc, char *argv[])
{
	int i;

	for(i=1;i<argc;i++)
	{
		switch(argv[i][0])
		{
			case '-':
				switch(argv[i][1])
				{
					case 'h':
					case '-':
						cout << endl;
						cout << my_name << " argument:" << endl;
						cout << endl;
						exit(0);
						break;

					default:
						break;
				}
				break;
			case '+':
				my_name = &argv[i][1];
				break;
			default:
				break;
		}
	}

	sprintf (in_img_name, "%s/i:img", my_name); 
	sprintf (out_img_name, "%s/o:img", my_name); 
}



void ProcessEye(YARPImageOf<YarpPixelMono>& mono, 
		YARPImageOf<YarpPixelBGR>& dest,
		int& xleft, int& yleft, int scale)
{
  YarpPixelBGR pix(255,122,0);
  float grey = 300; //mono.SafePixel(yleft,xleft);
  for (int x=xleft-2; x<=xleft+2; x++)
    {
      for (int y=yleft; y<=yleft+3; y++)
	{
	  float grey2 = (mono.SafePixel(x,y)+mono.SafePixel(x,y+1))/2;
	  if (grey2<grey)
	    {
	      grey = grey2;
	      yleft = y;
	      xleft = x;
	    }
	}
    }
  int th = 20;
  for (int k=0; k<10; k++)
    {
      //AddCrossHair(dest,pix,yleft,xleft,3);
      for (int y=0; y<scale; y++)
	{
	  if (mono.SafePixel(xleft,yleft+y)>grey+th &&
	      mono.SafePixel(xleft,yleft+y+1)>grey+th)
	    {
	      //AddCrossHair(dest,pix,yleft+y,xleft,3);
	      yleft = yleft+y-1;
	      break;
	    }
	}
      int xleftleft, xleftright;
      xleftleft = xleft;
      xleftright = xleft;
      for (int x=0; x<scale; x++)
	{
	  if (mono.SafePixel(xleft-x,yleft)>grey+th &&
	      mono.SafePixel(xleft-x,yleft+1)>grey+th)
	    {
	      //AddCrossHair(dest,pix,yleft,xleft-x,3);
	      xleftleft = xleft-x;
	      break;
	    }
	}
      for (int x=0; x<scale; x++)
	{
	  if (mono.SafePixel(xleft+x,yleft)>grey+th &&
	      mono.SafePixel(xleft+x,yleft+1)>grey+th)
	    {
	      //AddCrossHair(dest,pix,yleft,xleft+x,3);
	      xleftright = xleft+x;
	      break;
	    }
	}
      xleft = (int)(0.5+(xleftleft+xleftright)/2);
    }
  YarpPixelBGR pix2(100, 255, 100);
  AddCrossHair(dest,pix2,xleft,yleft,3);
}


int ProcessFace(YARPImageOf<YarpPixelBGR>& src, YARPImageOf<YarpPixelBGR>& dest)
{
  int h = src.GetHeight();
  int w = src.GetWidth();
  float best = -1;
  int xbest = 0;
  //SatisfySize(src,dest);
  for (int j=0; j<w; j++)
    {
      float accum = 0;
      for (int i=0; i<h; i++)
	{
	  YarpPixelBGR p2 = src.SafePixel(j+1,i);
	  YarpPixelBGR p3 = src.SafePixel(j-1,i);
	  float v = (src(j,i).r + src(j,i).g + src(j,i).b)/3.0;
	  float v2 = (p2.r + p2.g + p2.b)/3.0;
	  float v3 = (p3.r + p3.g + p3.b)/3.0;
	  v = (v+v2+v3)/3.0;
	  if (v>200)
	    {
	      accum += v;
	    }
	}
      if (accum>best)
	{
	  best = accum;
	  xbest = j;
	}
    }
  int ct = 0;
  int state = 0;
  int ybest = 0;
  YarpPixelBGR pix(255,0,0);
  int scale = 0;
  for (int i=0; i<h; i++)
    {
      dest(xbest,i) = pix;
      float v = (src(xbest,i).r + src(xbest,i).g + src(xbest,i).b)/3.0;
      int pstate = state;
      if (v>200 && pstate == 0) 
	{
	  state = 1;
	  ct = 0;
	}
      ct++;
      if (v<100 && pstate == 1)
	{
	  if (ct>10)
	    {
	      scale = ct;
	      state = 2;
	      ybest = i;
	      for (int jj=0; jj<w; jj++)
		{
		  dest(jj,i) = pix;
		}
	    }
	  else
	    {
	      state = 0;
	    }
	}
    }
  AddCrossHair(dest,pix,xbest,ybest-scale,10);
  AddCircleOutline(dest,pix,xbest,ybest,5);

  int rhead = scale;
  int xhead = xbest;
  int yhead = ybest;

  int xleft, yleft;
  int dx_eye = (int)(rhead*0.5);
  xleft = xhead-dx_eye;
  yleft = yhead;
  //AddCrossHair(dest,pix,yleft,xleft,3);

  int xright, yright;
  xright = xhead+dx_eye;
  yright = yhead;
  //AddCrossHair(dest,pix,yright,xright,3);

  static YARPImageOf<YarpPixelMono> mono;
  mono.CastCopy(src);
  int scale_eye = (int)(scale*0.30);
  ProcessEye(mono,dest,xleft,yleft,scale_eye);
  ProcessEye(mono,dest,xright,yright,scale_eye);

  YarpPixelBGR pix2(100, 255, 100);
  int ylower = (yleft+yright)/2;
  if (fabs(yleft-yright)<scale*0.2 && fabs(ylower-yhead)<scale*0.25 &&
      fabs(ylower-yhead)<h/5)
    {
      for (int x=0; x<w; x++)
	{
	  dest.SafePixel(x,ylower) = pix2;
	}
      printf("confirmation ratio is %g\n", fabs(yhead-ylower)/((float)scale));
    }
  else
    {
      for (int x=0; x<w; x++)
	{
	  dest.SafePixel(x,x) = pix2;
	  dest.SafePixel(x,h-x) = pix2;
	}
    }

  return 0;
}

/*-----------------*/











void main(int argc, char *argv[])
{
  ParseCommandLine (argc, argv);
  
  in.Register (in_img_name);
  out.Register (out_img_name);
  
  YARPTime::DelayInSeconds(0.2);
  
  while(1)
    {
      in.Read();
      YARPGenericImage& inImg = in.Content();
      YARPGenericImage& outImg = out.Content(); // Lasts until next Write()
      outImg.PeerCopy(inImg);
      YARPImageOf<YarpPixelBGR> src, dest;
      src.Refer(inImg);
      dest.Refer(outImg);
      ProcessFace(src,dest);
      out.Write();
    }
}
