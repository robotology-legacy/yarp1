// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Lorenzo Natale, Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  ILDBuffer.h
// 
//     Description:  
// 
//         Version:  $Id: ILDBuffer.h,v 1.1 2004-04-30 12:52:50 beltran Exp $
// 
//          Author:  Lorenzo Natale. YARP adaptation by Carlos Beltran 
//         Company:  Lira-Lab
//           Email:  nat@dist.unige.it; cbeltran@dist.unige.it
// 
// =====================================================================================

#if !defined _ILDBUFFERH_
#define _ILDBUFFERH_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <conf/YARPConfig.h>
#include "lev_data.h"

class CILDBuffer {
public:
	CILDBuffer();
	CILDBuffer(int size);
	~CILDBuffer();
	void PutItem (LEVEL_DATA item);
	LEVEL_DATA GetLastItem ();
	LEVEL_DATA GetModifiedMedia();
	LEVEL_DATA GetMedia();
	int Resize(int new_size);
	int GetSize(){
			    	return m_size;
	}
	void SetGlobals(int valid);
		
private:
	int m_size;
	LEVEL_DATA* pointer;
	int actual;
	bool global_valid;
};

#endif 
