// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Nat, Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  ITDBuffer.cpp
// 
//     Description:  Implementation of the CITDBuffer class 
// 
//         Version:  $Id: ITDBuffer.cpp,v 1.2 2004-08-30 17:51:44 beltran Exp $
// 
//          Author:  Lorenzo Natale. YARP adaptation from Carlos Beltran
//         Company:  Lira-Lab
// 
// =====================================================================================

//#include "stdafx.h"
//#include "resource.h"
#include "ITDBuffer.h"
#include <yarp/YARPConfigFile.h>
#include <ace/config.h>
#include <ace/OS.h>
#include "math.h"

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  CITDBuffer
// Description:  Creates a buffer with one empty element
//--------------------------------------------------------------------------------------
CITDBuffer::CITDBuffer()
{
	m_size=1;
	actual=0;
	global_peak=0;
	global_valid=0;

	pointer = new CORR_DATA [m_size];

	ACE_ASSERT(pointer!=NULL);
	for (int i=0;i<m_size;i++) 
	{
			pointer[i].itd=0;
			pointer[i].shift=0;
			pointer[i].peak_level=0;
			pointer[i].valid=0;
	}

}	

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  CITDBuffer(size)
// Description:  Creates a buffer of dimension size with all elements empty
//--------------------------------------------------------------------------------------
CITDBuffer::CITDBuffer(int size)
{
	int i=0;
	
	//ACE_ASSERT (size>0);

	if (size>0)
	{	
		m_size=size;
		actual=0;
		pointer = new CORR_DATA [m_size];

		ACE_ASSERT (pointer!=NULL);
		for (i=0;i<m_size;i++) 
		{
			pointer[i].itd=0;
			pointer[i].shift=0;
			pointer[i].peak_level=0;
			pointer[i].valid=0;
		}

	}
	else
	{
		m_size=1;
		actual=0;
		pointer = new CORR_DATA [m_size];

		ACE_ASSERT (pointer!=NULL);
		for (i=0;i<m_size;i++) 
		{
			pointer[i].itd=0;
			pointer[i].shift=0;
			pointer[i].peak_level=0;
			pointer[i].valid=0;
		}
	}

	global_peak=0;
	global_valid=0;
}	

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  ~CITDBuffer 
// Description:  Destructor
//--------------------------------------------------------------------------------------
CITDBuffer::~CITDBuffer()
{
	delete [] pointer;
}

//////////////////////////////////////////////////////////////////////
// Resize(new_size)
//
// Cambia le dimensioni del buffer. I valori precedenti non sono 
// mantenuti. Se le dimensioni non sono valide, ritorna 0
//
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  Resize(new_size)
// Description:  Change the buffer dimension to the new_size dimension. Previous values in
// the buffer alre lost. If the new dimension is not valid return a 0.
//--------------------------------------------------------------------------------------
int CITDBuffer::Resize(int new_size)
{
	if (new_size < 1) return 0;
	
	delete [] pointer;
	m_size=new_size;

	actual=0;
	pointer = new CORR_DATA [m_size];
	if (pointer == NULL) 
			return 0;
	
	for (int i=0;i<m_size;i++) 
	{
			pointer[i].itd=0;
			pointer[i].shift=0;
			pointer[i].peak_level=0;
			pointer[i].valid=0;
	}


	global_peak=0;
	global_valid=0;

	return 1;
}

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  PutItem(item)
// Description:  Adds a new item into the buffer
//--------------------------------------------------------------------------------------
void CITDBuffer ::PutItem (CORR_DATA item)
{ 
	pointer [actual].itd=item.itd;
	pointer [actual].shift=item.shift;
	pointer [actual].peak_level=item.peak_level;
	pointer [actual].valid=item.valid;

	actual++;
	
	if (actual == m_size) actual=0;
}

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  GetLastItem()
// Description:  Returns the last item of the buffer
//--------------------------------------------------------------------------------------
CORR_DATA CITDBuffer::GetLastItem ()
{
	if ((actual-1)>=0) return pointer[actual-1];
	else return pointer[m_size-1];
}

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  GetSimpleMedia()
// Description:  Calculates the mean of the elements of the buffer. The values of the valid
// and peak_level are the same that those global
//--------------------------------------------------------------------------------------
CORR_DATA CITDBuffer::GetSimpleMedia()
{ 
	int i;
	CORR_DATA tmp;
	tmp.itd=0;
	tmp.shift=0;
	tmp.peak_level=0;
	tmp.valid=0;

	for (i=0;i<m_size;i++ )
	{
		tmp.itd=tmp.itd+pointer[i].itd;
		tmp.shift=tmp.shift+pointer[i].shift;
	}

	tmp.itd=(tmp.itd)/m_size;
	tmp.shift=(tmp.shift)/m_size;
	
	tmp.peak_level=global_peak;
	tmp.valid=global_valid;
	
	return tmp;
}

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  GetMedia()
// Description:  Calculates the mean of the buffer's elements using only the last valid 
// values
//--------------------------------------------------------------------------------------
CORR_DATA CITDBuffer::GetMedia()
{ 
	int i;
	CORR_DATA tmp;
	tmp.itd=0;
	tmp.shift=0;
	tmp.peak_level=0;
	tmp.valid=0;

	int n = 0;

	for (i=0;i<m_size;i++ )
	{
		if (pointer[i].valid != 0)
		{
			tmp.itd=tmp.itd+pointer[i].itd;
			tmp.shift=tmp.shift+pointer[i].shift;
			n++;
		}
		
	}

	if (n>(m_size/3))
	{
		tmp.itd=(tmp.itd)/n;
		tmp.shift=(tmp.shift)/n;
		tmp.valid = 1;
	}
	else
		tmp.valid = 0;
	
	return tmp;
}

