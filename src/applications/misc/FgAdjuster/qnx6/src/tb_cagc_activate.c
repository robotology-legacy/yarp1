// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  tb_cagc_activate.c
// 
//     Description:  
// 
//         Version:  $Id: tb_cagc_activate.c,v 1.1 2004-01-21 20:24:27 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"
extern int cagc;

int
tb_cagc_activate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtBasicCallback_t * ptbcb;
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	ptbcb = (PtBasicCallback_t *)cbinfo->cbdata;	
	cagc = ptbcb->value;
	sendfginfo();

	return( Pt_CONTINUE );
}

