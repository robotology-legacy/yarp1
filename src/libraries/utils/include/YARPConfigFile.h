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
///							#nat#
///
///	     "Licensed under the Academic Free License Version 1.0"
///
/// $Id: YARPConfigFile.h,v 1.5 2003-07-04 16:25:57 babybot Exp $
///  
/// very simple class to handle config files... by nat May 2003
//

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
	int getString(const char *section, const char *name, char *out);

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
