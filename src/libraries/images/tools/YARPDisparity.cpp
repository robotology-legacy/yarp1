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
/// $Id: YARPDisparity.cpp,v 1.26 2004-07-09 10:48:54 babybot Exp $
///
///

// disparity.cpp: implementation of the YARPDisparityTool class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPDisparity.h"
#include "YARPLogPolar.h"
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
#define NR_END 1
#define FREE_ARG char*

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

inline bool isWithin(int v,int max, int min)
{
	if ( (v<=max) && (v>=min) )
		return true;
	else
		return false;
}

inline float colorToSaturation(float r, float g, float b)
{
	float max;
	float min;

	float tmp1;
	float tmp2;

	r += 255;
	g += 255;
	b += 255;

	//// MAX
	if (r > g)
		tmp1 = r;
	else 
		tmp1 = g;

	if (g > b)
		tmp2 = g;
	else
		tmp2 = b;

	if (tmp1>tmp2)
		max = tmp1;
	else
		max = tmp2;
	
	/// I know max, now find min
	if (r < g)
		tmp1 = r;
	else 
		tmp1 = g;

	if (g < b)
		tmp2 = g;
	else
		tmp2 = b;

	if (tmp1 < tmp2)
		min = tmp1;
	else
		min = tmp2;

	if (max > 0.5)
		return (max-min)/max;
	else
		return 0;
}

YARPDisparityTool::YARPDisparityTool()
{
	char *yarproot = GetYarpRoot();
	ACE_OS::sprintf(_path,"%s/%s/\0", yarproot, "conf");
//	ACE_OS::sprintf(_path,"%s","c:/temp/tables/prefix/");

	_dsTable		= NULL;
	_shiftFunction	= NULL;
	_shiftFunctionInv = NULL;
	_gaussFunction	= NULL;
	_shiftMap		= NULL;
	_corrFunct		= NULL;
	_actRings		= 21;
	_corrTreshold   = 2.0;
	_count			= NULL;
	_coeff			= NULL;
	_phase			= NULL;

	_ssdFunct = NULL;
	_varFunct = NULL;

	_maxCount = 0.0;
	_shiftMax = 0.0;
	_shiftMin = 0.0;

	_varMax = 800000;
	_varMin = 14000;
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

	if (_coeff != NULL)
		delete [] _coeff;

	if (_phase != NULL)
		delete [] _phase;

	if (_std1 != NULL)
		delete [] _std1;
	
	if (_std2 != NULL)
		delete [] _std2;

	if (_ssdFunct != NULL)
		delete [] _ssdFunct;
	
	if (_varFunct != NULL)
		delete [] _varFunct;

	if (_shiftFunctionInv == NULL)
		delete [] _shiftFunctionInv;
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
		ACE_OS::printf("Found shift table...\n");
	}
	else
	{
		ACE_OS::printf("Shift table not found, using default...\n");
		int mult = 1;
		_shiftLevels = 1+6*Par->Resolution/(4*mult);
		_shiftFunction = new int [_shiftLevels];
		for (j=0; j<mult*_shiftLevels; j+=mult)
			_shiftFunction[j/mult] = j-mult*(_shiftLevels/2);
	}

	_shiftMax = _shiftLevels-1;
	_shiftMin = 0;

	int dummy = _shiftLevels;
	// make the inverse of shift function
	_shiftFunctionInv = new int[2*Par->Resolution];
	float dispRef = 0;
	for(int l = 0; l < 2*Par->Resolution; l++)
	{
		bool found = false;
		int m = 0;
		// check first and last
		float conf = _shiftToDisparity(0);
		dispRef = (l-Par->Resolution)*Par->Size_X_Remap/Par->Resolution;
		if (dispRef<=conf)
		{
			found = true;
			m = 0;
		}
		conf = _shiftToDisparity(_shiftLevels-1);
		if (dispRef>=conf)
		{
			found = true;
			m = _shiftLevels-1;
		}

		while((!found) && (m+1 <= (_shiftLevels-1)) )
		{
			float tmpDisp1 = _shiftToDisparity(m);
			float tmpDisp2 = _shiftToDisparity(m+1);
			if ( (tmpDisp1<=dispRef) && (dispRef<=tmpDisp2) )
				found = true;
			else
				m++;
		}
		
		if (found)
			_shiftFunctionInv[l] = m;
	}

	if (Par->Ratio == 1.00)
		ACE_OS::sprintf(File_Name,"%s%s_P%d%s",_path,"ShiftMap",Par->padding,".gio");
	else
		ACE_OS::sprintf(File_Name,"%s%1.2f_%s_P%d%s",_path,Par->Ratio,"ShiftMap",Par->padding,".gio");

	/// alloc _corrFunct array.
	_corrFunct = new double [_shiftLevels];
	ACE_ASSERT (_corrFunct != NULL);
	memset(_corrFunct, 0, _shiftLevels*sizeof(double));

	_phase = new double [_shiftLevels];
	ACE_ASSERT (_phase != NULL);
	memset(_phase, 0, _shiftLevels*sizeof(double));

	_coeff = new double [_shiftLevels];
	ACE_ASSERT (_coeff!= NULL);
	memset(_coeff, 0, _shiftLevels*sizeof(double));

	_std2 = new double [_shiftLevels];
	ACE_ASSERT (_std2!=NULL);
	memset(_std2, 0, _shiftLevels*sizeof(double));

	_std1 = new double [_shiftLevels];
	ACE_ASSERT (_std1!=NULL);
	memset(_std1, 0, _shiftLevels*sizeof(double));

	_varFunct = new double [_shiftLevels];
	ACE_ASSERT (_varFunct!=NULL);
	memset(_varFunct, 0, _shiftLevels*sizeof(double));

	_ssdFunct = new double [_shiftLevels];
	ACE_ASSERT (_ssdFunct!=NULL);
	memset(_ssdFunct, 0, _shiftLevels*sizeof(double));

	_gaussFunction = new double [_shiftLevels];
	ACE_ASSERT (_gaussFunction != NULL);
	memset(_gaussFunction, 0, _shiftLevels*sizeof(double));

	if ((fin = ACE_OS::fopen(File_Name,"rb")) != NULL)
	{
		_shiftMap = new int [_shiftLevels * 1 * Par->Size_LP];
		
		k = 0;

		for(j=0; j<1+3*Par->Resolution/2; j++)
		{
			if (k<_shiftLevels)
			{
				ACE_OS::fread(&_shiftMap[k*1*Par->Size_LP],sizeof(int),1 * Par->Size_LP,fin);
				if (_shiftFunction[k]+3*Par->Resolution/4 == j)
					k++;
			}
		}
		_count = new int [_shiftLevels];
		memset(_count, 0, sizeof(int)*_shiftLevels);

		ACE_OS::fclose (fin);
		return 1;
	}

	return 0;
}

int YARPDisparityTool::loadDSTable(Image_Data * Par, Image_Data* ParDS)
{
	char File_Name[256];
	FILE * fin;

	int j,k;

	sprintf(File_Name,"%s%s%1.2f_P%d%s",_path,"DSMap_",ParDS->Ratio,Par->padding,".gio");

	if ((fin = fopen(File_Name,"rb")) != NULL)
	{
		_dsTable = new IntNeighborhood [Par->Size_LP / (ParDS->Ratio*ParDS->Ratio)];
		fread(&k,sizeof(int),1,fin);
		_dsTable[0].position = NULL;
		_dsTable[0].position = new unsigned int  [k * (Par->Size_LP/(ParDS->Ratio*ParDS->Ratio))];
		_dsTable[0].weight   = NULL;
		_dsTable[0].weight   = new unsigned char [k * (Par->Size_LP/(ParDS->Ratio*ParDS->Ratio))];

		for (j=0; j<Par->Size_LP/(ParDS->Ratio*ParDS->Ratio); j++)
			fread(&(_dsTable[j].NofPixels) ,sizeof(unsigned short),1,fin);

		fread((_dsTable[0].position) ,sizeof(unsigned int),k * (Par->Size_LP/(ParDS->Ratio*ParDS->Ratio)),fin);
		fread((_dsTable[0].weight)   ,sizeof(unsigned char) ,k * (Par->Size_LP/(ParDS->Ratio*ParDS->Ratio)),fin);

		for (j=0; j<Par->Size_LP/(ParDS->Ratio*ParDS->Ratio); j++)
		{
			_dsTable[j].position = _dsTable[0].position + k*j;
			_dsTable[j].weight   = _dsTable[0].weight   + k*j;
		}
		
		fclose (fin);
		return 1;
	}
	else
	{
		printf("%s not found!\n", File_Name);
		_dsTable = NULL;
	}

	return 0;
}

