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
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: TeleCtrl.cpp,v 1.1 2005-12-02 13:56:55 beltran Exp $
///

// ----------------------------------------------------------------------
// TeleCtrl.cpp - a c++ application which controls a setup (the SLAVE)
//  using commands gathered from a sensory setup (MASTER). Basically this is
//  the frontend between MASTER and SLAVE.
// ----------------------------------------------------------------------

// -----------------------------------
// headers
// -----------------------------------

// standard
#include <iostream>
#include <math.h>

// basic YARP/ace stuff
#include <yarp/YARPConfig.h>
#include <yarp/YARPTime.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPImageFile.h>

// straight from sendCmd (to be understood later on)
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPConfigRobot.h>
// robot hardware
#include <yarp/YARPRobotHardware.h>
// it is necessary to specify these in order for the SLAVE to know about the MASTER
// (should be improved in the future)
#include <yarp/YARPPresSensUtils.h>
#include <yarp/YARPDataGloveUtils.h>
#include <yarp/YARPTrackerUtils.h>
#include <yarp/YARPRobotHardware.h>
// this is needed, too: we must be able to interact with mirrorCollector
// in order to gather data from the MASTER
#include "CollectorCommands.h"

// -----------------------------------
// globals
// -----------------------------------

using namespace std;

// global options
struct SaverOptions {
  int sizeX, sizeY;
  int useCamera, useTracker, useDataGlove, usePresSens;
} _options;

// standard name for port
const char *DEFAULT_TOMASTER_NAME = "TeleCtrlToMaster";
const char *DEFAULT_TOSLAVE_NAME = "TeleCtrlToSlave";
// scripts (dis)connecting TeleCtrl's ports to MASTER's and SLAVE's ones
const char *CONNECT_SCRIPT = "TeleCtrlConnect.bat";
const char *DISCONNECT_SCRIPT = "TeleCtrlDisconnect.bat";

// communication ports with MASTER
YARPInputPortOf<MNumData> _master_data_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
YARPInputPortOf<YARPGenericImage> _master_img_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
YARPInputPortOf<int> _master_rep_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
YARPOutputPortOf<MCommands> _master_cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// communication port with SLAVE
YARPOutputPortOf<YARPBabyBottle> _slave_outport(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);

// prefix for saved images file names
char _prefix[255];
// *the* image
YARPImageOf<YarpPixelBGR> _img;
// data flowing from MASTER to TeleCtrl (glove, pressure, tracker)
MNumData _data;

// -----------------------------------
// prototypes
// -----------------------------------

void prepareDataStructures(void);
void cleanDataStructures(void);
void registerPorts(void);
void unregisterPorts(void);
int main(void);

// -----------------------------------
// streaming thread. this gathers data from the MASTER,
// processes them and the feeds the SLAVE the
// appropriate commands.
// -----------------------------------

class streamingThread : public YARPThread {
public:
  virtual void Body (void);
};

