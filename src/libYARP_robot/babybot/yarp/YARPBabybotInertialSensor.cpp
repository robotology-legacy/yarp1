/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
///  $Id: YARPBabybotInertialSensor.cpp,v 1.2 2004-07-28 21:46:21 babybot Exp $
///
///

#include "YARPBabybotInertialSensor.h"
#include <iostream>

using namespace std;

/**
 *
 *
 */
YARPLowPassFilter::YARPLowPassFilter(void)
{
	a[3]=0.0465;
	a[2]=0.1397;
	a[1]=0.1397;
	a[0]=0.0465;
	
	b[2]=0.1457;
	b[1]=-0.7245;
	b[0]=1.2045;
	
	c[3]=0.0013; // i coeff. c[] d[] ,usati per altra versione
	c[2]=0.0039; // del filtro
	c[1]=0.0039;
	c[0]=0.0013;
	
	d[2]=0.6241;
	d[1]=-2.1653;
	d[0]=2.5308;
	
	out3=0;

	memset (in, 0, sizeof(double) * 4);
	memset (out, 0, sizeof(double) * 4);
}

double YARPLowPassFilter::filter(double value)
{
	const int n=3;
	
	in[n-3]=in[n-2];
	in[n-2]=in[n-1];
	in[n-1]=in[n];
	in[n]=value;
	
	for(int h = 0; h < 3; h++)
	{
		out[h]=out[h+1];
	}

	out[n]=a[0]*in[n]+a[1]*in[n-1]+a[2]*in[n-2]+a[3]*in[n-3]+b[0]*out[n-1]+b[1]*out[n-2]+b[2]*out[n-3];

    return out[n];
}


/**
 *
 *
 */
YARPBabybotInertialSensor::YARPBabybotInertialSensor()
{
	_ns = 0;
	_parameters = NULL;
	_lowPass = NULL;
	_nIteration = 0;
	_isCalibrated = false;
	_skipCalibration = 0;
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

	// read calibration steps
	if (cfgFile.get("[GENERAL]", "CalibrationSteps", &_nsteps, 1) == YARP_FAIL)
		return YARP_FAIL;

	if (cfgFile.get("[GENERAL]", "SkipCalibration", &_skipCalibration) == YARP_FAIL)
		_skipCalibration = 0;
	
	if (_skipCalibration)
		_isCalibrated = true;
	else
		_isCalibrated = false;

	int i;
	for(i = 0; i<_ns; i++)
	{
		char dummy[80];
		sprintf(dummy, "%s%d", "Sensor", i);
		if (cfgFile.get("[PARAMETERS]", dummy, _parameters[i], SP_NumEntriesPerSensor) == YARP_FAIL)
			return YARP_FAIL;
	}

	// DEBUG, check calibration values
	YVector tmp(_ns);
	// if (cfgFile.get("[GENERAL]", "Calibration", tmp.data(), _ns) != YARP_FAIL)
	{
		// sensor is already calibrated
		for (int i = 1; i <= _ns; i++)
		{
			double *p0 = _parameters[i - 1];
			cout << p0[SP_Offset];
			cout << "\t";
		}
		cout << "\n";
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
	fprintf(fp, "SkipCalibration %d\n", _skipCalibration);
	fprintf(fp, "CalibrationSteps %d\n\n", _nsteps);
	fprintf(fp, "[PARAMETERS]\n");

	int i;
	for(i = 0; i < _ns; i++)
	{
		fprintf(fp, "%s%d ", "Sensor", i);

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