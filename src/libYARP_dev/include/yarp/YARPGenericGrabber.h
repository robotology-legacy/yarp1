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
/// $Id: YARPGenericGrabber.h,v 1.1 2004-07-29 22:11:46 babybot Exp $
///
///

#ifndef __YARPGenericGrabberh__
#define __YARPGenericGrabberh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPFrameGrabberUtils.h>

/**
 * \file YARPGenericGrabber.h a generic class incapsulating image acquisition.
 * Actual images might be coming from disk, network (maybe), grabber, digital
 * sources, etc. depending on the actual device driver employed.
 */

/**
 * A generic class incapsulating image acquisition.
 * This template class has two parameters. The first is called ADAPTER and
 * is an instance of a YARPDeviceDriver. This represents the actual hardware
 * and contains a standard interface. The second argument is called 
 * PARAMETERS and represents the details of the specific use of the 
 * hardware. It can contain specific initialization procedures, additional
 * methods, etc. Although this class refers to a grabbing procedure, it can
 * read from digital sources as well.
 */
template <class ADAPTER, class PARAMETERS>
class YARPGenericGrabber
{
protected:
	ADAPTER _adapter;		/// adapts the hw idiosyncrasies
	PARAMETERS _params;		/// actual grabber specific parameters

public:
	/**
	 * Constructor.
	 */
	YARPGenericGrabber () {}

	/**
	 * Destructor.
	 */
	~YARPGenericGrabber () {}

	/**
	 * Initializes the frame grabber.
	 * @param board is the number of the board. Certain device
	 * drivers allow for multiple instances and require specifying which
	 * instance to address commands to.
	 * @param sizex is the requested width of the acquired image.
	 * @param sizey is the requested height of the image.
	 * @return YARP_OK on success.
	 */
	int initialize (int board, int sizex, int sizey = -1);

	/**
	 * Initializes the frame grabber.
	 * @param param is a reference to the specific device driver structure
	 * containing initialization parameters.
	 * @return YARP_OK on success.
	 */
	int initialize (const PARAMETERS &param);

	/**
	 * Stops the frame grabber.
	 * @return YARP_OK on success.
	 */
	int uninitialize (void);

	/**
	 * Acquires (locks) the actual frame buffer (of raw type).
	 * @param buffer is the pointer to the frame buffer. Note the double pointer.
	 * @return YARP_OK on success.
	 */
	int acquireBuffer (unsigned char **buffer);

	/**
	 * Tells the driver that the user has finished with the frame buffer.
	 * @return YARP_OK on success.
	 */
	int releaseBuffer (void);

	/**
	 * Waits for a new frame. A new frame is available if this call 
	 * returns YARP_OK.
	 * @return YARP_OK on success.
	 */
	int waitOnNewFrame (void);

	/**
	 * Returns the actual width of the acquired image.
	 * @param w is the width.
	 * @return YARP_OK on success.
	 */
	int getWidth (int *w);

	/**
	 * Returns the actual height of the acquired image.
	 * @param h is the height.
	 * @return YARP_OK on success.
	 */
	int getHeight (int *h);

	/**
	 * Set the brightness.
	 * @param bright is the brightness value.
	 * @return YARP_OK on success.
	 */
	int setBright(unsigned int bright);

	/**
	 * Set the hue gain.
	 * @param hue is the value. Certain frame grabbers have
	 * a global hue control.
	 * @return YARP_OK on success.
	 */
	int setHue(unsigned int hue);

	/**
	 * Set the contrast gain.
	 * @param contrast is the new contrast value.
	 * @return YARP_OK on success.
	 */
	int setContrast(unsigned int contrast);

	/**
	 * Set the U channel gain.
	 * @param satu is the new value.
	 * @return YARP_OK on success.
	 */
	int setSatU(unsigned int satu);

	/**
	 * Set the V channel gain.
	 * @param satu is the new value.
	 * @return YARP_OK on success.
	 */
	int setSatV(unsigned int satv);

	/**
	 * Sets the Notch filter (specific of bt848 chips, others might have similar things).
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setLNotch(int state);

	/**
	 * Sets the Decimate filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setLDec(int state);

	/**
	 * Sets the peak filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setPeak(int state);

	/**
	 * Sets the automatic gain control filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setCagc(int state);

	/**
	 * Sets the chroma kill filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setCkill(int state);

	/**
	 * Sets the ADC range filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setRange(int state);

	/**
	 * Sets the luma sleep filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setYsleep(int state);

	/**
	 * Sets the chroma sleep filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setCsleep(int state);

	/**
	 * Sets the Crush filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setCrush(int state);

	/**
	 * Sets the Gamma correction filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setGamma(int state);

	/**
	 * Sets the Dithering filter.
	 * @param state is the new value.
	 * @return YARP_OK on success.
	 */
	int setDithFrame(int state);
};

///
///
///
///
template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::initialize (int board, int sizex, int sizey /* = -1 */)
{
	_params._unit_number = board;
	_params._video_type = 0;
	_params._size_x = sizex;
	if (sizey > 0)
		_params._size_y = sizey;
	else
		_params._size_y = sizex;

	/// calls the adapter init that parses the params appropriately.
	/// this is because initialization can vary depending on the specific setup.
	return _adapter.initialize (_params);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::initialize (const PARAMETERS &param)
{
	_params = param;
	
	/// calls the adapter init that parses the params appropriately.
	/// this is because initialization can vary depending on the specific setup.
	return _adapter.initialize (_params);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::uninitialize (void)
{
	return _adapter.uninitialize ();
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::acquireBuffer (unsigned char **buffer)
{
	return _adapter.IOCtl (FCMDAcquireBuffer, (void *)buffer);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::releaseBuffer (void)
{
	return _adapter.IOCtl (FCMDReleaseBuffer, NULL);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::waitOnNewFrame (void)
{
	return _adapter.IOCtl (FCMDWaitNewFrame, NULL);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::getWidth (int *w)
{
	return _adapter.IOCtl (FCMDGetSizeX, w);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::getHeight (int *h)
{
	return _adapter.IOCtl (FCMDGetSizeY, h);
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setBright(unsigned int bright)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetBright, &bright);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setHue(unsigned int hue)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetHue, &hue);
	return ret; 	
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setContrast(unsigned int contrast)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetContrast, &contrast);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setSatU(unsigned int satu)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetSatU, &satu);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setSatV(unsigned int satv)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetSatV,&satv);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setLNotch(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetLNotch,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setLDec(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetLDec,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setPeak(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetPeak,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCagc(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCagc,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCkill(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCkill,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setRange(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetRange,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setYsleep(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetYsleep,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCsleep(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCsleep,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setCrush(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetCrush,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setGamma(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetGamma,&state);
	return ret;
}

template <class ADAPTER, class PARAMETERS>
int YARPGenericGrabber<ADAPTER, PARAMETERS>::setDithFrame(int state)
{
	int ret;
	ret = _adapter.IOCtl(FCMDSetDithFrame,&state);
	return ret;
}
#endif