int YARPDisparityTool::loadRemapTable(Image_Data * Par)
{
	char filename[YARP_STRING_LEN];
	ACE_OS::sprintf(filename,"%s%1.2f_%s_%2.3f_%dx%d_P%d%s",
					_path,
					Par->Ratio,"RemapMap",
					Par->Zoom_Level,
					Par->Size_X_Remap,
					Par->Size_Y_Remap,
					Par->padding,".gio");

	ACE_OS::printf("Going to open: %s\n", filename);
	FILE *fin = ACE_OS::fopen(filename,"rb");
	if (fin == NULL)
	{
		ACE_OS::printf("error, aborting program\n");
		char c;
		cin >> c;
		return YARP_FAIL;
	}

	_remapMap = (int *) malloc (Par->Size_Img_Remap * sizeof(int));
	ACE_ASSERT (_remapMap != NULL);

	ACE_OS::fread(_remapMap, sizeof(int), Par->Size_Img_Remap, fin);
	ACE_OS::fclose (fin);

	return YARP_OK;
}

void YARPDisparityTool::remap(YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelBGR> &out)
{
	// using namespace _logpolarParams;
	// ACE_ASSERT (in.GetWidth() == _stheta && in.GetHeight() == _srho);
	// ACE_ASSERT (out.GetWidth() == _xsize && out.GetHeight() == _ysize);

	Remap ((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), &_imgL, _remapMap);
}

Image_Data YARPDisparityTool::lpInfo(int SXR, int SYR, int rho, int theta, int fovea, int res, double ratio, int pad, int zoom)
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
					(double)(zoom*SXR/(res/ratio)));

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

