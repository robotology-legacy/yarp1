
// 
// HeadControl.cpp implementation of head control classes.
//

#include "K4HeadControl.h"

#define DEBUG 0

//temporary for debugging
FILE *outfp = fopen ("/home/lijin/output/out", "wb");

//
// A single instance of the meid handler.
int YARPGenericControl::meidx = -1;
int YARPGenericControl::m_instances = 0;
YARPSemaphore YARPGenericControl::m_mutex(1);		// default = 1

//
//
//


YARPGenericControl::YARPGenericControl ()
{
}

YARPGenericControl::~YARPGenericControl ()
{
}

// class YARPGenericControl
int YARPGenericControl::Initialize (void)
{
	return Initialize (DEFAULTDEVICENAME);
}

int YARPGenericControl::Initialize (const char *mei_name)
{
	Lock ();
	if (m_instances == 0)
	{
		//stat = meid_dsp_init((char *)mei_name, &meidx);
		meidx = meid_open((char *)mei_name);
		if (meidx == -1)
		{
			printf("Failed to locate MEI daemon %s!\n", mei_name);
			Unlock ();
			return -1;
		}
		int stat = meid_dsp_reset(meidx);
		if (stat != 0)
		{
			printf("MEI Reset Failed... Error code %d!\n", stat);
			Unlock ();
			return -1;
		}
	}

	m_instances ++;
	Unlock ();

	return 0;
}

int YARPGenericControl::Uninitialize ()
{
	// close every single instance (copy) of meidx.
	Lock ();
	meid_close (meidx);
	m_instances--;
	Unlock ();

	return 0;
}

//
// Shared data. Multithread access to be protected by a MUTEX!
//
double * YARPHeadControl::m_old_pos = NULL;
clock_t  YARPHeadControl::m_timestamp = 0;

int16 ** YARPHeadControl::m_boot_filter = NULL;
int16 ** YARPHeadControl::m_control_params = NULL;

int *	 YARPHeadControl::m_enabled = NULL;
bool *   YARPHeadControl::m_calibrated = NULL;

double * YARPHeadControl::m_limits_max = NULL;
double * YARPHeadControl::m_limits_min = NULL;
double * YARPHeadControl::m_pad_max = NULL;
double * YARPHeadControl::m_pad_min = NULL;

double * YARPHeadControl::m_default_vel = NULL;
double * YARPHeadControl::m_default_acc = NULL;
double * YARPHeadControl::m_default_jerk = NULL;
double * YARPHeadControl::m_position = NULL;
double * YARPHeadControl::m_position_zero = NULL;
double * YARPHeadControl::m_velocity = NULL;
double * YARPHeadControl::m_calibrate_offset = NULL;
double * YARPHeadControl::m_max_error = NULL;
double * YARPHeadControl::m_etoangles = NULL;

bool	 YARPHeadControl::m_shutdown_flag = false;
double   YARPHeadControl::m_tilt_store = 0;

int		 YARPHeadControl::m_njoints = 0;
int		 YARPHeadControl::m_eyetilt = 0;
int		 YARPHeadControl::m_righteye = 0;
int		 YARPHeadControl::m_lefteye = 0;
int              YARPHeadControl::m_neckcrane = 0;
int		 YARPHeadControl::m_neckpan = 0;
int		 YARPHeadControl::m_necktilt = 0;

int		 YARPHeadControl::m_headtilt = 0;
int		 YARPHeadControl::m_neckroll = 0;

EHeadControlMode YARPHeadControl::m_mode = ModeVirtual;


//
//
// 
YARPHeadControl::YARPHeadControl ()
{
	// static vars are already initialized.
	// take care of the locals.

	m_set_cmd = new double[MaxAxes];
	m_set_vel = new double[MaxAxes];
	m_set_acc = new double[MaxAxes];
	assert (m_set_cmd != NULL &&
			m_set_vel != NULL &&
			m_set_acc != NULL);

	m_winding = new short[MaxAxes];
	m_16bit_oldpos = new double[MaxAxes];
	assert (m_winding != NULL && m_16bit_oldpos != NULL);
	memset (m_winding, 0, sizeof(short) * MaxAxes);
	memset (m_16bit_oldpos, 0, sizeof(double) * MaxAxes);

	// constants over instances.
	m_mode = ModeVirtual;
}


YARPHeadControl::~YARPHeadControl ()
{
	if (m_winding != NULL)
		delete[] m_winding;
	if (m_16bit_oldpos != NULL);
		delete[] m_16bit_oldpos;

	// free instance specific mem.
	if (m_set_cmd != NULL)
		delete[] m_set_cmd;
	if (m_set_vel != NULL)
		delete[] m_set_vel;
	if (m_set_acc != NULL)
		delete[] m_set_acc;

	m_set_cmd = NULL;
	m_set_vel = NULL;
	m_set_acc = NULL;

	m_winding = NULL;
	m_16bit_oldpos = NULL;
}

