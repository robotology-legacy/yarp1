/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
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
/// $Id: Controller.H,v 1.3 2004-05-02 22:25:12 babybot Exp $
///
///

#ifndef __ControllerH__
#define __ControllerH__

/* 
 * the purpose of including this file on Linux/Winnt/Qnx is to 
 * get the definition of messages and params of the dsp controller
 * define __ONLY_DEF before inclusion on Linux/Winnt/Qnx
 */

/* beware of true/false definitions */
#ifndef __ONLY_DEF
#define false 0
#define true 1
#endif

#define DEFAULT_BOARD_ID	1		/* default ID for receiving messages (4 bit field) */
#define SMALL_BUFFER_SIZE 	10		/* for serial I/O */
#define CONTROLLER_PERIOD 	1		/* espressed in ms */
#define JN 2						/* number of axes, can't be changed anyway */

#define MODE_IDLE		0
#define MODE_POSITION 	1
#define MODE_VELOCITY	2

#define DEFAULT_VELOCITY 10
#define DEFAULT_ACCELERATION 10

#define FLASH_START_ADDR 0x2000

#ifndef __ONLY_DEF
/* deals with the endianism - byte 4 is the MSB on the Pentium */
#define BYTE_1(x) (__shr(__extract_h(x), 8))
#define BYTE_2(x) (__extract_h(x) & 0xff)
#define BYTE_3(x) (__shr(__extract_l(x), 8))
#define BYTE_4(x) (__extract_l(x) & 0xff)

/* same as above, deals also with endianism */
#define BYTE_C(x4, x3, x2, x1) \
	(_L_deposit_h(__shl(x1,8) | x2) | \
	 _L_deposit_l(__shl(x3,8) | x4))
	 
#define BYTE_W(x2, x1) (__shl(x1,8) | x2)

#endif

/* CAN bus messages (our protocol) */
#define CHANNEL_0(x)	((x) &= 0x7F)
#define CHANNEL_1(x)	((x) |= 0x80)
#define CHANNEL(x)		((x) & 0x80)

/* CAN packet (our protocol):
 * 
 * byte 0, source ID (4 bits) + dest ID (4 bits)
 * byte 1, MSB, channel no 
 * byte 2, 7LSB, command ID 
 * byte 3-7, payload
 *
 */
 
/*
 * this is 8 bits long, MSB is the channel (0 or 1). 
 */
#define CAN_NO_MESSAGE				0
#define CAN_TOGGLE_POSITION_MODE 	1
#define CAN_TOGGLE_VELOCITY_MODE	2
#define CAN_TOGGLE_VERBOSE			3
#define CAN_CALIBRATE_ENCODER		4
#define CAN_TOGGLE_PWM_PAD			5
#define CAN_GET_CONTROL_MODE		6

#define CAN_WRITE_FLASH_MEM			10
#define CAN_READ_FLASH_MEM			11

#define CAN_GET_ENCODER_POSITION	20
#define CAN_SET_DESIRED_POSITION	21
#define CAN_GET_DESIRED_POSITION	22
#define CAN_SET_DESIRED_VELOCITY	23
#define CAN_GET_DESIRED_VELOCITY	24
#define CAN_SET_DESIRED_ACCELER		25
#define CAN_GET_DESIRED_ACCELER		26

#define CAN_POSITION_MOVE			27
#define CAN_VELOCITY_MOVE			28

#define CAN_SET_P_GAIN				30
#define CAN_GET_P_GAIN				31
#define CAN_SET_D_GAIN				32
#define CAN_GET_D_GAIN				33
#define CAN_SET_I_GAIN				34
#define CAN_GET_I_GAIN				35
#define CAN_SET_ILIM_GAIN			36
#define CAN_GET_ILIM_GAIN			37
#define CAN_SET_OFFSET				38
#define CAN_GET_OFFSET				39
#define CAN_SET_SCALE				40
#define CAN_GET_SCALE				41

#define CAN_SET_BOARD_ID			50
#define CAN_GET_BOARD_ID			51

#define CAN_GET_ERROR_STATUS		60

/* error status values */
#define ERROR_NONE					0			/* no error, all ok */
#define ERROR_UNSPECIFIED			1			/* generic error */
#define ERROR_MODE					2			/* mode error, can't apply command in current mode */
#define ERROR_FMT					3			/* format error, command in wrong format */
#define ERROR_SEND					4			/* can't send answer back */

/* */

#define GetModeAsString(mode) \
	(mode == MODE_IDLE) ? "idle" : \
	(mode == MODE_POSITION) ? "position" : "velocity"

/* this shouldn't be included in Linux/Winnt */
#ifndef __ONLY_DEF

/* prototypes from trajectory.c */
int init_trajectory (byte jj, long current, long final, int speed);
long step_trajectory (byte jj);

/* prototypes from controller.c */
byte serial_interface (void);
byte can_interface (void);
byte calibrate (int jnt);
byte writeToFlash (void);
byte readFromFlash (void);

void print_can (byte data[]);

#endif

#endif
