/*/////////////////////////////////////////////////////////////////////////////
//===========================================================================//
// COPYRIGHT:                                                                //
// ----------                                                                //
// Copyright (C) International Business Machines Corp., 2000.                //
//                                                                           //
// DISCLAIMER OF WARRANTIES:                                                 //
// -------------------------                                                 //
// The following [enclosed] code provided in source format is created by     //
// IBM Corporation.  This code is provided to you solely for                 //
// the purpose of assisting you in the development of your                   //
// applications.  This code is provided "AS IS", without warranty            //
// of any kind.  IBM shall not be liable for any damages arising             //
// out of your use of the code, even if they have been                       //
// advised of the possibility of such damages.                               //
//===========================================================================//
/////////////////////////////////////////////////////////////////////////////*/
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <unistd.h>

#include <gram.h>

#include <guifns.h>

char * GuiAppName = "AsciiDw";

/*----------------------------------------------------------------------------*/
/* The reco engine sends us notification of pending events via this socket.   */
/* when data is ready on it, the smapi_fn should be called and passed the     */
/* contents of smapi_data (see GuiMainLoop below)                             */
/*----------------------------------------------------------------------------*/


static int smapi_socket = 0;
static int ( * smapi_fn ) ( void * ) = NULL;
static void * smapi_data = NULL;


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
int GuiNotifier ( int socket_handle, int ( * recv_fn ) ( void * ),
                  void * recv_data, void * client_data )
{
  smapi_socket = socket_handle;
  smapi_fn     = recv_fn;
  smapi_data   = recv_data;

  return ( 0 );
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
int GuiCreate ( int argc, char * * argv )
{
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
int GuiDestroy ( )
{
  exit ( 0 );
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
int GuiMainLoop ( )
{
  fd_set rfds;
  int    retval;
  int    mic;
  char   buffer [ 255 ];
  char * cp;
  int    rc;
  int    n;

  mic = FALSE;

  n = smapi_socket + 1;

  /*--------------------------------------------------------------------------*/
  /* Loop forever over the socket that SMAPI passed in and stdio, processing  */
  /* whatever comes in on either...                                           */
  /*--------------------------------------------------------------------------*/

  //printf ( "*** Turning mic on\n" );
  fflush(stdout);
  TurnMicOn ( );


  while ( 1 )
  {
    FD_ZERO ( & rfds );
    FD_SET ( 0, & rfds );
    FD_SET ( smapi_socket, & rfds );

    retval = select ( n, & rfds, NULL, NULL, ( struct timeval * ) NULL );

    if ( retval < 0 )
    {
      printf ( "Retval is %d, %d\n", retval, errno );
    }

    /*------------------------------------------------------------------------*/
    /* Check to see whether something is on stdin.                            */
    /*------------------------------------------------------------------------*/

    /*
    if ( FD_ISSET ( 0, & rfds ) )
    {
      cp = gets ( buffer );

      switch ( buffer [ 0 ] )
      {
        case 'f':
        case 'F':
             printf ( ">> Turning mic off\n" );
             TurnMicOff ( );
             break;

        case 'n':
        case 'N':
             printf ( ">> Turning mic on\n" );
             TurnMicOn ( );
             break;

        case 'q':
        case 'Q':
             printf ( ">> Quitting\n" );
             CloseUpShop ( );
             break;

        default:
      }
    }
    */

    /*------------------------------------------------------------------------*/
    /* Look to see if anything came in from the reco engine.  If so, then     */
    /* call the function that smapi passed in (smapi_fn) with its data        */
    /*------------------------------------------------------------------------*/
    if ( FD_ISSET ( smapi_socket, & rfds ) )
    {
      //      printf ( ">> Incoming message\n" );
      rc = ( smapi_fn ) (smapi_data );
      if ( rc != 0 )
      {
        printf ( ">>  Return code is %d\n", rc );
      }
    }
  }
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
int GuiInsertText ( char * text )
{
  printf ( "<< Reco Text = '%s'\n", text );
}
