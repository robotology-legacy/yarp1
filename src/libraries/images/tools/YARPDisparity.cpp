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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPDisparity.cpp,v 1.19 2004-04-26 10:26:29 babybot Exp $
///
///

// disparity.cpp: implementation of the YARPDisparityTool class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPDisparity.h"
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
#define NR_END 1
#define FREE_ARG char*
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPDisparityTool::YARPDisparityTool()
{
	char *yarproot = GetYarpRoot();
	ACE_OS::sprintf(_path,"%s/%s/\0", yarproot, "conf");
//	ACE_OS::sprintf(_path,"%s","c:/temp/tables/prefix/");

	_dsTable		= NULL;
	_shiftFunction	= NULL;
	_gaussFunction	= NULL;
	_shiftMap		= NULL;
	_corrFunct		= NULL;
	_actRings		= 21;
	_corrTreshold   = 2.0;
	_count			= NULL;
}

YARPDisparityTool::~YARPDisparityTool()
{
	if (_shiftFunction != NULL)
		delete [] _shiftFunction;

	if (_gaussFunction != NULL)
		delete [] _gaussFunction;

	if (_shiftMap != NULL)
		delete [] _shiftMap;

	if (_dsTable->position != NULL)
		delete [] _dsTable->position;

	if (_dsTable->weight != NULL)
		delete [] _dsTable->weight;

	if (_dsTable != NULL)
		delete [] _dsTable;

	if (_corrFunct != NULL)
		delete [] _corrFunct;

	if (_count != NULL)
		delete [] _count;
}

int YARPDisparityTool::loadShiftTable(Image_Data * Par)
{
	char File_Name[256];
	FILE * fin;

	int j,k;

//	Build_Step_Function(_path,Par);

	if (Par->Ratio == 1.00)
		ACE_OS::sprintf(File_Name,"%s%s",_path,"StepList.gio");
	else
		ACE_OS::sprintf(File_Name,"%s%1.2f_%s",_path,Par->Ratio,"StepList.gio");

	if ((fin = ACE_OS::fopen(File_Name,"rb")) != NULL)
	{
		ACE_OS::fread (&_shiftLevels,sizeof(int),1,fin); //Lenght of the list
		_shiftFunction = new int [_shiftLevels];
		ACE_OS::fread (_shiftFunction,sizeof(int),_shiftLevels,fin); //List
		ACE_OS::fclose(fin);
	}
	else
	{
		int mult = 1;
		_shiftLevels = 1+6*Par->Resolution/(4*mult);
		_shiftFunction = new int [_shiftLevels];
		for (j=0; j<mult*_shiftLevels; j+=mult)
			_shiftFunction[j/mult] = j-mult*(_shiftLevels/2);
	}

	if (Par->Ratio == 1.00)
		ACE_OS::sprintf(File_Name,"%s%s_P%d%s",_path,"ShiftMap",Par->padding,".gio");
	else
		ACE_OS::sprintf(File_Name,"%s%1.2f_%s_P%d%s",_path,Par->Ratio,"ShiftMap",Par->padding,".gio");

	/// alloc _corrFunct array.
	_corrFunct = new double [_shiftLevels];
	ACE_ASSERT (_corrFunct != NULL);
	_gaussFunction = new double [_shiftLevels];
	ACE_ASSERT (_gaussFunction != NULL);

//	_pixelCount = new int [_shiftLevels];
//	ACE_ASSERT (_pixelCount != NULL);

	if ((fin = ACE_OS::fopen(File_Name,"rb")) != NULL)
	{
		_shiftMap = new int [_shiftLevels * 1 * Par->Size_LP];

		k = 0;

		for(j=0; j<1+3*Par->Resolution/2; j++)
		{
			if (k<_shiftLevels)
			{
				ACE_OS::fread(&_shiftMap[k*1*Par->Size_LP],sizeof(int),1 * Par->Size_LP,fin);
//				_shiftMap[k*1*Par->Size_LP] = j;

				if (_shiftFunction[k]+3*Par->Resolution/4 == j)
					k++;
			}
		}
/*		int l,m, valuesm;

		int r, t1,t2,t3,t4;
		int plus1r,plus2r,plus3r,plus4r;
		int minus1r,minus2r,minus3r,minus4r;

		int plus1t,plus2t,plus3t,plus4t;
//		int minus1t,minus2t,minus3t,minus4t;

		for (l=Par->Size_Fovea; l<Par->Size_Rho; l++)
		{
			for (m=0; m<Par->Size_Theta; m++)
			{
				r = l;
				t1 = m;
				if (m>Par->Size_Theta/2)
				{
					t2 = m - (Par->Size_Theta/2);
					t3 = 3 * (Par->Size_Theta/2) - m;
				}
				else
				{
					t2 = (Par->Size_Theta/2) - m;
					t3 = (Par->Size_Theta/2) + m;
				}

				t4 = (Par->Size_Theta) - m - 1;

				plus1r = ((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t1]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t1]/192)*3)/3)/63;
				plus2r = ((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t2]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t1]/192)*3)/3)/63;
				plus3r = ((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t3]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t1]/192)*3)/3)/63;
				plus4r = ((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t4]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t1]/192)*3)/3)/63;

				minus1r = ((_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t1]-(_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t1]/192)*3)/3)/63;
				minus2r = ((_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t2]-(_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t2]/192)*3)/3)/63;
				minus3r = ((_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t3]-(_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t3]/192)*3)/3)/63;
				minus4r = ((_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t4]-(_shiftMap[184*Par->Size_LP+Par->Size_Theta*r+t4]/192)*3)/3)/63;

				plus1t = ((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t1]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t1]/192)*3)/3)%63;

				plus2t = (Par->Size_Theta/2)-(((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t2]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t2]/192)*3)/3)%63);
				if (plus2t<0)
					plus2t += Par->Size_Theta;

				plus3t = (((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t3]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t3]/192)*3)/3)%63)-(Par->Size_Theta/2);
				if (plus3t<0)
					plus3t += Par->Size_Theta;

				plus4t = (Par->Size_Theta) - (((_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t4]-(_shiftMap[224*Par->Size_LP+Par->Size_Theta*r+t4]/192)*3)/3)%63) -1;

				if (_shiftMap[204*Par->Size_LP+Par->Size_Theta*l+m]!= 0)
					if (((_shiftMap[204*Par->Size_LP+Par->Size_Theta*l+m]-l*3)/3)!=(Par->Size_Theta*l+m))
						valuesm = (_shiftMap[204*Par->Size_LP+Par->Size_Theta*l+m]-l*3)/3;
					else
						valuesm = 0;
			}

				//set the 4 points
		}

		valuesm = 0;

*/

		_count = new int [_shiftLevels];

		ACE_OS::fclose (fin);
		return 1;
	}

	return 0;
}

