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