int YARPDisparityTool::computeMono (YARPImageOf<YarpPixelBGR> & inRImg,
											YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k, k1;
	int i2,i1;//iR,iL
	
	double numr   = 0;
	double den_1r = 0;
	double den_2r = 0;
	double numg   = 0;
	double den_1g = 0;
	double den_2g = 0;
	double numb   = 0;
	double den_1b = 0;
	double den_2b = 0;

	double sigma2 = 0;
	double sigma1 = 0;
	double grayAv1 = 0;
	double grayAv2 = 0;

	YarpPixelRGBFloat pixel1;
	YarpPixelRGBFloat pixel2;

	double sum1R = 0;
	double sum2R = 0;
	double sum1G = 0;
	double sum2G = 0;
	double sum1B = 0;
	double sum2B = 0;
	double nElem = 0;

	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{
		k1 = k * _imgS.Size_LP; //Positioning on the table

		numr   = 0;
		den_1r = 0;
		den_2r = 0;
		numg   = 0;
		den_1g = 0;
		den_2g = 0;
		numb   = 0;
		den_1b = 0;
		den_2b = 0;

		sum1R = 0;
		sum2R = 0;
		sum1G = 0;
		sum2G = 0;
		sum1B = 0;
		sum2B = 0;
		nElem = 0;

		sigma1 = 0;
		sigma2 = 0;
		grayAv1 = 0;
		grayAv2 = 0;

		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j<_actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						pixel1.r = *fovPtr++;
						pixel2.r = fullPtr[i2];
						pixel1.g = *fovPtr++;
						pixel2.g = fullPtr[i2+1];
						pixel1.b = *fovPtr++;
						pixel2.b = fullPtr[i2+2];

						double gray1 = (pixel1.r + pixel1.g+pixel1.b)/3.0;
						double gray2 = (pixel2.r+pixel2.g+pixel2.b)/3.0;

						numr   += (pixel1.r * pixel2.r);
						sum1R += pixel1.r;
						sum2R += pixel2.r;
						den_1r += (pixel1.r * pixel1.r);
						den_2r += (pixel2.r * pixel2.r);

						numg   += (pixel1.g * pixel2.g);
						sum1G += pixel1.g;
						sum2G += pixel2.g;
						den_1g += (pixel1.g * pixel1.g);
						den_2g += (pixel2.g * pixel2.g);

						numb   += (pixel1.b * pixel2.b);
						sum1B += pixel1.b;
						sum2B += pixel2.b;
						den_1b += (pixel1.b * pixel1.b);
						den_2b += (pixel2.b * pixel2.b);

						sigma1 += gray1*gray1;
						sigma2 += gray2*gray2;
						grayAv1 += gray1;
						grayAv2 += gray2;

						nElem++;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}

			double tmpR;
			double tmpG;
			double tmpB;
			
			tmpR = (den_1r-sum1R*sum1R/nElem)*(den_2r-sum2R*sum2R/nElem);
			tmpG = (den_1g-sum1G*sum1G/nElem)*(den_2g-sum2G*sum2G/nElem);
			tmpB = (den_1b-sum1B*sum1B/nElem)*(den_2b-sum2B*sum2B/nElem);

			_corrFunct[k] = 0;
			if (tmpR>0)
				_corrFunct[k] += (numr-sum1R*sum2R/nElem)/sqrt(tmpR);

			if (tmpG>0)
				_corrFunct[k] += (numg-sum1G*sum2G/nElem)/sqrt(tmpG);

			if (tmpB>0)
				_corrFunct[k] += (numb-sum1B*sum2B/nElem)/sqrt(tmpB);
				
			_corrFunct[k] /= 3.0;

			if (_isnan(_corrFunct[k]))
				printf("WARNING: found a NAN\n");
				
			_std2[k] = (1/nElem)*(sigma2-grayAv2*grayAv2/nElem)/(128*128);
			_std1[k]= (1/nElem)*(sigma1-grayAv1*grayAv1/nElem)/(128*128);
			
		}
		else
		{
			_corrFunct[k] = 0;
			_std2[k] = 0;
			_std1[k] = 0;
		}
	}

	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeCorrProd (YARPImageOf<YarpPixelBGR> & inRImg,
											YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k, k1;
	int i2,i1;//iR,iL
	
	double numr   = 0;
	double den_1r = 0;
	double den_2r = 0;
	double numg   = 0;
	double den_1g = 0;
	double den_2g = 0;
	double numb   = 0;
	double den_1b = 0;
	double den_2b = 0;

	double sigma2 = 0;
	double sigma1 = 0;
	double grayAv1 = 0;
	double grayAv2 = 0;

	YarpPixelRGBFloat pixel1;
	YarpPixelRGBFloat pixel2;

	double sum1R = 0;
	double sum2R = 0;
	double sum1G = 0;
	double sum2G = 0;
	double sum1B = 0;
	double sum2B = 0;
	double nElem = 0;

	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{
		k1 = k * _imgS.Size_LP; //Positioning on the table

		numr   = 0;
		den_1r = 0;
		den_2r = 0;
		numg   = 0;
		den_1g = 0;
		den_2g = 0;
		numb   = 0;
		den_1b = 0;
		den_2b = 0;

		sum1R = 0;
		sum2R = 0;
		sum1G = 0;
		sum2G = 0;
		sum1B = 0;
		sum2B = 0;
		nElem = 0;

		sigma1 = 0;
		sigma2 = 0;
		grayAv1 = 0;
		grayAv2 = 0;

		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j<_actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						pixel1.r = *fovPtr++;
						pixel2.r = fullPtr[i2];
						pixel1.g = *fovPtr++;
						pixel2.g = fullPtr[i2+1];
						pixel1.b = *fovPtr++;
						pixel2.b = fullPtr[i2+2];

						double gray1 = (pixel1.r + pixel1.g+pixel1.b)/3.0;
						double gray2 = (pixel2.r+pixel2.g+pixel2.b)/3.0;

						numr   += (pixel1.r * pixel2.r);
						sum1R += pixel1.r;
						sum2R += pixel2.r;
						den_1r += (pixel1.r * pixel1.r);
						den_2r += (pixel2.r * pixel2.r);

						numg   += (pixel1.g * pixel2.g);
						sum1G += pixel1.g;
						sum2G += pixel2.g;
						den_1g += (pixel1.g * pixel1.g);
						den_2g += (pixel2.g * pixel2.g);

						numb   += (pixel1.b * pixel2.b);
						sum1B += pixel1.b;
						sum2B += pixel2.b;
						den_1b += (pixel1.b * pixel1.b);
						den_2b += (pixel2.b * pixel2.b);

						sigma1 += gray1*gray1;
						sigma2 += gray2*gray2;
						grayAv1 += gray1;
						grayAv2 += gray2;

						nElem++;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}

			_corrFunct[k] = (numr-sum1R*sum2R/nElem)/sqrt((den_1r-sum1R*sum1R/nElem)*(den_2r-sum2R*sum2R/nElem));
			_corrFunct[k] *= (numg-sum1G*sum2G/nElem)/sqrt((den_1g-sum1G*sum1G/nElem)*(den_2g-sum2G*sum2G/nElem));
			_corrFunct[k] *= (numb-sum1B*sum2B/nElem)/sqrt((den_1b-sum1B*sum1B/nElem)*(den_2b-sum2B*sum2B/nElem));

			if (_isnan(_corrFunct[k])) _corrFunct[k]=0;
			
			_std2[k] = (1/nElem)*(sigma2-grayAv2*grayAv2/nElem)/(128*128);
			_std1[k]= (1/nElem)*(sigma1-grayAv1*grayAv1/nElem)/(128*128);
			
		}
		else
		{
			_corrFunct[k] = 0;
			_std2[k] = 0;
			_std1[k] = 0;
		}
	}

	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeMonoNorm (YARPImageOf<YarpPixelBGR> & inRImg,
										     YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	shiftnCorrFoveaNoAverageNorm((unsigned char*)inRImg.GetRawBuffer(),
					(unsigned char*)inLImg.GetRawBuffer(),
					&_imgS,
					_shiftLevels,
					_shiftMap,
					_corrFunct,
					_std1,
					_std2,
					_actRings,
					_count);

	
	_cleanCorr();
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeFullAverage (YARPImageOf<YarpPixelBGR> & inRImg,
													YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
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

	_cleanCorr();
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
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

int YARPDisparityTool::computeSSDRGB (YARPImageOf<YarpPixelBGR> & inRImg,
   												YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	double den1,den2;
	int nElem;
	YarpPixelRGBFloat first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		ssd = 0.0;
		den1 = 0.0;
		den2 = 0.0;
		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						ssd += (first.r-second.r)*(first.r-second.r);
						ssd += (first.g-second.g)*(first.g-second.g);
						ssd += (first.b-second.b)*(first.b-second.b);

						den1 += (first.r*first.r);
						den1 += (first.g*first.g);
						den1 += (first.b*first.b);

						den2 += (second.r*second.r);
						den2 += (second.g*second.g);
						den2 += (second.b*second.b);

						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = 1-ssd/sqrt(den1*den1);
		}
		else
		_corrFunct[k] = 0;

	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeSSDRGBUN (YARPImageOf<YarpPixelBGR> & inRImg,
   												YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	double den1,den2;
	int nElem;
	YarpPixelRGBFloat first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		ssd = 0.0;
		den1 = 0.0;
		den2 = 0.0;
		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						ssd += (first.r-second.r)*(first.r-second.r);
						ssd += (first.g-second.g)*(first.g-second.g);
						ssd += (first.b-second.b)*(first.b-second.b);

						den1 += (255/2*255/2);
						den1 += (255/2*255/2);
						den1 += (255/2*255/2);

						den2 += (second.r*second.r);
						den2 += (second.g*second.g);
						den2 += (second.b*second.b);

						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = 1-ssd/sqrt(den1*den1);
		}
		else
		_corrFunct[k] = 0;

	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeSSDRGBxVar (YARPImageOf<YarpPixelBGR> & inRImg,
   										  YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	float var;
	float ssd;
	float ssdDen1;
	float ssdDen2;
	float varDen1;
	float varDen2;

	float nElem;
	YarpPixelRGBFloat first, second;
	float deltaR;
	float deltaG;
	float deltaB;

	float phase;
	float normFact;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		var = 0;
		varDen1 = 0.0;
		varDen2 = 0.0;
		ssd = 0.0;
		ssdDen1 = 0.0;
		ssdDen2 = 0.0;
		phase = 0.0;
		normFact = 0.0;
		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];

						deltaR = first.r - second.r;
						deltaG = first.g - second.g;
						deltaB = first.b - second.b;
						
						float tmp;
						var += (deltaR*deltaR + deltaG*deltaG + deltaB*deltaB)/3;
						tmp = (deltaR + deltaG + deltaB)/3;
						var -= tmp*tmp;

						////////// varDen1
						varDen1 += ((first.r)*(first.r) + (first.g)*(first.g) + (first.b)*(first.b))/3.0;
						varDen1 -= ( (first.r + first.g + first.b) * (first.r + first.g + first.b))/9.0;
						////////////////////

						////////// varDen2
						varDen2 += ((second.r)*(second.r) + (second.g)*(second.g) + (second.b)*(second.b))/3.0;
						varDen2 -= ( (second.r + second.g + second.b) * (second.r + second.g + second.b))/9.0;
						////////////////////

						//////// SSD
						ssd += deltaR*deltaR + deltaG*deltaG + deltaB*deltaB;
						
						ssdDen1 += (first.r*first.r);
						ssdDen1 += (first.g*first.g);
						ssdDen1 += (first.b*first.b);

						ssdDen2 += (second.r*second.r);
						ssdDen2 += (second.g*second.g);
						ssdDen2 += (second.b*second.b);

						//// gray phase
						// float u = sqrt(3)/3;
						// float absvalue = sqrt(deltaR*deltaR + deltaG*deltaG + deltaB*deltaB);

						phase += colorToSaturation(deltaR, deltaG, deltaB);

						// tmp = (deltaR + deltaG + deltaB)*u/absvalue;
						// phase += fabs(tmp); // acos(tmp)/PI);
												
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			
		/*	if (var > _varMax)
			{
				_varMax = var;
				ACE_OS::printf("Aggiornati max: %lf\tmin: %lf\t %lf\n", _varMax, _varMin, nElem);			
			}
			if (var<_varMin)
			{
				_varMin = var;
				ACE_OS::printf("Aggiornati max: %lf\tmin: %lf\t %lf\n", _varMax, _varMin, nElem);			
			}*/
			
			_phase[k] = 1-(phase/nElem);

			// _varFunct[k] = 1 - var/sqrt(varDen1*varDen2);
			_varFunct[k] = 1 - (var-_varMin)/(_varMax-_varMin);
			_ssdFunct[k] = 1 - ssd/ssdDen1;

			if (_varFunct[k] < 0)
				_varFunct[k] = 0;
			if (_varFunct[k] > 1)
				_varFunct[k] = 1;
			
			if (_ssdFunct[k] < 0)
				_ssdFunct[k] = 0;
			if (_ssdFunct[k] > 1)
				_ssdFunct[k] = 1;
			//_corrFunct[k] = sqrt(_varFunct[k]*_ssdFunct[k]);
			_corrFunct[k] = _varFunct[k]*_ssdFunct[k];
		}
		else
		{
			_corrFunct[k] = 0;
			_ssdFunct[k] = 0;
			_varFunct[k] = 0;
			_phase[k] = 0;
		}
	}

	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeSSDRGBxVar2 (YARPImageOf<YarpPixelBGR> & inRImg,
   										  YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	float var;
	float ssd;
	float ssdDen1;
	float ssdDen2;
	float varDen1;
	float varDen2;

	float nElem;
	YarpPixelRGBFloat first, second;
	float deltaR;
	float deltaG;
	float deltaB;

	float phase;
	float normFact;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		var = 0;
		varDen1 = 0.0;
		varDen2 = 0.0;
		ssd = 0.0;
		ssdDen1 = 0.0;
		ssdDen2 = 0.0;
		phase = 0.0;
		normFact = 0.0;
		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];

						deltaR = first.r - second.r;
						deltaG = first.g - second.g;
						deltaB = first.b - second.b;
						
						float tmp;
						var += (deltaR*deltaR + deltaG*deltaG + deltaB*deltaB)/3;
						tmp = (deltaR + deltaG + deltaB)/3;
						var -= tmp*tmp;

						////////// varDen1
						varDen1 += ((first.r)*(first.r) + (first.g)*(first.g) + (first.b)*(first.b))/3.0;
						varDen1 -= ( (first.r + first.g + first.b) * (first.r + first.g + first.b))/9.0;
						////////////////////

						////////// varDen2
						varDen2 += ((second.r)*(second.r) + (second.g)*(second.g) + (second.b)*(second.b))/3.0;
						varDen2 -= ( (second.r + second.g + second.b) * (second.r + second.g + second.b))/9.0;
						////////////////////

						//////// SSD
						ssd += deltaR*deltaR + deltaG*deltaG + deltaB*deltaB;
						
						ssdDen1 += (first.r*first.r);
						ssdDen1 += (first.g*first.g);
						ssdDen1 += (first.b*first.b);

						ssdDen2 += (second.r*second.r);
						ssdDen2 += (second.g*second.g);
						ssdDen2 += (second.b*second.b);

						//// gray phase
						// float u = sqrt(3)/3;
						// float absvalue = sqrt(deltaR*deltaR + deltaG*deltaG + deltaB*deltaB);

						phase += colorToSaturation(deltaR, deltaG, deltaB);

						// tmp = (deltaR + deltaG + deltaB)*u/absvalue;
						// phase += fabs(tmp); // acos(tmp)/PI);
												
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			
		/*	if (var > _varMax)
			{
				_varMax = var;
				ACE_OS::printf("Aggiornati max: %lf\tmin: %lf\t %lf\n", _varMax, _varMin, nElem);			
			}
			if (var<_varMin)
			{
				_varMin = var;
				ACE_OS::printf("Aggiornati max: %lf\tmin: %lf\t %lf\n", _varMax, _varMin, nElem);			
			}*/
			
			_phase[k] = 1-(phase/nElem);

			// _varFunct[k] = 1 - var/sqrt(varDen1*varDen2);
			_varFunct[k] = sqrt(var/_maxCount);
			_ssdFunct[k] = ssd/ssdDen1;
		}
		else
		{
			_phase[k] = 0;
		}
	}

	
	double varFunctMean=0, ssdFunctMean=0;
	int shiftLevelsTrue=0;
	for (k=0; k<_shiftLevels; k++)
	{
		if (_count[k] == _maxCount)
		{
			shiftLevelsTrue++;
			varFunctMean+=_varFunct[k];
			ssdFunctMean+=_ssdFunct[k];
		}
	}
	varFunctMean/=shiftLevelsTrue;
	ssdFunctMean/=shiftLevelsTrue;

	for (k=0; k<_shiftLevels; k++)
	{
		if (_count[k] == _maxCount) {
			_varFunct[k]=1-0.5*_varFunct[k]/varFunctMean;
			_ssdFunct[k]=1-0.5*_ssdFunct[k]/ssdFunctMean;
			if (_varFunct[k] < 0)
				_varFunct[k] = 0;
			else if (_varFunct[k] > 1)
				_varFunct[k] = 1;
			
			if (_ssdFunct[k] < 0)
				_ssdFunct[k] = 0;
			else if (_ssdFunct[k] > 1)
				_ssdFunct[k] = 1;

			double a;
			//if (_varFunct[k]>=0.5 && _ssdFunct[k]>=0.5) {
				a=_varFunct[k]<_ssdFunct[k]?_varFunct[k]:_ssdFunct[k];
				a-=0.5;
			//} else
			//	a=0;
			if (a<0) 
				_corrFunct[k] = pow(_varFunct[k]*_ssdFunct[k],3*(-a)+1);
			else	
				_corrFunct[k] = pow(_varFunct[k]*_ssdFunct[k],1/(3*a+1));
		} else {
			_ssdFunct[k] = 0;
			_varFunct[k] = 0;
			_corrFunct[k] = 0;
		}
	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeSSDRGBVar (YARPImageOf<YarpPixelBGR> & inRImg,
   										YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	double den1, den2;
	
	int nElem;
	YarpPixelRGBFloat first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		ssd = 0.0;
		den1 = 0.0;
		den2 = 0.0;
		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						double tmp;
						tmp = (first.r-second.r)*(first.r-second.r);
						tmp += (first.g-second.g)*(first.g-second.g);
						tmp += (first.b-second.b)*(first.b-second.b);
						tmp /= 3.0;
						ssd += tmp;

						tmp = first.r-second.r;
						tmp += first.g-second.g;
						tmp += first.b-second.b;
						tmp /= 3;
						
						ssd -= tmp*tmp;
						
						////////// den1
						tmp = (first.r)*(first.r);
						tmp += (first.g)*(first.g);
						tmp += (first.b)*(first.b);
						tmp /= 3.0;
						den1 += tmp;

						tmp = first.r;
						tmp += first.g;
						tmp += first.b;
						tmp /= 3;
						den1 -= tmp*tmp;
						////////////////////

						////////// den2
						tmp = (second.r)*(second.r);
						tmp += (second.g)*(second.g);
						tmp += (second.b)*(second.b);
						tmp /= 3.0;
						den2 += tmp;

						tmp = second.r;
						tmp += second.g;
						tmp += second.b;
						tmp /= 3;
						den2 -= tmp*tmp;
						////////////////////
						
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = 1-ssd/sqrt(den1*den2);
		}
		else
		_corrFunct[k] = 0;

	}

	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeSSDRGBTH (YARPImageOf<YarpPixelBGR> & inRImg,
   												YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	int underTH;
	int nElem;
	YarpPixelRGBFloat first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		underTH=0;
		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						ssd = 0.0;
						
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						ssd += fabs(first.r-second.r);
						ssd += fabs(first.g-second.g);
						ssd += fabs(first.b-second.b);

						if (ssd<=50) underTH++;
						
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = (float) underTH/_maxCount;
		}
		else
		_corrFunct[k] = 0;

	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeABSRGB (YARPImageOf<YarpPixelBGR> & inRImg,
   												YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	double den1,den2;
	int nElem;
	YarpPixelRGBFloat first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		ssd = 0.0;
		den1 = 0.0;
		den2 = 0.0;
		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						ssd += fabs(first.r-second.r);
						ssd += fabs(first.g-second.g);
						ssd += fabs(first.b-second.b);

						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = 1-ssd/(255*3*nElem);
		}
		else
		_corrFunct[k] = 0;

	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeRGB (YARPImageOf<YarpPixelBGR> & inRImg,
											YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	rgbPixel aFov, aFull;

	aFull = computeAvg(inRImg.GetHeight(),inRImg.GetWidth(),inRImg.GetPadding(),(unsigned char*)inRImg.GetRawBuffer());
	aFov  = computeAvg(inLImg.GetHeight(),inLImg.GetWidth(),inLImg.GetPadding(),(unsigned char*)inLImg.GetRawBuffer());

	YarpPixelRGBFloat avX, avY;
	int nElem;

	int i,j,k,k1;
	int i2,i1;//iR,iL

	double rxy[4];
	double rxx[4];
	double ryy[4];
	int pX[3];
	int pY[3];

	memset(rxx, 0, 4*sizeof(double));
	memset(ryy, 0, 4*sizeof(double));
	memset(rxy, 0, 4*sizeof(double));

	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char *) inRImg.GetRawBuffer();
	fovPtr = (unsigned char *) inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		// reset counters
		memset(rxx, 0, 4*sizeof(double));
		memset(ryy, 0, 4*sizeof(double));
		memset(rxy, 0, 4*sizeof(double));

		k1 = k * _imgS.Size_LP; //Positioning on the table

		fullPtr = (unsigned char *) inRImg.GetRawBuffer();
		fovPtr = (unsigned char *) inLImg.GetRawBuffer();

		avX.r = 0;
		avX.g = 0;
		avX.b = 0;

		avY.r = 0;
		avY.g = 0;
		avY.b = 0;
		nElem = 0;

		if (_count[k] == _maxCount)
		{/*
			/////////
			// firstly compute average
			for (j=0; j<_actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						avX.r += *fovPtr;
						fovPtr++;
						avX.g += *fovPtr;
						fovPtr++;
						avX.b += *fovPtr;
						fovPtr++;
						
						avY.r += fullPtr[i2];
						avY.g += fullPtr[i2+1];
						avY.b += fullPtr[i2+2];
						nElem++;
					}
					else fovPtr +=3;
					}
				fovPtr+=AddedPadSize;
			}

			aFov.Red = avX.r/nElem;
			aFov.Gre = avX.g/nElem;
			aFov.Blu = avX.b/nElem;
			aFull.Red = avY.r/nElem;
			aFull.Gre = avY.g/nElem;
			aFull.Blu = avY.b/nElem;*/

			/////////

			k1 = k * _imgS.Size_LP; //Positioning on the table
			fullPtr = (unsigned char *) inRImg.GetRawBuffer();
			fovPtr = (unsigned char *) inLImg.GetRawBuffer();

			for (j=0; j<_actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						pX[0] = *fovPtr++ - aFov.Red;
						pY[0] = fullPtr[i2] - aFull.Red;

						pX[1] = *fovPtr++ - aFov.Gre;
						pY[1] = fullPtr[i2+1] - aFull.Gre;
						
						pX[2] = *fovPtr++ - aFov.Blu;
						pY[2] = fullPtr[i2+2] - aFull.Blu;

					
						rxy[0] += (-pX[0]*pY[0] - pX[1]*pY[1] - pX[2]*pY[2]);
						rxy[1] += (pX[1]*pY[2] - pX[2]*pY[1]);
						rxy[2] += (pX[2]*pY[0] - pX[0]*pY[2]);
						rxy[3] += (pX[0]*pY[1] - pX[1]*pY[0]);

						rxx[0] += (-pX[0]*pX[0] - pX[1]*pX[1] - pX[2]*pX[2]);
						rxx[1] += (pX[1]*pX[2] - pX[2]*pX[1]);
						rxx[2] += (pX[2]*pX[0] - pX[1]*pX[2]);
						rxx[3] += (pX[0]*pX[1] - pX[0]*pX[0]);

						ryy[0] += (-pY[0]*pY[0] - pY[1]*pY[1] - pY[2]*pY[2]);
						ryy[1] += (pY[1]*pY[2] - pY[2]*pY[1]);
						ryy[2] += (pY[2]*pY[0] - pY[0]*pY[2]);
						ryy[3] += (pY[0]*pY[1] - pY[1]*pY[0]);
/*
						rxy[0] += (-pX[0]*pY[0] - pX[1]*pY[1] - pX[2]*pY[2]);
						rxy[1] += (pX[1]*pY[2] - pX[2]*pY[1]);
						rxy[2] += (pX[0]*pY[1] - pX[1]*pY[0]);
						rxy[3] += (pX[2]*pY[0] - pX[0]*pY[2]);

						rxx[0] += (-pX[0]*pX[0] - pX[1]*pX[1] - pX[2]*pX[2]);
						rxx[1] += (pX[1]*pX[2] - pX[2]*pX[1]);
						rxx[2] += (pX[0]*pX[1] - pX[1]*pX[0]);
						rxx[3] += (pX[2]*pX[0] - pX[0]*pX[2]);

						ryy[0] += (-pY[0]*pY[0] - pY[1]*pY[1] - pY[2]*pY[2]);
						ryy[1] += (pY[1]*pY[2] - pY[2]*pY[1]);
						ryy[2] += (pY[0]*pY[1] - pY[1]*pY[0]);
						ryy[3] += (pY[2]*pY[0] - pY[0]*pY[2]);*/
					}
					else fovPtr +=3;
					}
				fovPtr+=AddedPadSize;
			}

			double absRxySq=rxy[0]*rxy[0] + rxy[1]*rxy[1] + rxy[2]*rxy[2] + rxy[3]*rxy[3];  
			double absV = sqrt(rxy[1]*rxy[1] + rxy[2]*rxy[2] + rxy[3]*rxy[3]);  
			double absRxxSq=rxx[0]*rxx[0] + rxx[1]*rxx[1] + rxx[2]*rxx[2] + rxx[3]*rxx[3];  
			double absRyySq=ryy[0]*ryy[0] + ryy[1]*ryy[1] + ryy[2]*ryy[2] + ryy[3]*ryy[3];  

			_coeff[k]  = (absRxySq/(sqrt(absRxxSq)*sqrt(absRyySq)));
			_phase[k] = (atan2(absV,rxy[0])/PI);
			_corrFunct[k] = _coeff[k]*_phase[k];
		}
		else
		{
			_coeff[k]  = 0;
			_phase[k] = 0;
			_corrFunct[k] = 0;
		}
	}
	

	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeRGBAv (YARPImageOf<YarpPixelBGR> & inRImg,
											YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	YarpPixelRGBFloat avx;
	YarpPixelRGBFloat avy;

	int i,j,k,k1;
	int i2,i1;//iR,iL
	int nElem;

	double rxy[4];
	double rxx[4];
	double ryy[4];
	
	double quatAvXY[4];
	double quatAvYY[4];
	double quatAvXX[4];
	
	int pX[3];
	int pY[3];

	memset(rxx, 0, 4*sizeof(double));
	memset(ryy, 0, 4*sizeof(double));
	memset(rxy, 0, 4*sizeof(double));
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char *) inRImg.GetRawBuffer();
	fovPtr = (unsigned char *) inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		// reset counters
		memset(rxx, 0, 4*sizeof(double));
		memset(ryy, 0, 4*sizeof(double));
		memset(rxy, 0, 4*sizeof(double));
		
		avx.r = 0;
		avx.g = 0;
		avx.b = 0;

		avy.r = 0;
		avy.g = 0;
		avy.b = 0;
		nElem = 0;

		k1 = k * _imgS.Size_LP; //Positioning on the table

		fullPtr = (unsigned char *) inRImg.GetRawBuffer();
		fovPtr = (unsigned char *) inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			for (j=0; j<_actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						pX[0] = *fovPtr++;
						pX[1] = *fovPtr++;
						pX[2] = *fovPtr++;
						
						pY[0] = fullPtr[i2];
						pY[1] = fullPtr[i2+1];
						pY[2] = fullPtr[i2+2];
						
						avx.r += pX[0];
						avx.g += pX[1];
						avx.b += pX[2];

						avy.r += pY[0];
						avy.g += pY[1];
						avy.b += pY[2];
					
						rxy[0] += (-pX[0]*pY[0] - pX[1]*pY[1] - pX[2]*pY[2]);
						rxy[1] += (pX[1]*pY[2] - pX[2]*pY[1]);
						rxy[2] += (pX[2]*pY[0] - pX[0]*pY[2]);
						rxy[3] += (pX[0]*pY[1] - pX[1]*pY[0]);

						rxx[0] += (-pX[0]*pX[0] - pX[1]*pX[1] - pX[2]*pX[2]);
						rxx[1] += (pX[1]*pX[2] - pX[2]*pX[1]);
						rxx[2] += (pX[2]*pX[0] - pX[1]*pX[2]);
						rxx[3] += (pX[0]*pX[1] - pX[0]*pX[0]);

						ryy[0] += (-pY[0]*pY[0] - pY[1]*pY[1] - pY[2]*pY[2]);
						ryy[1] += (pY[1]*pY[2] - pY[2]*pY[1]);
						ryy[2] += (pY[2]*pY[0] - pY[0]*pY[2]);
						ryy[3] += (pY[0]*pY[1] - pY[1]*pY[0]);
						nElem++;
					}
					else fovPtr +=3;
					}
				fovPtr+=AddedPadSize;
			}
			
			avx.r /= nElem;
			avx.g /= nElem;
			avx.b /= nElem;

			avy.r /= nElem;
			avy.g /= nElem;
			avy.b /= nElem;
			
			quatAvXY[0] = -nElem*(avx.r*avy.r + avx.g+avy.g+avx.b*avy.b);
			quatAvXY[1] = nElem*(avx.g*avy.b - avx.b+avy.g);
			quatAvXY[2] = nElem*(avx.b*avy.r - avx.r+avy.b);
			quatAvXY[3] = nElem*(avx.r*avy.g - avx.g+avy.r);
			
			quatAvXX[0] = -nElem*(avx.r*avx.r + avx.g+avx.g+avx.b*avx.b);
			quatAvXX[1] = nElem*(avx.g*avx.b - avx.b+avx.g);
			quatAvXX[2] = nElem*(avx.b*avx.r - avx.r+avx.b);
			quatAvXX[3] = nElem*(avx.r*avx.g - avx.g+avx.r);

			quatAvYY[0] = -nElem*(avy.r*avy.r + avy.g+avy.g+avy.b*avy.b);
			quatAvYY[1] = nElem*(avy.g*avy.b - avy.b+avy.g);
			quatAvYY[2] = nElem*(avy.b*avy.r - avy.r+avy.b);
			quatAvYY[3] = nElem*(avy.r*avy.g - avy.g+avy.r);

			rxy[0] -= quatAvXY[0];
			rxy[1] -= quatAvXY[1];
			rxy[2] -= quatAvXY[2];
			rxy[3] -= quatAvXY[3];

			rxx[0] -= quatAvXX[0];
			rxx[1] -= quatAvXX[1];
			rxx[2] -= quatAvXX[2];
			rxx[3] -= quatAvXX[3];

			ryy[0] -= quatAvYY[0];
			ryy[1] -= quatAvYY[1];
			ryy[2] -= quatAvYY[2];
			ryy[3] -= quatAvYY[3];

			double absRxySq=rxy[0]*rxy[0] + rxy[1]*rxy[1] + rxy[2]*rxy[2] + rxy[3]*rxy[3];  
			double absV = sqrt(rxy[1]*rxy[1] + rxy[2]*rxy[2] + rxy[3]*rxy[3]);  
			double absRxxSq=rxx[0]*rxx[0] + rxx[1]*rxx[1] + rxx[2]*rxx[2] + rxx[3]*rxx[3];  
			double absRyySq=ryy[0]*ryy[0] + ryy[1]*ryy[1] + ryy[2]*ryy[2] + ryy[3]*ryy[3];  

			_coeff[k]  = (absRxySq/(sqrt(absRxxSq)*sqrt(absRyySq)));
			_phase[k] = (atan2(absV,rxy[0])/PI);
			_corrFunct[k] = _coeff[k]; //*_phase[k];
		}
		else
		{
			_coeff[k]  = 0;
			_phase[k] = 0;
			_corrFunct[k] = 0;
		}
	}
	

	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeSSDWorstCase (YARPImageOf<YarpPixelBGR> & inRImg,
   											    	YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	int nElem;
	YarpPixelRGBFloat ssd;
	YarpPixelRGBFloat den1,den2;

	YarpPixelRGBFloat first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char *) inRImg.GetRawBuffer();
	fovPtr = (unsigned char *) inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		fullPtr = (unsigned char *) inRImg.GetRawBuffer();
		fovPtr = (unsigned char *) inLImg.GetRawBuffer();

		ssd.r = 0.0;
		ssd.g = 0.0;
		ssd.b = 0.0;

		den1.r = 0.0;
		den1.g = 0.0;
		den1.b = 0.0;
		
		den2.r = 0.0;
		den2.g = 0.0;
		den2.b = 0.0;
		
		nElem = 0;
		
		if (_count[k] == _maxCount)
		{
			for (j=0; j<_actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);
					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						ssd.r += (first.r-second.r)*(first.r-second.r);
						ssd.g += (first.g-second.g)*(first.g-second.g);
						ssd.b += (first.b-second.b)*(first.b-second.b);
					
						den1.r += (first.r)*(first.r);
						den1.g += (first.g)*(first.g);
						den1.b += (first.b)*(first.b);

						den2.r += (second.r)*(second.r);
						den2.g += (second.g)*(second.g);
						den2.b += (second.b)*(second.b);
						
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			
			YarpPixelRGBFloat corrTmp;
			corrTmp.r = 1-ssd.r/sqrt(den1.r*den1.r);
			corrTmp.g = 1-ssd.g/sqrt(den1.g*den1.g);
			corrTmp.b = 1-ssd.b/sqrt(den1.b*den1.b);
		
			// find worst case
			double min1;
			double min2;
			if (corrTmp.r < corrTmp.g)
				min1 = corrTmp.r;
			else
				min1 = corrTmp.g;
	
			if (corrTmp.g < corrTmp.b)
				min2 = corrTmp.g;
			else
				min2 = corrTmp.b;

			if (min1<min2)
				_corrFunct[k] = min1;
			else
				_corrFunct[k] = min2;
		}
		else
		{
			_corrFunct[k] = 0;
		}
	}

	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeSSDMono (YARPImageOf<YarpPixelBGR> & inRImg,
   											YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double ssd;
	double den1,den2;
	int nElem;
	YarpPixelRGBFloat first, second;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		ssd = 0.0;
		den1 = 0.0;
		den2 = 0.0;
		nElem = 0;
		
		if (_count[k] == _maxCount)
		{
			for (j=0; j<_actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						double gray1 = (first.r+first.g+first.b)/3.0;
						double gray2 = (second.r+second.g+second.b)/3.0;

						ssd += (gray1-gray2)*(gray1-gray2);
						den1 += (gray1*gray1);
						den2 += (gray2*gray2);
						
						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}

			// corr[k] = 1-ssd/sqrt(den1*den2);	old normalization
			_corrFunct[k] = 1-ssd/sqrt(den1*den1);
		}
		else
			_corrFunct[k] = 0;
	}

	// search max
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeRGBVar2 (YARPImageOf<YarpPixelBGR> & inRImg,
   									  YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double tmp;
	double delta_mean;
	double absdelta_mean;
	double tot;
	int nElem;
	YarpPixelRGBFloat first, second, delta;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			
			tot = 0;

			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						delta.r = first.r-second.r;
						delta.g = first.g-second.g;
						delta.b = first.b-second.b;
						
						delta_mean = (delta.r+delta.g+delta.b)/3;
						absdelta_mean = (fabs(delta.r)+fabs(delta.g)+fabs(delta.b))/3;
						tmp = (delta.r*delta.r+delta.g*delta.g+delta.b*delta.b)/3;
						tmp -= (delta_mean*delta_mean);

						if (tmp<0) tmp=0;

						tot += (sqrt(tmp) + absdelta_mean)/2;

						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = 1-tot/(64*_maxCount);
		}
		else
		_corrFunct[k] = 0;

	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeRGBChainVar (YARPImageOf<YarpPixelBGR> & inRImg,
   									  YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double tot;
	double tot2;
	double tmp;
	int nElem;
	YarpPixelRGBFloat first, second, delta;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			
			tot = 0;
			tot2 = 0;

			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						delta.r = first.r-second.r;
						delta.g = first.g-second.g;
						delta.b = first.b-second.b;
						
						tmp=(fabs(delta.r)+fabs(delta.r-delta.g)+fabs(delta.g-delta.b)+fabs(delta.b))/2;
						tot+=tmp;
						tot2+=tmp*tmp;

						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = 1-sqrt(tot2/_maxCount-tot*tot/(_maxCount*_maxCount))/64;
		}
		else
		_corrFunct[k] = 0;

	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

