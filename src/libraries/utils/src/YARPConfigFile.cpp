//
// YARPIniFile.cpp
// $Id: YARPConfigFile.cpp,v 1.2 2003-06-30 21:14:23 babybot Exp $

#include "YARPConfigFile.h"

//
// gestione del path di default.
//
using namespace std;

bool YARPConfigFile::_open(const char *path, const char *filename)
{
	std::string tmp = std::string(path) + std::string(filename);
	_pFile = fopen(tmp.c_str(), "r");
	if (_pFile != NULL)
	{
		_openFlag = true;
		return true;
	}
	else 
		return false;
}

int YARPConfigFile::get(const char *section, const char *name, double *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%lf", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, short *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%hd", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, char *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%x", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, int *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%d", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::_get(const char *section, const char *name)
{
	if (!_open(_path.c_str(), _filename.c_str()))
		return YARP_FAIL;

	if (!_findSection(section))
	{
		_close();
		return YARP_FAIL;
	}

	if (_findString(name))
		return YARP_OK;
	
	_close();
	return YARP_FAIL;
}

bool YARPConfigFile::_findSection(const char *sec)
{
	char row[255];
	
	while (fscanf(_pFile, "%s", row) != EOF)
	{
		if (strcmp(sec, row) == 0)
			return true;
	}

	return false;
}

bool YARPConfigFile::_findString(const char *str)
{
	char row[255];

	int l = strlen(str);
	if (l == 0)
		return false;
	
	while (fscanf(_pFile, "%s", row) != EOF)
	{
		if (row[0] == '[')
			return false;		//end of section
		
		if (strncmp(row, str, l) == 0)
			return true;
	}

	return false;
}