int YARPDisparityTool::loadDSTable(Image_Data * Par)
{
	char File_Name[256];
	FILE * fin;

	int j,k;

	sprintf(File_Name,"%s%s%1.2f_P%d%s",_path,"DSMap_",4.00,Par->padding,".gio");

	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		_dsTable = new IntNeighborhood [Par->Size_LP / 16];
		fread(&k,sizeof(int),1,fin);
		_dsTable[0].position = NULL;
		_dsTable[0].position = new unsigned int  [k * (Par->Size_LP/16)];
		_dsTable[0].weight   = NULL;
		_dsTable[0].weight   = new unsigned char [k * (Par->Size_LP/16)];

		for (j=0; j<Par->Size_LP/16; j++)
			fread(&(_dsTable[j].NofPixels) ,sizeof(unsigned short),1,fin);

		fread((_dsTable[0].position) ,sizeof(unsigned int),k * (Par->Size_LP/16),fin);
		fread((_dsTable[0].weight)   ,sizeof(unsigned char) ,k * (Par->Size_LP/16),fin);

		for (j=0; j<Par->Size_LP/16; j++)
		{
			_dsTable[j].position = _dsTable[0].position + k*j;
			_dsTable[j].weight   = _dsTable[0].weight   + k*j;
		}
		
		fclose (fin);
		return 1;
	}
	else
		_dsTable = NULL;

	return 0;
}