int YARPDisparityTool::computeRGBChain (YARPImageOf<YarpPixelBGR> & inRImg,
   									  YARPImageOf<YarpPixelBGR> & inLImg, double *value)
{
	int i,j,k,k1;
	int i2,i1;//iR,iL

	double tot;
	int nElem;
	YarpPixelRGBFloat first, second, delta;
	
	unsigned char * fullPtr,* fovPtr;

	fullPtr = (unsigned char*)inRImg.GetRawBuffer();
	fovPtr = (unsigned char*)inLImg.GetRawBuffer();

	int tIndex;

	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	for (k=0; k<_shiftLevels; k++)
	{

		k1 = k * _imgS.Size_LP; //Positioning on the table

		nElem = 0;
		fullPtr = (unsigned char*)inRImg.GetRawBuffer();
		fovPtr = (unsigned char*)inLImg.GetRawBuffer();

		if (_count[k] == _maxCount)
		{
			
			tot = 0;

			for (j=0; j< _actRings; j++)
			{
				tIndex = j*_imgS.Size_Theta;
				for (i=0; i<_imgS.Size_Theta; i++)
				{
					i2 = _shiftMap[k1 + tIndex+i];
					i1 = 3 * (tIndex+i);

					if (i2 > 0)
					{
						double maxC, minC;
						
						first.r = fovPtr[0];
						first.g = fovPtr[1];
						first.b = fovPtr[2];
					
						second.r = fullPtr[i2];
						second.g = fullPtr[i2+1];
						second.b = fullPtr[i2+2];
						
						delta.r = first.r-second.r;
						delta.g = first.g-second.g;
						delta.b = first.b-second.b;

						maxC=delta.r;
						if (maxC<delta.g)
							maxC=delta.g;
						else if (maxC<delta.b)
							maxC=delta.b;

						minC=delta.r;
						if (minC>delta.g)
							minC=delta.g;
						else if (maxC>delta.b)
							minC=delta.b;
						
						if (fabs(maxC-minC)/2>fabs(maxC))
							tot += fabs(maxC-minC)/2;
						else
							tot += fabs(maxC);

						nElem++;
						fovPtr+=3;
					}
					else fovPtr +=3;
				}
				fovPtr+=AddedPadSize;
			}
			_corrFunct[k] = 1-tot/(64*_maxCount);
		}
		else
		_corrFunct[k] = 0;

	}
	
	// search max
	_findShift(_corrFunct, _shiftLevels);
	_findSecondMax(_corrFunct, _shiftLevels, _shifts[0]);

	//printf("Max at %d, second max at %d\n", _shift, secondMax);
		
	int tmpShift = filterMaxes();
	*value = (double) _corrFunct[tmpShift];
	
	return _shiftToDisparity(tmpShift);
}

