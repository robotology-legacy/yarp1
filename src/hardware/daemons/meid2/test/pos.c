#include <stdlib.h>
#include <i86.h>

#include <stdio.h>
#include <errno.h>

#include <meid.h>

void main(int argc, char *argv[])
{
  int meidx;
  int16 stat;
  int i;
  double pos;
  int axis;

  
  if (argc != 3) {
    fprintf(stderr, "usage:  %s mei_device axis\n", argv[0]);
    exit(1);
  }
  if ((meidx = meid_open(argv[1])) == -1) {
    perror("meid_open");
    exit(errno);
  }
  axis = strtol(argv[2], NULL, 0);

  stat = meid_dsp_reset(meidx);
  fprintf(stderr, "reset return is %d\n", stat);  

  for (i=0; i<100000; i++) {
    stat = meid_get_position(meidx, axis, &pos);
    fprintf(stderr, "%d:  axis%d  %8.1f(%d)\n", i, axis, pos, stat);
    delay(500);
  }

  meid_close(meidx);
}
