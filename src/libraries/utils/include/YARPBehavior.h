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
/// $Id: YARPBehavior.h,v 1.1 2003-07-08 17:41:13 babybot Exp $
///  
/// Behavior class -- by nat July 2003
//

#ifndef __YARPBEHAVIORCLASS__
#define __YARPBEHAVIORCLASS__

#include <YARPFSM.h>
#include <YARPPort.h>
#include <string>

template <class OUT_DATA>
class YARPBehaviorSharedData
{
	public:
		YARPBehaviorSharedData(std::string portName):
		  _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
		  {
			  _outPort.Register(portName.c_str());
		  };

		  void send(OUT_DATA &d)
		  {
			  printf("Sending data\n");
			  memcpy(_outPort.Content(), d, sizeof(int)*2);
			  _outPort.Write();
		  }

	YARPOutputPortOf<OUT_DATA> _outPort;
};

template <class MY_BEHAVIOR, class SHARED_DATA>
class YARPBehavior: public YARPThread , public YARPFSM<MY_BEHAVIOR, SHARED_DATA>
{
public:
	YARPBehavior(SHARED_DATA *s): YARPThread(), YARPFSM<MY_BEHAVIOR, SHARED_DATA>(s)
	{
		YARPThread::Begin();
	}

	~YARPBehavior()
	{
		YARPThread::End();	// check this
	}

	virtual void Body(void)
	{
		while(!IsTerminated())
		{
			// handle stuff
			_stopEvent.wait();
			doYourDuty();
		}
	}
	
	void pulse(YARPFSMStateBase<MY_BEHAVIOR, SHARED_DATA> *s)
	{
		// signal pulse event
		if (checkState(s))
			_stopEvent.signal();
	}

	ACE_Auto_Event _stopEvent;
};

template <class MY_BEHAVIOR, class SHARED_DATA>
class YARPBehaviorMsgHandler
{
public:
	YARPBehaviorMsgHandler(MY_BEHAVIOR *fsm, int k, std::string pName):
	_inport(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
	{
		_fsm = fsm;
		_key = k;
		init(pName);
	}

	void init(std::string name)
	{
		_inport.Register(name.c_str());	// register port
	}
	
	void handle()
	{
		_inport.Read();
		// read some data
		memcpy(_data, _inport.Content(), sizeof(int) * 2);

		_parse(_data);
	}

	void _parse(int *d)
	{
		if (d[0] != _key)
		{
			// this is not for you, return
			return;
		}

		// handle message
		ENTRY_HANDLER_IT it;
		it = _table.begin();
		while(it != _table.end())
		{
			if (it->key == d[1])
				it->function(_fsm);
			it++;
		}

		// handle signals
		ENTRY_SIGNALS_IT is;
		is = _signals.begin();
		while(is != _signals.end())
		{
			if (is->key == d[1])
				is->function(_fsm, is->state);
			is++;
		}
	}

	void add(int k, void (*f)(MY_BEHAVIOR *fsm))
	{
		entryHandler tmp;
		tmp.function = f;
		tmp.key = k;

		_table.push_back(tmp);
	}
	
	void add(int k, void (*f)(MY_BEHAVIOR * , YARPFSMStateBase<MY_BEHAVIOR, SHARED_DATA> *), YARPFSMStateBase<MY_BEHAVIOR, SHARED_DATA> *s)
	{
		sigHandler tmp;
		tmp.function = f;
		tmp.state = s;
		tmp.key = k;

		_signals.push_back(tmp);
	}


	MY_BEHAVIOR *_fsm;
	YARPInputPortOf<int [2]> _inport;
	int _data[2];

	struct entryHandler
	{
		int key;
		void (*function)(MY_BEHAVIOR *);
	};

	struct sigHandler
	{
		int key;
		YARPFSMStateBase<MY_BEHAVIOR, SHARED_DATA> *state;
		void (*function)(MY_BEHAVIOR *, YARPFSMStateBase<MY_BEHAVIOR, SHARED_DATA> *);
	};
	
	typedef std::list<entryHandler> ENTRY_HANDLER_LIST;
	typedef ENTRY_HANDLER_LIST::iterator ENTRY_HANDLER_IT;

	typedef std::list<sigHandler> ENTRY_SIGNALS;
	typedef ENTRY_SIGNALS::iterator ENTRY_SIGNALS_IT;

	ENTRY_HANDLER_LIST _table;
	ENTRY_SIGNALS	   _signals;

	int _key;
};

template <class MY_BEHAVIOR, class SHARED_DATA>
void sigFunction(YARPBehavior<MY_BEHAVIOR, SHARED_DATA> *fsm, YARPFSMStateBase<MY_BEHAVIOR, SHARED_DATA> *s)
{
	fsm->pulse(s);
};

#endif