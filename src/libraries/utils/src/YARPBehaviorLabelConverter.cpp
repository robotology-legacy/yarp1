#include "YARPBehaviorLabelConverter.h"
#include <stdio.h>

YARPBehaviorLabelConverter::YARPBehaviorLabelConverter()
{
	// ctor
	_label.value = 0;
	_label.descriptor = YARPString("Null");
	_label.verbose = YARPString("Still null");
}

YARPBehaviorLabelConverter::~YARPBehaviorLabelConverter()
{
	//dctor
}


int YARPBehaviorLabelConverter::read(const YARPString &path, const YARPString &fileName)
{
	_path = path;
	_fileName = fileName;

	return _refresh();
}
	
int YARPBehaviorLabelConverter::_refresh()
{
	// read label
	char sTmp1[255];
	char sTmp2[255];
	int ret;
	int	 iTmp;
	_list.clear();

	YARPString completeFile = _path + _fileName;

	FILE *fp = fopen(completeFile.c_str(), "r");
	if (fp == NULL)
	 return YARP_FAIL;
	
	_skip(fp, "[LABEL]");
	ret = fscanf(fp, "%s %d", sTmp1, &iTmp);
	ret = _readFullString(fp, sTmp2, 255);

	_label.descriptor = YARPString(sTmp1);
	_label.value = iTmp;
	_label.verbose = YARPString(sTmp2);

	ret = _skip(fp, "[COMMANDS]");

	YBLabelConverterEntry tmpLabel;
	while(ret != EOF)
	{
		ret = fscanf(fp, "%s %d", sTmp1, &iTmp);
		if (ret == EOF)
			break;
		ret = _readFullString(fp, sTmp2, 255);

		tmpLabel.descriptor = YARPString(sTmp1);
		tmpLabel.value = iTmp;
		tmpLabel.verbose = YARPString(sTmp2);

		_list.push_back(tmpLabel);
	}

		
	// read list
	return YARP_OK;
}

int YARPBehaviorLabelConverter::_skip(FILE *fp, const YARPString &str)
{
	if (fp == NULL)
		return EOF;
	
	char tmp[255];
	bool found = false;

	while(fscanf(fp, "%s", tmp) != EOF)
	{
		if (str == YARPString(tmp))
		{
			found = true;
			break;
		}
	}

	if (!found)
		return EOF;
	
	return 1;
}

void YARPBehaviorLabelConverter::dump()
{
	ACE_OS::printf("Label: %s %d: %s", _label.descriptor.c_str(), _label.value, _label.verbose.c_str());

	ListEntriesIterator it(_list);
	while(!it.done())
	{
		ACE_OS::printf("Label: %s %d: %s", (*it).descriptor.c_str(), (*it).value, (*it).verbose.c_str());
		it++;
	}
}

int YARPBehaviorLabelConverter::_readFullString(FILE *fp, char *s, int max)
{
	char cTmp;
	int i = 0;
	while((cTmp != '\n')&&(i<max))
	{
		cTmp = fgetc(fp);
		if (cTmp == EOF)
		{
			s[i] = '\0';
			return EOF;
		}
		s[i] = cTmp;
		i++;
	}

	s[i] = '\0';
	return i;
}

