// HState.h
//
//
// July 2002 -- nat
// Januray 2003 -- smooth and saccadic cotrol by nat
//////////////////////////////////////////////////////////////////////

#define __reachingstateh__

#ifndef __reachingstateh__
#define __reachingstateh__

#include "HeadThread.h"

// #define HEAD_STATE_VERBOSE		//define this to enable debug messages
#ifdef HEAD__STATE_VERBOSE
	#include <iostream>
	#define HEAD_STATE_DEBUG(string) \
		std::cout << "HSTATE: " << string << flush
#else
	#define HEAD_STATE_DEBUG if (1)
#endif

//forward declarations
class HeadThread;

// abstract class to implement tracking states  --> see state pattern
class HState
{
protected:
	HState(){
		deltaQ.Resize(_head::_nj);
		deltaQ = 0;
		count = 0;
	}
	virtual ~HState(){}
	
public:
	// virtual function
	virtual void handle(HeadThread*){}
	void changeState (HeadThread*, HState *);

protected:
	CVisDVector deltaQ;		// tmp command
	int count;
};

class State1: public HState
{
	public:
		State1(){};
	virtual void handle(HeadThread *t);
	
	static State1 _instance;

	inline static State1* Instance()
		{return &_instance;}

};

class State2: public HState
{
	public:
		State2() {};
	virtual void handle(HeadThread *t);
	
	static State2 _instance;

	inline static State2* Instance()
		{return &_instance;}
};
	  
class State3: public HState
{
	public:
		State3() {};
	virtual void handle(HeadThread *t);
	
	static State3 _instance;

	inline static State3* Instance()
		{return &_instance;}
};

class State4: public HState
{
	public:
		State4() {};
	virtual void handle(HeadThread *t);
	
	static State4 _instance;

	inline static State4* Instance()
		{return &_instance;}
};

class HStateSmooth: public HState
{
	public:
		HStateSmooth(){};
	virtual void handle(HeadThread *t);
	
	static HStateSmooth _instance;

	inline static HStateSmooth* Instance()
		{return &_instance;}

};

class HStateSaccadeInit: public HState
{
	public:
		HStateSaccadeInit(){};
	virtual void handle(HeadThread *t);
	
	static HStateSaccadeInit _instance;

	inline static HStateSaccadeInit* Instance()
		{return &_instance;}

};

class HStateSaccade: public HState
{
	public:
		HStateSaccade(){
			count = 0;
			n_steps = _head::_ns_saccade;
		};
	virtual void handle(HeadThread *t);
	
	static HStateSaccade _instance;

	inline static HStateSaccade* Instance()
		{return &_instance;}

	inline void set_steps(int s)
		{n_steps = s;}

	protected:
		int count;
		int n_steps;
};

class HStateSmoothNoSaccade: public HState
{
	public:
		HStateSmoothNoSaccade(){
			count = 0;
			n_steps = _head::_ns_smooth_no_saccade;
		};
	virtual void handle(HeadThread *t);
	
	static HStateSmoothNoSaccade _instance;

	inline static HStateSmoothNoSaccade* Instance()
		{return &_instance;}

	inline void set_steps(int s)
		{n_steps = s;}

	protected:
		int count;
		int n_steps;
};


#endif	//.h