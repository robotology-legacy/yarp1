/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "abimport.h"
#include "proto.h"
#include "motortester.h"

int upload(char *fname)
{
  sprintf(errmsg, "Not implemented yet!\n");
  ApError(ABW_base, NULL, "file_accept", errmsg,
    __FILE__);

#if 0  
  int stat;
  stat = meid_upload_firmware(meidx, fname);
  if (stat != 0)
    {
      sprintf(errmsg, "Failed to upload MEI firmware... Error code %d!\n", stat);
      ApError(ABW_base, NULL, "file_accept", errmsg,
	      __FILE__);
     }
  return(stat);
#endif
  return -1;
}

int download(char *fname)
{
  sprintf(errmsg, "Not implemented yet!\n");
  ApError(ABW_base, NULL, "file_accept", errmsg,
    __FILE__);

#if 0 
  int stat;
  stat = meid_download_firmware(meidx, fname);
  if (stat != 0)
    {
      sprintf(errmsg, "Failed to download MEI firmware... Error code %d!\n", stat);
      ApError(ABW_base, NULL, "file_accept", errmsg,
	      __FILE__);
     }
  stat = meid_checksum(meidx);
  return(stat);
#endif
  return -1;
}


int
file_accept( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  char *fname;
  char *buttonname;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  fname = read_text_from_widget(ABW_selection_input);
  buttonname = read_text_from_widget(ABW_file_accept_button);
  switch(buttonname[0])
    {
    case 's':
    case 'S': // "Save ROM to File"
      upload(fname);
      break;
    case 'B':
    case 'b': // "Burn File to ROM"
      download(fname);
      break;
    default:
      printf("unknown\n");
      break;
    }
  
  return( Pt_CONTINUE );
  
}

