// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Lorenzo Natale, Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  corr_data.h
// 
//     Description:  Definition of the CORR_DATA structure
// 
//         Version:  $Id: corr_data.h,v 1.1 2004-04-30 12:52:50 beltran Exp $
// 
//          Author:  Lorenzo Natale, YARP adaptation by Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  nat@dist.unige.it; cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __CORR_DATA__
#define __CORR_DATA__

typedef struct  corrdata {
		double	itd;	// shift tra i due canali espresso in mSec
		double  shift;  // shift tra i due canali
		double	peak_level;
		int		valid;
		} CORR_DATA;

#endif
