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


#include <YARPImage.h>
#include <YARPMatrix.h>
#include "YARPConvKernel.h"

/*YMatrix YMatrix::operator*(const YMatrix& refmatrix) const
{
    YMatrix matrixRet(NRows(), refmatrix.NCols());

    assert(NCols() == refmatrix.NRows() &&
           NRows() == matrixRet.NRows() &&
           refmatrix.NCols() == matrixRet.NCols());

    for (int i = 0; i < NRows(); i++) {
        for (int j = 0; j < refmatrix.NCols(); j++) {
            double sum = 0.0;
            for (int k = 0; k < NCols(); k++)
                sum += (*this)[i][k] * refmatrix[k][j];
            matrixRet[i][j] = sum;
        }
    }

    return matrixRet;
}*/


void prodotto(int *vetA, int rowsA, int colsA, int *vetB, int colsB, int *ris)
{
    //YMatrix matrixRet(NRows(), refmatrix.NCols());
	// rowsA, colsB
	
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            int sum = 0;
            for (int k = 0; k < colsA; k++)
                sum += vetA[i*colsA+k] * vetB[k*colsB+j];
            ris[i*colsB+j] = sum;
        }
    }
}


void Transposed(int *matA, int rows, int cols, int *matB)
{
	int i, j, n = rows, m = cols;

	ACE_ASSERT(m > 0 && n > 0);

	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			matB[i*n+j] = matA[j*m+i];
}


//long int YARPConvKernel::Gain(int *data, int cols, int rows)
long int Gain(int *data, int cols, int rows)
{
	int i;
	long int somma=0;

	for (i=0; i<cols*rows; i++)
		somma+=data[i];

	return somma;
}


YARPConvKernel::YARPConvKernel(int cols, int rows, int anchorX, int anchorY, int* values, int ShiftR)
{
	filtro=NULL;
	Resize(cols, rows, anchorX, anchorY, values, ShiftR);
}


YARPConvKernel::~YARPConvKernel()
{
	if (filtro!=NULL)
		iplDeleteConvKernel(filtro);
	filtro=NULL;
}


void YARPConvKernel::Resize(int cols, int rows, int anchorX, int anchorY, int* values, int shiftR)
{
	nCols=cols;
	nRows=rows;
	centerX=anchorX;
	centerY=anchorY;
	nShiftR=shiftR;

	if (filtro!=NULL)
		iplDeleteConvKernel(filtro);

	filtro=iplCreateConvKernel(cols, rows, anchorX, anchorY, values, shiftR);
}


void YARPConvKernel::GetData(int &cols, int &rows, int &anchorX, int &anchorY, int &shiftR)
{
	cols=nCols;
	rows=nRows;
	anchorX=centerX;
	anchorY=centerY;
	shiftR=nShiftR;
}


// values deve essere già allocato
void YARPConvKernel::GetValues(int *values)
{
	for (int i=0; i<nRows*nCols; i++)
		values[i]=(filtro->values)[nRows*nCols-1-i];
}


void YARPArrayConvKernel::_free()
{
	// non controlla se kernels è != NULL
	for (int i=0; i<num; i++)
		delete kernels[i];
	delete [] kernels; // verificare se questo delete cancella i singoli oggetti
	delete [] ipl_array;
	if (rowsT!=0) delete [] psumT;
	if (rowsB!=0) delete [] psumB;
	kernels=NULL;
}


YARPArrayConvKernel::YARPArrayConvKernel()
{
	kernels=NULL;
	
	rowsT=0;
	rowsB=0;

	int temp=0;
	mult=iplCreateConvKernel(1, 1, 0, 0, &temp, 0);
}


YARPArrayConvKernel::YARPArrayConvKernel(int n)
{ 
	Resize(n);

	rowsT=0;
	rowsB=0;

	int temp=0;
	mult=iplCreateConvKernel(1, 1, 0, 0, &temp, 0);
}


YARPArrayConvKernel::~YARPArrayConvKernel()
{
	if (kernels!=NULL)
		_free();
}


