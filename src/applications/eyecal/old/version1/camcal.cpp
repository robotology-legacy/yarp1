#include <iostream.h>
#include <math.h>

#include "YARPTime.h"
#include "YARPImage.h"
#include "YARPPort.h"
#include "YARPImagePort.h"

#define for if(1) for

// INPUT PORTS.
YARPInputPortOf<YARPGenericImage> in_fixed;
YARPInputPortOf<YARPGenericImage> in_moving;

char *my_name = "/yarp/camcal";

char in1_name[256];

char in2_name[256];


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

	sprintf (in1_name, "%s/fixed/i:img", my_name); 

	sprintf (in2_name, "%s/moving/i:img", my_name);
}

#define HASH_WIDTH 64
#define HASH_RES (256/HASH_WIDTH)
#define HASH_SIZE (HASH_RES*HASH_RES*HASH_RES)

int Hash(YarpPixelBGR& pix)
{
  int x = (pix.r/HASH_WIDTH);
  x *= HASH_RES;
  x += (pix.g/HASH_WIDTH);
  x *= HASH_RES;
  x += (pix.b/HASH_WIDTH);
  return x;
}

float Process(YARPImageOf<YarpPixelBGR>& ref, 
	      YARPImageOf<YarpPixelBGR>& target)
{
  static float bounce = 0;
  static int hash_ref[HASH_SIZE];
  static int hash_target[HASH_SIZE];
  static int hash_target_ylo[HASH_SIZE];
  static int hash_target_yhi[HASH_SIZE];
//  printf("Clearing hash tables\n");
  for (int r=0; r<HASH_SIZE; r++)
    {
      hash_ref[r] = hash_target[r] = 0;
      hash_target_ylo[r] = hash_target_yhi[r] = 0;
    }
//  printf("Comparing images\n");
  int h = ref.GetHeight();
  int w = ref.GetWidth();
  assert(h==target.GetHeight());
  assert(w==target.GetWidth());
  for (int y=0; y<h; y++)
    {
      for (int x=0; x<w; x++)
	{
	  YarpPixelBGR pix_ref = ref(x,y);
	  YarpPixelBGR pix_target = target(x,y);
	  int idx_ref = Hash(pix_ref);
	  int idx_target = Hash(pix_target);
	  hash_ref[idx_ref]++;
	  hash_target[idx_target]++;
	  if (y>=h/2)
	    {
	      hash_target_yhi[idx_target]++;
	    }
	  else
	    {
	      hash_target_ylo[idx_target]++;
	    }
	}
    }
  int votes = 0;
  float dir = 0;
  for (int r=0; r<HASH_SIZE; r++)
    {
      int diff = hash_ref[r] - hash_target[r];
      if (fabs(diff)>3)
	{
	  int hi = hash_target_yhi[r];
	  int lo = hash_target_ylo[r];
	  if (fabs(hi-lo)>3)
	    {
	      float power = fabs(hi-lo)*fabs(diff);
	      if (power>20) power = 20;
	      float direction = power;
	      if (diff>0)
		{
		  if (hi<lo)
		    {
		      direction = -direction;
		    }
		}
	      else
		{
		  if (hi>lo)
		    {
		      direction = -direction;
		    }
		}
	      
	      votes++;
	      dir += direction;
	    }
	}
    }
  printf("%8d votes, verdict is %8.4g ", votes, dir);
  bounce = 0.8*bounce + 0.2*((dir>0)?1:-1);
  if (bounce>0.5) printf("LOOK DOWN");
  if (bounce<-0.5) printf("LOOK UP");
  printf("\n");
  return dir;
}

void main(int argc, char *argv[])
{
  YARPImageOf<YarpPixelBGR> img_ref;
  YARPImageOf<YarpPixelBGR> img_target;

  ParseCommandLine (argc, argv);
  
  in_fixed.Register (in1_name);
  in_moving.Register (in2_name);

  YARPTime::DelayInSeconds(0.5);
  
//  printf("Waiting for fixed\n");
  in_fixed.Read();
  img_ref.PeerCopy(in_fixed.Content());
  while (1)
    {
//      printf("Waiting for moving\n");
      in_moving.Read();
      if (in_fixed.Read(0))
	{
	  img_ref.PeerCopy(in_fixed.Content());
	}
//      printf("Images collected\n");
      img_target.Refer(in_moving.Content());
      Process(img_ref,img_target);
    }
}
