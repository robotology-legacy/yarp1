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
/// $Id: global.h,v 1.2 2005-10-28 16:11:08 natta Exp $
///
///

#ifndef __globalh__
#define __globalh__

///
/// only to include enumerations (for C code).
#include <yarp/YARPBottleCodes.h>
#include <yarp/YARPImage.h>

///
/// possible port types within matlab.
enum _dataType 
{ 
	MX_YARP_INVALID = 0, 
	MX_YARP_INT = 1, 
	MX_YARP_DOUBLE = 2,
	MX_YARP_FLOAT = 3,
	MX_YARP_YVECTOR = 4,
	MX_YARP_IMAGE = 5,
	MX_YARP_BOTTLE = 6,
};

///
/// struct to pass params to the dispatch function (and to the YARP ports).
struct _paramsTag
{
	char *_portname;		/// symbolic name of the port.
	enum _dataType _type;	/// type of the port.
	char *_protocol;		/// protocol TCP/UDP/MCAST...
	bool _direction;		/// true means INPUT port, false means OUTPUT port
	char *_network;			/// name of the subnetwork, otherwise default.
	int _extra_params;		/// buffering, multiple output specs.
	int _portnumber;		/// if the entry is known, identifies the port uniquely.
	char *_extra_content;	/// a string for many uses.
	int _sizex, _sizey;		/// for multi-dimensional data types.
};

typedef struct _paramsTag _dispatchParams;

///
/// type of entry of the static list of ports within the DLL.
///		this is all what I need to cast the port to the correct type and call
///		a method on it.
///
struct _portTypeEntryTag
{
	void *_port;
	enum _dataType _type;
	bool _direction;
	char *_name;
};

typedef struct _portTypeEntryTag _portEntry;

///
/// max number of ports.
const int MAX_PORTS = 20;

///
///
#ifdef __cplusplus
extern "C"
{
#endif

	int dispatcher (const char *operation, void *params);
	void cleanUpList(void);

#ifdef __cplusplus
}
#endif

#endif
