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
///  $Id: YARPBabybotHandParameters.h,v 1.6 2004-01-17 00:15:15 gmetta Exp $
///
///

// originally feb 2003 -- by nat
// adapted to yarp June 2003 -- by nat

#ifndef __YARPBABYBOTHANDPARAMETERSH__
#define __YARPBABYBOTHANDPARAMETERSH__

#include <YARPGalilDeviceDriver.h>
#include <YARPNIDAQDeviceDriver.h> // required to define int16

#include <YARPMath.h>

namespace _BabybotHand
{
	// these values are (almost) written in stone

	const int __naj = 6;		// #motors
	const int __nj = 8;			// galil # joints
	const int __nidaq_ch = 15;	// hall sensors

	const double __coupling[__naj][__naj] = {
		{1.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 1.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 1.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 1.0, 1.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 1.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 1.0, 1.0},
	};

	const double __de_coupling[__naj][__naj] = {
		{1.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 1.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 1.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, -1.0, 1.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 1.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, -1.0, 1.0},
	};

	const double __encoderToAngles[__naj] = {-0.015*degToRad, -0.0127*degToRad, -0.03*degToRad, -0.03*degToRad, 0.03*degToRad, 0.03*degToRad};

}; // namespace

class YARPBabybotHandParameters
{
public:
	YARPBabybotHandParameters()
	{
		_naj = _BabybotHand::__naj;
		_nj = _BabybotHand::__nj;
		_nidaq_ch = _BabybotHand::__nidaq_ch;

		_mask = char (0xFF);
	
		_galil_id = 0;
		_nidaq_id = 0;
		_nidaq_input_mode = 0;
		_nidaq_input_polarity = 0;

		memcpy(_coupling, _BabybotHand::__coupling, sizeof(double)*_naj*_naj);
		memcpy(_de_coupling, _BabybotHand::__de_coupling, sizeof(double)*_naj*_naj);

		memcpy(_encoderToAngles, _BabybotHand::__encoderToAngles, sizeof(double)*_naj);
	}

	~YARPBabybotHandParameters()
	{
		
	}

	int load(const YARPString &path, const YARPString &init_file)
	{
		YARPConfigFile cfgFile;
		// set path and filename
		cfgFile.set(path.c_str(),init_file.c_str());
		
		// GALIL
		int nJ;
		// get number of joints
		if (cfgFile.get("[GALIL]", "N", &nJ, 1) == YARP_FAIL)
			return YARP_FAIL;
		// check number of joints
		if (nJ != _naj)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "AxesLut", _axes_lut, _naj) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.getHex("[GALIL]", "Mask", &_mask, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "Id", &_galil_id, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "VectorLut", _vector_lut, _naj) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "Speeds", _speeds, _naj) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "Accelerations", _accelerations, _naj) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "IntLimits", _int_limits, _naj) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "TorqueLimits", _torque_limits, _naj) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "TorqueThresholds", _torque_thresholds, _naj) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GALIL]", "MaxTime", &_max_time, 1) == YARP_FAIL)
			return YARP_FAIL;

		int i;
		for(i = 0; i<_naj; i++)
		{
			char dummy[80];
			double tmp[12];
			sprintf(dummy, "%s%d", "Pid", i);
			if (cfgFile.get("[PID]", dummy, tmp, 12) == YARP_FAIL)
				return YARP_FAIL;
			_highPIDs[i] = LowLevelPID(tmp);
		}

		// NIDAQ
		int nH;
		// get number of joints
		if (cfgFile.get("[NIDAQ]", "N", &nH, 1) == YARP_FAIL)
			return YARP_FAIL;
		// check expected number of sensors
		if (nH != _nidaq_ch)
			return YARP_FAIL;

		if (cfgFile.get("[NIDAQ]", "Id", &_nidaq_id, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[NIDAQ]", "Mode", &_nidaq_input_mode, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[NIDAQ]", "Polarity", &_nidaq_input_polarity, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[NIDAQ]", "Sequence", _nidaq_ch_seq, _nidaq_ch) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[NIDAQ]", "Gains", _nidaq_gains, _nidaq_ch) == YARP_FAIL)
			return YARP_FAIL;

		return YARP_OK;
	}

public:
	int _naj;
	int _nj;	// Galil total joints
	char _mask;
		
	i16 _galil_id;
	i16 _nidaq_ch;	
	i16 _nidaq_id;
	i16 _nidaq_input_mode;
	i16 _nidaq_input_polarity;
	i16 _nidaq_ch_seq[_BabybotHand::__nidaq_ch];
	i16 _nidaq_gains[_BabybotHand::__nidaq_ch];

	// vectorialized and single axis commands use different lut
	int _axes_lut[_BabybotHand::__naj];
	int _vector_lut[_BabybotHand::__naj];

	double _speeds[_BabybotHand::__naj];
	double _accelerations[_BabybotHand::__naj];

	double _int_limits[_BabybotHand::__naj];
	double _torque_limits[_BabybotHand::__naj];

	int _max_time;
	double _torque_thresholds[_BabybotHand::__naj];

	double _coupling[_BabybotHand::__naj][_BabybotHand::__naj];
	double _de_coupling[_BabybotHand::__naj][_BabybotHand::__naj];
	double _encoderToAngles[_BabybotHand::__naj];

	LowLevelPID _highPIDs[_BabybotHand::__naj];
};

#endif