// if Initialize fails please exit from calling process.
int YARPHeadControl::Initialize (const char *name, const char *filename)
{
  
  if (YARPGenericControl::Initialize (name) < 0)
	{
		printf ("YARPHeadControl: base class init failed\n");
		return -1;
	}

	Lock ();

	// LATER: allocation might be moved after we know the number of joints!
	// initialization needs to be done only once.
	if (m_instances == 1)
	{
		// this is the first instance - m_instances correctly incremented 
		// by base class Initialize.
		assert (filename != NULL);

		// alloc mem.
		m_boot_filter = new int16 *[MaxAxes];
		m_control_params = new int16 *[MaxAxes];
		assert (m_boot_filter != NULL && m_control_params != NULL);

		for (int i = 0; i < MaxAxes; i++)
		{
			m_boot_filter[i] = new int16[COEFFICIENTS];
			m_control_params[i] = new int16[COEFFICIENTS];
			assert (m_boot_filter[i] != NULL && m_control_params[i]);

			memset (m_boot_filter[i], 0, sizeof(int16) * COEFFICIENTS);
			memset (m_control_params[i], 0, sizeof(int16) * COEFFICIENTS);
		}

		m_enabled = new int[MaxAxes];
		m_calibrate_offset = new double[MaxAxes];
		m_max_error = new double[MaxAxes];
		m_calibrated = new bool[MaxAxes];
		m_default_vel = new double[MaxAxes];
		m_default_acc = new double[MaxAxes];

		assert (m_enabled != NULL && 
				m_calibrate_offset != NULL &&
				m_max_error != NULL &&
				m_calibrated != NULL &&
				m_default_vel != NULL &&
				m_default_acc != NULL);

		memset (m_max_error, 0, sizeof(double) * MaxAxes);

		// limits stuff.
		m_limits_max = new double[MaxAxes];
		m_limits_min = new double[MaxAxes];
		m_pad_max = new double[MaxAxes];
		m_pad_min = new double[MaxAxes];

		assert (m_limits_max != NULL &&
				m_limits_min != NULL &&
				m_pad_max != NULL &&
				m_pad_min != NULL);

		memset (m_limits_max, 0, sizeof(double) * MaxAxes);
		memset (m_limits_min, 0, sizeof(double) * MaxAxes);
		memset (m_pad_max, 0, sizeof(double) * MaxAxes);
		memset (m_pad_min, 0, sizeof(double) * MaxAxes);

		m_position = new double[MaxAxes];
		m_position_zero = new double[MaxAxes];
		m_velocity = new double[MaxAxes];
		assert (m_position != NULL && m_velocity != NULL && m_position_zero != NULL);

		m_old_pos = new double[MaxAxes];
		assert (m_old_pos != NULL);

		m_etoangles = new double[MaxAxes];
		assert (m_etoangles != NULL);
		memset (m_etoangles, 0, sizeof(double) * MaxAxes);

		m_shutdown_flag = false;
		m_tilt_store = 0;

		m_njoints = 0;

		//read configuration file 		
		ReadConfig (filename);
	       
		for (i = 0; i < m_njoints; i++)
		{
			m_enabled[i] = FALSE;
			meid_error_check(meid_get_boot_filter(meidx, i, m_boot_filter[i]));
		  
		  		
		}

		//compare gain values in boot memory against values read from
		//config file
		int stat = CheckBootParams ();
	  
	  if (stat < 0)
		{
			printf ("WARNING!!!: MEI boot params do not match...\n");
			printf ("You need to raise the gains\n");
			m_shutdown_flag = true;
		}
		else
		{
		  printf("MEI Boot params MATCHES...\n");
			m_shutdown_flag = false;
		}

		m_tilt_store = double(TILTSTORE); // if == TILTSTORE... don't rely on this value.

		SetRobotSpecific();

}
else
  {
    // this is not the first instance.
    // we don't need to read the config file.
    printf("Not the first instance\n");
  }

Unlock ();

return meidx;
}

int YARPHeadControl::Initialize (void)
{
	return Initialize (DEFAULTDEVICENAME, DEFAULTFILENAME);
}

// eat all char until a \n is encountered.
void YARPHeadControl::EatLine (FILE *fp)
{
	// check if line begins with '//'
	for (;;)
	{
		char c1, c2;
		fread (&c1, 1, 1, fp);
		fread (&c2, 1, 1, fp);

		if (c1 == '/' && c2 == '/')
		{
			do
			{
				fread (&c1, 1, 1, fp);
			}
			while (c1 != '\n' && !feof(fp));
		}
		else
		{
			fseek (fp, -2, SEEK_CUR);
			return;		// exit from here.
		}
	}
}

