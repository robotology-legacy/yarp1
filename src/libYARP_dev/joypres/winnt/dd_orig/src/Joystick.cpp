// Joystick.cpp: implementation of the CJoystick class.
//
//////////////////////////////////////////////////////////////////////

#include "../include/Joystick.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJoystick::CJoystick()
{
	_joyinfo.dwSize = sizeof(JOYINFOEX);
	_gain[0] =  1.0;
	_gain[1] =  1.0;
	_gain[2] =  1.0;
	_gain[3] =  1.0;
	_offset[0] = 0.0;
	_offset[1] = 0.0;
	_offset[2] = 0.0;
	_offset[3] = 0.0;
}

CJoystick::~CJoystick()
{

}

bool CJoystick::getAxe(unsigned int nAxe, int* value)
{
	if (nAxe > 3)
		return false;
	DWORD pres;
	MMRESULT ret;
	switch(nAxe)
	{
	case 0:
		_joyinfo.dwFlags = JOY_CAL_READALWAYS || JOY_CAL_READXONLY;
		ret = joyGetPosEx(JOYSTICKID1,&_joyinfo);
		pres = _joyinfo.dwXpos;
		break;
	case 1:
		_joyinfo.dwFlags = JOY_CAL_READALWAYS || JOY_CAL_READYONLY;
		ret = joyGetPosEx(JOYSTICKID1,&_joyinfo);
		pres = _joyinfo.dwYpos;
		break;
	case 2:
		_joyinfo.dwFlags = JOY_CAL_READALWAYS || JOY_CAL_READZONLY;
		ret = joyGetPosEx(JOYSTICKID1,&_joyinfo);
		pres = _joyinfo.dwZpos;
		break;
	case 3:
		_joyinfo.dwFlags = JOY_CAL_READALWAYS || JOY_CAL_READUONLY;
		ret = joyGetPosEx(JOYSTICKID1,&_joyinfo);
		pres = _joyinfo.dwUpos;
		break;
	}

	if (ret != JOYERR_NOERROR)
		return false;
	double pressure;
	pressure = _gain[nAxe] * pres + _offset[nAxe];
	*value = (int)pressure;
	
	return true;
}

bool CJoystick::calibrateAxe(int nAxe, double offset, double gain)
{
	if (nAxe > 32)
		return false;

	_offset[nAxe] = offset;
	_gain[nAxe] = gain;

	return true;
}

bool CJoystick::getButton(unsigned int nButton, int *value)
{
	MMRESULT ret;
	_joyinfo.dwFlags = JOY_CAL_READALWAYS;
	ret = joyGetPosEx(JOYSTICKID1,&_joyinfo);
	if (nButton > 3)
		return false;

	if (ret != JOYERR_NOERROR)
		return false;
	
	switch (nButton)
	{
	case 0:
		*value = (_joyinfo.dwButtons & JOY_BUTTON1);
		break;
	case 1:
		*value = (_joyinfo.dwButtons & JOY_BUTTON2);
		break;
	case 2:
		*value = (_joyinfo.dwButtons & JOY_BUTTON3);
		break;
	case 3:
		*value = (_joyinfo.dwButtons & JOY_BUTTON4);
		break;
	}
	
	return true;
}

bool CJoystick::getRawData(JoyData *data)
{
	MMRESULT ret;
	_joyinfo.dwFlags = JOY_CAL_READALWAYS;
	ret = joyGetPosEx(JOYSTICKID1,&_joyinfo);

	if (ret != JOYERR_NOERROR)
		return false;

	data->x = (int)_joyinfo.dwXpos;
	data->y = (int)_joyinfo.dwYpos;
	data->z = (int)_joyinfo.dwZpos;
	data->u = (int)_joyinfo.dwUpos;

	data->buttons = _joyinfo.dwButtons;

	return true;
}

bool CJoystick::getData(JoyData *data)
{
	MMRESULT ret;
	_joyinfo.dwFlags = JOY_CAL_READALWAYS;
	ret = joyGetPosEx(JOYSTICKID1,&_joyinfo);

	if (ret != JOYERR_NOERROR)
		return false;
	double pressure;
	pressure = _gain[0] * ((int)_joyinfo.dwXpos) + _offset[0];
	data->x = (int)pressure;
	pressure = _gain[1] * ((int)_joyinfo.dwYpos) + _offset[1];
	data->y = (int)pressure;
	pressure = _gain[2] * ((int)_joyinfo.dwZpos) + _offset[2];
	data->z = (int)pressure;
	pressure = _gain[3] * ((int)_joyinfo.dwUpos) + _offset[3];
	data->u = (int)pressure;

	data->buttons = _joyinfo.dwButtons;

	return true;
}