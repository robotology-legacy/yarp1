// YARPCounter.h: interface for the YARPCounter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YARPCOUNTER_H__37044881_1517_4ED4_8FD0_D8533E2875D7__INCLUDED_)
#define AFX_YARPCOUNTER_H__37044881_1517_4ED4_8FD0_D8533E2875D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class YARPCounter  
{
public:
	void inc();
	void setLevel(int level);
	void reset();
	void reset(int size);
	YARPCounter();
	YARPCounter(int size);
	virtual ~YARPCounter();

private:
	int _size;
	int _level;
};

#endif // !defined(AFX_YARPCOUNTER_H__37044881_1517_4ED4_8FD0_D8533E2875D7__INCLUDED_)