/* this function reads parameters from configuration file */
void YARPHeadControl::ReadConfig (const char *filename)
{
  FILE *fp = fopen (filename, "r");
  assert (fp != NULL);
  
  // joints numbers
  EatLine (fp);
  fscanf (fp, "%d\n", &m_njoints);
  
  EatLine (fp);
  fscanf (fp, "%d\n", &m_necktilt); //0
  fscanf (fp, "%d ", &m_lefteye);  //1
  fscanf (fp, "%d ", &m_righteye);  //2
  fscanf (fp, "%d ", &m_eyetilt);  //3
  fscanf (fp, "%d ", &m_neckpan); //4
  fscanf (fp, "%d ", &m_neckroll); //5
  fscanf (fp, "%d ", &m_headtilt);  //6
  fscanf (fp, "%d ", &m_neckcrane); //7


	// control params
	EatLine (fp);
	for (int i = 0; i < m_njoints; i++)
	{
		for (int j = 0; j < COEFFICIENTS; j++)
		{
			fscanf (fp, "%hd ", &m_control_params[i][j]);
		}
		fscanf (fp, "\n");
	}

	// robot specific
	// offset for calibration.
	EatLine (fp);
	for (i = 0; i < m_njoints; i++)
	{
		fscanf (fp, "%lf ", &m_calibrate_offset[i]);
	}
	fscanf (fp, "\n");

	// default velocity
	EatLine (fp);
	for (i = 0; i < m_njoints; i++)
	{
		fscanf (fp, "%lf ", &m_default_vel[i]);
	}
	fscanf (fp, "\n");

	// range (this is subtracted from the max limit)
	EatLine (fp);
	for (i = 0; i < m_njoints; i++)
	{
		fscanf (fp, "%lf ", &m_pad_max[i]);
	}
	fscanf (fp, "\n");
	
	EatLine (fp);
	for (i = 0; i < m_njoints; i++)
	{
		fscanf (fp, "%lf ", &m_pad_min[i]);
	}
	fscanf (fp, "\n");

	EatLine (fp);
	for (i = 0; i < m_njoints; i++)
	{
		fscanf (fp, "%lf ", &m_max_error[i]);
	}
	fscanf (fp, "\n");

	EatLine (fp);
	for (i = 0; i < m_njoints; i++)
	{
		fscanf (fp, "%lf ", &m_etoangles[i]);
	}
	fscanf (fp, "\n");

  EatLine (fp);
  for (i = 0; i < m_njoints; i++)
    {
      fscanf (fp, "%lf ", &m_limits_max[i]);
    }
  fscanf (fp, "\n");
  
  EatLine (fp);
  for (i = 0; i < m_njoints; i++)
    {
              fscanf (fp, "%lf ", &m_limits_min[i]);
    }
  fscanf (fp, "\n");
  
  
	fclose (fp);
}

int YARPHeadControl::Uninitialize (void)
{
	Lock ();

	if (m_instances == 1)
	{
		// last instance... delete mem.
		if (m_etoangles != NULL)
			delete[] m_etoangles;
		if (m_old_pos != NULL)
			delete[] m_old_pos;

		if (m_position != NULL)
			delete[] m_position;
		if (m_position_zero != NULL)
			delete[] m_position_zero;
		if (m_velocity != NULL)
			delete[] m_velocity;

		if (m_limits_max != NULL)
			delete[] m_limits_max;
		if (m_limits_min != NULL)
			delete[] m_limits_min;
		if (m_pad_max != NULL)
			delete[] m_pad_max;
		if (m_pad_min != NULL)
			delete[] m_pad_min;

		if (m_enabled != NULL)
			delete[] m_enabled;
		if (m_calibrate_offset != NULL)
			delete[] m_calibrate_offset;
		if (m_max_error != NULL)
			delete[] m_max_error;
		if (m_calibrated != NULL)
			delete[] m_calibrated;
		if (m_default_vel != NULL)
			delete[] m_default_vel;
		if (m_default_acc != NULL)
			delete[] m_default_acc;

		for (int i = 0; i < MaxAxes; i++)
		{
			if (m_boot_filter[i] != NULL)
				delete[] m_boot_filter[i];
			if (m_control_params[i] != NULL)
				delete[] m_control_params[i];
		}

		if (m_boot_filter != NULL)
			delete[] m_boot_filter;
		if (m_control_params != NULL)
			delete[] m_control_params;
	}
	else
	{
		// any other instance.
		// do nothing.
	}

	Unlock ();

	YARPGenericControl::Uninitialize ();	

	return 0;
}

/* this function checks if gain values from boot memory are the same as the
   values read from config file */
int YARPHeadControl::CheckBootParams (void)
{
	int check = 0;
	for (int i = 0; i < m_njoints; i++)
	{

	  //for (int j = 0; j < COEFFICIENTS; j++)
	    //printf(" i=%d j=%d boot=%d file=%d",i,j,(int)m_boot_filter[i][j],(int)m_control_params[i][j]);
	  
	  
	  check |= memcmp (m_boot_filter[i], m_control_params[i], sizeof(int16) * COEFFICIENTS);
	}
	return (check == 0) ? 0 : -1;
}


