//
// Define a common interface  for device drivers
//
// 
// feb 2003 -- by nat and pasa
// $ID$

#ifndef __YARP_DEVICE_DRIVER__
#define __YARP_DEVICE_DRIVER__

#include <conf/YARPConfig.h>

//
// .h must be compilable across multiple architectures (NT, Linux, QNX) with minimal
//	conditional compile
//

template <class SYNC, class DERIVED>
class YARPDeviceDriver
{
public:
	YARPDeviceDriver(int n_cmds)
	{
		m_cmds = new  cmd_function_t [n_cmds];

		for(int i = 0; i < n_cmds; i++)
			m_cmds[i] = &DERIVED::defaultCmd;

		m_handle = 0;
	}

	virtual ~YARPDeviceDriver()
	{
		delete m_cmds;

		if (m_handle != 0)
			((DERIVED *) this)->close();
	}

protected:

	void *m_handle;
	//char szBuffer[64]; moved to the derivate class
	//long rc;

	typedef int (DERIVED::* cmd_function_t)(void *);

	cmd_function_t *m_cmds;
	int defaultCmd (void *p)
	{
		// default function (for safety)
		return 0;
	}

	inline void lock(void)
	{m_mutex.Wait();}	// add timeout ?

	inline void unlock(void)
	{m_mutex.Post();}

	SYNC m_mutex;

public:
	// open the device driver
	virtual int open(void *) = 0;
	// close the device driver
	virtual int close() = 0;

	// send command 
	int IOCtl(int cmd, void *data)
	{
		lock();

		int ret = ((DERIVED *)this->*m_cmds[cmd])(data);

		unlock();
		return ret;
	}

};

#endif
