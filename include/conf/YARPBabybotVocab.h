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
const char *const YBVArmForceResting = "ArmForceResting";
const char *const YBVArmInhibitResting = "ArmInhibitResting";
const char *const YBVArmZeroG = "ArmZeroG";
const char *const YBVArmHibernate = "ArmHibernate";
const char *const YBVArmResume = "ArmResume";
const char *const YBVArmIssuedCmd = "ArmIssuedCmd";
const char *const YBVArmQuit = "ArmQuit";

// reaching behavior
const char *const YBVReachingLearn = "ReachingLearn";
const char *const YBVReachingReach = "ReachingReach";
const char *const YBVReachingQuit = "ReachingQuit";

// arm trigger behavior
const char *const YBVTriggerQuit = "TriggerQuit";

// hand behavior
const char *const YBVHandNewCmd = "HandNewCmd";
const char *const YBVHandDone = "HandDone";
const char *const YBVHandShake = "HandShake";
const char *const YBVHandResetEncoders = "HandResetEncoders";
const char *const YBVHandQuit = "HandQuit";

// attention behavior
const char *const YBVAttentionQuit = "AttentionQuit";
const char *const YBVAttentionLookHand = "AttentionLookHand";
const char *const YBVAttentionLookTarget = "AttentionLookTarget";

// head behavior
const char *const YBVHeadNewCmd = "HeadNewCmd";
const char *const YBVHeadHibernate = "HeadHibernate";
const char *const YBVHeadResume = "HeadResume";
const char *const YBVHeadQuit = "HeadQuit";

// sink behavior
const char *const YBVSinkQuit = "SinkQuit";
const char *const YBVSinkInhibitAll = "SinkInhibitAll";
const char *const YBVSinkEnableAll = "SinkEnableAll";
const char *const YBVSinkInhibitVor = "SinkInhibitVor";
const char *const YBVSinkInhibitTracker = "SinkInhibitTracker";
const char *const YBVSinkInhibitVergence = "SinkInhibitVergence";
const char *const YBVSinkDisplayStatus = "SinkDisplayStatus";

// grasprflx
const char *const YBVGraspRflxQuit = "GraspRflxQuit";
const char *const YBVGraspRflxClutch = "GraspRflxClutch";

// handlocalization
const char *const YBVHandLocQuit = "HandLocQuit";

// hand kinematics
const char *const YBVHandKinQuit = "HandKinQuit";
const char *const YBVHandKinView = "HandKinView";
const char *const YBVHandKinFreeze = "HandKinFreeze";
const char *const YBVHandKinSetFile = "HandKinSetFile";
const char *const YBVHandKinSavePosture = "HandKinSavePosture";

// general
const char *const YBVExit = "GeneralExit";
#endif