int YARPHeadControl::SetRobotSpecific (void)
{
	
	//NOTE: why * 20?? head.ini indicated 10??
	for (int i = 0; i < m_njoints; i++)
		m_default_acc[i] = m_default_vel[i] * 20;

	//NOTE: initialize m_calibrated and m_position_zero to zeros
	memset (m_calibrated, 0, sizeof(bool) * MaxAxes);
	memset (m_position_zero, 0, sizeof(double) * MaxAxes);       
	return 0;
}


/*
//this function was used for calibrating cog's axes
// single axis standard calibration.
*/
int YARPHeadControl::Calibrate (int axis)
{
  
	//const double CalibrationSpeed = 5000;
	//const double CalibrationAcc = 50000;
  const double CalibrationSpeed = 1000;
  const double CalibrationAcc = 10000;
	//NOTE: Comment this out
	//if (axis == TopDifferential() || axis == BottomDifferential())
	//	return -1;

	if (axis < 0 || axis >= m_njoints)
		return -1; 

	// when moving no other commands should be sent to the head.
	double p1,p2;
	p1 = VelocityMove (axis, CalibrationSpeed, CalibrationAcc, m_max_error[axis]);
	p2 = VelocityMove (axis, -CalibrationSpeed, CalibrationAcc, m_max_error[axis]);

	meid_start_move (meidx, axis, (p1+p2)/2.0, CalibrationSpeed, CalibrationAcc);
	printf ("p1: %f p2: %f\n", p1, p2);
	printf ("Moving to %f\n", (p1+p2)/2.0);
	while (!meid_axis_done(meidx, axis)) delay(20);

	Lock ();
	//meid_zero_axis (meidx, axis);
	meid_set_position (meidx, axis, 0.0);
	
	printf ("Moving to offset: %d\n", double(m_calibrate_offset[axis]));
	meid_start_move (meidx, axis, double(m_calibrate_offset[axis]), CalibrationSpeed, CalibrationAcc);
	while (!meid_axis_done(meidx, axis)) delay(20);
	//meid_zero_axis (meidx, axis);
	meid_set_position (meidx, axis, 0.0);
	m_position_zero[axis] = double(meid_dsp_encoder (meidx, axis));
	
	// store this to take care of 16bit resolution.
	m_16bit_oldpos[axis] = m_position_zero[axis];
	m_winding[axis] = 0;

	// take into account that the zero might not be in the center.
	printf ("padding (non uniform limits) max: %f min: %f\n", m_pad_max[axis], m_pad_min[axis]); 
	double maxrange = (p1 - p2) / 2.0 - fabs(m_calibrate_offset[axis]);
	m_limits_max[axis] = maxrange - m_pad_max[axis];
	m_limits_min[axis] = -maxrange + m_pad_min[axis];

	printf ("Max range: %f\n", maxrange);
	printf ("Limits are %f %f\n", m_limits_max[axis], m_limits_min[axis]);

	m_calibrated[axis] = true;

	Unlock ();

	return 0;
}



/* this function resets all encoders to zero and set calibrated flag to 1 */
int YARPHeadControl::CalibrateAll(){
  for (int j = 0; j < m_njoints; j++)
	{
	    int stat = KismetStyleCalibrate(j);
	    if (stat < 0)
	      return stat;

	}
  
  //now set all m_calibrated to true
  for (int k = 0; k < m_njoints; k++)
    m_calibrated[k] = true;
  
	return 0;
}



int YARPHeadControl::KismetStyleCalibrate( int axis){

  //simply set encoder to zero
  //m_limits_max and min have been set in ReadConfig (from head.ini)
  //m_calibrated is set to true
  
  if (ZeroAxis(axis))
    {
      printf("Error in kismet style calibration of axis %d\n", axis);
      return -1;
    }
  printf("Kismet style calibration of axis %d is completed\n",axis);
  return 0;
}

// original Calibrate function
int YARPHeadControl::Calibrate (int numaxes, int *axes)
  {
            for (int i = 0; i < numaxes; i++)
      {
	                int j = axes[i];
	                int stat = Calibrate (j);
	                if (stat < 0)
	                          return stat;
      }
    
            return 0;
  }


//This function was used in Cog
//will have to be modified if to be used for K4
double YARPHeadControl::VelocityMove(int axis, double vel, double acc, double errlimit)
{
	//NOTE: why 1000000??
	const double FAST_ACCEL = 1000000.0;

	double err, pos;
	meid_error_check(meid_clear_status (meidx, axis));
	meid_error_check(meid_v_move(meidx, axis, vel, acc));
	
	delay (100);
	do
	{
		meid_get_error (meidx, axis, &err);
	}
	while (fabs(err) < errlimit);

	meid_error_check(meid_v_move (meidx, axis, 0.0, FAST_ACCEL));

	// LATER: we need meid_motion_done here.
	//	add it to the device driver.
	//
	//	while (!meid_motion_done (meidx, axis)) 
	//	{
	//		printf (".");
	//		delay (20);
	//	}
	//	printf ("\n");

	delay(100);

	meid_error_check(meid_get_position (meidx, axis, &pos));
	meid_error_check(meid_set_command (meidx, axis, pos));

	return pos;
}

