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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: main.cpp,v 1.1 2003-05-29 22:32:26 gmetta Exp $
///
///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage (const char *name)
{
	fprintf (stderr, "Usage : %s -n <nodename> [-t <pid>] || [<exec name>]\n", name+1);
}

///
///
int main (int argc, char *argv[])
{
	char cmdstring[4096];
	
	if (argc < 4)
	{
		usage (argv[0]);
		return -1;
	}

	char nodename[256];
	memset (nodename, 0, 256);

	int j = -1;
	int tid = 0;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] != '-')
		{
			j = i;
			break;
		}

		if (strcmp(argv[i], "-n") == 0)
		{
			memcpy (nodename, argv[i+1], strlen(argv[i+1]));
			i++;	/// skip next.
		}
		else
		if (strcmp(argv[i], "-t") == 0)
		{
			tid = 1;
		}
	}

	if (j == -1)
	{
		usage (argv[0]);
		return -1;
	}

	memset (cmdstring, 0, 4096);
	char *tmp = cmdstring;
	if (tid)
	{
		sprintf (tmp, "rkill /kill \\\\"); tmp += 14;
	}
	else
	{
		sprintf (tmp, "rkill /nkill \\\\"); tmp += 15;
	}

	sprintf (tmp, "%s ", nodename); tmp += (strlen(nodename) + 1);
	sprintf (tmp, "%s", argv[j]); tmp += (strlen(argv[j]));

	system (cmdstring);

	return 0;
}