Image_Data YARPDisparityTool::lpInfo(int SXR, int SYR, int rho, int theta, int fovea, int res, double ratio, int pad)
{
	Image_Data Par;

	/// WARNING, truncating /ratio, is it the actual meaning?
	Par = Set_Param((int)(res/ratio),
					(int)(res/ratio),
					SXR,
					SYR,
					(int)(rho/ratio),
					(int)(theta/ratio),
					(int)(fovea/ratio),
					(int)(res/ratio),
					20,
					(double)(SXR/(res/ratio)));

	Par.Ratio = ratio;
	Par.padding = pad;
	Par.dres = 1090.0 / ratio;

	return Par;
}


void YARPDisparityTool::downSample(YARPImageOf<YarpPixelBGR> & inImg, YARPImageOf<YarpPixelBGR> & outImg)
{
	if (outImg.GetHeight() == _imgS.Size_Rho)
		DownSample(	(unsigned char*)inImg.GetRawBuffer(),
					(unsigned char*)outImg.GetRawBuffer(),
					_path,
					&_imgL,
					_imgS.Ratio,
					_dsTable);
	else
		DownSampleFovea((unsigned char*)inImg.GetRawBuffer(),
						(unsigned char*)outImg.GetRawBuffer(),
						_path,
						&_imgL,
						_imgS.Ratio,
						_dsTable,
						_imgS.Size_Fovea);

}

int YARPDisparityTool::computeDisparity (YARPImageOf<YarpPixelBGR> & inRImg,
										 YARPImageOf<YarpPixelBGR> & inLImg)
{
	int k;
	int disparity;

	rgbPixel avgLeft,avgRight;

	avgRight = computeAvg(inRImg.GetHeight(),inRImg.GetWidth(),inRImg.GetPadding(),(unsigned char*)inRImg.GetRawBuffer());
	avgLeft  = computeAvg(inLImg.GetHeight(),inLImg.GetWidth(),inLImg.GetPadding(),(unsigned char*)inLImg.GetRawBuffer());

	
	shiftnCorrFovea((unsigned char*)inRImg.GetRawBuffer(),
					(unsigned char*)inLImg.GetRawBuffer(),
					&_imgS,
					_shiftLevels,
					_shiftMap,
					_corrFunct,
					avgRight,
					avgLeft,
					_actRings,
					_count);


	disparity = corrAdjust(_count); //Computes where the best correspondance is and removes non valid points


//	double avg = 0.0;

//	int value1 = disparity;
//	double value2 = 3.0 - _corrFunct[disparity];

	for (k=0; k<_shiftLevels; k++)
		_corrFunct[k] = (3.0 - _corrFunct[k]);

	_snRatio = computeSNRatio(disparity);

	_gMagn  = _corrFunct[disparity];
	_gMean  = double(disparity);
	_gSigma = 10.0;

//	findFittingFunction();

	for (k=0; k<_shiftLevels; k++)
		_corrFunct[k] = 3.0 - _corrFunct[k];
	
	
	disparity = _shiftFunction[disparity];
	disparity = (int)(0.5 + disparity * _imgS.Size_X_Remap / (float)_imgS.Resolution);
/*
	double min = _corrFunct [disparity];

	if ((min/avg)<_corrTreshold)
		disparity = -1;

	if (disparity != -1)
		{

		}
		else 
			disparity = 0;
*/
	return disparity;
}

int YARPDisparityTool::computeDisparityRGB (YARPImageOf<YarpPixelBGR> & inRImg,
											YARPImageOf<YarpPixelBGR> & inLImg)
{
	int k;
	int disparity;

	rgbPixel avgLeft,avgRight;

	avgRight = computeAvg(inRImg.GetHeight(),inRImg.GetWidth(),inRImg.GetPadding(),(unsigned char*)inRImg.GetRawBuffer());
	avgLeft  = computeAvg(inLImg.GetHeight(),inLImg.GetWidth(),inLImg.GetPadding(),(unsigned char*)inLImg.GetRawBuffer());

	
	shiftnCorrFoveaRGB	((unsigned char*)inRImg.GetRawBuffer(),
						(unsigned char*)inLImg.GetRawBuffer(),
						&_imgS,
						_shiftLevels,
						_shiftMap,
						_corrFunct,
						avgRight,
						avgLeft,
						_actRings,
						_count);


	disparity = corrAdjust(_count); //Computes where the best correspondance is and removes non valid points

	disparity = _shiftFunction[disparity];
	disparity = (int)(0.5 + disparity * _imgS.Size_X_Remap / (float)_imgS.Resolution);

	return disparity;
}