inline int _sgn(double x) { return (x>0) ? 1 : -1; }

// this should be called from only one thread 
// (e.g. the low-level controlling thread).
// 
// 
 
// this function reads encoder values from meid
int YARPHeadControl::Input (void)
{
	Lock ();

	clock_t now = clock();

	for (int i = 0; i < m_njoints; i++)
	{
	  //not using this b/c too slow
	  // meid_get_position (meidx, i, &m_position[i]);
		
	  //read directly from dsp encoder instead b/c it's faster
	  //however it is only 16 bits, thus, will have to deal w/ values over
	  //32767
	  m_position[i] = double(meid_dsp_encoder (meidx, i));
		if (_sgn(m_position[i]) < 0 && m_16bit_oldpos[i] > 16383.0 && m_winding[i] == 0)
		{
			m_winding[i] = 1;
		}
		else
		if (_sgn(m_position[i]) > 0 && m_16bit_oldpos[i] < -16384.0 && m_winding[i] == 1)
		{
			m_winding[i] = 0;
		}
		else
		if (_sgn(m_position[i]) > 0 && m_16bit_oldpos[i] < -16384.0 && m_winding[i] == 0)
		{
			m_winding[i] = -1;
		}
		else
		if (_sgn(m_position[i]) < 0 && m_16bit_oldpos[i] > 16383.0 && m_winding[i] == -1)
		{
			m_winding[i] = 0;
		}

		m_16bit_oldpos[i] = m_position[i];

		switch (m_winding[i])
		{
			case 1:
				m_position[i] = 65535.0 + m_position[i] - m_position_zero[i];
				break;

			case -1:
				m_position[i] = -65535.0 + m_position[i] - m_position_zero[i];
				break;

			case 0:
				m_position[i] -= m_position_zero[i];
				break;
		}
	   

	  //if (i == 4)
	    //printf(" m_position= %f ",m_position[i]);
	  
	  
	  m_velocity[i] = (m_position[i] - m_old_pos[i]) / 
					(now - m_timestamp) * CLOCKS_PER_SEC;
	}

	m_timestamp = now;
	memcpy (m_old_pos, m_position, sizeof(double) * m_njoints);

	Unlock ();

	return 0;
}

//this function is currently called, but not doing anything b/c all m_etoangles
//are actually 1. in K4, conversion from rad to ticks & vice versa are done
//on the client side. see YAPOCMessage class.
int YARPHeadControl::GetActualPosition (int vaxis, double *position)
{
	if (vaxis < 0 || vaxis >= m_njoints)
	{
		*position = 0;
		return -1;
	}

	Lock ();
	*position = m_position[vaxis] * m_etoangles[vaxis];
	Unlock ();


	return 0;
}

int YARPHeadControl::GetActualPosition (double *position)
{
	assert (position != NULL);

	Lock ();
	for (int i = 0; i < m_njoints; i++)
		position[i] = m_position[i] * m_etoangles[i];
	Unlock ();

        //printf(" actual= %f \n", position[4]);
	return 0;
}

int YARPHeadControl::GetActualVelocity (int vaxis, double *velocity)
{
	if (vaxis < 0 || vaxis >= m_njoints)
	{
		*velocity = 0;
		return -1;
	}

	Lock ();
	*velocity = m_velocity[vaxis] * m_etoangles[vaxis];
	Unlock ();

	return 0;
}

int YARPHeadControl::GetActualVelocity (double *velocity)
{
	assert (velocity != NULL);

	Lock ();
	for (int i = 0; i < m_njoints; i++)
		velocity[i] = m_velocity[i] * m_etoangles[i];
	Unlock ();

	return 0;
}

//
void YARPHeadControl::_out_vector (int len, double *v)
  {
    for (int i = 0; i < len; i++)
      {
	printf ("%.4f ", v[i]);
      }
    printf ("\n");
  }

