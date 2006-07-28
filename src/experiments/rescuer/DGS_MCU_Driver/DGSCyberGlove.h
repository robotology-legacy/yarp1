/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *   Based on YARP MirrorCollector by Amebi (Matteo Brunettini)
 *
 * @file DGSCyberGlove.h
 * @brief 
 * @version 1.0
 * @date 25-Jul-06 2:47:13 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGSCyberGlove.h,v 1.1 2006-07-28 12:41:57 beltran Exp $
 */

#ifndef __DGSCyberGloveH__
#define __DGSCyberGloveH__

#include <yarp/YARPConfig.h>
#include "DGSTask.h"

//ccc struct CyberGloveOpenParameters
//ccc {
//ccc unsigned short comPort;
//ccc unsigned int baudRate;
//ccc };

enum DataGloveCmd
{
    DGCMDGetData        = 1,
    DGCMDStartStreaming = 2,
    DGCMDStopStreaming  = 3,
    DGCMDGetLed         = 4,
    DGCMDGetSwitch      = 5,
    DGCMDResetGlove     = 6,
    DGCMDSetLed         = 7,

    DGCMDNCmds          = 8
};

//ccc #include<yarp/begin_pack_for_net.h>

struct DataGloveData {
    DataGloveData(){ clean(); };
    void clean() {
        thumb[0]     = 0;  thumb[1]    = 0;  thumb[2]    = 0;
        index[0]     = 0;  index[1]    = 0;  index[2]    = 0;
        middle[0]    = 0; middle[1]    = 0; middle[2]    = 0;
        ring[0]      = 0;   ring[1]    = 0;   ring[2]    = 0;
        pinkie[0]    = 0; pinkie[1]    = 0; pinkie[2]    = 0;
        abduction[0] = 0; abduction[1] = 0; abduction[2] = 0; abduction[3] = 0;
        palmArch     = 0;	wristPitch   = 0; wristYaw     = 0;
    };
    int thumb[3];     // [rotation, inner, outer]
    int index[3];     // [inner, middle, outer phalanx]
    int middle[3];    // [inner, middle, outer phalanx]
    int ring[3];      // [inner, middle, outer phalanx]
    int pinkie[3];    // [inner, middle, outer phalanx]
    int abduction[4]; // [thumb-index, index-middle, middle-ring, ring-pinkie relative abductions]
    int palmArch;     // palm arch
    int wristPitch;   // wrist pitch
    int wristYaw;     // wrist yaw
};

/** 
 * @class DGSCyberGlove
 */
class DGSCyberGlove : public DGSTask
{
private:
    DGSCyberGlove(const DGSCyberGlove&);
    void operator=(const DGSCyberGlove&);
    void fillDataStructure(unsigned char *inBytes, DataGloveData *destStruct);

public:
    DGSCyberGlove();
    virtual ~DGSCyberGlove();

    // overload open, close
    int open(void *d);
    int close(void);

    int getData(void *cmd);     /// (DataGloveData* data)
    int getSwitch(void *cmd);   /// (int* switchStatus)
    int getLed(void *cmd);      /// (int* ledStatus)

    int setLed(void *cmd);      /// (int* ledStatus)

    int resetGlove(void *);     /// (void)
    int startStreaming(void *); /// (void)
    int stopStreaming(void *);  /// (void)

protected:
    unsigned short comPort;
    unsigned int baudRate;
    ///@todo add some kind of mutex to access the glove data.
    DataGloveData _glovedata;
    bool _bStreamStarted;
    bool _bError;

    ///@todo add a concurrent synchronization mechanism (mutex, semaphore..)
    virtual int svc(void);
};

#endif