void YARPDisparityTool::makeHistogram(YARPImageOf<YarpPixelMono>& hImg)
{
	int i,j;
	int height = hImg.GetHeight();
	int width = hImg.GetWidth();
//	int height = 512;
//	int width = 512;
	unsigned char * hist = (unsigned char *)hImg.GetRawBuffer();

	int offset = (height-_shiftLevels+1)/2;

	for (j=0;j<height*width;j++)
		hist[j] = 0;


	for (i=0; i<_shiftLevels-1; i++)
	{
		if ((i+offset >=0)&&(i+offset<width))
		{
			for (j=height-(int)(height/3.0*(3-_corrFunct[i])); j<height; j++)
					hist[(j*width+i+offset)] = 128;
		}
	}

	/*
	for (i=0; i<_shiftLevels-1; i++)
	{
		if ((i+offset >=0)&&(i+offset<width))
		{
			for (j=height-(int)(height/3.0*(3-_gaussFunction[i])); j<height; j++)
					hist[(j*width+i+offset)] = (hist[(j*width+i+offset)]+192)/2;
		}
	}
	*/
		
	for (j=0; j<height; j++)
		hist[(j*width+width/2)] = 255;


}

int YARPDisparityTool::functFitting(YVector& x,
									 double y[],
									 YVector& sig,
									 int ndata,
									 YVector& a,
									 bool *ia,
									 int ma,
									 YMatrix& covar,
									 YMatrix& alpha,
									 double *chisq,
									 void (YARPDisparityTool::*fittingFunct) (double,YVector&,double*,YVector&,int),
									 double *alambda)
{
//	void covsrt(YMatrix& covar, int ma, bool * ia, int mfit);
//	int gaussj(YMatrix& a, int n, YMatrix& b, int m);
//	void mrqcof(YVector& x, double y[], YVector& sig, int ndata, YVector& a,
//				bool * ia, int ma, YMatrix& alpha, YVector& beta, double *chisq,
//				void (*funcs)(double, YVector&, double *, YVector&, int));

	int j,k,l;
	int error;
	static int mfit;
	static double ochisq;

	static YVector atry, beta, da;
	static YMatrix oneda;

	if (*alambda < 0.0) 
	{ 
		atry.Resize(ma);
		beta.Resize(ma);
		da.Resize(ma);

		for (mfit=0,j=1;j<=ma;j++)
			if (ia[j-1]) 
				mfit++;

		oneda.Resize(mfit,1);

		*alambda=0.001f;
		mrqCof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq,fittingFunct);
		ochisq=(*chisq);
		for (j=1;j<=ma;j++) 
			atry(j)=a(j);
	}

	for (j=1;j<=mfit;j++) 
	{ 
		for (k=1;k<=mfit;k++) 
			covar(j,k)=alpha(j,k);
		covar(j,j)=alpha(j,j)*(1.0+(*alambda));
		oneda(j,1)=beta(j);
	}

	error = gaussJordan(covar,mfit,oneda,1);
	if (error==-1)
		return -1;


	for (j=1;j<=mfit;j++) 
		da(j)=oneda(j,1);

	if (*alambda == 0.0) 
	{
		covSwap(covar,ma,ia,mfit);
		covSwap(alpha,ma,ia,mfit);
		return 0;
	}

	for (j=0,l=1;l<=ma;l++) 
		if (ia[l-1]) 
			atry(l)=a(l)+da(++j);

	mrqCof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq,fittingFunct);
//	if (*chisq < ochisq) 
	if (*chisq - ochisq < 0) 
	{
		*alambda *= 0.1f;
		ochisq=(*chisq);
		for (j=1;j<=mfit;j++) 
		{
			for (k=1;k<=mfit;k++) 
				alpha(j,k)=covar(j,k);
			beta(j)=da(j);
		}
		for (l=1;l<=ma;l++) 
			a(l)=atry(l);
	} 
	else 
	{
		*alambda *= 10.0;
		*chisq=ochisq;
	}
	return 0;

}