/* this function receives an array of 8 desired positions and statusbits.
for axes whose statusbit is 1 & has been calibrated, a move command is issued.
if statusbits is 1, software limits are checked. otherwise, it assumes that
this move command is calibration related, thus won't check for limits.
*/
int YARPHeadControl::PositionCommand (double *command, int *statusbits, int uselimit_flag)
  {
    Lock ();
    
    // interval is in second.
    assert (command != NULL);
    
    if (DEBUG) printf("Verified that command parameter is NOT null\n");

    int validcount = 0;
    double command_ticks;
    short axes[MaxAxes];
    
    for (int i = 0; i < m_njoints; i++){
      
      if (DEBUG) printf("status bit[%d] = %d\n",i,statusbits[i]);
            
      if ( !m_enabled[i] || statusbits[i] == OFF || (m_calibrated[i] == false && uselimit_flag == 1)){
	if (DEBUG) printf("Ignore axis %d\n", i);
      }
      else {
	
	  command_ticks = command[i]/m_etoangles[i];
	
	if (uselimit_flag == 1){
	  if (command_ticks > m_limits_max[i]){
	    if (DEBUG || i == 7) fprintf(outfp, "desired position = %f > max = %f\n", command_ticks, m_limits_max[i]);
	    //reduce command to max
            if (i == 7)
	      command_ticks = m_limits_max[i]-100;
	    else
	      command_ticks = m_limits_max[i]-1;
	  }
	  if (command_ticks < m_limits_min[i]){
	    if (DEBUG || i == 7) fprintf(outfp, "desired position = %f < min = %f\n", command_ticks, m_limits_min[i]);
	    //increase command to min
	    command_ticks = m_limits_min[i]+1;
	  }
	}
	
	m_set_cmd[validcount] = command_ticks;
	m_set_vel[validcount] = m_default_vel[i];
	m_set_acc[validcount] = m_default_acc[i];
	axes[validcount] = i;
	printf("Axis %d is moving to %f w/ vel = %f acc = %f\n",i,command_ticks,m_default_vel[i],m_default_acc[i]);
        fprintf(outfp, "Axis %d is moving to %f w/ vel = %f acc = %f\n",i,command_ticks,m_default_vel[i],m_default_acc[i]);
	validcount++;
      }
      
    }
    
    if (validcount > 0){ 
      if (DEBUG) printf("Executing meid_start_move_all, validcount = %d\n", validcount);
      int status = meid_start_move_all (meidx, validcount, axes, m_set_cmd, m_set_vel, m_set_acc);
      if (status) fprintf(outfp, "status after move_all = %d\n",status);
    }
    
    Unlock ();
    
    return 0;
  }

	
/* this function checks for whether all axes are done moving */
bool YARPHeadControl::AllDone (void)
{
	Lock ();

	bool ret = true;
	for (int i = 0; i < m_njoints; i++)
		ret &= (AxisDone (i) ? true : false);

	Unlock ();

	return ret;
}


int YARPHeadControl::CheckAxis (int i, double command, double interval)
{
	// command is gonna be speed.
	double estimated = m_position[i] + command * interval;
	if (estimated > m_limits_max[i] && command > 0 ||
		estimated < m_limits_min[i] && command < 0 ||
		m_calibrated == false)
		return -1;

	return 0;
}

/* this function was used in Cog.
   Will have to be modified if to be used in K4 */
int YARPHeadControl::VelocityCommand (double *command, double interval)
{
	Lock ();

	// interval is in second.
	assert (command != NULL);

	// uses m_position as starting point for checking.
	for (int i = 0; i < m_njoints; i++)
		if (CheckAxis (i, command[i] / m_etoangles[i], interval) < 0)
		{
			//if (i == HeadTilt() || i == HeadRoll())
			//	m_set_cmd[HeadTilt()] = m_set_cmd[HeadRoll()] = 0;

			m_set_cmd[i] = 0;
		}
		else
			m_set_cmd[i] = command[i] / m_etoangles[i];

		
  //DO WE NEED TO DO THIS ??
	// enforce vergence on limits.
	i = RightEye();
	if (CheckAxis (i, command[i] / m_etoangles[i], interval) < 0)
	{
		m_set_cmd[RightEye()] = m_set_cmd[LeftEye()] = 0;
	}
	i = LeftEye();
	if (CheckAxis (i, command[i] / m_etoangles[i], interval) < 0)
	{
		m_set_cmd[RightEye()] = m_set_cmd[LeftEye()] = 0;
	}

	//
	// check vergence limit.
	//
	const int vergence = m_position[LeftEye()] + m_set_cmd[LeftEye()] * interval - m_position[RightEye()] - m_set_cmd[RightEye()] * interval;
	const int delta_v = interval * (m_set_cmd[LeftEye()] - m_set_cmd[RightEye()]);
	if ((vergence < 1000 && delta_v < 0) ||
		(vergence > 20000 && delta_v > 0))
		m_set_cmd[LeftEye()] = m_set_cmd[RightEye()] = 0;

	// convert the command for the differential.
	//NOTE: Comment this out
	//ConvertToReal (m_set_cmd[HeadTilt()], m_set_cmd[HeadRoll()], &m_set_cmd[HeadTilt()], &m_set_cmd[HeadRoll()]);

	// this loop is stupid but keeps the internal representation
	// completely in virtual coordinates.
	for (i = 0; i < m_njoints; i++)
	{
		if (m_enabled[i] == true)
		{
			if (meid_frames_left (meidx, i) == FALSE)
				meid_v_move (meidx, i, m_set_cmd[i], m_default_acc[i]);
		}
		else
		{
			// send 0.
			if (meid_frames_left (meidx, i) == FALSE)
				meid_v_move (meidx, i, 0.0, m_default_acc[i]);
		}
	}

	Unlock ();

	return 0;
}

