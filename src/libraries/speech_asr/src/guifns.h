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


#ifndef _GUIFNS_H
#define _GUIFNS_H

#ifdef __cplusplus
extern "C" {
#endif

extern void TurnMicOn    ( void );

extern int GuiNotifier ( int socket_handle, int ( * recv_fn ) ( void * ),
                         void * recv_data, void * client_data );

extern int GuiCreate ( int argc, char * * argv );

extern int GuiMainLoop ( );

extern int GuiDestroy ( );

extern int GuiInsertText ( char * text );

extern char * GuiAppName;

#ifdef __cplusplus
}
#endif 

#endif