// streaming thread
void streamingThread::Body (void)
{

  int j = 0;
  char imageFileName[255];
 
  cleanDataStructures();
 
  // prepare data structures to send commands to the head
  YARPBabyBottle tmpBottle;
  tmpBottle.setID(YBVMotorLabel);
  YVector headCmd, armCmd;
  headCmd.Resize(5);
  armCmd.Resize(6);
  double azimuth, elevation, roll;
  double prevAzimuth, prevElevation, prevRoll;

  cout << endl << endl << "       \tX\tY\tZ\tAZIM\tELEV\tROLL" << endl;

  // loop until the thread is terminated ( thread.End() )
  while ( !IsTerminated() ) {

    // we assume the MASTER fires at 50Hz, so it's no point looping more often!
    YARPTime::DelayInSeconds(0.02);

    // sequence order number
    j++;
    
    // gather glove, tracker and pressens data
    if ( _options.useDataGlove || _options.useTracker || _options.usePresSens ) {

      // get data
      _master_data_inport.Read();
      _data = _master_data_inport.Content();

      // normalise azimuth
      if ( _data.tracker.azimuth>90.0 ) {
        azimuth = 90.0;
      } else if ( _data.tracker.azimuth<-90.0 ) {
        azimuth = -90.0;
      } else {
        azimuth = _data.tracker.azimuth;
      }
      azimuth /= 3.0;
      azimuth = floor(azimuth);
      // normalise elevation
      if ( _data.tracker.elevation>90.0 ) {
        elevation = 90.0;
      } else if ( _data.tracker.elevation<-90.0 ) {
        elevation = -90.0;
      } else {
        elevation = _data.tracker.elevation;
      }
      elevation /= 3.0;
      elevation = -floor(elevation);
      // normalise roll
      if ( _data.tracker.roll>90.0 ) {
        roll = 90.0;
      } else if ( _data.tracker.roll<-90.0 ) {
        roll = -90.0;
      } else {
        roll = _data.tracker.roll;
      }
      roll /= 2.0;
      roll = floor(roll);

      // output to screen what we are actually sending
      cout.precision(5);
      cout << "Sending\t"
           << floor(_data.tracker.x) << "\t"
           << floor(_data.tracker.y) << "\t"
           << floor(_data.tracker.z) << "\t"
           << floor(_data.tracker.azimuth) << "\t"
           << floor(_data.tracker.elevation) << "\t"
           << floor(_data.tracker.roll) << "         \r";
      cout.flush();

      // prepare vector and send, unless nothing has changed (saves bandwidth)
      // command to head
      if ( azimuth!=prevAzimuth || elevation!=prevElevation ) {
        headCmd(1) = azimuth*M_PI/180.0;
        headCmd(2) = elevation*M_PI/180.0;
        headCmd(3) = 0.0; headCmd(4) = 0.0; headCmd(5) = 0.0;
        tmpBottle.reset();
        tmpBottle.writeVocab(YBVocab(YBVHeadNewCmd));
        tmpBottle.writeYVector(headCmd);
        _slave_outport.Content() = tmpBottle;
        _slave_outport.Write();
        prevElevation=elevation;
      }

      // command to arm
      if ( roll!=prevRoll ) {
        armCmd(1) = 0.0; armCmd(2) = 0.0; armCmd(3) = 0.0;
        armCmd(4) = roll*M_PI/180.0;
        armCmd(5) = 0.0; armCmd(6) = 0.0;
        tmpBottle.reset();
        tmpBottle.writeVocab(YBVocab(YBVArmNewCmd));
        tmpBottle.writeYVector(armCmd);
        _slave_outport.Content() = tmpBottle;
        _slave_outport.Write();
        prevRoll=roll;
      }

    }

    // gather images from the cameras
    if ( _options.useCamera ) {	
      _master_img_inport.Read();
      _img.Refer(_master_img_inport.Content());
      ACE_OS::sprintf(imageFileName,"%s_%03d.pgm", _prefix, j);
      YARPImageFile::Write(imageFileName, _img,YARPImageFile::FORMAT_PPM);
    }

  }
  
  return;

}

// -----------------------------------
// functions & procedures
// -----------------------------------

void prepareDataStructures (void)
{

  _img.Resize (_options.sizeX, _options.sizeY);

}

void cleanDataStructures (void)
{

  // Pressure Sensors
  _data.pressure.channelA = 0;
  _data.pressure.channelB = 0;
  _data.pressure.channelC = 0;
  _data.pressure.channelD = 0;

  // Tracker
  _data.tracker.x = 0.0;
  _data.tracker.y = 0.0;
  _data.tracker.z = 0.0;
  _data.tracker.azimuth = 0.0;
  _data.tracker.elevation = 0.0;
  _data.tracker.roll = 0.0;

  // DataGlove
  _data.glove.thumb[0] = 0;	// inner
  _data.glove.thumb[1] = 0;	// middle
  _data.glove.thumb[2] = 0;	// outer
  _data.glove.index[0] = 0;	// inner
  _data.glove.index[1] = 0;	// middle
  _data.glove.index[2] = 0;	// outer
  _data.glove.middle[0] = 0;	// inner
  _data.glove.middle[1] = 0;	// middle
  _data.glove.middle[2] = 0;	// outer
  _data.glove.ring[0] = 0;	// inner
  _data.glove.ring[1] = 0;	// middle
  _data.glove.ring[2] = 0;	// outer
  _data.glove.pinkie[0] = 0;	// inner
  _data.glove.pinkie[1] = 0;	// middle
  _data.glove.pinkie[2] = 0;	// outer
  _data.glove.abduction[0] = 0; // thumb-index
  _data.glove.abduction[1] = 0; // index-middle
  _data.glove.abduction[2] = 0; // middle-ring
  _data.glove.abduction[3] = 0; // ring-pinkie
  _data.glove.abduction[4] = 0;
  _data.glove.palmArch = 0; // palm
  _data.glove.wrist[0] = 0; // pitch
  _data.glove.wrist[1] = 0; // yaw
  
  _img.Zero();

}

