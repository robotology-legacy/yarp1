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
/// $Id: YARPLogFile.h,v 1.4 2003-07-04 12:27:00 babybot Exp $
///  
/// very simple class to handle dump files...  -- May 2003

#ifndef __YARPDUMPFILE__
#define __YARPDUMPFILE__

#include <conf/YARPConfig.h>
#include <YARPErrorCodes.h>
#include <YARPMatrix.h>
#include <stdio.h>

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
