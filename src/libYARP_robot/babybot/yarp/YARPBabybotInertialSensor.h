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
///  $Id: YARPBabybotInertialSensor.h,v 1.3 2004-09-03 14:43:24 babybot Exp $
///
///

#ifndef __YARPBABYBOTINERTIALSENSOR__
#define __YARPBABYBOTINERTIALSENSOR__

#include <yarp/YARPMatrix.h>
///#include <yarp/YARPLowPassFilter.h>
#include <yarp/YARPConfigFile.h>
#include <yarp/YARPString.h>

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

/**
 * Simple digital low-pass filter for the inertial cube data.
 * This class shouldn't be part of the interface of this library: it's for
 * internal use only.
 */
class YARPLowPassFilter
{
private:
	YARPLowPassFilter(const YARPLowPassFilter&);
	void operator=(const YARPLowPassFilter&);

    double a[4];   
	double b[3];
    double c[4];
	double d[3];
  
	double in[4];
	double out[4];

	double out3;

public:
	YARPLowPassFilter(void);
	virtual ~YARPLowPassFilter(void) {}

	double filter(double value);
};

/**
 * A wrapper class to handle the intertial cube through the MEI card.
 */
class YARPBabybotInertialSensor
{
public:
	YARPBabybotInertialSensor();
	~YARPBabybotInertialSensor();
	
	int load(const YARPString &path, const YARPString &init_file);
	int save(const YARPString &path, const YARPString &init_file);

	int convert(const short *input, double *output);
	bool calibrate(const short *input, int *p = NULL);
		
	inline double _doConvert(double volt2, double offset2, double ka2, double db);
	inline bool calibrated() { return _isCalibrated; }

	double getOffsets(int i)
	{
		double *p0 = _parameters[i - 1];
		return p0[SP_Offset];
	}

	friend class YARPMEIOnBabybotHeadAdapter;

private:
	void _realloc(int ns);
	

	int _ns;
	int _nsteps;
	double **_parameters;

	// old parameters
	bool _isCalibrated;		// calibrato?? 
	int _skipCalibration;

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