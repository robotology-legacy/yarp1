#include "YARPLogFile.h"

YARPLogFile::YARPLogFile()
{
	_fp = NULL;
	_freeze = true;
}

YARPLogFile::YARPLogFile(const YARPString &filename)
{
	open(filename);
}

YARPLogFile::YARPLogFile(const YARPString &path, const YARPString &filename)
{
	YARPString tmp;
	tmp = path;
	tmp.append(filename);
	open(tmp);
}

YARPLogFile::~YARPLogFile()
{
	close();
}

