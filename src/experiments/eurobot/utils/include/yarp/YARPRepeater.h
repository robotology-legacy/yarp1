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
///							#nat#
///
///	     "Licensed under the Academic Free License Version 1.0"
///
/// $Id: YARPRepeater.h,v 1.1 2004-12-27 16:54:15 beltran Exp $
///  
//

#ifndef __YARPREPEATER__
#define __YARPREPEATER__

#include <yarp/YARPThread.h>
#include <yarp/YARPBottleContent.h>
#include <yarp/YARPPort.h>
#include <string>

template <class DATATYPE>
class YARPRepeater: public YARPThread
{
public:
	YARPRepeater(YARPString &inPortName, YARPString &outPortName, int inputProtocol, int outputProtocol):
		_inputPort(YARPInputPort::NO_BUFFERS, inputProtocol),
		_outputPort(YARPOutputPort::DEFAULT_OUTPUTS, outputProtocol)
	{
		_inputPort.Register(inPortName.c_str());
		_outputPort.Register(outPortName.c_str());

	//	YARPThread::Begin();
	}

	~YARPRepeater()
	{
	//	YARPThread::End();
	}

	virtual void Body(void)
	{
		while(!IsTerminated())
		{
			if (_inputPort.Read())
			{
				ACE_OS::printf("Received:\n");
				_inputPort.Content().display();	// this is ok only if DATATYPE is a YARPBottle

				_outputPort.Content() = _inputPort.Content();
				ACE_OS::printf("Sent:\n");
				_outputPort.Content().display();
				_outputPort.Write(1);
			}
		}
	}

	virtual void End(int dontkill = -1)
	{
		YARPThread::AskForEnd();

		_inputPort.End();
		_outputPort.End();

		YARPThread::Join();
	}

	YARPInputPortOf<DATATYPE> _inputPort;
	YARPOutputPortOf<DATATYPE> _outputPort;

	DATATYPE _tmp;
};

#endif;

