// 
// InertialControl.h
//

#ifndef __InertialControl__
#define __InertialControl__

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "YARPSafeNew.h"

#include "isd.h"

//
// This class is far too simple right now.
// LATER: extend to cover all deamon methods.
// 
class YARPInertialSensor 
{
protected:
	int isdx;

	clock_t timestamp;
	double old_y;
	double old_p;
	double old_r;
		
public:
	YARPInertialSensor (void)
	{
	}

	virtual ~YARPInertialSensor ()
	{
	}

	inline int Initialize (void);
	inline int Initialize (const char *isd_name);
	inline int Uninitialize (void) { return 0; }
	inline int SetOutputMode (int ispolled, int isascii);
	inline int GetReading (double *yaw, double *pitch, double *roll);
	inline int GetVelocity(double *y, double *p, double *r);
	inline int GetAll (double *yaw, double *pitch, double *roll,
					   double *ydot, double *pdot, double *rdot);
	inline int GetAll (double *inertial);
};

//
// inlines
inline int YARPInertialSensor::Initialize (void)
{
	Initialize ("/yarp/isd");
	return 0;
}

inline int YARPInertialSensor::Initialize (const char *isd_name)
{
	int stat = isd_initialize((char *)isd_name, &isdx);
	if (stat != 0)
	{
		printf("Failed to locate InterSense daemon... Error code %d!\n", stat);
		return -1;
	}
  
	// set gyro to binary mode
	stat = isd_set_output_mode(isdx, TRUE, FALSE);

	old_y = 0;
	old_p = 0;
	old_r = 0;
	timestamp = clock();

	return 0;
}

inline int YARPInertialSensor::SetOutputMode (int ispolled, int isascii)
{
	//return isd_set_output_mode(isdx, TRUE, FALSE);
	return isd_set_output_mode(isdx, ispolled, isascii);
}

inline int YARPInertialSensor::GetReading (double *yaw, double *pitch, double *roll)
{
	float yy, pp, rr;
	int stat = isd_poll_all(isdx, &yy, &pp, &rr);
	*yaw = double(yy);
	*pitch = double(pp);
	*roll = double(rr);

	timestamp = clock();
	if (stat >= 0)
	{
		old_y = *yaw;
		old_p = *pitch;
		old_r = *roll;
	}

	return stat;
}

inline int YARPInertialSensor::GetVelocity(double *y, double *p, double *r)
{
	float yaw, pitch, roll;
	int stat = isd_poll_all(isdx, &yaw, &pitch, &roll);
	clock_t now = clock();

	*y = (yaw - old_y) / (now - timestamp) * CLOCKS_PER_SEC;
	*p = (pitch - old_p) / (now - timestamp) * CLOCKS_PER_SEC;
	*r = (roll - old_r) / (now - timestamp) * CLOCKS_PER_SEC;

	old_y = yaw;
	old_p = pitch;
	old_r = roll;
	timestamp = now;

	return 0;
}

inline int YARPInertialSensor::GetAll (double *yaw, 
									double *pitch, 
									double *roll,
									double *ydot, 
									double *pdot, 
									double *rdot)
{
	float y, p, r;
	int stat = isd_poll_all(isdx, &y, &p, &r);
	clock_t now = clock();

	*yaw = y;
	*pitch = p;
	*roll = r;

	*ydot = (*yaw - old_y) / (now - timestamp) * CLOCKS_PER_SEC;
	*pdot = (*pitch - old_p) / (now - timestamp) * CLOCKS_PER_SEC;
	*rdot = (*roll - old_r) / (now - timestamp) * CLOCKS_PER_SEC;

	old_y = *yaw;
	old_p = *pitch;
	old_r = *roll;
	timestamp = now;

	return 0;
}

inline int YARPInertialSensor::GetAll (double *inertial)
{
	assert (inertial != NULL);

	float y, p, r;
	int stat = isd_poll_all(isdx, &y, &p, &r);
	clock_t now = clock();

	inertial[0] = y;
	inertial[1] = p;
	inertial[2] = r;

	inertial[3] = (inertial[0] - old_y) / (now - timestamp) * CLOCKS_PER_SEC;
	inertial[4] = (inertial[1] - old_p) / (now - timestamp) * CLOCKS_PER_SEC;
	inertial[5] = (inertial[2] - old_r) / (now - timestamp) * CLOCKS_PER_SEC;

	old_y = inertial[0];
	old_p = inertial[1];
	old_r = inertial[2];
	timestamp = now;

	return 0;
}

#endif