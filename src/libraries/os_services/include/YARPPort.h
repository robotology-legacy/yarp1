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
///
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
/// $Id: YARPPort.h,v 1.4 2003-04-22 17:01:19 gmetta Exp $
///
///

/*
	paulfitz Sat May 26 22:34:44 EDT 2001
*/

#ifndef YARPPort_INC
#define YARPPort_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/Log_Msg.h>

#include "YARPAll.h"

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///#include <assert.h>

#include "YARPBool.h"
#include "YARPPortContent.h"
#include "YARPNameID.h"

class YARPPort
{
protected:
	void *system_resource;  
	YARPPortContent *content;

public:
	YARPPort();
	virtual ~YARPPort();

	virtual int Register(const char *name);
	int Connect(const char *name);
	virtual YARPPortContent *CreateContent() = 0;
	YARPPortContent& Content();
	int IsReceiving();
	int IsSending();
	void FinishSend();
	void Deactivate();
	static void DeactivateAll();
	static int Connect(const char *src_name, const char *dest_name);
};

class YARPInputPort : public YARPPort
{
public:
	enum
	{
		NO_BUFFERS,
		DOUBLE_BUFFERS,
		TRIPLE_BUFFERS,
		DEFAULT_BUFFERS = TRIPLE_BUFFERS
	};

	YARPInputPort(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_UDP);
	virtual ~YARPInputPort();

	virtual int Register(const char *name);
	bool Read(bool wait=true);
	virtual void OnRead() {}
	YARPPortContent& Content() { return YARPPort::Content(); }
};


class YARPOutputPort : public YARPPort
{
public:
	enum
	{
		MANY_OUTPUTS,
		SINGLE_OUTPUT,
		DEFAULT_OUTPUTS = MANY_OUTPUTS
	};

	YARPOutputPort(int n_service_type = MANY_OUTPUTS, int n_protocol_type = YARP_UDP);
	virtual ~YARPOutputPort();

	virtual int Register(const char *name);
	YARPPortContent& Content();
	void Write(bool wait=false);
	virtual void OnWrite() {}
};


template <class T>
class YARPInputPortOf : public YARPInputPort
{
public:
	YARPInputPortOf(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_UDP) 
		: YARPInputPort(n_service_type, n_protocol_type) {}
  
	virtual YARPPortContent *CreateContent() { return new YARPPortContentOf<T>; }

	T& Content()
	{
		ACE_ASSERT(content!=NULL);
		return ((YARPPortContentOf<T>*)content)->Content();
	}
};

template <class T>
class YARPOutputPortOf : public YARPOutputPort
{
public:
	YARPOutputPortOf(int n_service_type = MANY_OUTPUTS, int n_protocol_type = YARP_UDP) 
		: YARPOutputPort(n_service_type, n_protocol_type) {}

	virtual YARPPortContent *CreateContent() { return new YARPPortContentOf<T>; }

	T& Content()
    {
		YARPOutputPort::Content();
		ACE_ASSERT (content!=NULL);
		return ((YARPPortContentOf<T>*)content)->Content();
    }
};

template <class T>
class YARPBasicInputPort : public YARPInputPort
{
public:
	YARPBasicInputPort(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_UDP) 
		: YARPInputPort(n_service_type, n_protocol_type) {}

	virtual YARPPortContent *CreateContent() { return new T; }

	T& Content() { return *((T*)(&YARPPort::Content())); }
};

template <class T>
class YARPBasicOutputPort : public YARPOutputPort
{
public:
	YARPBasicOutputPort(int n_service_type = MANY_OUTPUTS, int n_protocol_type = YARP_UDP) 
		: YARPOutputPort(n_service_type, n_protocol_type) {}

	virtual YARPPortContent *CreateContent() { return new T; }

	T& Content() { return *((T*)(&YARPOutputPort::Content())); }
};

#endif
