/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPConfigFile.cpp,v 1.10 2004-07-12 08:36:31 babybot Exp $
///
///


//
// YARPIniFile.cpp

#include "YARPConfigFile.h"

//
// gestione del path di default.
//
using namespace std;

bool YARPConfigFile::_open(const YARPString &path, const YARPString &filename)
{
	YARPString tmp = path;
	tmp.append(filename);
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
	while ( (i<n) && (fscanf(_pFile, "%c", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::getHex(const char *section, const char *name, char *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	int tmp;
	while ( (i<n) && (fscanf(_pFile, "%x", &tmp) != EOF) )
	{
		*out = (char) tmp;
		out++;
		i++;
	}
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::getHex(const char *section, const char *name, short *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	int tmp;
	while ( (i<n) && (fscanf(_pFile, "%x", &tmp) != EOF) )
	{
		*out = (short) tmp;
		i++;
		out++;
	}
			
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

int YARPConfigFile::get(const char *section, const char *name, unsigned int *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%u", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, double **matrix, int n, int m)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	// we assume the matrix is implemented as a C bidimensional vector
	double *out = matrix[0];

	int i = 0;
	while ( (i<n*m) && (fscanf(_pFile, "%lf", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n*m)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;		
}

int YARPConfigFile::getString(const char *section, const char *name, char *out)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	i = fscanf(_pFile, "%s", out);
					
	_close();
	
	if (i > 0)
		return YARP_OK;		// found at least a single char
	else 
		return YARP_FAIL;
}

int YARPConfigFile::getString(const char *section, const char *name, YARPString &out)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	char tmp[255];
	i = fscanf(_pFile, "%s", tmp);
	out = YARPString(tmp);
					
	_close();
	
	if (i > 0)
		return YARP_OK;		// found at least a single char
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
		
		if (strcmp(row, str) == 0)
			return true;
	}

	return false;
}