#include <unistd.h>
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
  pid_t cpid;

  
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

  fprintf(stderr, "Fork You!\n");
  if ((cpid = fork()) == 0) {
    fprintf(stderr, "I am the child.\n");
    for (i=0; i<100000; i++) {
      stat = meid_get_position(meidx, axis, &pos);
      fprintf(stderr, "chld %d:  axis%d  %8.1f(%d)\n", i, axis, pos, stat);
      delay(1000);
    }
    meid_close(meidx);
    exit(0);
  }

  delay(500);
  fprintf(stderr, "I am the parent (of %d).\n", cpid);
  for (i=0; i<100000; i++) {
    stat = meid_get_position(meidx, axis, &pos);
    fprintf(stderr, "prnt %d:  axis%d  %8.1f(%d)\n", i, axis, pos, stat);
    delay(1000);
  }
  meid_close(meidx);
}
