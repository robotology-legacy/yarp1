// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundprocessing.cpp
// 
//     Description:  Implements all the sound processing algorithms.
// 
//         Version:  $Id: soundprocessing.cpp,v 1.1 2004-04-07 15:51:52 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos), cbeltran@dist.unige.it
//         Company:  Lira-Lab
// 
// =====================================================================================

#include "soundprocessing.h"
#include <YARPConfigFile.h>

SoundProcessing::SoundProcessing(const YARPString &iniFile, int outsize)
{
	YARPConfigFile file;
	char *root = GetYarpRoot();

	_Channels      = 2;
	_SamplesPerSec = 44100;
	_BitsPerSample = 16;
	_BufferLength  = 8192;
	_iniFile       = iniFile;
	_outSize       = outsize;

	_path.append(root);
	_path.append("/conf/babybot/"); 
	
	file.set(_path, _iniFile);
	file.get("[GENERAL]", "Channels"     , &_Channels     , 1);
	file.get("[GENERAL]", "SamplesPerSec", &_SamplesPerSec, 1);
	file.get("[GENERAL]", "BitsPerSample", &_BitsPerSample, 1);
	file.get("[GENERAL]", "BufferLength" , &_BufferLength , 1);
}

SoundProcessing::~SoundProcessing()
{
}
