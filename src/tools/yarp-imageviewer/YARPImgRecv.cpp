// YARPImgRecv.cpp: implementation of the YARPImgRecv class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPImgRecv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPImgRecv::YARPImgRecv()
{
	sprintf(_portName,"\0");
	sprintf(_network,"\0");
	_connected = false;
	_portNameIsValid = false;
	_logpolar = false;
	_fovea = false;
	_currentFrameTime = 0.0;
	_lastFrameTime = 0.0;
}

YARPImgRecv::~YARPImgRecv()
{
	
}

YARPImgRecv::YARPImgRecv(char *portName, char *network)
{
	sprintf(_portName, portName);
	if (network == NULL)
	{
		sprintf(_network, "default");
	}
	else
	{
		sprintf(_network, network);
	}
	_connected = false;
	_portNameIsValid = true;
	_width = 0;
	_height = 0;
	_logpolar = false;
	_fovea = false;
	_currentFrameTime = 0;
	_lastFrameTime = 0;
}

bool YARPImgRecv::Connect()
{
	if ( _connected || !_portNameIsValid)
		return false;

	bool res = false;
	res = _registerPorts();
	if (res == true)
	{
		_connected = true;
		return true;
	}
	else
	{
		_connected = false;
		return false;
	}
}

bool YARPImgRecv::Connect(char *portName, char *network)
{
	bool res = false;

	if ( _connected )
		return false;
	sprintf(_portName,portName);
	if (network == NULL)
	{
		sprintf(_network, "default");
	}
	else
	{
		sprintf(_network, network);
	}
	_portNameIsValid = true;

	res = _registerPorts();
	if (res == true)
	{
		_connected = true;
		return true;
	}
	else
	{
		_connected = false;
		return false;
	}
	_width = 0;
	_height = 0;
}

bool YARPImgRecv::Update()
{
	int ret = 0;
	
	if ( _connected == false)
		return false;
	
	ret = _inPort.Read(0);

	if (!ret)
		return false;

	_width = (_inPort.Content()).GetWidth();
	_height = (_inPort.Content()).GetHeight();
	
	_lastFrameTime = _currentFrameTime;
	_currentFrameTime = YARPTime::GetTimeAsSeconds();
	return true;
}

bool YARPImgRecv::GetLastImage(YARPGenericImage *data)
{
	if ( _connected == false)
		return false;
	
	if ((_width == 0) || (_height == 0))
		return false;

	if (_logpolar)
	{
		_img.CastCopy(_inPort.Content());
		_logpolarConversion(_fovea, data);
	}
	else
	{
		if ( (_width != data->GetWidth()) || (_height != data->GetHeight()) )
			data->Resize(_width, _height);
		data->CastCopy(_inPort.Content());
	}
	
	return true;
}


bool YARPImgRecv::SaveLastImage(char *fileName, int format)
{
	if ( _connected == false)
		return false;
	
	if ((_width == 0) || (_height == 0))
		return false;
	
	if (_logpolar)
	{
		_img.CastCopy(_inPort.Content());
		_logpolarConversion(_fovea, &_logImg);
		YARPImageFile::Write(fileName, _logImg, format);
	}
	else
	{
		_img.CastCopy(_inPort.Content());
		YARPImageFile::Write(fileName, _img, format);
	}

	return true;
}

bool YARPImgRecv::Disconnect()
{
	bool res = false;

	if ( _connected == false)
		return false;

	res = _unregisterPorts();
	if (res == true)
	{
		_connected = false;
		return true;
	}
	else
		return false;

}

bool YARPImgRecv::_registerPorts()
{
	int res = 0;
	res = _inPort.Register(_portName, _network);

	if  (res == YARP_OK)
		return true;
	else 
		return false;
}

bool YARPImgRecv::_unregisterPorts()
{
	int res = 0;
	res = _inPort.Unregister();


	if  (res == YARP_OK)
		return true;
	else 
		return false;
	
}
int YARPImgRecv::GetWidth()
{
	return _width;
}

int YARPImgRecv::GetHeight()
{
	return _height;
}

double YARPImgRecv::GetEstimatedInterval()
{
	double estInterval;
	estInterval = _currentFrameTime - _lastFrameTime;
	return estInterval;
}

void YARPImgRecv::SetLogopolar(bool status)
{
	_logpolar = status;
}

void YARPImgRecv::SetFovea(bool status)
{
	_fovea = status;
}

bool YARPImgRecv::GetFovea()
{
	return _fovea;
}

bool YARPImgRecv::GetLogpolar()
{
	return _logpolar;
}

void YARPImgRecv::_logpolarConversion(bool fovea, YARPGenericImage *dest)
{
	_img.Refer (_inPort.Content());
	if (fovea)
	{
		_logopolarMapper.SafeLogpolar2CartesianFovea (_img, _logImg);
		
	}
	else
	{
		_logopolarMapper.SafeLogpolar2Cartesian (_img, _logImg);
	}
	*dest = _logImg;
}
