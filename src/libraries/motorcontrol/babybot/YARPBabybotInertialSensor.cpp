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

	if (cfgFile.get("[GENERAL]", "CalibrationSteps", &_nsteps, 1) == YARP_FAIL)
		return YARP_FAIL;

	// delete and allocate new memory
	_realloc(_ns);
		
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