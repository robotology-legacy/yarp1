/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPVector.cpp,v 1.1 2004-07-06 13:56:08 eshuy Exp $
///
///

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  VisMatrix.cpp -- implementation of matrix/vector ops
//
// DESCRIPTION
//  The YMatrix class provides some basic matrix operations, using calls
//  to external software (IMSL for the moment) to perform the more
//  complicated operations.
//
// SEE ALSO
//  YMatrix.h        more complete description
//
// BUGS
//  I sure hope that the IMSL routines don't modify their input
//  parameters!
//
// DESIGN
//  Right now, we call out to IMSL to perform the linear systems
//  solving.  We could change this later.
//
//  To use IMSL, include the following in your 
//  Build Settings Link Object/Library modules:
//      mathd.lib libf.lib
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// Modified by PASA. 
//	see VisDMatrix.h
//
// 
///////////////////////////////////////////////////////////////////////////

#include <yarp/YARPVector.h>
#include <string.h>


//  IMSL routines used (double precision)


// LATER:  The vector and matrix arithmetic operators assume that both
// arguments are of the same size.  We could easily change many of these
// operators to do the right thing when the arguments are not the same
// size.  (For example, adding a 3-vector to a 4-vector.)

#ifdef __QNX6__
#define UNUSED(x) do {} while (&x == 0)
#else
#define UNUSED(x)
#endif

//
// Constructors (default destructor and copy constructor)
//

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        YVector::YVector
//  
//  DECLARATION:
//          YVector::YVector(int length, double *storage);
//  
//  INPUT:
//      length (int) - length of vector
//  
//      *storage (double) - optional storage location (will not dealloca
//                  te on destruction)
//  
//  DISCRIPTION:
//      Create a dynamically sized double precision vector
//  
////////////////////////////////////////////////////////////////////////////
void YVector::Resize(int length, const double *storage)
{
    m_length = length;
	if (m_data != NULL)
		delete[] m_data;

	m_data = new double[m_length];
	if (storage != 0)
	{
		memcpy (m_data, storage, sizeof(double) * m_length);
	}
	else
	{
		memset (m_data, 0, sizeof(double) * m_length);
	}
}

YVector::YVector(int length, const double *storage) :  m_length(0), m_data(0)
{
    Resize(length, storage);
}

YVector::YVector(void)
  : m_length(0), m_data(0)
{
}

YVector::YVector(const YVector &refvector) :  m_length(0), m_data(0)
{
    //Resize(refvector.Length());
	*this = refvector;
}

YVector::~YVector ()
{
	if (m_data != NULL)
		delete[] m_data;
}




////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          YVector& YVector::operator=(const YVector &vec);
//  
//  RETURN VALUE:
//      vector being copied
//  INPUT:
//      &vec (const YVector) - vector being copied
//  
//  DISCRIPTION:
//      assignment operator
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::operator=(const YVector &vec)
{
	if (Length() != vec.Length())
		Resize(vec.Length());
    memcpy(m_data, vec.m_data, m_length*sizeof(double));
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          YVector& YVector::operator=(double value);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//  INPUT:
//      value (double) - fill value
//  
//  DISCRIPTION:
//      Fill vector with constant value
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::operator=(double value)
{
	assert (Length() != 0);
    if (value == 0.0)   // IEEE float
        memset(m_data, 0, m_length*sizeof(double));
    else
        for (int i = 0; i < m_length; i++)
            m_data[i] = value;
    return *this;
}




//
//  Matrix / vector products, dot product
//

YVector& YVector::operator+=(const YVector& refvector)
{
    int n = refvector.Length(); 
    assert(n == Length()); 

    for (int i=0; i<n; i++)
        (*this)[i] += refvector[i];

    return *this; 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+=
//  
//  DECLARATION:
//          YVector& YVector::operator+=(double dbl);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//
//  INPUT:
//      dbl (double) - add value
//  
//  DISCRIPTION:
//      Sum and assign operator
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::operator+=(double dbl)
{
	int n = Length();
    assert(n != 0); 

    for (int i=0; i<n; i++)
        (*this)[i] += dbl;

    return *this; 
}
