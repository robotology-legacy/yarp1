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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPPicoloDeviceDriver.h,v 1.1 2004-07-13 13:21:10 babybot Exp $
///
///

#ifndef __YARPPicoloDeviceDriverh__
#define __YARPPicoloDeviceDriverh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPDeviceDriver.h>

#include <stdlib.h>
#include <string.h>

struct PicoloOpenParameters
{
	/// add here params for the open.
	PicoloOpenParameters()
	{
		_unit_number = 0;
		_video_type = 0;
		_size_x = 256;
		_size_y = 256;
		_offset_y = 0;
		_offset_x = 0;
		_alfa = 1.055f;
		// _alfa = 1.125f;
	}

	int _unit_number;		/// board number 0, 1, 2, etc.
	int _video_type;		/// 0 composite, 1 svideo.
	int _size_x;			/// requested size x.
	int _size_y;			/// requested size y.
	int _offset_y;			/// y offset, with respect to the center 
	int _offset_x;			/// x offset, with respect to the center
	float _alfa;			/// to have the possibility to shift the roi vertically, the requested size is actually _alfa*_size_y
};


class YARPPicoloDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPPicoloDeviceDriver>, public YARPThread
{
private:
	YARPPicoloDeviceDriver(const YARPPicoloDeviceDriver&);
	void operator=(const YARPPicoloDeviceDriver&);

public:
	YARPPicoloDeviceDriver();
	virtual ~YARPPicoloDeviceDriver();

	// overload open, close
	virtual int open(void *d);
	virtual int close(void);

	virtual int acquireBuffer(void *);
	virtual int releaseBuffer(void *);
	virtual int waitOnNewFrame (void *cmd);
	virtual int getWidth(void *cmd);
	virtual int getHeight(void *cmd);
	virtual int setBrightness (void *cmd);
	virtual int setHue (void *cmd);
	virtual int setContrast (void *cmd);
	virtual int setSatU (void *cmd);
	virtual int setSatV (void *cmd);
	virtual int setLNotch (void *cmd);
	virtual int setLDec (void *cmd);
	virtual int setCrush (void *cmd);

protected:
	void *system_resources;

	virtual void Body(void);
};


#endif