/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file glovedata.h
 * @brief Contains the data of the dataglove.
 * @version 1.0
 * @date 01-Aug-06 9:46:13 AM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: glovedata.h,v 1.1 2006-08-01 09:20:13 beltran Exp $
 */

#ifndef _GLOVEDATAH_ 
#define _GLOVEDATAH_


extern const char * finalmessage;
extern const int __finalmessagelength;

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

class DataGloveData {
    DataGloveData(){ clean(); };
    void clean() {
        length = 24 + __finalmessagelength;
        thumb[0]     = 1;  thumb[1]    = 10;  thumb[2]    = 25;
        //index[0]     = 0;  index[1]    = 0;  index[2]    = 0;
        //middle[0]    = 0; middle[1]    = 0; middle[2]    = 0;
        //ring[0]      = 0;   ring[1]    = 0;   ring[2]    = 0;
        //pinkie[0]    = 0; pinkie[1]    = 0; pinkie[2]    = 0;
        //abduction[0] = 0; abduction[1] = 0; abduction[2] = 0; abduction[3] = 0;
        palmArch     = 200;	wristPitch   = 255; wristYaw     = 111;
    };
public:
    ACE_INT32 length;
    ACE_INT32 thumb[3];     // [rotation, inner, outer]
    //int index[3];     // [inner, middle, outer phalanx]
    //int middle[3];    // [inner, middle, outer phalanx]
    //int ring[3];      // [inner, middle, outer phalanx]
    //int pinkie[3];    // [inner, middle, outer phalanx]
    //int abduction[4]; // [thumb-index, index-middle, middle-ring, ring-pinkie relative abductions]
    ACE_INT32 palmArch;     /// palm arch
    ACE_INT32 wristPitch;   /// wrist pitch
    ACE_INT32 wristYaw;     /// wrist yaw
};
#endif