void YARPArrayConvKernel::Resize(int n)
{
	if (kernels==NULL) {
		kernels = new YARPConvKernel* [n];
		ipl_array = new IplConvKernel* [n]; // devo inizializzarlo a NULL??
		num=n;
		for (int i=0; i<n; i++)
			kernels[i]=NULL;
	} else
		_free();
}


void YARPArrayConvKernel::Set(int cols, int rows, int anchorX, int anchorY, int* values, int shiftR, int pos)
{
	ACE_ASSERT(kernels[pos] == NULL);

	YARPConvKernel *tmp = new YARPConvKernel(cols, rows, anchorX, anchorY, values, shiftR);

	ipl_array[pos]=tmp->GetPointer();

	kernels[pos]=tmp;
}


void YARPArrayConvKernel::SetGaussianRow(int radius, double variance, double norm, int shiftR, int pos)
{
	ACE_ASSERT(kernels[pos] == NULL);
	ACE_ASSERT(norm <= 1);
	
	double sum=0;

	double *decimals = new double [2*radius+1];
	int *values = new int [2*radius+1];

	for (int i=0; i<2*radius+1; i++) {
		decimals[i]=exp(-(i-radius)*(i-radius)/(2*variance*variance));
		sum+=decimals[i];
	}

	for (i=0; i<2*radius+1; i++)
		values[i]=decimals[i]/sum*norm*(1<<shiftR)+.5;
		
	YARPConvKernel *tmp = new YARPConvKernel(2*radius+1, 1, radius, 0, values, shiftR);

	ipl_array[pos]=tmp->GetPointer();

	kernels[pos]=tmp;

	delete [] decimals;
	delete [] values;
}


void YARPArrayConvKernel::InitFixBorder()
{
	// somme parziali

	if (num==1) {
		int m;
		int rows=kernels[0]->GetRows();
		int cols=kernels[0]->GetRows();

		// in realtà dovrebbe considerare la posizione del centro
		rowsB = (rows-1)/2;
		rowsT = (rows-1)/2;
		
		psumT = new long int[rowsB];
		psumB = new long int[rowsT];

		int *values = new int [rows*cols];
		
		kernels[0]->GetValues(values);

		sum=Gain(values, rows, cols);
		
		for (m = 0; m<rowsT; m++)
			psumT[m]=Gain(values, rows-(rowsT-m), cols);
		for (m = 0; m<rowsB; m++)
			psumB[m]=Gain(values+cols*(m+1), rows-(m+1), cols);

		// 31 preciso, 30 come margine
		mult->nShiftR=30-kernels[0]->GetShiftR();
				
		delete [] values;

	} else if (num==2) {
		int m;
		int rows0=kernels[0]->GetRows();
		int cols0=kernels[0]->GetCols();
		
		int rows1=kernels[1]->GetRows();
		int cols1=kernels[1]->GetCols();
		
		ACE_ASSERT(rows0==1 && cols1==1 && rows1==cols0);

		int *values0 = new int [rows0*cols0];
		int *values1 = new int [rows1*cols1];
		
		kernels[0]->GetValues(values0);
		kernels[1]->GetValues(values1);

		int *prod = new int [rows1*cols0];

		prodotto(values0, rows1, cols1, values1, cols0, prod);

		rowsT=(rows1-1)/2;
		rowsB=(rows1-1)/2;
		
		psumT = new long int[rowsT];
		psumB = new long int[rowsB];

		sum=Gain(prod, rows1, cols0);
		
		// sono invertiti perchè la maschera viene invertita durante la convoluzione
		for (m = 0; m<rowsT; m++)
			psumT[m]=Gain(prod, rows1-(rowsT-m), cols0);
		for (m = 0; m<rowsB; m++)
			psumB[m]=Gain(prod+cols0*(rowsB-m), rows1-(rowsB-m), cols0);

		// 31 preciso, 30 come margine
		mult->nShiftR=30-kernels[0]->GetShiftR()-kernels[1]->GetShiftR();
		
		delete [] prod;
		delete [] values0;
		delete [] values1;

	} else
		// error
		return;
}


