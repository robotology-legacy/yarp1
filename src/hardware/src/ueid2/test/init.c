
#include <stdio.h>
#include <i86.h> //for delay

#include "ueid.h"

#define BOARD 0
#define MAX_SAMP 64

void main()
{
  int ueidx, stat, i;
//  short analbuffer[MAX_SAMP];
  uint16_t analbuffer[MAX_SAMP];
  
  stat = ueid_initialize("/yarp/uei", &ueidx);
  fprintf(stderr, "init return is %d\n", stat);
  
  stat = ueid_sw_cl_start(ueidx, BOARD);
  fprintf(stderr, "first clock return is %d\n", stat);
  
  for(i=0;i<50;i++)
    {
      delay(100);
      stat = ueid_get_samples(ueidx, BOARD, MAX_SAMP, &analbuffer);
      //fprintf(stderr, "get_samples %d return is %d\n", i, stat);
      stat = ueid_sw_cl_start(ueidx, BOARD);      
      //fprintf(stderr, "clock %d return is %d\n", i, stat);
      
      fprintf(stderr, "buffer[2]: %d | stat = %d\n", analbuffer[2], stat);
    }
  
  fprintf(stderr, "i love alex ivchenko\n");
}
