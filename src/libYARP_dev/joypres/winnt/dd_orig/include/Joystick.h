// Joystick.h: interface for the CJoystick class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOYSTICK_H__98EFFBDB_F1AE_4050_973F_AF7FF060FD1A__INCLUDED_)
#define AFX_JOYSTICK_H__98EFFBDB_F1AE_4050_973F_AF7FF060FD1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <Mmsystem.h>

struct JoyData
{
	int x;
	int y;
	int z;
	int u;

	DWORD buttons;
};

class CJoystick  
{
public:
	bool getRawData(JoyData* data);
	bool getData(JoyData* data);
	bool getButton(unsigned int nButton, int* value);
	bool calibrateAxe(int nAxe, double offset, double gain);
	bool getAxe(unsigned int nAxe, int* value);

	CJoystick();
	virtual ~CJoystick();

private:
	JOYINFOEX _joyinfo;
	double _gain[4];
	double _offset[4];
};

#endif // !defined(AFX_JOYSTICK_H__98EFFBDB_F1AE_4050_973F_AF7FF060FD1A__INCLUDED_)
