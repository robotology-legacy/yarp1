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
/// $Id: YARPBehavior.h,v 1.25 2004-07-27 18:02:59 babybot Exp $
///  
/// Behavior class -- by nat July 2003
//

#ifndef __YARPBEHAVIORCLASS__
#define __YARPBEHAVIORCLASS__

#include <YARPFSM.h>
#include <YARPPort.h>
#include <YARPThread.h>

#include "YARPBottle.h"
#include "YARPBottleContent.h"

#define YARP_BEHAVIOR_VERBOSE

#ifdef YARP_BEHAVIOR_VERBOSE
#define YARP_BEHAVIOR_DEBUG(string) YARP_DEBUG("YARP_BEHAVIOR :", string)
#else #define YARP_BEHAVIOR_DEBUG(string) YARP_NULL_DEBUG
#endif

class YARPBehaviorSharedData
{
	public:
		YARPBehaviorSharedData(const YARPString &portName, const YBVocab &label):
		  _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
		  {	
			  _data.setID(label);
			  _outPort.Register(portName.c_str());
		  }

		  void send()
		  {
			  // printf("YARB_BEHAVIOR: sending ");
			  // _data.display();
			  _outPort.Content() = _data;
			  _outPort.Write(1);
			  _data.reset();
		  }

		  void writeAndSend(const YBVocab &v)
		  {
			  _data.writeVocab(v);
			  send();
		  }

		  void writeAndSend(const YVector &v)
		  {
			  _data.writeYVector(v);
			  send();
		  }

	YARPBottle _data;
protected:
	YARPOutputPortOf<YARPBottle> _outPort;
};

template<class MY_SHARED_DATA>
class YARPBaseBehaviorInput
{
public:
	virtual bool input(YARPBottle *, MY_SHARED_DATA *) = 0; 
};

template <class MY_SHARED_DATA>
class YARPPulseState: public YARPFSMInput<MY_SHARED_DATA>
{
	public:
		YARPPulseState()
		{ signaled = false;	}

		bool input(MY_SHARED_DATA *)
		{
			bool ret = signaled ;
			signaled = false;
			return ret;
		}
	
		void post()
		{ signaled = true; }

	bool signaled;
};

template <class MY_BEHAVIOR, class MY_SHARED_DATA>
class YARPBehavior: public YARPThread, public YARPFSM<MY_BEHAVIOR, MY_SHARED_DATA>
{
public:
	typedef YARPPulseState<MY_SHARED_DATA> PulseStates;
	typedef YARPFSMStateBase<MY_BEHAVIOR, MY_SHARED_DATA> MyBaseStates;
	typedef YARPFSMOutput<MY_SHARED_DATA> BaseBehaviorOutput;
	typedef YARPBaseBehaviorInput<MY_SHARED_DATA> BaseBehaviorInput;

	struct BTableEntry
	{
		BaseBehaviorInput *input;		
		MyBaseStates *s;
		PulseStates *activation;
	};

	struct FunctionTableEntry
	{
		BaseBehaviorInput *input;		
		BaseBehaviorOutput *output;
	};

	typedef YARPList<BTableEntry> BEHAVIOR_TABLE;
	typedef YARPList<FunctionTableEntry> FUNCTION_TABLE;
	typedef YARPList<PulseStates *> PULSE_TABLE;
	typedef BEHAVIOR_TABLE::iterator BEHAVIOR_TABLE_IT;
	typedef PULSE_TABLE::iterator PULSE_TABLE_IT;
	typedef FUNCTION_TABLE::iterator FUNCTION_TABLE_IT;

	YARPBehavior(MY_SHARED_DATA *d, const YARPString &pName, const YBVocab &label, const YBVocab &exitC):
	YARPFSM<MY_BEHAVIOR, MY_SHARED_DATA>(d), _inport(YARPInputPort::NO_BUFFERS, YARP_MCAST)
	{
		_key = label;			
		_bottle.setID(_key);	// sent and received bottles have same ID
		_exitCode = exitC;		
		_init(pName);
	}

	~YARPBehavior()
	{
		// destroy pulse states
		PULSE_TABLE_IT it(_pulse_table);
		while(!it.done())
		{
			delete (*it);
			it++;
		}
	}

	// thread body
	virtual void Body(void);

	void pulse(MyBaseStates *s)
	{
		// signal pulse event
		if (checkState(s))
		{
			_stopEvent.signal();
		}
	}

	// handle messages
	int handleMsg();
	// loop untile exit message is received
	void loop()
	{
		while(true)
		{
			if (handleMsg() == -1)
				return ;
		}
	}
	// add a transition: if 'in' is true move from 's1' to 's2' producing 'out'
	void add(BaseBehaviorInput *in, MyBaseStates *s1, MyBaseStates *s2, BaseBehaviorOutput *out = NULL);
	// add a transition: if 'in' then produce 'out', from any state without changing state
	void add(BaseBehaviorInput *in, BaseBehaviorOutput *out);
	
	void updateTable(BTableEntry entry)
	{ _table.push_back(entry); }

	void updateFunctionTable(FunctionTableEntry entry)
	{ _functions.push_back(entry); }

private:
	// parse message
	int _parse(YARPBottle &bottle);
	// handle exit message
	void _quit()
	{
		// put here your code to handle quit
		YARPThread::AskForEnd();
		// signal event so that the thread can exit
		_stopEvent.signal();
		YARPThread::Join(); // wait
	}
	// init class
	void _init(const YARPString &name)
	{ 
		_inport.Register(name.c_str());	// register port
	}

