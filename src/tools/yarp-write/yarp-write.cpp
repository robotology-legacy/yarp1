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
/// $Id: yarp-write.cpp,v 1.11 2004-09-14 17:38:10 eshuy Exp $
///
///

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPBottleContent.h>

#include <yarp/debug.h>

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
  //set_yarp_debug(100,100);

  argc--;
  argv++;

  if (argc<1) {
    return YARP_FAIL;
  }

  YARPOutputPortOf<YARPBottle> out_port(YARPOutputPort::MANY_OUTPUTS);
  out_port.SetRequireAck(0);
  //YARPOutputPortOf<YARPBottle> out_port(YARPOutputPort::MANY_OUTPUTS, YARP_UDP);
  //out_port.SetAllowShmem(0);	/// uncommenting this disables SHMEM communication.
  out_port.Register(argv[0]);
  argc--;
  argv++;

  while (argc>=1) {
    out_port.Connect(argv[0]);
    argc--;
    argv++;
  }

  while (!(cin.bad()||cin.eof())) {
    // make sure this works on windows
    char buf[25600];
    cin.getline(buf,sizeof(buf),'\n');

    if (!(cin.bad()||cin.eof())) {
      YARPBottle& bottle = out_port.Content();
      bottle.reset();
      bottle.writeInt(0);
      bottle.writeText(buf);
      out_port.Write(1);
    }
  }

  YARPBottle& bottle = out_port.Content();
  bottle.reset();
  bottle.writeInt(1);
  bottle.writeText("<EOF>");
  out_port.Write(1);
  out_port.FinishSend();

  return YARP_OK;
}
