// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Lorenzo Natale, Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  ITDBuffer.h
// 
//     Description:  CITDBuffer class definition
// 
//         Version:  $Id: ITDBuffer.h,v 1.2 2004-08-30 17:51:44 beltran Exp $
// 
//          Author:  Lorenzo Natale, YARP adaptation by Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  nat@dist.unige.it; cbeltran@dist.unige.it
// 
// =====================================================================================

#if !defined _ITDBUFFERH_
#define _ITDBUFFERH_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <yarp/YARPConfig.h>
#include "corr_data.h"

class CITDBuffer {
public:
	CITDBuffer();
	CITDBuffer(int size);
	~CITDBuffer();
	void PutItem (CORR_DATA item);
	void SetGlobals(int valid,double peak);
	CORR_DATA GetLastItem ();
	CORR_DATA GetMedia();
	CORR_DATA GetSimpleMedia();
	CORR_DATA GetModifiedMedia();
	CORR_DATA GetWeighedMedia();
	int Resize(int new_size);
	int GetSize() {
				   return m_size;
	};
	
private:
	int m_size;
	CORR_DATA* pointer;
	int actual;
	int global_valid;
	double global_peak;
};

#endif 