void YARPDisparityTool::plotRegion(const YARPImageOf<YarpPixelBGR> &imageRight, YARPImageOf<YarpPixelBGR> &out, int shift)
{
	int i,j,k1;
	int i1,i2;
	int tIndex;

	unsigned char *fullPtr;
	unsigned char *outPtr;
	fullPtr = (unsigned char *) imageRight.GetRawBuffer();
	outPtr = (unsigned char *) out.GetRawBuffer();
	
	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	k1 = shift * _imgS.Size_LP; //Positioning on the table

	for (j=0; j<_actRings; j++)
	{
		tIndex = j*_imgS.Size_Theta;
		for (i=0; i<_imgS.Size_Theta; i++)
		{
			i2 = _shiftMap[k1 + tIndex+i];
			i1 = 3 * (tIndex+i);
			if (i2 > 0)
				{
					*outPtr = fullPtr[i2];
					outPtr++;
					*outPtr = fullPtr[i2+1];
					outPtr++;
					*outPtr = fullPtr[i2+2];
					outPtr++;
				}
			else outPtr +=3;
		}
		outPtr+=AddedPadSize;
	}
}

void YARPDisparityTool::plotCorrelationMap(const YARPImageOf<YarpPixelBGR> &inRight, const YARPImageOf<YarpPixelBGR> &inLeft, YARPImageOf<YarpPixelBGR> &out, int shift)
{
	int i,j,k1;
	int i1,i2;
	int tIndex;
	
	unsigned char *fullPtr;
	unsigned char *fovPtr;
	unsigned char *outPtr;
	fullPtr = (unsigned char *) inRight.GetRawBuffer();
	outPtr = (unsigned char *) out.GetRawBuffer();
	fovPtr = (unsigned char *) inLeft.GetRawBuffer();

	YarpPixelRGBFloat temp1, temp2;
	double nElem = 0.0;
	double av1 = 0.0;
	double av2 = 0.0;
	double var1 = 0.0;
	double var2 = 0.0;
	
	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	k1 = shift * _imgS.Size_LP; //Positioning on the table
	
	// compute average and variance
	for (j=0; j<_actRings; j++)
	{
		tIndex = j*_imgS.Size_Theta;
		for (i=0; i<_imgS.Size_Theta; i++)
		{
			i2 = _shiftMap[k1 + tIndex+i];
			i1 = 3 * (tIndex+i);
			if (i2 > 0)
				{
					temp1.r = fullPtr[i2];
					temp1.g = fullPtr[i2+1];
					temp1.b = fullPtr[i2+2];
					
					temp2.r = *fovPtr++;
					temp2.g = *fovPtr++;
					temp2.b = *fovPtr++;

					double gray1;
					double gray2;
					gray1 = (temp1.r+temp1.g+temp1.b)/3;
					gray2 = (temp2.r+temp2.g+temp2.b)/3;

					av1 += gray1;
					av2 += gray2;

					var1 += (gray1*gray1);
					var2 += (gray2*gray2);
						
					nElem++;
				}
			else 
			{
				fovPtr += 3;
			}
		}
		fovPtr+=AddedPadSize;
	}

	if (nElem> 0)
	{
		av1 = av1/nElem;
		av2 = av2/nElem;
		var1 = (1/nElem)*(var1)-(av1*av1);
		var2 = (1/nElem)*(var2)-(av2*av2);
	}
	else
	{
		av1 = 0;
		av2 = 0;
		var1 = 0;
		var2 = 0;
	}
	
	fovPtr = (unsigned char *) inLeft.GetRawBuffer();

	// compute map
	for (j=0; j<_actRings; j++)
	{
		tIndex = j*_imgS.Size_Theta;
		for (i=0; i<_imgS.Size_Theta; i++)
		{
			i2 = _shiftMap[k1 + tIndex+i];
			i1 = 3 * (tIndex+i);
			if (i2 > 0)
				{
					temp1.r = fullPtr[i2];
					temp1.g = fullPtr[i2+1];
					temp1.b = fullPtr[i2+2];
					
					temp2.r = fovPtr[0];
					temp2.g = fovPtr[1];
					temp2.b = fovPtr[2];

					double gray1;
					double gray2;
					gray1 = (temp1.r + temp1.g + temp1.b)/3;
					gray2 = (temp2.r + temp2.g + temp2.b)/3;

					// double c = (gray1-av1)*(gray2-av2)/(128*128);
					double c = (gray1-av1)*(gray2-av2)/(sqrt(var1*var2));
					if (c>1) c = 1;
					if (c<-1) c = -1;
										
					if (c>0)
					{
						outPtr[0] = unsigned char (c*255 + 0.5);
						outPtr[1] = 0;
						outPtr[2] = 0;
					}
					else
					{
						outPtr[0] = 0;
						outPtr[1] = 0;
						outPtr[2] = unsigned char (-c*255 + 0.5);
					}

					outPtr+=3;
					fovPtr+=3;
				}
			else 
			{
				outPtr +=3;
				fovPtr += 3;
			}
		}
		outPtr+=AddedPadSize;
		fovPtr+=AddedPadSize;
	}
}