// ingresso e uscita dovrebbero essere di tipo generic???
void YARPArrayConvKernel::Convolve2D(const YARPGenericImage &srcImage,
								const YARPGenericImage &dstImage,
								int nKernels,
								int combineMethod)
{
	/*ACE_ASSERT((IplImage *)srcImage != NULL);
	ACE_ASSERT((IplImage *)dstImage != NULL);
	ACE_ASSERT(kernels != NULL);*/

	iplConvolve2D(srcImage, dstImage, ipl_array, nKernels, combineMethod);
}


void YARPArrayConvKernel::Convolve2D(const YARPGenericImage &srcImage,
								const YARPGenericImage &dstImage,
								int nKernel)
{
	/*ACE_ASSERT((IplImage *)srcImage != NULL);
	ACE_ASSERT((IplImage *)dstImage != NULL);
	ACE_ASSERT(kernels != NULL);*/

	iplConvolve2D(srcImage, dstImage, ipl_array+nKernel, 1, IPL_SUM);
}


// ingresso e uscita dovrebbero essere di tipo generic???
void YARPArrayConvKernel::ConvolveSep2D(const YARPGenericImage &srcImage,
										const YARPGenericImage &dstImage)
{
	int n;
	IplROI zdi;
	IplROI *pOldZdi;

	//NOTA lo shift totale delle due maschere non può essere 24 o più
	// è probabile che per questioni di velocità lo shift venga fatto tutto in una volta
	// alla fine delle due convoluzioni
	
	ACE_ASSERT((IplImage *)srcImage != NULL);
	ACE_ASSERT((IplImage *)dstImage != NULL);
	ACE_ASSERT(kernels != NULL);
	ACE_ASSERT(num>=2);
	ACE_ASSERT(kernels[0] != NULL);
	ACE_ASSERT(kernels[1] != NULL);
	
	YARPGenericImage tmp;
	tmp.SetID(srcImage.GetID());
	//YARPImageOf<YarpPixelMono> tmp;
	tmp.Resize(srcImage.GetWidth(), srcImage.GetHeight());
		
	//iplConvolveSep2D(IplImage* srcImage,IplImage* dstImage, IplConvKernel* xKernel,IplConvKernel* yKernel);
	//iplConvolveSep2D((IplImage *)srcImage, (IplImage *)dstImage, ipl_array[0], ipl_array[1]);

	// in questo modo evito il problema di prima, ma dovrebbe essere più lento
	iplConvolveSep2D((IplImage *)srcImage, (IplImage *)tmp, ipl_array[0], NULL);
	iplConvolveSep2D((IplImage *)tmp, (IplImage *)dstImage, NULL, ipl_array[1]);

	zdi.coi=0;
	zdi.xOffset=0;
	zdi.width=dstImage.GetWidth();
	zdi.height=1;
	
	pOldZdi=((IplImage *)dstImage)->roi;
	((IplImage *)dstImage)->roi=&zdi;
	
	// top
	for (n=0;n<rowsT;n++) {
		zdi.yOffset=n;
		*(mult->values)=sum*(1<<(mult->nShiftR))/psumT[n];
		// forse va più veloce usando la convoluzione separabile
		iplConvolve2D((IplImage *)dstImage, (IplImage *)dstImage, &mult, 1, IPL_SUM);
	}

	// bottom
	for (n=0;n<rowsB;n++) {
		zdi.yOffset=dstImage.GetHeight()-1-n;
		*(mult->values)=sum*(1<<(mult->nShiftR))/psumB[n];
		iplConvolve2D((IplImage *)dstImage, (IplImage *)dstImage, &mult, 1, IPL_SUM);
	}
	
	//for (n=0;n<(rows-1)/2;n++)
	//alMultiplyF(tmpMapC, srcC->height-(msk->righeC-1)/2+n, (float)msk->sommeC0/msk->sommeC_M_B[n]);

	((IplImage *)dstImage)->roi=pOldZdi;
}
