/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

extern PhImage_t * phimage;
extern bool freeze;
extern int W;
extern int H;

int
save_image( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  char file_name[100] = "image";
  char temp[255];
  int fd;
  static int cnt = 0;
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  bool _i_freeze_it = false;

  if (!freeze)
  {
    freeze =true;
    _i_freeze_it = true;
  }

  strcat(file_name,itoa(cnt,temp,10));
  strcat(file_name,".raw");
  fd = open(file_name,O_WRONLY | O_CREAT | O_APPEND,
      	    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  write(fd,phimage->image,W*H*3);
  close(fd);

  cnt++;

  if (_i_freeze_it)
    freeze = false;

  return( Pt_CONTINUE );
}

