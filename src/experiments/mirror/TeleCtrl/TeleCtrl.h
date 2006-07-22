///
/// $Id: TeleCtrl.h,v 1.3 2006-07-22 15:37:05 babybot Exp $
///

#ifndef TeleCtrlH
#define TeleCtrlH

// -----------------------------------
// headers
// -----------------------------------

// C++ standards
#include <iostream>
// YARP standards
#include <yarp/YARPRobotHardware.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPImages.h>
// commands exchanged with mirrorCollector
#include <mirror/mirrorCollector/CollectorCommands.h>
// need to know what a babybottle is
#include <babybot/utils/include/yarp/YARPBabyBottle.h>

using namespace std;

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))
#define DegRad (2*M_PI/360.0)
#define InchCm 2.45

#include "ControlThread.h"
#include "Calibration.h"

// -----------------------------------
// function prototypes - these are seen by all threads, too.
// -----------------------------------

void SendCommandToCollector(const CollectorCommand);
void ReadCollectorData(void);

// -----------------------------------
// globals
// -----------------------------------

#ifdef MainRoutine

// ----------------- ports
// mirrorCollector (numerical data, commands)
YARPInputPortOf<CollectorNumericalData> _coll_data_in(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
YARPInputPortOf<int>                    _coll_cmd_in(YARPInputPort::NO_BUFFERS, YARP_TCP);
YARPOutputPortOf<CollectorCommand>      _coll_cmd_out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// Repeater (position commands)
YARPOutputPortOf<YARPBabyBottle>        _rep_out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// grabber (image grabbed by babybot)
YARPInputPortOf<YARPGenericImage>       _img_in(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
// camview (image shown to the user)
YARPOutputPortOf<YARPGenericImage>      _img_out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// headSaccades (coordinates fixated by the user)
YARPOutputPortOf<YARPBottle>            _hs_out(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
// arm position
YARPInputPortOf<YVector> _arm_pos_in(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
// -----------------

// traffic to repeater control port
YARPSemaphore _repeaterSema;
// traffic to/from read image
YARPSemaphore _imageSema;

// numerical data from the collector
CollectorNumericalData _data;
// image from the grabber
YARPGenericImage _image;
// positions from the arm
YVector _armPos;

#else

extern YARPInputPortOf<CollectorNumericalData> _coll_data_in;
extern YARPInputPortOf<int>                    _coll_cmd_in;
extern YARPOutputPortOf<CollectorCommand>      _coll_cmd_out;
extern YARPOutputPortOf<YARPBabyBottle>        _rep_out;
extern YARPInputPortOf<YARPGenericImage>       _img_in;
extern YARPOutputPortOf<YARPGenericImage>      _img_out;
extern YARPOutputPortOf<YARPBottle>            _hs_out;
extern YARPInputPortOf<YVector>                _arm_pos_in;
extern YARPSemaphore _repeaterSema, _controlSema;
extern CollectorNumericalData _data;
extern YARPGenericImage _image;
extern YVector _armPos;

#endif

#endif