// register ports
void registerPorts()
{

  char portName[255];
  int ret = 0;

  // master input data port registration (glove, tracker, pressens)
  cout << "Registering master input data port..." << endl;
  ACE_OS::sprintf(portName,"/%s/i:str", DEFAULT_TOMASTER_NAME);
  ret = _master_data_inport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL: could not register port " << portName << endl;
    exit(YARP_FAIL);
  }
  
  // master input image port registration
  cout << "Registering master input image port..." << endl;
  ACE_OS::sprintf(portName,"/%s/i:img", DEFAULT_TOMASTER_NAME);
  ret = _master_img_inport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL: could not register port " << portName << endl;
    exit(YARP_FAIL);
  }
  
  // master input command port registration
  cout << "Registering master input cmd port..." << endl;
  ACE_OS::sprintf(portName,"/%s/i:int", DEFAULT_TOMASTER_NAME);
  ret = _master_rep_inport.Register(portName, "default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL ERROR: problems registering port " << portName << endl;
  }

  // master output command port registration
  cout << "Registering master output cmd port..." << endl;
  ACE_OS::sprintf(portName,"/%s/o:int", DEFAULT_TOMASTER_NAME);
  ret = _master_cmd_outport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL ERROR: problems registering port " << portName << endl;
    exit(YARP_FAIL);
  }
  
  // slave output command port registration
  cout << "Registering slave output cmd port..." << endl;
  ACE_OS::sprintf(portName,"/%s/o:str", DEFAULT_TOSLAVE_NAME);
  ret = _slave_outport.Register(portName,"default");
  if (ret != YARP_OK) {
    cout << endl << "FATAL ERROR: problems registering port " << portName << endl;
    exit(YARP_FAIL);
  }

}

void unregisterPorts()
{

  _slave_outport.Unregister();
  _master_cmd_outport.Unregister();
  _master_rep_inport.Unregister();
  _master_img_inport.Unregister();
  _master_data_inport.Unregister();

}

// -----------------------------------
// main loop
// -----------------------------------

int main()
{

  int reply;

  // the streaming thread
  streamingThread myStreamingThread;

  // register communication ports
  registerPorts();

  // launch batch file which yarp-connects ports
  cout << endl << "Now connecting ports. Hit any key upon completion." << endl;
  system(CONNECT_SCRIPT);
  cin.get();

  // declare image
  YARPImageOf<YarpPixelBGR> img;
  _options.sizeX = 0;
  _options.sizeY = 0;

  YARPScheduler::setHighResScheduling();

  // ----------------------------
  // first: connect to the master
  cout << endl << "Initialising master... ";
  cout.flush();
  _master_cmd_outport.Content() = CCMDConnect;
  _master_cmd_outport.Write();
  _master_rep_inport.Read();
  reply = _master_rep_inport.Content();

  if (reply != CMD_FAILED) {
    // success!
    cout << "done." << endl;
    // gather which sensors we use
    _options.useDataGlove = reply & HW_DATAGLOVE;
    _options.useTracker = reply & HW_TRACKER;
    _options.usePresSens = reply & HW_PRESSENS;
    _options.useCamera = reply & HW_CAMERA;
    // and if we use the camera,
    if (_options.useCamera) {
      // gather image size
      _master_rep_inport.Read();
      _options.sizeX = _master_rep_inport.Content();
      _master_rep_inport.Read();
      _options.sizeY = _master_rep_inport.Content();
      cout << "image size is " << _options.sizeX << "x" << _options.sizeY << endl;
    }
    // then set data structures up accordingly
    prepareDataStructures();
  } else {
    cout << "failed." << endl;
    unregisterPorts();
    return 0;
  }

  // ----------------------------
  // second: activate stream from master
  cleanDataStructures();
  // issue streaming command!
  cout << "Starting streaming mode (hit a key to stop)... ";
  cout.flush();
  _master_cmd_outport.Content() = CCMDStartStreaming;
  _master_cmd_outport.Write();
  _master_rep_inport.Read();
  reply = _master_rep_inport.Content();
  if ( reply == CMD_ACK) {
    // start streaming thread
    cout << "done." << endl;
    myStreamingThread.Begin();
  } else {
    cout << "failed." << endl;
    unregisterPorts();
    return 0;
  }

  // wait for a key to stop streaming mode  
  cin.get();

  // stop streaming
  cout << endl << "Stopping streaming mode...";
  cout.flush();
  _master_cmd_outport.Content() = CCMDStopStreaming;
  _master_cmd_outport.Write();
  myStreamingThread.End();
  _master_rep_inport.Read();
  reply = _master_rep_inport.Content();
  if ( reply == CMD_ACK ) {
    cout << "done." << endl;
  } else {
    cout << "failed." << endl;
    unregisterPorts();
    return 0;
  }

  // ----------------------------
  // fourth: release MASTER and bail out
  cout << "Now releasing MASTER... ";
  cout.flush();
  _master_cmd_outport.Content() = CCMDDisconnect;
  _master_cmd_outport.Write();
  _master_rep_inport.Read();
  reply = _master_rep_inport.Content();
  if ( reply == CMD_ACK ) {
    cout << "done." << endl;
  } else {
    cout << "failed." << endl;
  }
  sleep(2);

  // launch batch file which yarp-disconnects ports
  cout << endl << "Now disconnecting ports." << endl;
  system(DISCONNECT_SCRIPT);

  // unregister ports and bail out
  cout << endl << "Unregistering ports and bailing out." << endl;
  unregisterPorts();
  return 0;
  
}
