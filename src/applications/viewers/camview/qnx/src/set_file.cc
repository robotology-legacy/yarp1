/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.14C */

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
#include "cogcam.h"
#include "abimport.h"
#include "proto.h"

char global_filename[200];

char *read_text_from_widget(PtWidget_t *widget)
{
  PtArg_t args[1];
  char *str;
  
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &str, 0);
  PtGetResources(widget, 1, args);
  return(str);
}


int
set_selection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtFileSelItem_t *item;
  char *selection_name;
  PtArg_t args[2];
  int len;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  item = PtFSGetCurrent(ABW_file_selector);
  if (item == NULL)
    selection_name = "";
  else
    selection_name = item->fullpath;
  
//  printf("selection name <%s>\n", selection_name);
  len = strlen(selection_name);
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, selection_name, len);
  PtSetArg(&args[1], Pt_ARG_CURSOR_POSITION, len-1, 0);
  PtSetResources(ABW_filename_input, 2, args);
  
  strcpy(global_filename, selection_name);
  
  return( Pt_CONTINUE );
}


int
change_selection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  char *newfile;
  PtArg_t args[1];
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  newfile = read_text_from_widget(ABW_filename_input);
  
  // printf("new file <%s>\n", newfile);
  strcpy(global_filename, newfile);

  PtSetArg(&args[0], Pt_ARG_FS_ROOT_DIR, newfile, 0);
  PtSetResources(ABW_file_selector, 1, args);
  
  return( Pt_CONTINUE );
}


int
change_filter( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  char *newfilter;
  PtArg_t args[2];
  PtFileSelItem_t *rootitem;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  newfilter = read_text_from_widget(ABW_filter_input);
  // printf("new filter <%s>\n", newfilter);
  
  rootitem = PtFSRootItem(ABW_file_selector);
  
  PtSetArg(&args[0], Pt_ARG_FS_FILE_SPEC, newfilter, 0);
  PtSetArg(&args[1], Pt_ARG_FS_REFRESH, rootitem, 0);
  PtSetResources(ABW_file_selector, 2, args);
  
  return( Pt_CONTINUE );
  
}


int
realized( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  	PtArg_t args[2];

//	cout << "set filename to : " << global_filename << endl;
  	int len = strlen(global_filename);
  	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, global_filename, len);
  	PtSetArg(&args[1], Pt_ARG_CURSOR_POSITION, len-1, 0);
  	PtSetResources(ABW_filename_input, 2, args);
  
	return( Pt_CONTINUE );

	}


int
unrealized( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  	char *newfile = read_text_from_widget(ABW_filename_input);
//	cout << "unrealized : " << newfile << endl;
  	strcpy(global_filename, newfile);

	return( Pt_CONTINUE );

	}

