/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <dirent.h>
    

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "abimport.h"
#include "proto.h"

#define MAX_PROC 1000

void SoftUpdate(int x)
{
  DIR *dirp;
  struct dirent *direntp;
  char *str;
  int index = 1;
  int index2;
  int i;
  short int rec[MAX_PROC];
  char *items[2];
  char **bufs = NULL;
  PtArg_t args[1]; 
  short *num; 

  PtListDeleteAllItems(ABW_DummyList);
  dirp = opendir( "/home/cog/active" );
  if( dirp != NULL ) {
    for(;;) {
      direntp = readdir( dirp );
      if( direntp == NULL ) break;
      str = strstr(direntp->d_name,".o");
      if (str!=NULL)
	{
	  *str = '\0';
	  //printf( "%s\n", direntp->d_name );
	  items[0] = direntp->d_name;
	  index = PtListItemPos(ABW_RunningList,items[0]);
	  if (index == 0)
	    {
	      PtListAddItems(ABW_RunningList,items,1,0);
	      index2 = PtListItemPos(ABW_ActiveList,items[0]);	      
	      if (index2 != 0)
		{
		  PtListDeleteItemPos(ABW_ActiveList,1,index2);
		}
	      
	      index2 = PtListItemPos(ABW_ProcessList,items[0]);
	      if (index2 != 0)
		{
		  PtListDeleteItemPos(ABW_ProcessList,1,index2);
		}
	       
	    }
	  PtListAddItems(ABW_DummyList,items,1,0);
	  index++;
	}
    }

    PtSetArg(&args[0], Pt_ARG_ITEMS, &bufs, &num);
    PtGetResources(ABW_RunningList, 1, args);
    i = 0;
    while (i<*num)
      {
	index = PtListItemPos(ABW_DummyList,bufs[i]);
	if (index==0)
	  {
	    printf("i is %d %d\n", index, i);
	    PtListAddItems(ABW_ProcessList,bufs+i,1,0);
	    PtListDeleteItemPos(ABW_RunningList,1,i+1);
	    //items[0] = bufs[i];
	    PtSetArg(&args[0], Pt_ARG_ITEMS, &bufs, &num);
	    PtGetResources(ABW_RunningList, 1, args);
	  }
	else
	  {
	    i++;
	  }
      }

    closedir( dirp );
  }
}


int
TestButton_OnClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  SoftUpdate(1);
  
  return( Pt_CONTINUE );
}