int YARPHeadControl::SetDefaultVelocity (double *velocity)
{
	assert (velocity != NULL);
	//memcpy (m_default_vel, velocity, sizeof(double) * m_njoints);
	Lock ();
	for (int i = 0; i < m_njoints; i++)
		m_default_vel[i] = velocity[i] / m_etoangles[i];
	Unlock ();

	return 0;
}


int YARPHeadControl::SetDefaultVelocity (int vaxis, double velocity)
  {
    if (vaxis < 0 || vaxis >= m_njoints)
      return -1;
    
    Lock ();
    m_default_vel[vaxis] = velocity / m_etoangles[vaxis];
    
    Unlock ();
    return 0;
}

int YARPHeadControl::SetDefaultVelocity (double *velocity, int* statusbits)
  {
            assert (velocity != NULL);
            //memcpy (m_default_vel, velocity, sizeof(double) * m_njoints);
            Lock ();
            for (int i = 0; i < m_njoints; i++){
	      if (statusbits[i] == ON)
		m_default_vel[i] = velocity[i] / m_etoangles[i];
	      printf("Axis %d's velocity is now %f\n",i,m_default_vel[i]);
	    }
            Unlock ();
    
            return 0;
  }
int YARPHeadControl::SetDefaultAcc (double *acc)
{
	assert (acc != NULL);
	//memcpy (m_default_acc, acc, sizeof(double) * m_njoints);

	Lock ();
  
	for (int i = 0; i < m_njoints; i++)
		m_default_acc[i] = acc[i] / m_etoangles[i];
	Unlock ();

	return 0;
}

int YARPHeadControl::SetDefaultAcc (int vaxis, double acc)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return -1;

	Lock ();
  m_default_acc[vaxis] = acc / m_etoangles[vaxis];
  
  Unlock ();
  	return 0;
}

int YARPHeadControl::SetDefaultAcc (double *acc, int *statusbits)
  {
            assert (acc != NULL);
            //memcpy (m_default_acc, acc, sizeof(double) * m_njoints);
    
            Lock ();
    
            for (int i = 0; i < m_njoints; i++){
	      if (statusbits[i] == ON)
		m_default_acc[i] = acc[i] / m_etoangles[i];
	      printf("Axis %d's acc is now %f\n",i,m_default_acc[i]);
	    }
	      Unlock ();
    
            return 0;
  }


int YARPHeadControl::EnableAxis (int vaxis)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return -1;

	Lock ();
	m_enabled[vaxis] = true;
	
	//NOTE: Comment this out
	/*if (vaxis == HeadTilt() || vaxis == HeadRoll())
	{
		m_enabled[HeadTilt()] = m_enabled[HeadRoll()] = true;
	}*/
	Unlock ();

	return 0;
}

int YARPHeadControl::DisableAxis (int vaxis)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return -1;

	Lock ();
	m_enabled[vaxis] = false;
	//NOTE: comment this out
	/*
	if (vaxis == HeadTilt() || vaxis == HeadRoll())
	{
		m_enabled[HeadTilt()] = m_enabled[HeadRoll()] = false;
	}*/
	Unlock ();

	return 0;
}

int YARPHeadControl::SetCalibrationFlag (int vaxis, bool flag)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return -1;

	Lock ();
	m_calibrated[vaxis] = flag;
	//NOTE: comment this out
	/*
	if (vaxis == HeadTilt() || vaxis == HeadRoll())
	{
		m_enabled[HeadTilt()] = m_enabled[HeadRoll()] = flag;
	}*/
	Unlock ();

	return 0;
}

bool YARPHeadControl::IsCalibrated (int vaxis)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return false;

	Lock ();
	bool flag = m_calibrated[vaxis];
	Unlock ();

	return flag;
}

int YARPHeadControl::SetLimits (int vaxis, double min, double max)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return -1;

	Lock ();
	m_limits_max[vaxis] = max / m_etoangles[vaxis];
	m_limits_min[vaxis] = min / m_etoangles[vaxis];
	Unlock ();

	return 0;
}

int YARPHeadControl::GetLimits (int vaxis, double *min, double *max)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return -1;

	Lock ();
	*max = m_limits_max[vaxis] * m_etoangles[vaxis];
	*min = m_limits_min[vaxis] * m_etoangles[vaxis];
	Unlock ();

	return 0;
}

int YARPHeadControl::SetZeroHere (int vaxis)
{
	if (vaxis < 0 || vaxis >= m_njoints)
		return -1;

	//NOTE: Comment this out
	/*
	if (vaxis == HeadTilt() || vaxis == HeadRoll())
		return -1;
	*/
	if (m_calibrated[vaxis] == false)
		return -1;

	Lock ();

	double actual;
	GetPosition (vaxis, &actual);
	m_limits_max[vaxis] -= actual;
	m_limits_min[vaxis] -= actual;

	ZeroAxis (vaxis);

	Unlock ();

	return 0;
}

int YARPHeadControl::SetZeroHere (void)
{
	for (int i = 0; i < m_njoints; i++)
	{
		SetZeroHere (i);
	}

	return 0;
}

