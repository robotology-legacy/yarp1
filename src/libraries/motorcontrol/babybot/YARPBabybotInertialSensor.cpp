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
///  $Id: YARPBabybotInertialSensor.cpp,v 1.4 2004-07-12 08:36:31 babybot Exp $
///
///

#include "YARPBabybotInertialSensor.h"


YARPBabybotInertialSensor::YARPBabybotInertialSensor()
{
	_ns = 0;
	_parameters = NULL;
	_lowPass = NULL;
	_nIteration = 0;
	_isCalibrated = false;
}

YARPBabybotInertialSensor::~YARPBabybotInertialSensor()
{
	int j;
	if (_parameters != NULL)
	{
		for (j = 0; j<_ns; j++)
		{
			if (_parameters[j]!=NULL)
				delete [] _parameters[j];
		}
		delete [] _parameters;
		_parameters = NULL;
	}

	if (_lowPass != NULL)
	{
		delete [] _lowPass;
		_lowPass = NULL;
	}

}

int YARPBabybotInertialSensor::load(const YARPString &path, const YARPString &init_file)
{
	YARPConfigFile cfgFile;
	// set path and filename
	cfgFile.set(path.c_str(),init_file.c_str());
		
	// get number of joints
	if (cfgFile.get("[GENERAL]", "Sensors", &_ns, 1) == YARP_FAIL)
		return YARP_FAIL;

	// delete and allocate new memory
	_realloc(_ns);

	// try to read calibration values
	YVector tmp(_ns);
	if (cfgFile.get("[GENERAL]", "Calibration", tmp.data(), _ns) != YARP_FAIL)
	{
		// sensor is already calibrated
		for (int i = 1; i <= _ns; i++)
		{
			double *p0 = _parameters[i - 1];
			p0[SP_Offset] = tmp(i);
		}
		_isCalibrated = true;
	}
			
	// read calibration steps
	if (cfgFile.get("[GENERAL]", "CalibrationSteps", &_nsteps, 1) == YARP_FAIL)
		return YARP_FAIL;

	int i;
	for(i = 0; i<_ns; i++)
	{
		char dummy[80];
		sprintf(dummy, "%s%d", "Sensor", i);
		if (cfgFile.get("[PARAMETERS]", dummy, _parameters[i], SP_NumEntriesPerSensor) == YARP_FAIL)
			return YARP_FAIL;
	}
		
	return YARP_OK;
}

int YARPBabybotInertialSensor::save(const YARPString &path, const YARPString &init_file)
{
	YARPString file;
	file = path;
	file.append(init_file);

	FILE *fp = fopen(init_file.c_str(), "w");

	fprintf(fp, "[GENERAL]\n");
	fprintf(fp, "Sensors %d\n", _ns);
	fprintf(fp, "Calibration ");

	int i;
	for(i = 0; i < _ns; i++)
	{
		double *p0 = _parameters[i];
		fprintf(fp, "%lf ", p0[SP_Offset]);
	}
	fprintf(fp, "\n");
		
	fprintf(fp, "CalibrationSteps %d\n\n", _nsteps);
	fprintf(fp, "[PARAMETERS]\n");

	for(i = 0; i < _ns; i++)
	{
		fprintf(fp, "%s%d", "Sensor", i);

		int j;
		for(j = 0; j < SP_NumEntriesPerSensor; j++)
			fprintf(fp, "%lf ", _parameters[i][j]);
		fprintf(fp, "\n");
	}

	fclose(fp);		
	return YARP_OK;
}

void YARPBabybotInertialSensor::_realloc(int ns)
{
	int j;
	if (_parameters != NULL)
	{
		for (j = 0; j<_ns; j++)
		{
			if (_parameters[j]!=NULL)
				delete [] _parameters[j];
		}
		delete [] _parameters;
		_parameters = NULL;
	}

	if (_lowPass != NULL)
	{
		delete [] _lowPass;
		_lowPass = NULL;
	}
		
	_ns = ns;
		
	_parameters = new double *[ns];
	for (j = 0; j<ns; j++)
		_parameters[j] = new double [SP_NumEntriesPerSensor];

	_lowPass = new YARPLowPassFilter [ns];

	// YVectors
	_signal.Resize(ns);
	_signalConverted.Resize(ns);
	_rawSignal.Resize(ns);
	_averageTmp.Resize(ns);

	_signal = 0.0;
	_signalConverted = 0.0;
	_rawSignal = 0.0;
	_averageTmp = 0.0;
}

bool YARPBabybotInertialSensor::calibrate(const short *input, int *p)
{
	if (_isCalibrated)
		return true;

	_nIteration++;
	for (int i = 1; i <= _ns; i++)
	{
		double *p0 = _parameters[i - 1];
		int meiCh = (int) p0[SP_MeiChannel];
		double tmpIn = (double) input[meiCh];
		_signal(i) = _lowPass[i - 1].filter (tmpIn) / p0[SP_ScaleFactor];

		if (_nIteration >= 500)
		{
			_averageTmp(i) += _signal(i);
		}
				
		if (_nIteration >= _nsteps)
		{
			p0[SP_Offset] = _averageTmp(i) / (_nsteps - 500 + 1);
			_isCalibrated = true;
		}

	}

	if (p != NULL)
	{
		double tmp;
		tmp = ((double) _nIteration/_nsteps)*100 + 0.5;
		if (tmp>100.0)
			tmp = 100.0;

		*p = (int) (tmp+0.5);
	}

	return _isCalibrated;
}

int YARPBabybotInertialSensor::convert(const short *input, double *output)
{
	if (!_isCalibrated)
		return YARP_FAIL;
	
	for (int i = 1; i <= _ns; i++)
	{
		double *p0 = _parameters[i - 1];
		int meiCh = (int) p0[SP_MeiChannel];
		double tmpIn = (double) input[meiCh];

		_rawSignal(i) = tmpIn / p0[SP_ScaleFactor];
	    _signal(i) = (_lowPass[i - 1].filter(tmpIn)) / p0[SP_ScaleFactor];
		_signalConverted(i) = _doConvert(_signal(i), p0[SP_Offset], p0[SP_Ka], p0[SP_DeadBand]);

		output[i-1] = _signalConverted(i);
	}		

	return YARP_OK;
}