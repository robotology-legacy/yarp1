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

#include <dirent.h>

void Setup()
{
  DIR *dirp;
  struct dirent *direntp;
  char *str;
  char *str2;
  int index = 1;

  char *items[2];

  dirp = opendir( "." );
  if( dirp != NULL ) {
    for(;;) {
      direntp = readdir( dirp );
      if( direntp == NULL ) break;
      str = strstr(direntp->d_name,".run");
      str2 = strstr(direntp->d_name,"~");
      if (str!=NULL && str2==NULL)
	{
	  *str = '\0';  // bad, bad thing to do
	  //printf( "%s\n", direntp->d_name );
	  items[0] = direntp->d_name;
	  PtListAddItems(ABW_ProcessList,items,1,1); //index);
	  index++;
	}
    }
    closedir( dirp );
  }
}

