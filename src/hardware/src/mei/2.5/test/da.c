
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <i86.h>
#include "meid.h"

#define SIZE 100000
int err, i, j;
int val[SIZE];
int meidx = -1;

void main()
{
  err = meid_dsp_init("/cog/mei", &meidx);
  if (err) {
    fprintf(stderr, "dsp_init failed! (%d)\n", err);
    exit(1);
  }
  err = meid_dsp_reset(meidx);
  if (err) {
    fprintf(stderr, "dsp_reset failed! (%d)\n", err);
    exit(1);
  }

  for(i=0;i<SIZE;i++)
    {
      for(j=0;j<4;j++)
	err = meid_get_analog(meidx, j, &val[j]);
      if (err) 
	{
	  printf("Error! %d\n", err);
	  exit(1);
	}
    }
  
}

