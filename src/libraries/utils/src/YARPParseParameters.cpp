// $Id: YARPParseParameters.cpp,v 1.2 2003-07-20 14:46:28 natta Exp $

#include "YARPParseParameters.h"

using namespace std;

bool YARPParseParameters::parse (int argc, char *argv[], const string &key, string &out)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == string(argv[i]+1)) 
			{
				// found key
				i++;
				if (i==argc)
					return false;
				else if (argv[i][0] == '-')
					return false;
				else if (key == string("name"))	// exception, "name"
				{
					// append "/"
					out = "/";	
					out.append(string(argv[i]));
					return true;
				}
				else
				{
					out = string (argv[i]);
					return true;
				}
			}
		}
	}
	return false; 
}

bool YARPParseParameters::parse (int argc, char *argv[], const string &key)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == string(argv[i]+1))
				return true;
		}
	}
	return false; 
}

bool YARPParseParameters::parse (int argc, char *argv[], const string &key, int *out) 
{
	string dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		*out = atoi(dummy.c_str());
		return true;
	}
	else
		return false;

}

bool YARPParseParameters::parse (int argc, char *argv[], const string &key, double *out) 
{
	string dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		*out = atof(dummy.c_str());
		return true;
	}
	else
		return false;

}

bool YARPParseParameters::parse (int argc, char *argv[], const string &key, char *out) 
{
	string dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		strcpy(out, dummy.c_str());
		return true;
	}
	else
		return false;

}