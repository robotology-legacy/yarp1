// $Id: YARPJR3DeviceDriver.cpp,v 1.2 2003-06-20 16:58:35 babybot Exp $

#include "YARPJR3DeviceDriver.h"

#include <sys/jr3pci_ft.h>

int YARPJR3DeviceDriver::open(void *d)
{
	JR3OpenParameters *tmp = (JR3OpenParameters *) d;

	p_num = (tmp->_nump-1);
	
	int a = init_jr3(tmp->_vendorID, tmp->_deviceID, 1, tmp->_nump, 1);
	return a;
}

int YARPJR3DeviceDriver::close(void)
{
	close_jr3();

	return 0;	// no error
}

int YARPJR3DeviceDriver::resetOffsets(void *d)
{
	int ret = reset_offsets (p_num);
	return ret;
}

int YARPJR3DeviceDriver::setActualFullScales(void *f)
{
	six_axis_array *fs = (six_axis_array *) f;
	int ret = set_full_scales(*fs, p_num);
	return ret;
}

int YARPJR3DeviceDriver::setMinFullScales(void *d)
{
	six_axis_array fs = get_recommended_full_scales(MIN_F_S, p_num);
	int ret = set_full_scales(fs,p_num);
	return ret;
}

int YARPJR3DeviceDriver::setMaxFullScales(void *d)
{
	six_axis_array fs = get_recommended_full_scales(MAX_F_S, p_num);
	int ret = set_full_scales(fs,p_num);
	return ret;
}

int YARPJR3DeviceDriver::readData(void *d)
{
	force_array forces = read_ftdata(FILTER3, p_num);

	short *tmp = (short *)d;

	tmp[0] = forces.fx;
	tmp[1] = forces.fy;
	tmp[2] = forces.fz;

	tmp[3] = forces.mx;
	tmp[4] = forces.my;
	tmp[5] = forces.mz;

	return 0;
}