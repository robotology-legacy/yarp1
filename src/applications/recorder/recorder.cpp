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

#define for if(1) for

// still some functions to be updated.
//#include "temporary/YARPConverter.h"

YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;

char *my_name = "/recorder";
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

//#define WIDE_IMAGE

#ifdef WIDE_IMAGE
#define MAX_FRAMES 30
#define WIDTH 640
#define HEIGHT 240
#endif

#ifndef MAX_FRAMES
#define MAX_FRAMES 30*60
#define WIDTH 128
#define HEIGHT 128
#endif

YARPImageOf<YarpPixelBGR> store[MAX_FRAMES];

void main(int argc, char *argv[])
{
  ParseCommandLine (argc, argv);
  
  printf("Testing allocation\n");
  for (int i=0; i<MAX_FRAMES; i++)
    {
      store[i].Resize(WIDTH,HEIGHT);
    }
  printf("Allocation complete\n");
  
  in.Register (in_img_name);
  out.Register (out_img_name);
  
  YARPImageOf<YarpPixelBGR> in_image;
  
  int index = 0;
  while (index<MAX_FRAMES)
    {
      in.Read();
      in_image.Refer (in.Content());
      store[index].PeerCopy(in_image);
      if (index%30 == 0)
	{
	  printf("Stored frame %d (%g)\n", index, index/30.0);
	}
      index++;
    }
  for (int i=0; i<MAX_FRAMES; i++)
    {
      char fname[200] = "";
      sprintf(fname,"/tmp/video/grab%05d.ppm",i);
      printf("Writing frame %d to %s\n", i, fname);
      YARPImageFile::Write(fname,store[i]);
    }
  index = 0;
  while(1)
    {
      in.Read();
      YARPGenericImage& outImg = out.Content(); // Lasts until next Write()
      outImg.PeerCopy(store[index]);
      out.Write();
      if (index%30 == 0)
	{
	  printf("Displayed frame %d (%g)\n", index, index/30.0);
	}
      index++;
      if (index>=MAX_FRAMES)
	{
	  index = 0;
	}
    }
}
