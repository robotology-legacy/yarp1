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
#ifndef _DW_H
#define _DW_H


#ifndef TRUE
#define TRUE ( 1 )
#endif

#ifndef FALSE
#define FALSE ( 0 )
#endif

/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
#define CheckSmRC(fn)                                                      \
{                                                                          \
  int rc;                                                                  \
                                                                           \
  SmGetRc ( reply, & rc );                                                 \
                                                                           \
  if ( rc != SM_RC_OK )                                                    \
  {                                                                        \
    sprintf ( buffer, "%s: rc = %d", fn, rc );                             \
    LogMessage ( buffer );                                                 \
    exit(1);                                                               \
  }                                                                        \
}


/*------------------------------------------------------------------------*/
/*                                                                        */
/*------------------------------------------------------------------------*/
extern void LogMessage   ( char * );

extern void CloseUpShop  ( );

//extern void TurnMicOn    ( );

extern void PlayTags     ( );

extern void TurnMicOff   ( );


#endif
