// $Id: YARPNIDAQDeviceDriver.cpp,v 1.3 2003-04-20 21:36:25 natta Exp $

#include "YARPNIDAQDeviceDriver.h"

#include <sys/nidaqex.h>


int YARPNIDAQDeviceDriver::open(void *d)
{
	NIDAQOpenParameters *tmp = (NIDAQOpenParameters *) d;

	m_handle = (void *) tmp->device_id;

	return 0;	// no error
}

int YARPNIDAQDeviceDriver::close(void)
{
	// empty

	// TODO: check whether we need a SCAN stop or something like that ...

	return 0;	// no error
}

int YARPNIDAQDeviceDriver::ai_configure(void *p)
{
	i16 st = 0;
	NIDAQAIConfigureParameters *tmp = (NIDAQAIConfigureParameters *) p;

	st = AI_Configure((i16) m_handle,tmp->chan,
						tmp->inputMode,
						tmp->inputRange,
						tmp->polarity,
						tmp->driveAIS);
	return st;
}

int YARPNIDAQDeviceDriver::scan_setup(void *p)
{
	i16 st = 0;
	NIDAQScanSetupParameters *tmp = (NIDAQScanSetupParameters *) p;

	st = SCAN_Setup((i16) m_handle, tmp->nCh,
					tmp->ch_sequence,
					tmp->ch_gains);

	return st;
}

int YARPNIDAQDeviceDriver::ai_vread_scan(void *p)
{
	i16 st = 0;

	st = AI_VRead_Scan((i16) m_handle, (f64 *) p);

	return st;
}

int YARPNIDAQDeviceDriver::ai_read_scan(void *p)
{
	i16 st = 0;

	st = AI_Read_Scan((i16) m_handle, (i16 *) p);

	return st;
}