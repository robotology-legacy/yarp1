//
// YARPSmallStateMachine.h
//

#ifndef __YARPSmallStateMachineh__
#define __YARPSmallStateMachineh__

#include "YARPSafeNew.h"
#include "YARPSemaphore.h"

//
//
// LATER: remove CALLER type from the template arg list.
//

//
// 2 types of MT support.
//	- use YARPMTStateMachine to support thread safe calls
//	- use YARPSTStateMachine for single thread access 
//

typedef YARPSemaphore YARPMTStateMachine;
class YARPSTStateMachine
{
	inline Wait() {}
	inline Post() {}
	YARPSTStateMachine(int i) {}
};

//
//
//
template <class DERIVED, class MTTYPE, class CALLER>
class SmallStateMachine
{
protected:
	typedef void (DERIVED::*PFVOID)(CALLER *);

	int m_state_num;	// current state.
	int m_max_state;	// maximum of the state.
	PFVOID *m_array;	// array of functions to call.

	MTTYPE m_sema;

	inline void Lock () { m_sema.Wait(); }
	inline void Unlock () { m_sema.Post(); }

	virtual void Common (CALLER *) 
	{
		printf ("SmallStateMachine: calling the empty Common function\n");
	}

	void SetState (PFVOID f)
	{
		for (int i = 0; i <= m_max_state; i++)
		{
			if (m_array[i] == f)
			{
				m_state_num = i;
				break;
			}
		}

		if (i > m_max_state)
			printf ("YARPSmallStateMachine: state not found\n");
	}

	PFVOID GetState (void) const
	{
		return m_array[m_state_num];
	}

public:
	SmallStateMachine (int size) : m_sema(1)
	{
		assert (size >= 0);
		m_state_num = 0;
		m_max_state = size-1;
		m_array = new PFVOID[size];
		assert (m_array != NULL);
	}

	virtual ~SmallStateMachine () 
	{
		if (m_array != NULL)
			delete[] m_array;
	}

	// to force a change of state (usually to initialize or reset).
	void SetState (int num)
	{
		Lock ();
		if (num >= 0 && num <= m_max_state)
		{
			m_state_num = num;
		}
		Unlock ();
	}

	virtual void Update (DERIVED* instance, CALLER *caller)
	{
		Lock ();
		Common(caller);

		(instance->*m_array[m_state_num])(caller);	// calls with the default arg "caller".
		Unlock ();
	}
};

//
// example? 
// class Derived : public SmallStateMachine<Derived>
// { ... }
//
#endif