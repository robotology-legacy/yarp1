/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
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
/// $Id: headsaccades.cpp,v 1.4 2004-08-02 09:16:15 babybot Exp $
///
///

// headsaccades.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPScheduler.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPString.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPParseParameters.h>

#include "SaccadeBehavior.h"

const int __inSize = 2;
const int __outSize = 5;
const char *__baseName = "/headsaccades/";
const char *__configFile = "headcontrol.ini";

int main(int argc, char* argv[])
{
	YARPScheduler::setHighResScheduling();

	bool selectiveReaching = false;
	if (YARPParseParameters::parse(argc, argv, "-strict_reaching"))
		selectiveReaching = true;

	SBSharedData _sdata(selectiveReaching);
	SaccadeBehavior _behavior(&_sdata);
	SBOpenLoop		_openLoop;
	SBClosedLoop	_closedLoop;
	SBStop			_stop;
	SBWaitIdle		_wait("WaitIdle");
	// SBManualInitSaccade	_start("Init saccade");
	SBWaitFixation	_fixation("Wait fixation");
	SBWaitIdle		_refract("Refractory time\n", 1);
	SBWaitIdle		_waitNewTarget("Wait for new target\n", 0.5);
	SBInitSaccade   _initSaccade;
	// SBSimpleOutput  _outRef;
	SBCheckOpenLoopDone _olDone;
	SBCheckSaccadeDone  _saccadeDone;

	SBNewTarget		_newTarget;

	_behavior.setInitialState(&_waitNewTarget);
	_behavior.add(&_newTarget, &_waitNewTarget, &_initSaccade);
	_behavior.add(NULL, &_initSaccade, &_openLoop);
	_behavior.add(&_olDone, &_openLoop, &_closedLoop);
	_behavior.add(&_saccadeDone, &_closedLoop, &_stop);
	_behavior.add(NULL, &_stop, &_wait);
	_behavior.add(&_fixation, &_wait, &_refract);
	_behavior.add(NULL, &_refract, &_waitNewTarget);
	
	//	_behavior.add(&_start, &_wait, &_openLoop);
	// _behavior.Begin();
	while(true)
	{
		_behavior.doYourDuty();
	}
		
	return 0;
}
