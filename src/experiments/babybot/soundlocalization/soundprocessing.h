// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  soundprocessing.h
// 
//     Description:  Declaration of the SoundProcessing class
// 
//         Version:  $Id: soundprocessing.h,v 1.1 2004-04-07 15:51:52 beltran Exp $
// 
//          Author:  Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __soundprocessinghh__
#define __soundprocessinghh__

#include <YARPMatrix.h>
#include <YARPPidFilter.h>
#include <YARPString.h>
#include <YARPSound.h>
#include <YARPLogPolar.h>

class SoundProcessing
{
public:
	SoundProcessing(const YARPString &iniFile, int outsize);
	~SoundProcessing();

	void apply(const YARPSoundBuffer &in, YVector &out)
	{
		//----------------------------------------------------------------------
		// Here the soundprocesing should be done. 
		//----------------------------------------------------------------------
	}

private:
	void _threshold(double *v, double th)
	{
		// Surely, this is not necessary
	}

	int _inSize;
	int _outSize;
	//----------------------------------------------------------------------
	// sound variables 
	//----------------------------------------------------------------------
	int _Channels;
	int _SamplesPerSec;
	int _BitsPerSample;
	int _BufferLength;

	YARPString _iniFile;
	YARPString _path;
};

#endif
