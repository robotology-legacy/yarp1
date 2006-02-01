// YARPImgRecv.h: interface for the YARPImgRecv class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YARPIMGRECV_H__2654CA92_2656_48E0_9A91_F6CD59F11AC4__INCLUDED_)
#define AFX_YARPIMGRECV_H__2654CA92_2656_48E0_9A91_F6CD59F11AC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//=============================================================================
// YARP Includes - General
//=============================================================================
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
//=============================================================================
// YARP Includes - Class Specific
//=============================================================================
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPLogpolar.h>
//=============================================================================
// System Includes
//=============================================================================
#include <time.h>

// TO DO: convert to thread??

class YARPImgRecv  
{
public:
	bool GetLogpolar();
	bool GetFovea();
	void SetFovea(bool status=true);
	void SetLogopolar(bool status=true);
	double GetEstimatedInterval(void);
	bool Disconnect();
	int GetWidth();
	int GetHeight();
	bool Update();
	bool SaveLastImage(char *fileName, int format=YARPImageFile::FORMAT_PPM);
	bool GetLastImage(YARPGenericImage *data);
	bool Connect();
	bool Connect(char *portName, char *network=NULL);
	YARPImgRecv(char *portNamechar, char *network=NULL);
	YARPImgRecv();
	virtual ~YARPImgRecv();

private:
	bool _registerPorts();
	bool _unregisterPorts();
	void _logpolarConversion(bool fovea, YARPGenericImage *dest);

	char _portName[256];
	char _network[256];
	YARPInputPortOf<YARPGenericImage> _inPort;
	YARPImageOf<YarpPixelBGR> _img, _logImg;
	YARPLogpolar _logopolarMapper;
	bool _connected;
	bool _portNameIsValid;
	int _width;
	int _height;
	bool _logpolar;
	bool _fovea;
	clock_t _currentFrameTime;
	clock_t _lastFrameTime;
};

#endif // !defined(AFX_YARPIMGRECV_H__2654CA92_2656_48E0_9A91_F6CD59F11AC4__INCLUDED_)
