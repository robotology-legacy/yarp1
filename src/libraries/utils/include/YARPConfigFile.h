/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///							#nat#
///
///	     "Licensed under the Academic Free License Version 1.0"
///
/// $Id: YARPConfigFile.h,v 1.11 2004-07-28 21:46:21 babybot Exp $
///  
/// very simple class to handle config files... by nat May 2003
//

#ifndef __YARPCONFIGFILE__
#define __YARPCONFIGFILE__

#include <conf/YARPConfig.h>
#include <stdio.h>
#include <YARPErrorCodes.h>
#include <YARPString.h>
////////////////////////////////////////////////////////////////////////////
//
class YARPConfigFile
{
public:
	YARPConfigFile() {
		_openFlag = false;
	}

	YARPConfigFile(const YARPString &path) {
		_openFlag = false;
		_path = YARPString(path);
	}

	YARPConfigFile(const YARPString &path, const YARPString &filename)
	{
		_path = path;
		_filename = filename;
		_openFlag = false;
	}
	
	virtual ~YARPConfigFile(void)
	{ _close(); };

	int get(const char *section, const char *name, double *out, int n = 1);
	int get(const char *section, const char *name, int *out, int n = 1);
	int get(const char *section, const char *name, unsigned int *out, int n = 1);
	int get(const char *section, const char *name, short *out, int n = 1);
	int get(const char *section, const char *name, char *out, int n = 1);
	int get(const char *section, const char *name, double **out, int n, int m);
	int getHex(const char *section, const char *name, char *out, int n = 1);
	int getHex(const char *section, const char *name, short *out, int n = 1);
	int getString(const char *section, const char *name, char *out);
	int getString(const char *section, const char *name, YARPString &str);

	void set(const YARPString &path, const YARPString &name)
	{
		setName(name);
		setPath(path);
	}

	void setName(const YARPString &name)
	{ _filename = name; }
	
	void setPath(const YARPString &path)
	{ _path = path; }

private:
	YARPString _path;
	YARPString _filename;

	bool _open(const YARPString &path, const YARPString &filename);
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
