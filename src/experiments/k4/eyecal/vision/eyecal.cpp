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

YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;

char *my_name = "/eyecal";
char in_img_name[256];
char out_img_name[256];

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


void ProcessImage(YARPImageOf<YarpPixelBGR>& src,
		  YARPImageOf<YarpPixelBGR>& dest)
{
  int h = src.GetHeight();
  int w = dest.GetWidth();
  int top_grace = 4;
  int bot_grace = 4;
  int top_index = -1;
  int bot_index = h;
  int filter = 1;
  for (int x=0; x<w; x++)
    {
      YarpPixelBGR pix(255,0,0);
      dest(x,h/2) = pix;
    }
  static YARPImageOf<YarpPixelMono> mono;
  mono.CastCopy(src);
  for (int y=0; y<h-2; y++)
    {
      float total = 0;
      float mean = 0;
      float flat = 0;
      for (int x=0; x<w-1; x++)
	{
	  //float diff = fabs(mono(x,y)-mono(x+2,y));
	  float diff = fabs(mono(x,y)-mono(x+2,y+2));
	  total += diff;
	  if (diff>=15)
	    {
	      flat++;
	    }
	}
      mean = total/w;
      if (flat<=10)
	{
	  if (top_grace)
	    {
	      top_index = y;
	    }
	  if (!filter)
	    {
	      for (int x=0; x<w; x++)
		{
		  //YarpPixelBGR pix(0,255,0);
		  dest(x,y).g = 255;
		}
	    }
	}
      else
	{
	  if (top_grace>0) top_grace--;
	}
    }

  for (int y=h-1; y>=1; y--)
    {
      float total = 0;
      float mean = 0;
      float flat = 0;
      for (int x=0; x<w-1; x++)
	{
	  //float diff = fabs(mono(x,y)-mono(x+2,y));
	  float diff = fabs(mono(x,y)-mono(x+2,y-1));
	  total += diff;
	  if (diff>=15)
	    {
	      flat++;
	    }
	}
      mean = total/w;
      if (flat<=10)
	{
	  if (bot_grace)
	    {
	      bot_index = y;
	    }
	  if (!filter)
	    {
	      for (int x=0; x<w; x++)
		{
		  //YarpPixelBGR pix(0,255,0);
		  dest(x,y).g = 255;
		}
	    }
	}
      else
	{
	  if (bot_grace>0) bot_grace--;
	}
    }
  
  
  for (int y=0; y<=top_index; y++)
    {
      for (int x=0; x<w; x++)
	{
	  dest(x,y).r = 255;
	}
    }

  for (int y=bot_index; y<h; y++)
    {
      for (int x=0; x<w; x++)
	{
	  dest(x,y).r = 255;
	}
    }
}


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
      ProcessImage(src,dest);
      out.Write();
    }
}
