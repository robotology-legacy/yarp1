
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <i86.h>
#include "mei/pcdsp.h"

#define SIZE 50000
int err, i, j;
int16 val[SIZE];

void main()
{
  
  err = dsp_init(0x320);
  if (err) {
    fprintf(stderr, "dsp_init failed! (%d)\n", err);
    exit(1);
  }
  dsp_reset();

  for(i=0;i<SIZE;i++)
    {
      for(j=0;j<8;j++)
	err = get_analog(j, &val[j]);
      if (err) 
	{
	  printf("Error! %d\n", err);
	  exit(1);
	}
    }
  
}

void other_stuff()
{
  while(1)
    {
      err = start_analog(0);
      err |= start_analog(1);
      delay(1);
      err |= read_analog(&val[0]);
      err |= read_analog(&val[1]);
      delay(1);
      err |= get_analog(0, &val[2]);
      delay(1);
      err |= get_analog(1, &val[3]);
      printf("[0]:%d [1]:%d [0 alone]:%d [1 alone]:%d\n", 
	     val[0], val[1], val[2], val[3]);
      
      sleep(1);
    }
}
