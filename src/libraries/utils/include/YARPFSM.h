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
/// $Id: YARPFSM.h,v 1.2 2003-07-07 16:11:07 babybot Exp $
///  
/// Finite State Machine class -- by nat July 2003
//

#ifndef __YARPFSMH__
#define __YARPFSMH__

#include <list>

template <class FSM_SHARED_DATA>
class YARPFSMInput
{	
	public:
		virtual bool input(FSM_SHARED_DATA *input) = 0;
};

// forward
template <class MYFSM, class FSM_SHARED_DATA>
class YARPFSMStateBase;

template <class MYFSM, class FSM_SHARED_DATA>
class YARPFSMTableEntry
{
	public:
	YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> *state;
	YARPFSMInput<FSM_SHARED_DATA> *function;

	// operator == used by <list> to remove entries
	bool operator == (const YARPFSMTableEntry<MYFSM, FSM_SHARED_DATA> &i)
	{
		if ( (function == i.function) &&
			(state == i.state) )
			return true;
		else
			return false;
	}

};

template <class MYFSM, class FSM_SHARED_DATA>
class YARPFSMStateBase
{
protected:
	YARPFSMStateBase()
	{ _default = this; }
	virtual ~YARPFSMStateBase(){}

	typedef YARPFSMTableEntry<MYFSM, FSM_SHARED_DATA> FSM_TABLE_ENTRY;
	typedef std::list<FSM_TABLE_ENTRY> FSM_TABLE;
	typedef FSM_TABLE::iterator FSM_TABLE_IT;

public:
	bool decideState(MYFSM *t, FSM_SHARED_DATA *d)
	{
		FSM_TABLE_IT it;
		// if table is empty use default value
		if (_table.empty())
		{
			t->changeState(_default);
			return false;
		}

		// else
		bool flag = false;
		it = _table.begin();
	
		while (it != _table.end())
		{
			flag = it->function->input(d);
	
			if (flag)
			{
				t->changeState(it->state);
				return true;;
			}
			it++;
		}
		// none of the input were valid, use default
		t->changeState(_default);
		return false;
	}

	// virtual functions
	virtual void handle(void *) = 0;
	virtual bool wakeUp(void *) = 0;

	void setDefault(YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> *n)
	{ _default = n; }

	FSM_TABLE _table;
	YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> *_default;
};

template <class MYFSM, class FSM_SHARED_DATA>
class YARPFSMState: public YARPFSMStateBase<class MYFSM, class FSM_SHARED_DATA>
{
	public:
	virtual bool wakeUp(void *)
	{ return true; }

	YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> *operator &()
	{
		return (YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> *) this;
	}
};

template <class MYFSM, class FSM_SHARED_DATA>
class YARPFSMStateSynchro: public YARPFSMStateBase<class MYFSM, class FSM_SHARED_DATA>
{
	public:
	virtual bool wakeUp(void *) = 0;

	YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> *operator &()
	{
		return (YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> *) this;
	}
};

template <class MYFSM, class FSM_SHARED_DATA>
class YARPFSM
{
public:
	typedef YARPFSMStateBase<MYFSM, FSM_SHARED_DATA> FSM_STATE;

	YARPFSM (FSM_SHARED_DATA *s)
	{
		_data = s;
	}

	void doYourDuty()
	{
		if (state->wakeUp(NULL))
		{
			state->decideState((MYFSM *) this, _data);
			state->handle(NULL);
		}
	}

	void add(YARPFSMInput<FSM_SHARED_DATA> *in,  FSM_STATE *s1, FSM_STATE *s2)
	{
		if (in == NULL)
		{
			s1->setDefault(s2);
			s1->_table.clear();
		}
		else
		{
			YARPFSMTableEntry<MYFSM, FSM_SHARED_DATA> tmp;
			tmp.function = in;
			tmp.state = (FSM_STATE *) s2;
			FSM_STATE *tmpS = (FSM_STATE *)s1;
			tmpS->_table.push_back(tmp);
		}
	}

	void remove(YARPFSMInput<FSM_SHARED_DATA> *in,  FSM_STATE *s1, FSM_STATE *s2)
	{
		if (in == NULL)
			return;

		YARPFSMTableEntry<MYFSM, FSM_SHARED_DATA> tmp;
		tmp.function = in;
		tmp.state = s2;
	
		s1->_table.remove(tmp);
		
	}

	void changeState(FSM_STATE *n)
	{ state = n; }

	void setInitialState(FSM_STATE *n)
	{ changeState(n); }
	
	FSM_SHARED_DATA *_data;
	FSM_STATE *state;
};

#endif
