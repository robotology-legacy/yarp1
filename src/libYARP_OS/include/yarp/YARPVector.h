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
/// $Id: YARPVector.h,v 1.2 2004-07-09 13:45:59 eshuy Exp $
///
///

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  YARPMatrix.h -- double precision matrix/vector operations
//
// DESCRIPTION
//  The YMatrix class provides some basic matrix operations, using calls
//  to external software (IMSL for the moment) to perform the more
//  complicated operations.
//
//  To take advantage of the IMSL numerical analysis routines,
//  define VIS_USE_IMSL in the Build Settings C/C++ property page.
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
// Modifiche di PASA. 
//
// OK --> 1) Aggiunta operatori del tipo double op Matrix/Vector.
// OK --> 1.1) Aggiunta operatori del tipo op= double e somma con double.
// 2) Supporto least square (copiate da NR). ----->>Da completare<<-----
// 2.bis) Supporto QR decomposition.
// OK --> 3) Operatore () con indici che partono da 1.
// OK --> 4) Operatori cos e sin (solo per vettori).
// 5) moltiplicazione vettore * matrice.
// 6) allocazione automatica su op= vettore/matrice non allocata (da testare). 
//
// 
///////////////////////////////////////////////////////////////////////////

// Readapted by pasa on Dec 2000
// Avoid MFC.
// Dll-> Static library.
// Modified class names,
// 
 
#ifndef __VIS_MATRIX_DMATRIX_H__
#define __VIS_MATRIX_DMATRIX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <yarp/YARPConfig.h>

#include <iostream>

#include <math.h>
#include <assert.h>

//#define VISAPI 
//#define VisMatrixExport

//#ifndef _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC
//#endif

// UNDONE: Move inline functions to an INL file.


// LATER:  Could we use std::vector internally or derive from it?  (Would
// the data be adjacent in memory and the operators be as efficient?)


// LATER:  Should there be options to initialize elements when creating or
// resizing?



// QNX Specific.
#ifdef __QNX4__
#include "YARPSafeNew.h"
#include "YARPBool.h"
#endif

////////////////////////////////////////////////////////////////////////////
//  
//  CLASS:
//      YVector
//  
////////////////////////////////////////////////////////////////////////////
class YVector
{
public:
    // Constructors (default destructor and copy constructor)
    YVector(void);
    YVector(int length, const double *storage = 0);
    YVector(const YVector &vec);
	virtual ~YVector ();

    // Vector length
    int Length(void) const { return m_length; }
    void Resize(int length, const double *storage = 0);

    // Element access
    double& operator[](int i) { return m_data[i]; }
    const double& operator[](int i) const { return m_data[i]; }

	double& operator()(int i) { return m_data[--i]; }
	const double& operator()(int i) const { return m_data[--i]; }

    // Assignment
    YVector& operator=(const YVector &vec);
    YVector& operator=(double value);

	// We define operator < so that this class can be used in STL containers.
	bool operator==(const YVector& refvector) const;
	bool operator!=(const YVector& refvector) const;
	bool operator==(double dbl) const;
	bool operator!=(double dbl) const;
	bool operator<(const YVector& refvector) const;
	
	YVector operator+(const YVector &b) const;

	YVector operator-(void) const;
	YVector operator-(const YVector &b) const;

	YVector operator*(double dbl) const;
	double operator*(const YVector &b) const;   // dot product

	// norm 2.
	double norm2(void) const;
	double norm2square(void) const;

	// assume vector is row and multiply.
	//YVector operator*(const YMatrix &A) const;

	YVector operator/(double dbl) const;

	YVector& operator+=(const YVector &b);
	YVector& operator-=(const YVector &b);

	YVector& operator+=(double dbl);
	YVector& operator-=(double dbl);
	YVector operator+(double dbl) const;
	YVector operator-(double dbl) const;

	YVector& operator*=(double dbl);
	YVector& operator/=(double dbl);

	YVector& cos(void);
	YVector& sin(void);

    // Self-describing input/output format
    //enum FieldType { eftName, eftLength, eftData, eftEnd};
    //const char *ReadWriteField(CVisSDStream& s, int field_id);

	// Flag used with the std::ostream file I/O methods.
	// This may not be supported in future releases.
    static bool s_fVerboseOutput; // print out dimensions on output

	inline double * data(void) { return m_data; }

	inline const double * data(void) const {return m_data; }

private:
    int m_length;       // number of elements
    double *m_data;     // pointer to data
    //CVisMemBlockOf<double> m_memblockStorage;  // reference counted storage
};



#endif