int YARPDisparityTool::gaussJordan(YMatrix& a, int n, YMatrix& b, int m)
{
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	double big,dum,pivinv,temp;

	indxc=(int*)malloc(n*sizeof(int));
	indxr=(int*)malloc(n*sizeof(int));	
	ipiv =(int*)malloc(n*sizeof(int));

	for (j=0;j<n;j++)
	{
		ipiv[j] =0;
		indxc[j]=0;
		indxr[j]=0;
	}

	for (i=1;i<=n;i++) 
	{ 
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j-1] != 1)
				for (k=1;k<=n;k++) 
				{
					if (ipiv[k-1] == 0) 
					{
						if (fabs(a(j,k)) >= big) 
						{
							big=fabs(a(j,k));
							irow=j;
							icol=k;
						}
					}
				}

		++(ipiv[icol-1]);
		if (irow != icol) 
		{
			for (l=1;l<=n;l++) 
				SWAP(a(irow,l),a(icol,l))
			for (l=1;l<=m;l++) 
				SWAP(b(irow,l),b(icol,l))
		}
		indxr[i-1]=irow; 
		indxc[i-1]=icol;
		if (a(icol,icol) == 0.0)
		{
			printf("gaussJordan: Singular Matrix\n");
			return -1;
		}
		pivinv=1.0/a(icol,icol);
		a(icol,icol)=1.0;
		for (l=1;l<=n;l++) 
			a(icol,l) *= pivinv;
		for (l=1;l<=m;l++) 
			b(icol,l) *= pivinv;

		for (ll=1;ll<=n;ll++)
			if (ll != icol)
			{
				dum=a(ll,icol);
				a(ll,icol)=0.0;
				for (l=1;l<=n;l++) 
					a(ll,l) -= a(icol,l)*dum;
				for (l=1;l<=m;l++) 
					b(ll,l) -= b(icol,l)*dum;
			}
	}
	for (l=n;l>=1;l--) 
	{
		if (indxr[l-1] != indxc[l-1])
			for (k=1;k<=n;k++)
			{
				SWAP(a(k,indxr[l-1]),a(k,indxc[l-1]));
			}
	}

	free(ipiv);
	free(indxr);
	free(indxc);

	return 0;
}

void YARPDisparityTool::gaussian(double x, YVector& a, double *y, YVector& dyda, int na)
{
	int i;
	double fac,ex,arg;
	
	*y=0.0;
	
	for (i=1;i<=na-1;i+=3) 
	{
		arg=(x-a(i+1))/a(i+2);
		ex=exp(-arg*arg);
		fac=a(i)*ex*2.0*arg;
		*y += a(i)*ex;

		dyda(i)=ex;
		dyda(i+1)=fac/a(i+2);
		dyda(i+2)=fac*arg/a(i+2);
	}
}
void YARPDisparityTool::covSwap(YMatrix& covar, int ma, bool * ia, int mfit)
{
	int i,j,k;
	double temp;
	for (i=mfit+1;i<=ma;i++)
		for (j=1;j<=i;j++) 
			covar(i,j)=covar(j,i)=0.0;
	k=mfit;
	for (j=ma;j>=1;j--) 
	{
		if (ia[j-1]) 
		{
			for (i=1;i<=ma;i++) 
				SWAP(covar(i,k),covar(i,j))
			for (i=1;i<=ma;i++) 
				SWAP(covar(k,i),covar(j,i))
			k--;
		}
	}
}

