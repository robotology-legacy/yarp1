#include <iostream.h>

#include "YARPImage.h"
#include "YARPPort.h"
#include "YARPImagePort.h"
#include "YARPImageDraw.h"
#include "YARPImageFile.h"
#include "YARPTime.h"

#include <fstream.h>

#define for if(1) for

YARPInputPortOf<YARPGenericImage> in;

char *my_name = "/cogcam";
char in_img_name[256];
char out_img_name[256];
char file_name[256] = "list.txt";
int duration = 60;

// parses command line.
void ParseCommandLine(int argc, char *argv[])
{
  int i;
  char *fname = "list.txt";
  
  for(i=1;i<argc;i++)
    {
      switch(argv[i][0])
	{
	case '-':
	  switch(argv[i][1])
	    {
	    case 'h':
	    case '-':
	      //	      cout << endl;
	      //	      cout << my_name << " argument:" << endl;
	      //	      cout << endl;
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
	  fname = &argv[i][0];
	  break;
	}
    }
  
  sprintf (in_img_name, "%s", my_name); 
  sprintf (out_img_name, "%s/o:img", my_name); 
  sprintf (file_name, "%s", fname); 
}


void main(int argc, char *argv[])
{
  ParseCommandLine (argc, argv);

  in.Register(in_img_name);

  while (1)
    {
      in.Read();
      YARPImageOf<YarpPixelBGR> im;
      im.Refer(in.Content());
      for (int y=0; y<im.GetHeight(); y++)
	{
	  for (int x=0; x<im.GetWidth(); x++)
	    {
	      YarpPixelBGR& pix = im(x,y);
	      putchar(pix.r);
	      putchar(pix.g);
	      putchar(pix.b);
	    }
	}
      fflush(stdout);
    }
}