	BEHAVIOR_TABLE _table;
	FUNCTION_TABLE _functions;
	PULSE_TABLE _pulse_table;				//keep track of the input states created, so that we can delete them later

	YBVocab			_key;					// only bottles with this ID are considered
	YBVocab			_exitCode;				// exit when this vocab code is received
	ACE_Auto_Event	_stopEvent;
	
	YBVocab			_tmpVocab;				// temp variable to avoid using a local var within _parse method
	YARPBottle		_bottle;				// ths is the bottle used for the communication
	YARPInputPortOf<YARPBottle> _inport;	// input port
	YARPSemaphore	_mutex;
	inline void _lock(){ _mutex.Wait(); }
	inline void _unlock(){ _mutex.Post(); }
};

template <class MY_BEHAVIOR, class MY_SHARED_DATA> 
void YARPBehavior<MY_BEHAVIOR, MY_SHARED_DATA>::
add(BaseBehaviorInput *in, MyBaseStates *s1, MyBaseStates *s2, BaseBehaviorOutput *out)
{
	PulseStates *tmp = new PulseStates;
	_pulse_table.push_back(tmp);	// keep track of the state so that later we can delete them
	if (in != NULL)
	{
		s1->_as = false;
		YARPFSM<MY_BEHAVIOR, MY_SHARED_DATA>::add(tmp, s1, s2, out);	// add transition
	}
	else
	{
		s1->_as = true;
		YARPFSM<MY_BEHAVIOR, MY_SHARED_DATA>::add(NULL, s1, s2, out);	// add transition
	}
		
	BTableEntry tmpEntry;
	tmpEntry.input = in;
	tmpEntry.activation = tmp;
	tmpEntry.s = s1;
	updateTable(tmpEntry);
}

template <class MY_BEHAVIOR, class MY_SHARED_DATA> 
void YARPBehavior<MY_BEHAVIOR, MY_SHARED_DATA>::
add(BaseBehaviorInput *in, BaseBehaviorOutput *out)
{
	FunctionTableEntry tmpEntry;
	tmpEntry.input = in;
	tmpEntry.output = out;
	updateFunctionTable(tmpEntry);
}

template <class MY_BEHAVIOR, class MY_SHARED_DATA>
int YARPBehavior<MY_BEHAVIOR, MY_SHARED_DATA>::
_parse(YARPBottle &bottle)
{
	if (bottle.getID() != _key)
	{
		// this is not for you, return
		return 0;
	}

	// handle exit code and alive message
	if (bottle.tryReadVocab(_tmpVocab) )
	{
		if (_tmpVocab == _exitCode)
		{
			_quit();
			return -1;
		}
		else if (_tmpVocab == YBVIsAlive)
		{
			YARP_BEHAVIOR_DEBUG(("I'm Alive!\n"));
		}
	}

	//
	#ifdef YARP_BEHAVIOR_EXTRA_DEBUG
	YBVocab tmp;
	if (bottle.tryReadVocab(tmp))
			bottle.display();
	#endif

	// handle signals, global functions
	FUNCTION_TABLE_IT itf(_functions);
	while(!itf.done())
	{
		if ((*itf).input->input(&bottle, _data))
		{
			(*itf).output->output(_data);
			return 1;
		}
		itf++;
	}
	////////////////////////////////////////
				
	// handle signals
	BEHAVIOR_TABLE_IT is(_table);
	while(!is.done())
	{
		if (checkState((*is).s))
			if ((*is).input == NULL)
			{
				// as state, return without executing anything
				return 1;
			}
			else  if ((*is).input->input(&bottle, _data))
			{
				(*is).activation->post();	// enable input
				pulse((*is).s);
				return 1;
			}
		is++;
	}
	/////////////////////////////////////////
	return 1;
}

template <class MY_BEHAVIOR, class MY_SHARED_DATA>
int YARPBehavior<MY_BEHAVIOR, MY_SHARED_DATA>::
handleMsg()
{
	_inport.Read();

	_lock();

	YARPBottle tmp;
	tmp = _inport.Content();

	//ACE_OS::printf("Received:\n");
	//tmp.display();
	int ret = _parse(tmp);

	_unlock();
	return ret;
}

template <class MY_BEHAVIOR, class MY_SHARED_DATA>
void YARPBehavior<MY_BEHAVIOR, MY_SHARED_DATA>::
Body(void)
{
	// first iteration
	_lock();		
		state->handle(_data);
	_unlock();
	if (!currentState()->isAs())
		_stopEvent.wait();
	else 
	{
		// state is AS, just loop !
		// and still check inputs
		_parse(_bottle);
	}

	while(!IsTerminated())
	{
		// handle state
		_lock();
			state->decideState((MY_BEHAVIOR *)this, _data);	//switch state
			state->handle(_data);							// execute
		_unlock();

		// wait, if not asyncrhonous

		if (!currentState()->isAs())
		{
			_stopEvent.wait();
		}
		else 
		{
			// state is AS, just loop !
			// and still check inputs
			_parse(_bottle);
		}
	}
}

#endif