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
///                            #bremen#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id:
///
///

#ifndef __YARPCONVKERNEL__
#define __YARPCONVKERNEL__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPImage.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

typedef int* p_int;

// la eredito da YARPFilter???
class YARPConvKernel
{
protected:
	IplConvKernel *filtro; // anche più filtri???
	
	// questi servono?????????
	// si potrebbero mettere delle routine inline che accedono
	// alle parti della struttura
	int nCols;
	int nRows;
	int centerX;
	int centerY;
	int nShiftR;

public:
	YARPConvKernel() { filtro=NULL; }
	YARPConvKernel(int cols, int rows, int anchorX, int anchorY, int* values, int shiftR);
	virtual ~YARPConvKernel();

	void Resize(int cols, int rows, int anchorX, int anchorY, int* values, int shiftR);
	
	/*void YARPConvKernel::Convolve2D(const YARPImageOf<YarpPixelMono> &srcImage,
									const YARPImageOf<YarpPixelMono> &dstImage,
									int nKernels,
									int combineMethod);*/

	void GetData(int &cols, int &rows, int &anchorX, int &anchorY, int &shiftR);
	
	void GetValues(int *values);

	int GetCols() { return nCols; }
	int GetRows() { return nRows; }
	int GetAnchorX() { return centerX; }
	int GetAnchorY() { return centerY; }
	int GetShiftR() { return nShiftR; }

	/*void SetBorderMode(YARPGenericImage &src,
										int mode,
										int border,
										int constVal);*/

	//long int Gain(int *data, int cols, int rows);

	IplConvKernel* GetPointer(void) const { return filtro; }
	
	virtual void Cleanup() {}
	virtual bool InPlace() const { return true; }
};

class YARPArrayConvKernel
{
protected:
	IplConvKernel **ipl_array;
	void _free();
	YARPConvKernel **kernels;
	int num;

	int rowsT;
	int rowsB;
	
	long int sum; //DC Gain
	long int *psumT;
	long int *psumB;

	IplConvKernel* mult;
public:

	void Resize(int n);

	YARPArrayConvKernel();
	YARPArrayConvKernel(int n);
	virtual ~YARPArrayConvKernel();

	void Set(int cols, int rows, int anchorX, int anchorY, int* values, int shiftR, int pos);
	void SetGaussianRow(int radius, double variance, double norm, int shiftR, int pos);
	void SetGaussianCol(int radius, double variance, double norm, int shiftR, int pos);

	//void Resize(int cols, int rows, int anchorX, int anchorY, int* values, int shiftR);
	
	void InitFixBorder();
	
	void Convolve2D(const YARPGenericImage &srcImage,
					const YARPGenericImage &dstImage,
					int nKernels,
					int combineMethod);

	void Convolve2D(const YARPGenericImage &srcImage,
					const YARPGenericImage &dstImage,
					int nKernel);
	
	void ConvolveSep2D(const YARPGenericImage &srcImage,
		               const YARPGenericImage &dstImage);


	//void YARPConvKernel::GetData(int &cols, int &rows, int &anchorX, int &anchorY, int &shiftR, int &gain);
	
	//void YARPConvKernel::GetValues(int *values);

	virtual void Cleanup() {}
	virtual bool InPlace() const { return true; }
};

#endif
