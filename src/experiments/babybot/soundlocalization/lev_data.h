// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Lorenzo Natale, Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  lev_data.h
// 
//     Description:  LEVEL_DATA structure definition
// 
//         Version:  $Id: lev_data.h,v 1.1 2004-04-30 12:52:50 beltran Exp $
// 
//          Author:  Lorenzo Natale, YARP adaptation by Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  nat@dist.unige.it; cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __LEVEL_DATA__
#define __LEVEL_DATA__

typedef struct  leveldata {
		double	ild;
		double  left_level;
		double	right_level;
		int		valid;
		} LEVEL_DATA;

#endif
