// originally feb 2003 -- by nat
// adapted to yarp June 2003 -- by nat

// $Id: YARPBabybotHandParameters.h,v 1.2 2003-07-01 21:29:44 babybot Exp $

#ifndef __YARPBABYBOTHANDPARAMETERSH__
#define __YARPBABYBOTHANDPARAMETERSH__

#include <YARPGalilDeviceDriver.h>
#include <YARPNIDAQDeviceDriver.h> // required to define int16

namespace _BabybotHand
{
	const int __naj = 6;
	const int __nj = 8;
	const int __nidaq_ch = 15;

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
	}

	~YARPBabybotHandParameters()
	{
		
	}

	int load(const std::string &path, const std::string &init_file)
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

		if (cfgFile.get("[GALIL]", "Mask", &_mask, 1) == YARP_FAIL)
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

	LowLevelPID _highPIDs[_BabybotHand::__naj];
};

#endif