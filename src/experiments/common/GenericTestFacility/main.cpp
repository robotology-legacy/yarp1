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
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: main.cpp,v 1.2 2003-07-02 12:00:10 gmetta Exp $
///
///


#include <conf/YARPConfig.h>
#include <YARPAll.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <stdio.h>
#include <string.h>

#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPNetworkTypes.h"
#include "YARPScheduler.h"

#include "YARPMath.h"
#include "YARPVectorPortContent.h"

extern int __debug_level;

YVector foo;
char name[256];

///YARPInputPortOf<NetInt32> in(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
///YARPOutputPortOf<NetInt32> out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
YARPInputPortOf<YMatrix> in;
YARPOutputPortOf<YMatrix> out;

class Thread1 : public YARPThread
{
public:
	virtual void Body()
	{
		int er = in.Register(name);
		if (er != YARP_OK)
		{
			ACE_DEBUG ((LM_DEBUG, "Thread1: can't register port name, bailing out\n"));
			return;
		}

		YARPTime::DelayInSeconds(2);

		while (1)
		{
			printf("Waiting for input\n");
			in.Read();
			YMatrix& tmp = in.Content();
			ACE_OS::printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$Read %d\n", tmp.NRows() * tmp.NCols());
			for (int i = 0; i < tmp.NRows(); i++)
				for (int j = 0; j < tmp.NCols(); j++)
			{
				ACE_OS::printf ("%d -- %lf\n", i, tmp[i][j]);
			}
		}
	}
};

class Thread2 : public YARPThread
{
public:
	virtual void Body()
	{
		int er = out.Register(name);
		if (er != YARP_OK)
		{
			ACE_DEBUG ((LM_DEBUG, "Thread2: can't register port name, bailing out\n"));
			return;
		}

		int ct = 2;

		while (1)
		{
			YMatrix tmp(3, 3);
			tmp = 0;
			for (int i = 1; i <= 3; i++)
				tmp(i,i) = 3.2222 + ct;

			out.Content() = tmp;

			printf("$$$$$$$$$$$$$$$$$$$$$$$$$Writing %d\n", out.Content().NRows());

			out.Write();
			ct++;
			if (ct > 10) ct = 2;

			YARPTime::DelayInSeconds(1);
		}
	}
};

int main(int argc, char *argv[])
{
	///__debug_level = 80;


	Thread1 t1;
	Thread2 t2;
	int s = 1, c = 1;

	if (argc>=2)
	{
		s = c = 0;
		for (unsigned int i=0; i<strlen(argv[1]); i++)
		{
			if (argv[1][i] == 's') s=1;
			if (argv[1][i] == 'c') c=1;
		}
	}

	if (argc == 3 && s)
	{
		ACE_OS::sprintf (name, "%s:s", argv[2]);
		ACE_OS::printf ("name is %s\n", name);
	}
	else 
	if (argc == 3 && c)
	{
		ACE_OS::sprintf (name, "%s:c", argv[2]);
		ACE_OS::printf ("name is %s\n", name);
	}
	else
	{
		ACE_OS::printf ("pls, provide a channel name\n");
		return -1;
	}

	if (s)
	{
		t1.Begin();
	}

	if (c)
	{
		if (s)
		{
			YARPTime::DelayInSeconds(1);
		}
		t2.Begin();
	}

	if (s && c)
	{
		t1.End();
		t2.End();
		ACE_OS::printf ("can't be server and client at the same time\n");
		return -1;
	}

	YARPTime::DelayInSeconds(6000.0);
	return 0;
}