void YARPDisparityTool::mrqCof(	YVector& x, double y[], YVector& sig, int ndata, YVector& a, bool * ia,
								int ma, YMatrix& alpha, YVector& beta, double *chisq,
								void (YARPDisparityTool::*fittingFunct)(double, YVector&, double *, YVector&, int))
{
	int i,j,k,l,m,mfit=0;
	double ymod,wt,sig2i,dy;

	YVector dyda;
	dyda.Resize(ma);

	for (j=1;j<=ma;j++)
		if (ia[j-1]) 
			mfit++;
	for (j=1;j<=mfit;j++) 
	{
		for (k=1;k<=j;k++) 
			alpha(j,k)=0.0;
		beta(j)=0.0;
	}
	*chisq=0.0;

	for (i=1;i<=ndata;i++) 
	{
		(this->*fittingFunct)(x(i),a,&ymod,dyda,ma);
		sig2i=1.0/(sig(i)*sig(i));
		dy=y[i-1]-ymod;

		for (j=0,l=1;l<=ma;l++)
		{
			if (ia[l-1]) 
			{
				wt=dyda(l)*sig2i;
				for (j++,k=0,m=1;m<=l;m++)
					if (ia[m-1]) 
						alpha(j,++k) += wt*dyda(m);
				beta(j) += dy*wt;
			}
		}
		*chisq += dy*dy*sig2i; 
	}
	for (j=2;j<=mfit;j++)
		for (k=1;k<j;k++) 
			alpha(k,j)=alpha(j,k);

}

int YARPDisparityTool::corrAdjust(int * count)
{
	int MAX = 0;
	double MIN = 1000.0;
	int minindex;
	int k;

	for (k=0; k<_shiftLevels; k++)
		if (count[k]>MAX)
			MAX = count[k];

	for (k=0; k<_shiftLevels; k++)
	{
		if (count[k]<MAX)
			_corrFunct[k] = 3.0;
		else
			_corrFunct[k] = 3.0-(count[k]*_corrFunct[k]/MAX);
	}

	for (k=0; k<_shiftLevels; k++)
		if (_corrFunct[k]<MIN)
		{
			MIN = _corrFunct[k];
			minindex = k;
		}

	return minindex;
}

double YARPDisparityTool::computeSNRatio(int disparity)
{
	int k;
	double avg = 0.0;
	double snRatio;
	int counter = 0;

	for (k=0; k<_shiftLevels; k++)
	{
		avg += _corrFunct[k];
		if (_corrFunct[k]>0.0)
			counter ++;
	}

//	avg /= _shiftLevels;
	avg /= counter;

	snRatio = _corrFunct[disparity]/(avg+0.00001);

	return snRatio;
}

void YARPDisparityTool::findFittingFunction()
{
	int k;

	YVector adef(3);
	YVector a(3);

	a(1) = _gMagn;
	a(2) = _gMean;
	a(3) = _gSigma;

	YVector x;
	x.Resize(_shiftLevels);

	YVector sig;
	sig.Resize(_shiftLevels);

	bool *ia;
	ia = (bool*) malloc (3*sizeof(bool));

	ia[0] = 1;
	ia[1] = 1;
	ia[2] = 1;

	YMatrix covar (3,3);
	YMatrix alpha (3,3);

	double chisq = 1000;
	double prchisq = 1100;
	double alambda;
	double min_chisq = 100000;

	fittingFunct = &YARPDisparityTool::gaussian;

	for (k=1; k<=_shiftLevels; k++)
	{
		x  (k) = (double)(k-1);
		sig(k) = 1.0f;
	}

	double d;
	int error;

	for (d=0.1; d<100.1; d+=10)
	{

		a(1) = _gMagn;
		a(2) = _gMean;
		a(3) = d;

		alambda = -1.0;
		error = functFitting(x,_corrFunct,sig,_shiftLevels,a,ia,3,covar,alpha,&chisq,fittingFunct,&alambda);
		if (error != -1)
		{
			while (fabs(chisq-prchisq) > 0.00001)
			{
				prchisq = chisq;
				error = functFitting(x,_corrFunct,sig,_shiftLevels,a,ia,3,covar,alpha,&chisq,fittingFunct,&alambda);
				if (error == -1)
					break;
			}
			if (error != -1)
			{
				alambda = 0.0;
				error = functFitting(x,_corrFunct,sig,_shiftLevels,a,ia,3,covar,alpha,&chisq,fittingFunct,&alambda);
				if (error == -1)
					break;

				if (chisq<min_chisq)
				{
					min_chisq = chisq;
					adef = a;
				}
			}
		}

	}
	a = adef;
	_squareError = min_chisq;

	for (k=0; k<_shiftLevels; k++)
		_gaussFunction[k] = 3.0-(a(1) * exp(-((k-a(2))/a(3))*((k-a(2))/a(3))));

	_gMagn  = a(1);
	_gMean  = a(2);
	_gSigma = a(3);
}
