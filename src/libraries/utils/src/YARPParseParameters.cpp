// $Id: YARPParseParameters.cpp,v 1.4 2003-08-13 00:23:18 gmetta Exp $

#include "YARPParseParameters.h"

///using namespace std;

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, YARPString &out)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == YARPString(argv[i]+1)) 
			{
				// found key
				i++;
				if (i==argc)
					return false;
				else if (argv[i][0] == '-')
					return false;
				else if (key == YARPString("name"))	// exception, "name"
				{
					// append "/"
					out = "/";	
					out.append(YARPString(argv[i]));
					return true;
				}
				else
				{
					out = YARPString (argv[i]);
					return true;
				}
			}
		}
	}
	return false; 
}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == YARPString(argv[i]+1))
				return true;
		}
	}
	return false; 
}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, int *out) 
{
	YARPString dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		*out = atoi(dummy.c_str());
		return true;
	}
	else
		return false;

}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, double *out) 
{
	YARPString dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		*out = atof(dummy.c_str());
		return true;
	}
	else
		return false;

}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, char *out) 
{
	YARPString dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		strcpy(out, dummy.c_str());
		return true;
	}
	else
		return false;

}


