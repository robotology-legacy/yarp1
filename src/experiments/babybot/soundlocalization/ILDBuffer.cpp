// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #nat, Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  ILDBuffer.cpp
// 
//     Description: This class implements a buffer of level data items 
// 
//         Version:  $Id: ILDBuffer.cpp,v 1.1 2004-04-30 12:52:50 beltran Exp $
// 
//          Author:  Lorenzo Natale. YARP adaptation by Carlos Beltran
//         Company:  Lira-Lab
// 
// =====================================================================================

//#include "stdafx.h"
//#include "resource.h"
#include <YARPConfigFile.h>
#include "ILDBuffer.h"
#include <ace/config.h>
#include <ace/OS.h>
#include "math.h"
//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  CILDBuffer (Constructor)
// Description: Creates a buffer with one element. Initialy empty. 
//--------------------------------------------------------------------------------------
CILDBuffer::CILDBuffer()
{
	int i=0;
	global_valid=0;
	m_size=1;
	actual=0;

	pointer = new LEVEL_DATA [m_size];

	ACE_ASSERT(pointer!=NULL);
	for (i=0;i<m_size;i++) 
	{
		pointer[i].ild=0;
		pointer[i].left_level=0;
		pointer[i].right_level=0;
		pointer[i].valid=0;
	}

}	

//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  CILDBuffer
// Description:  Creates a buffer of dimension size with all the elements empty
//--------------------------------------------------------------------------------------
CILDBuffer::CILDBuffer(int size)
{
	int i=0;
	global_valid=0;
	
	//ACE_ASSERT (size>0);

	if (size>0)
	{	
		m_size=size;
		actual=0;
		pointer = new LEVEL_DATA [m_size];

		for (i=0;i<m_size;i++)
		{
			pointer[i].ild=0;
			pointer[i].left_level=0;
			pointer[i].right_level=0;
			pointer[i].valid=0;
		}

		ACE_ASSERT (pointer != NULL);
	}
	else
	{
		m_size=1;
		actual=0;
		pointer = new LEVEL_DATA [m_size];

		for (i=0;i<m_size;i++) 	
		{
			pointer[i].ild=0;
			pointer[i].left_level=0;
			pointer[i].right_level=0;
			pointer[i].valid=0;
		}
		ACE_ASSERT (pointer != NULL);
	}
}	

//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  ~CILDBuffer
// Description:  Destructor
//--------------------------------------------------------------------------------------
CILDBuffer::~CILDBuffer()
{
	delete [] pointer;
}

//--------------------------------------------------------------------------------------
//       Class: CILDBuffer 
//      Method: Resize(new_size) 
// Description: Change the buffer dimension. The previous values are lost. If the dimensions
// are not valit it returns 0 
//--------------------------------------------------------------------------------------
int CILDBuffer::Resize(int new_size)
{
	if (new_size < 1) return 0;
	
	delete [] pointer;
	m_size=new_size;

	actual=0;
	pointer = new LEVEL_DATA [m_size];

	if (pointer == NULL) 
			return 0;
	for (int i=0;i<m_size;i++) 	
	{
		pointer[i].ild=0;
		pointer[i].left_level=0;
		pointer[i].right_level=0;
		pointer[i].valid=0;
	}

	global_valid=0;

	return 1;
}

//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  PutItem(item)
// Description:  Adds an element to the buffer
//--------------------------------------------------------------------------------------
void CILDBuffer ::PutItem (LEVEL_DATA item)
{ 
	pointer [actual]=item;
	actual++;
	if (actual == m_size) actual=0;
}

//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  GetLastItem()
// Description:  Return the last item in the buffer 
//--------------------------------------------------------------------------------------
LEVEL_DATA CILDBuffer::GetLastItem ()
{
	if ((actual-1)>=0) return pointer[actual-1];
	else return pointer[m_size-1];
}

//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  GetMedia()
// Description:  Calculates and returns the arithmetic mean of the elements of the buffer 
//--------------------------------------------------------------------------------------
LEVEL_DATA CILDBuffer::GetMedia()
{ 
	int i;
	LEVEL_DATA tmp_data;
	tmp_data.ild=0;
	tmp_data.left_level=0;
	tmp_data.right_level=0;

	for (i=0;i<m_size;i++ )
	{
		tmp_data.ild=(tmp_data.ild+pointer[i].ild);
		tmp_data.left_level=(tmp_data.left_level+pointer[i].left_level);
		tmp_data.right_level=(tmp_data.right_level+pointer[i].right_level);
	}
	
	tmp_data.ild=tmp_data.ild/m_size;
	tmp_data.left_level=tmp_data.left_level/m_size;
	tmp_data.right_level=tmp_data.right_level/m_size;

	tmp_data.valid=global_valid;
	
	return tmp_data;
}

//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  GetModifiedMedia()
// Description:  Calculates the mean in the buffer's elements trying to maintain variations
// over 3
//--------------------------------------------------------------------------------------
LEVEL_DATA CILDBuffer::GetModifiedMedia()
{ 
	int i;
	LEVEL_DATA tmp_data;
	LEVEL_DATA last;
	tmp_data.ild=0;
	tmp_data.left_level=0;
	tmp_data.right_level=0;

	if (!global_valid)
		return tmp_data;

	last = GetLastItem();

	for (i=0;i<m_size;i++ )
	{
		tmp_data.ild=(tmp_data.ild+pointer[i].ild);
		tmp_data.left_level=(tmp_data.left_level+pointer[i].left_level);
		tmp_data.right_level=(tmp_data.right_level+pointer[i].right_level);
	}
	
	tmp_data.ild=tmp_data.ild/m_size;
	tmp_data.left_level=tmp_data.left_level/m_size;
	tmp_data.right_level=tmp_data.right_level/m_size;
	tmp_data.valid = 1;
	
	if (fabs(last.ild - tmp_data.ild) > 3)
	 	return last;
	else
		return tmp_data;
}

//--------------------------------------------------------------------------------------
//       Class:  CILDBuffer
//      Method:  SetGlobals(valid)
// Description:  
//--------------------------------------------------------------------------------------
void CILDBuffer::SetGlobals(int valid)
{
	global_valid=(valid) ? true:false;
}
