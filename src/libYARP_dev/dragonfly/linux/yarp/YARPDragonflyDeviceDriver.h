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
///       YARP - Yet Another Robotic Platform (c) 2005
///
///                    #paulfitz#
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPDragonflyDeviceDriver.h,v 1.1 2005-10-28 12:54:18 eshuy Exp $
///
///

#ifndef __YARPDragonflyDeviceDriverh__
#define __YARPDragonflyDeviceDriverh__

//=============================================================================
// YARP Includes
//=============================================================================

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPScheduler.h>


/**
 * \file YARPDragonflyDeviceDriver.h device driver for managing the Dragonfly IEEE-1394 Camera.
 */

/**
 * Structure for defining the open() parameters of the camera.
 */
struct DragonflyOpenParameters
{
	/**
	 * Constructor. Add here the parameters for the open().
	 */
	DragonflyOpenParameters()
	{
		// parameters initialization
		_unit_number = 0;
		_size_x = 0;
		_size_y = 0;
		_video_type = 0;
		_offset_y = 0;
		_offset_x = 0;
		_alfa = 0;
	}

	// Parameters
	unsigned int _unit_number;
	unsigned int _size_x;
	unsigned int _size_y;
	unsigned int _video_type;
	int _offset_y;				/** NOT USED */
	int _offset_x;				/** NOT USED */
	float _alfa;				/** NOT USED */
};

class YARPDragonflyDeviceDriver : 
	public YARPDeviceDriver<YARPNullSemaphore, YARPDragonflyDeviceDriver>, public YARPThread
{
private:
	YARPDragonflyDeviceDriver(const YARPDragonflyDeviceDriver&);
	void operator=(const YARPDragonflyDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	YARPDragonflyDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~YARPDragonflyDeviceDriver();

	/**
	 * Opens the device driver. For the meaning of parameters see DragonflyOpenParameters.
	 * @param p is a pointer to the open arguments.
	 * @return returns YARP_OK on success.
	 */
	virtual int open(void *d);

	/**
	 * Closes the device driver.
	 * @return returns YARP_OK on success.
	 */
	virtual int close(void);

protected:
	/**
	 * Locks the current image buffer.
	 * @param buffer is a pointer to the buffer address (i.e. a double pointer).
	 * @return YARP_OK if successful.
	 */
	virtual int acquireBuffer(void *buffer);

	/**
	 * Releases the current image buffer.
	 * @param cmd is not used.
	 * @return YARP_OK if successful.
	 */
	virtual int releaseBuffer(void *cmd);

	/**
	 * Waits on a new frame. An event is signaled when a new frame is acquired,
	 * the calling thread waits efficiently on this event.
	 * @param cmd is not used.
	 * @return YARP_OK on success.
	 */
	virtual int waitOnNewFrame (void *cmd);

	/**
	 * Returns the width of the acquired image.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int getWidth(void *cmd);

	/**
	 * Returns the height of the acquired image.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int getHeight(void *cmd);

	/**
	 * Sets the average image brightness. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setBrightness (void *cmd);

	/**
	 * Sets the acquisition hue. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setHue (void *cmd);

	/**
	 * Sets the acquisition contrast. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setContrast (void *cmd);

	/**
	 * Sets the gain of the amplifier on the U chroma channel. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setSatU (void *cmd);

	/**
	 * Sets the gain of the amplifier on the V chroma channel. UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setSatV (void *cmd);

	/**
	 * Enables the notch filter (bt848 hardware). UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setLNotch (void *cmd);

	/**
	 * Enables the decimation filter (bt848 hardware). UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setLDec (void *cmd);

	/**
	 * Enables the crush filter (bt848 hardware). UNIMPLEMENTED.
	 * @param cmd is a pointer to an integer.
	 * @return YARP_OK on success.
	 */
	virtual int setCrush (void *cmd);

protected:
	void *system_resources;

	virtual void Body(void);
};


#endif