void YARPDisparityTool::plotSSDMap(const YARPImageOf<YarpPixelBGR> &inRight, const YARPImageOf<YarpPixelBGR> &inLeft, YARPImageOf<YarpPixelBGR> &out, int shift)
{
	int i,j,k1;
	int i1,i2;
	int tIndex;
	
	unsigned char *fullPtr;
	unsigned char *fovPtr;
	unsigned char *outPtr;
	fullPtr = (unsigned char *) inRight.GetRawBuffer();
	outPtr = (unsigned char *) out.GetRawBuffer();
	fovPtr = (unsigned char *) inLeft.GetRawBuffer();

	YarpPixelRGBFloat temp1, temp2;
	double nElem = 0.0;
	double den = 0.0;
		
	int AddedPadSize = computePadSize(_imgS.Size_Theta*_imgS.LP_Planes,_imgS.padding) - _imgS.Size_Theta*_imgS.LP_Planes;

	k1 = shift * _imgS.Size_LP; //Positioning on the table
	
	den = 0.0;
	// compute average and variance
	for (j=0; j<_actRings; j++)
	{
		tIndex = j*_imgS.Size_Theta;
		for (i=0; i<_imgS.Size_Theta; i++)
		{
			i2 = _shiftMap[k1 + tIndex+i];
			i1 = 3 * (tIndex+i);
			if (i2 > 0)
				{
					temp1.r = *fovPtr++;
					temp1.g = *fovPtr++;
					temp1.b = *fovPtr++;

					temp2.r = fullPtr[i2];
					temp2.g = fullPtr[i2+1];
					temp2.b = fullPtr[i2+2];

					//temp1.norm();
					//temp2.norm();
					
					den+=temp1.r*temp1.r;
					den+=temp1.g*temp1.g;
					den+=temp1.b*temp1.b;
											
					nElem++;
				}
			else 
			{
				fovPtr += 3;
			}
		}
		fovPtr+=AddedPadSize;
	}

	fovPtr = (unsigned char *) inLeft.GetRawBuffer();

	// compute map
	for (j=0; j<_actRings; j++)
	{
		tIndex = j*_imgS.Size_Theta;
		for (i=0; i<_imgS.Size_Theta; i++)
		{
			i2 = _shiftMap[k1 + tIndex+i];
			i1 = 3 * (tIndex+i);
			if (i2 > 0)
				{
					temp2.r = fullPtr[i2];
					temp2.g = fullPtr[i2+1];
					temp2.b = fullPtr[i2+2];
					
					temp1.r = fovPtr[0];
					temp1.g = fovPtr[1];
					temp1.b = fovPtr[2];

					//temp1.norm();
					//temp2.norm();

					double c;
					c = (temp1.r-temp2.r)*(temp1.r-temp2.r);
					c += (temp1.g-temp2.g)*(temp1.g-temp2.g);
					c += (temp1.b-temp2.b)*(temp1.b-temp2.b);

					c /= den;
					c *= nElem;
					if (c>=1) c = 1;
					
					// double ssd = 1-log(1/(1+c))/log(0.5);
					double ssd = 1-c;
					// c/=sqrt(den*den);

					outPtr[0] = (unsigned char) (ssd*255 + 0.5);
					outPtr[1] = 0;
					outPtr[2] = 0;
									
					outPtr+=3;
					fovPtr+=3;
				}
			else 
			{
				outPtr +=3;
				fovPtr += 3;
			}
		}
		outPtr+=AddedPadSize;
		fovPtr+=AddedPadSize;
	}
}

