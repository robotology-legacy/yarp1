#include "smoothcontrol.h"
#include <YARPConfigFile.h>

SmoothControl::SmoothControl(const YARPString &iniFile, int insize, int outsize)
{
	_iniFile = iniFile;
	_inSize = insize;
	_outSize = outsize;

	YARPConfigFile file;
	char *root = GetYarpRoot();
	_path.append(root);
	_path.append("/conf/babybot/"); 
	
	file.set(_path, _iniFile);

	file.get("[SMOOTH]", "nPids", &_nPids);

	_pids = new YARPPidFilter[_nPids];
	double temp[3];
	int i;
	for(i = 0; i < _nPids; i++)
	{
		char tmpField[80];
		sprintf(tmpField, "Pid%d", i);
		file.get("[SMOOTH]", tmpField, temp, 3);
		_pids[i].setKs(temp[0], temp[1], temp[2]);
	}
	
}

SmoothControl::~SmoothControl()
{
	delete [] _pids;
}