////////////////////////////////////////////////////////////////////////////
//
//
// very simple class to handle dump files...

// by nat May 2003
//
// $Id: YARPLogFile.h,v 1.2 2003-05-31 06:31:38 gmetta Exp $

#ifndef __YARPDUMPFILE__
#define __YARPDUMPFILE__

#include <conf/YARPConfig.h>
#include <YARPErrorCodes.h>
#include <YARPMatrix.h>

class YARPLogFile
{
public:
	YARPLogFile();
	YARPLogFile(const char *filename);
	~YARPLogFile();
	
	int open(const char *filename)
	{
		if (filename == NULL)
			return YARP_FAIL;

		int n = strlen(filename);
		close();		
		
		_name = new char [n+1];
		strcpy(_name, filename);

		_fp = fopen(filename,"w");

		if (_fp != NULL)
		{
			_freeze = false;
			return YARP_OK;
		}
		else
			return YARP_FAIL;
	}

	int close()
	{
		if (_fp != NULL)
			fclose(_fp);
		if (_name != NULL)
			delete [] _name;

		_fp = NULL;
		_name = NULL;

		_freeze = true;
		return YARP_OK;
	}

	inline int newLine()
	{
		if (_freeze)
			return YARP_OK;

		fprintf(_fp, "\n");
		return YARP_OK;
	}

	inline int dump(const YVector &v)
	{
		if (_freeze)
			return YARP_OK;

		int n = v.Length();
		for(int i = 1; i <= n; i++)
			fprintf(_fp, "%lf ", v(i));
		
		return YARP_OK;
	}

	inline int dump(double v)
	{
		if (_freeze)
			return YARP_OK;

		fprintf(_fp, "%lf ", v);
		return YARP_OK;
	}

	inline int dump(int v)
	{
		if (_freeze)
			return YARP_OK;

		fprintf(_fp, "%d ", v);
		return YARP_OK;
	}

private:
	char *_name;
	FILE *_fp;
	bool _freeze;
};

#endif