void YARPDisparityTool::_findSecondMax(const double *corr, int lv, int posMax)
{
	// search max
	double corrMax;
	int shMin;
	int max1;
	int max2;

	int ret = 0;
	int l;

	if (_shiftMax<lv)
		lv = _shiftMax;
	shMin = _shiftMin;		// = 0
	

	l = posMax-1;
	while(l>shMin && corr[l]<=corr[l+1]+0.02)
		l--;
	
	corrMax=corr[l];
	max1=l;
	for(; l>=shMin; l--)
	{
		if (corr[l]>corrMax)
		{
			corrMax = corr[l];
			max1 = l;
		}
	}

	l = posMax+1;
	while(l<lv && 0.02+corr[l-1]>=corr[l])
		l++;

	corrMax=corr[l];
	max2 = l;
	for(; l<lv; l++)
	{
		if (corr[l]>corrMax)
		{
			corrMax = corr[l];
			max2 = l;
		}
	}

	if (max1>max2)
	{
		_shifts[1] = max1;
		_shifts[2] = max2;
	}
	else
	{
		_shifts[2] = max1;
		_shifts[1] = max2;
	}

	_shifts[3] = zeroShift();
}

int YARPDisparityTool::init(int rings, float ratio)
{
	using namespace _logpolarParams;
	_imgL = lpInfo (_xsize, _ysize, _srho, _stheta, _sfovea, 1090, 1.00, YarpImageAlign);
	_imgS = lpInfo (_xsize, _ysize, _srho, _stheta, _sfovea, 1090, ratio, YarpImageAlign);
	_imgFovea = lpInfo (_xsize, _ysize, _srho, _stheta, _sfovea, 1090, ratio, YarpImageAlign, 4);
	_actRings = int (rings/ratio + 0.5);
	loadShiftTable();
	loadDSTable();
	loadRemapTable();
	_computeCountVector(_count);

	_shiftMax = _shiftLevels;
	_shiftMin = 0;
	return YARP_OK;
}

int YARPDisparityTool::filterMaxes()
{
	int maxIndex;
	int closest;
	
	float threshold = 0.2f;
	float threshold2 = 0.1f;
	float vals[__nShifts+1];

	for (int i = 0; i < __nShifts-1; i++)
	{
		vals[i] = _corrFunct[_shifts[i]];
		
		if ( !isWithin(_shifts[i], _shiftMax, _shiftMin) || (vals[i]<threshold) )
			vals[i] = -1;
	}
	vals[__nShifts-1] = 0;

	// find closest
	closest = 0;
	int zs = zeroShift();
	for (i = 0; i < (__nShifts-1); i++)
	{
		if (abs(_shifts[i]-zs)<(_shifts[closest]-zs))
			closest = i;
	}

	// find higher, starting from closest, so that if no best matches are found we go 
	// to the closest peak
	maxIndex = closest;
	for (i = 0; i < __nShifts; i++)
	{
		if (vals[i]>vals[maxIndex]+threshold2)
			maxIndex = i;
	}

	return _shifts[maxIndex];
}