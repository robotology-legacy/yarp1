/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPJR3DeviceDriver.cpp,v 1.1 2004-07-13 13:21:07 babybot Exp $
///

#include "YARPJR3DeviceDriver.h"

#include "../dd_orig/include/jr3pci_ft.h"

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