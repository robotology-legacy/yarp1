#include "YARPLogFile.h"

YARPLogFile::YARPLogFile()
{
	_name = NULL;
	_fp = NULL;
	_freeze = true;
}

YARPLogFile::YARPLogFile(const char *filename)
{
	open(filename);
}


YARPLogFile::~YARPLogFile()
{
	close();
}