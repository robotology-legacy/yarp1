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
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNameService.h,v 1.7 2003-05-15 16:57:45 gmetta Exp $
///
///
/*
	paulfitz Tue May 22 15:34:43 EDT 2001
 */

#ifndef YARPNameService_INC
#define YARPNameService_INC

#include <conf/YARPConfig.h>
#include "YARPAll.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include "YARPNameID.h"

///
#ifdef __WIN32__
#define NAMER_CONFIG_FILE "conf\\namer.conf"
#else
#define NAMER_CONFIG_FILE "conf/namer.conf"
#endif
#define DONT_OPTIMIZE_CONSTRUCTOR if (0) { int i; i++; }

///
///
///
class YARPNameService
{
public:
	YARPNameService () { DONT_OPTIMIZE_CONSTRUCTOR; Initialize (); }
	~YARPNameService () { Finalize (); }

	/// shouldn't be just an opportunistic call to the name server?
	static int ConnectNameServer(const char *name = NULL);
	static int Initialize (void);
	static int Finalize (void);

	/* zero if successful */
	///// OLD: if native flag set, register with native name service only
	/// YARP_QNX4 flag is used to ask a name to the native QNX4 name service.
	/// static int RegisterName(const char *name, int native=1);
	static YARPUniqueNameID RegisterName (const char *name, int reg_type = YARP_DEFAULT_PROTOCOL, int num_ports_needed = YARP_PROTOCOL_REGPORTS);
	/// reg_type is one of the enumeration in YARPNameID_defs.h

	/// recover the name ID assigned by the server.
	/// static YARPUniqueNameID GetRegistration (void);

	// if native flag set, search native name service first, then global
	/// it does a search on various protocols instead asking for a type.
	static YARPUniqueNameID LocateName(const char *name, int name_type = YARP_NO_SERVICE_AVAILABLE);
};


///
/// handles endpoint(s) allocation and connection.
///	LATER: maybe a "close" is needed?
///
class YARPEndpointManager
{
public:
	static YARPNameID CreateInputEndpoint(YARPUniqueNameID& name);
	static YARPNameID CreateOutputEndpoint(YARPUniqueNameID& name);
	static int ConnectEndpoints(YARPNameID& dest);
	static int Close(const YARPNameID& endp);
	static int SetTCPNoDelay (const YARPNameID& endp);
};

#endif