int YARPHeadControl::ResetAllEncoders (void)
{
	Lock ();
	for (int i = 0; i < m_njoints; i++)
	{
		ZeroAxis (i);
		m_calibrated[i] = false;
		m_limits_max[i] = m_limits_min[i] = 0;
		m_enabled[i] = false;
	}
	Unlock ();

	return 0;
}

/* the following functions were used in Cog.
   won't be relevant in K4 */

// the rest of the system shouldn't be running...
//NOTE: ParkHead and UnparkHead may have to change???
int YARPHeadControl::ParkHead (void)
{
	const double SHUTDOWN_VEL = 5000.0;

	Lock ();

	if (m_calibrated[NeckTilt()] == false || m_enabled[NeckTilt()] == false)
	{
		printf ("Joint 7 must be calibrated to park properly\n");
		Unlock ();
		return -1;
	}

	// record the current tilt position
	meid_get_position(meidx, NeckTilt(), &m_tilt_store);

	// drop the head down to a reasonable position
	meid_dsp_velocity(meidx, NeckTilt(), SHUTDOWN_VEL, 3);
	meid_dsp_velocity(meidx, NeckTilt(), 0, 1);

	Unlock ();

	return 0;
}

int YARPHeadControl::UnparkHead (void)
{
	if (m_tilt_store == double(TILTSTORE))
		return -1;

	if (m_calibrated[NeckTilt()] == false || m_enabled[NeckTilt()] == false)
	{
		printf ("Joint 7 must be calibrated to park properly\n");
		return -1;
	}

	const double SHUTDOWN_VEL = 5000.0;

	Lock ();
	meid_start_move(meidx, NeckTilt(), m_tilt_store, SHUTDOWN_VEL, SHUTDOWN_VEL * 10.0);
	Unlock();

	return 0;
}

int YARPHeadControl::LowerGains (void)
{
//	Lock ();

	int16 i;
	int psum;
	int16 allgains[MaxAxes][COEFFICIENTS];
	
	if (m_shutdown_flag == true)
	{
//		Unlock ();
		return -1;
	}

	printf ("Decreasing gains...\n");

	m_shutdown_flag = true;
  
	// start the gain decrement 
	psum = 0;
	for(i = 0; i < m_njoints; i++)
	{
		meid_get_filter(meidx, i, &allgains[i][0]);
		allgains[i][DF_I] = 0;
		psum += allgains[i][DF_P];
	}

	while(psum > 0)
	{
		psum = 0;
		for(i = 0; i < m_njoints; i++)
		{
			if (allgains[i][DF_P] > 0)
			{
				allgains[i][DF_P]--; // = allgains[i][DF_P] * 999 / 1000;
				meid_set_filter(meidx, i, &allgains[i][0]);
			}
			psum += allgains[i][DF_P];
		}

 		YARPTime::DelayInSeconds(0.020);
	}

	for(i = 0; i < m_njoints; i++)
	{
		meid_get_filter(meidx, i, &allgains[i][0]);
		allgains[i][DF_D] = 0;
		meid_set_filter(meidx, i, &allgains[i][0]);
	}

	printf ("Gains are zero now\n");
//	Unlock ();

	return 0;
}

int YARPHeadControl::RaiseGains (void)
{
//	Lock ();

	int16 i;
	int psum, pgoal;
	int16 allgains[MaxAxes][COEFFICIENTS];
	//float factor = 0.02;
	const double SHUTDOWN_VEL = 5000.0;
  
	if (m_shutdown_flag == false)
	{
//		Unlock ();
	    return -1;
	}

	printf ("Raising gains...\n");

	m_shutdown_flag = false;
  
	psum = 0;
	pgoal = 0;
	for(i = 0; i < m_njoints; i++)
    {
		meid_get_filter(meidx, i, &allgains[i][0]);
		psum += allgains[i][DF_P];
		allgains[i][DF_D] = m_boot_filter[i][DF_D];
		meid_set_filter(meidx, i, &allgains[i][0]);
		pgoal += m_boot_filter[i][DF_P];
    }
  
	int count = 0;
	while (pgoal - psum > 0)
	{
		psum = 0;
		for(i = 0;i < m_njoints; i++)
		{
			if (allgains[i][DF_P] < m_boot_filter[i][DF_P])
			{
				allgains[i][DF_P]++; // = boot_filter[i][DF_P] * factor;
				meid_set_filter(meidx, i, &allgains[i][0]);
			}
			psum += allgains[i][DF_P];
		}

		//factor = factor * 1.025;
		//if (factor > 1.0) factor = 1.0;
		count++;
		if (count < 100)
			YARPTime::DelayInSeconds (0.040);
	}

	for(i = 0; i < m_njoints; i++)
	{
		meid_get_filter(meidx, i, &allgains[i][0]);
		allgains[i][DF_I] = m_boot_filter[i][DF_I];
		meid_set_filter(meidx, i, &allgains[i][0]);
	}

	printf ("Gains are up\n");

//	Unlock ();
	return 0;
}




