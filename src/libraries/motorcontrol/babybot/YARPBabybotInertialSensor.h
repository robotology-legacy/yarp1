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
///  $Id: YARPBabybotInertialSensor.h,v 1.3 2004-01-17 00:15:15 gmetta Exp $
///
///

#ifndef __YARPBABYBOTINERTIALSENSOR__
#define __YARPBABYBOTINERTIALSENSOR__

#include <YARPMatrix.h>
#include <YARPLowPassFilter.h>
#include <YARPConfigFile.h>
#include <YARPString.h>

/////////////////////////////////////////////////////////////////////////////
// These are definitions for the inertial sensor(s).
enum sensorType	
{
	ST_Angular = 0, 
	ST_Linear = 1
};

const int SP_NumEntriesPerSensor = 6;

enum sensorPrms
{
	SP_MeiChannel = 0,
	SP_SensorType = 1,
	SP_ScaleFactor = 2,
	SP_Ka = 3,
	SP_DeadBand = 4,
	SP_Offset = 5
};

class YARPBabybotInertialSensor
{
public:
	YARPBabybotInertialSensor();
	~YARPBabybotInertialSensor();
	
	int load(const YARPString &path, const YARPString &init_file);

	int convert(const short *input, double *output);
	bool calibrate(const short *input, int *p = NULL);
		
	inline double _doConvert(double volt2, double offset2, double ka2, double db);
	inline bool calibrated()
	{ return _isCalibrated; }
	
friend class YARPMEIOnBabybotHeadAdapter;

private:
	void _realloc(int ns);
	

	int _ns;
	int _nsteps;
	double **_parameters;

	// old parameters
	bool _isCalibrated;		// calibrato?? 

	YVector	_signal;			// measured linear+angular info (+ LP).
	YVector _signalConverted;	// segnale convertito.
	YVector _rawSignal;			// segnale letto dall'ADC.
	YVector _averageTmp;		// average tmp used during calibration

	int _nIteration;

	YARPLowPassFilter *_lowPass;
};

inline double YARPBabybotInertialSensor::_doConvert(double volt2, double offset2, double kap2, double db)
{
	double dif = volt2 - offset2;

	if (fabs(dif) < db) 
		return 0.0;
	else	
		return dif * kap2;
}

#endif