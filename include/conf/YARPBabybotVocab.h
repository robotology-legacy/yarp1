#ifndef __YARPBABYBOTMOTORVOCAB__
#define __YARPBABYBOTMOTORVOCAB__

// label
const char *const YBVMotorLabel = "Motor";

// random behavior
const char *const YBVArmRndStart = "ArmRndStart";
const char *const YBVArmRndStop = "ArmRndStop";
const char *const YBVArmRndQuit = "ArmRndQuit";

// arm behavior
const char *const YBVArmNewCmd = "ArmNewCmd";
const char *const YBVArmDone = "ArmDone";
const char *const YBVArmShake = "ArmRndShake";
const char *const YBVArmRest = "ArmRndRest";
const char *const YBVArmRestDone = "ArmRndRestDone";
const char *const YBVArmForceResting = "ArmRndForceResting";
const char *const YBVArmInhibitResting = "ArmInhibitResting";
const char *const YBVArmZeroG = "ArmZeroG";
const char *const YBVArmQuit = "ArmQuit";

// hand behavior
const char *const YBVHandNewCmd = "HandNewCmd";
const char *const YBVHandDone = "HandDone";
const char *const YBVHandShake = "HandShake";
const char *const YBVHandResetEncoders = "HandResetEncoders";
const char *const YBVHandQuit = "HandQuit";

// head behavior
const char *const YBVHeadNewCmd = "HeadNewCmd";
const char *const YBVHeadStop = "HeadStop";
const char *const YBVHeadQuit = "HeadQuit";

// grasprflx
const char *const YBVGraspRflxQuit = "GraspRflxQuit";

// handlocalization
const char *const YBVHandLocQuit = "HandLocQuit";

// general
const char *const YBVExit = "GeneralExit";
#endif