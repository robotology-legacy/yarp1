////////////////////////////////////////////////////////////////////////////
//
//
// very simple class to handle config files...

// by nat May 2003
//
// $Id: YARPConfigFile.h,v 1.3 2003-06-30 21:14:23 babybot Exp $

#ifndef __YARPCONFIGFILE__
#define __YARPCONFIGFILE__

#include <conf/YARPConfig.h>
#include <stdio.h>
#include <YARPErrorCodes.h>
#include <string>

////////////////////////////////////////////////////////////////////////////
//
class YARPConfigFile
{
public:
	YARPConfigFile() {
		_openFlag = false;
	}

	YARPConfigFile(const char *path) {
		_openFlag = false;
		_path = std::string(path);
	}

	YARPConfigFile(const char *path, const char *filename)
	{
		_path = std::string(path);
		_filename = std::string(filename);
		_openFlag = false;
	}
	
	virtual ~YARPConfigFile(void)
	{ _close(); };

	int get(const char *section, const char *name, double *out, int n = 1);
	int get(const char *section, const char *name, int *out, int n = 1);
	int get(const char *section, const char *name, short *out, int n = 1);
	int get(const char *section, const char *name, char *out, int n = 1);

	void set(const char *path, const char *name)
	{
		setName(name);
		setPath(path);
	}

	void setName(const char *name)
	{ _filename = std::string(name); }
	
	void setPath(const char *path)
	{ _path = std::string(path); }

private:
	std::string _path;
	std::string _filename;

	bool _open(const char *path, const char *filename);
	void _close()
	{
		if (_openFlag) {
			fclose(_pFile);
			_openFlag = false;
		}
	}

	int _get(const char *section, const char *name);
	bool _findString(const char *str);
	bool _findSection(const char *sec);

	FILE *_pFile;
	bool _openFlag;
};

#endif
