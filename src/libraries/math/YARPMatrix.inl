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
/// $Id: YARPMatrix.inl,v 1.2 2003-05-31 06:02:58 gmetta Exp $
///
///

// VisDMatrix.inl
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved


#ifndef __QNX__
#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif // _DEBUG

#else
	// QNX specific stuff here.
#endif


inline bool YVector::operator!=(const YVector& refvector) const
{
    return !(*this == refvector);
}

inline bool YMatrix::operator!=(const YMatrix& refmatrix) const
{
    return !(*this == refmatrix);
}

inline bool YVector::operator!=(double dbl) const
{
    return !(*this == dbl);
}

inline bool YMatrix::operator!=(double dbl) const
{
    return !(*this == dbl);
}

inline YMatrix& YMatrix::Invert(void)
{
	*this = Inverted();

	return *this;
}

inline YMatrix& YMatrix::Transpose(void)
{
	*this = Transposed();

	return *this;
}


//
// Self-describing stream output.
//
// LATER:  We should consider having both op<< and op>> with these streams.
//
#ifdef VIS_INCLUDE_SDSTREAM
inline CVisSDStream& operator<<(CVisSDStream& s, YVector& o)
	{ return VisSDReadWriteClass(s, o); }

inline CVisSDObject<YVector>& CVisSDObject<YVector>::Exemplar(void)
{
	// Global variable used with self-describing streams.
	// LATER:  Find a way to avoid using this global variable.
	extern VisMatrixExport CVisSDObject<YVector> g_visdvectorExemplar;

	return g_visdvectorExemplar;
}


inline CVisSDStream& operator<<(CVisSDStream& s, YMatrix& o)
	{ return VisSDReadWriteClass(s, o); }

inline CVisSDObject<YMatrix>& CVisSDObject<YMatrix>::Exemplar(void)
{
	// Global variable used with self-describing streams.
	// LATER:  Find a way to avoid using this global variable.
	extern VisMatrixExport CVisSDObject<YMatrix> g_visdmatrixExemplar;

	return g_visdmatrixExemplar;
}
#endif

#ifndef __QNX__
#ifdef _DEBUG
#undef new
#endif // _DEBUG
#endif
