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
/// $Id: YARPDisparity.cpp,v 1.13 2004-01-16 23:08:34 babybot Exp $
///
///

// disparity.cpp: implementation of the YARPDisparityTool class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPDisparity.h"

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
	_shiftMap		= NULL;
	_corrFunct		= NULL;
	_actRings		= 21;
}

YARPDisparityTool::~YARPDisparityTool()
{
	if (_shiftFunction != NULL)
		delete [] _shiftFunction;

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
				_shiftMap[k*1*Par->Size_LP] = j;

				if (_shiftFunction[k]+3*Par->Resolution/4 == j)
					k++;
			}
		}
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

int YARPDisparityTool::computeDisparity (YARPImageOf<YarpPixelBGR> & inLImg,
										 YARPImageOf<YarpPixelBGR> & inRImg)
{
	int disparity;

	rgbPixel avgLeft,avgRight;

	avgLeft  = computeAvg(inLImg.GetHeight(),inLImg.GetWidth(),inLImg.GetPadding(),(unsigned char*)inLImg.GetRawBuffer());
	avgRight = computeAvg(inRImg.GetHeight(),inRImg.GetWidth(),inRImg.GetPadding(),(unsigned char*)inRImg.GetRawBuffer());

//	if (inLImg.GetHeight() == _imgS.Size_Rho)
//		disparity = Shift_and_Corr(	(unsigned char*)inLImg.GetRawBuffer(),
//									(unsigned char*)inRImg.GetRawBuffer(),
//									&_imgS,
//									_shiftLevels,
//									_shiftMap,
//									_corrFunct);
//	else
		disparity = shiftnCorrFovea((unsigned char*)inLImg.GetRawBuffer(),
									(unsigned char*)inRImg.GetRawBuffer(),
									&_imgS,
									_shiftLevels,
									_shiftMap,
									_corrFunct,
									avgLeft,
									avgRight,
									_actRings);

	disparity = _shiftFunction[disparity];

	disparity = (int)(0.5 + disparity * _imgS.Size_X_Remap / (float)_imgS.Resolution);
	return disparity;
}

void YARPDisparityTool::makeHistogram(YARPImageOf<YarpPixelMono>& hImg)
{
	int i,j;
	int height = 64;
	int width = 256;
	unsigned char * hist = (unsigned char *)hImg.GetRawBuffer();

	int offset = (width-_shiftLevels+1)/2;

	for (j=0;j<height*width;j++)
		hist[j] = 0;

	float sum=0;
	for (i=0; i<_shiftLevels-1; i++)
		sum += _corrFunct[i];
	sum /= (_shiftLevels-1);

	float MAX = 1000;

	for (i=0; i<_shiftLevels-1; i++)
	{
		if (_corrFunct[i]<MAX)
		{
			MAX = _corrFunct[i];
		}
	}

	MAX = MAX/sum;

	MAX = height * (MAX);

	int value = height - (int) (height / 3.0*(3-sum));

	for (i=0; i<_shiftLevels-1; i++)
	{
		if ((i+offset >=0)&&(i+offset<width))
		{
			for (j=height-(int)(height/3.0*(3-_corrFunct[i])); j<height; j++)
				{
					hist[(j*width+i+offset)] = 128;
				}
		}
	}

	for (i=0; i<width; i++)
	{
		hist [value*width+i] = 192;
		hist [(int)MAX*width+i] = 64;
	}

		
	for (j=0; j<height; j++)
		hist[(j*width+width/2)] = 255;
}