//////////////////////////////////////////////////
// Calcola la media dell'intero buffer; i valori
// di peak level e valid sono uguali a quelli 
// globali. La procedura di calcolo cerca di non 
// eliminare le variazioni al di sopra di una soglia
// di 3
// 

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  GetModifiedMedia()
// Description:  Calculates the mean of the buffer's elements; the values of the peak level and
// valid are the same that those global. The algorithm tries not to eliminate the variations
// over a threshold of 3
//--------------------------------------------------------------------------------------
CORR_DATA CITDBuffer::GetModifiedMedia()
{
	int i;
	CORR_DATA tmp;
	CORR_DATA last;
	tmp.itd=0;
	tmp.shift=0;
	tmp.peak_level=0;
	tmp.valid=0;

	if (!global_valid)
		return tmp;

	last = GetLastItem();

	for (i=0;i<m_size;i++ )
	{
		tmp.itd=tmp.itd+pointer[i].itd;
		tmp.shift=tmp.shift+pointer[i].shift;
		tmp.peak_level = tmp.peak_level+pointer[i].peak_level;
	}

	tmp.itd=tmp.itd/m_size;
	tmp.shift=tmp.shift/m_size;
	tmp.peak_level=tmp.peak_level/m_size;
	tmp.valid = 1;
	
	if (fabs(last.shift - tmp.shift) > 3)
	 	return last;
	else
	return tmp;
}

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  GetWeighedMedia()
// Description:  Calculates the weighed mean of the buffer's elements. This has never
// been really working (note from Lorenzo)
//--------------------------------------------------------------------------------------
CORR_DATA CITDBuffer::GetWeighedMedia()
{ 
	int i;
	CORR_DATA tmp;
	tmp.itd=0;
	tmp.peak_level=0;
	tmp.valid=0;

	for (i=0;i<m_size;i++ )
	{
		tmp.itd=tmp.itd+pointer[i].itd;
		tmp.peak_level=tmp.peak_level+pointer[i].peak_level;
		tmp.valid=(tmp.valid)||(pointer[i].valid);
	}

	tmp.itd=(tmp.itd)/m_size;
	tmp.peak_level=(tmp.peak_level)/m_size;
	
	return tmp;
}

//--------------------------------------------------------------------------------------
//       Class:  CITDBuffer
//      Method:  SetGloblas(valid, peak)
// Description:  Set the globals values
//--------------------------------------------------------------------------------------
void CITDBuffer::SetGlobals(int valid, double peak)
{
	global_valid=valid;
	global_peak=peak;
}
