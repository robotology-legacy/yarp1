#ifndef __YARPBABYBOTVOCAB__
#define __YARPBABYBOTVOCAB__

// labels
const char *const YBVMotorLabel = "Motor";
const char *const YBVVisualLabel = "Visual";

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
const char *const YBVArmIsBusy = "ArmIsBusy";
const char *const YBVArmQuit = "ArmQuit";

// reaching behavior
const char *const YBVReachingLearn = "ReachingLearn";
const char *const YBVReachingReach = "ReachingReach";
const char *const YBVReachingPrepare = "ReachingPrepare";
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
const char *const YBVAttentionLookPrediction = "AttentionLookPrediction";
const char *const YBVAttentionLookEgoMap = "AttentionLookEgoMap";
const char *const YBVAttentionLookTarget = "AttentionLookTarget";

// head behavior
const char *const YBVHeadNewCmd = "HeadNewCmd";
const char *const YBVHeadNewAcc = "HeadNewAcc";
const char *const YBVHeadHibernate = "HeadHibernate";
const char *const YBVHeadResume = "HeadResume";
const char *const YBVHeadQuit = "HeadQuit";

// hand tracker switch behavior
const char *const YBVHTSPrediction= "HTSPrediction";
const char *const YBVHTSCurrent= "HTSCurrent";
const char *const YBVHTSAuto= "HTSAuto";
const char *const YBVHTSInhibit = "HTSInhibit";
const char *const YBVHTSEnable = "HTSEnable";

// sink behavior
const char *const YBVSinkQuit = "SinkQuit";
const char *const YBVSinkInhibitAll = "SinkInhibitAll";
const char *const YBVSinkEnableAll = "SinkEnableAll";
const char *const YBVSinkInhibitVor = "SinkInhibitVor";
const char *const YBVSinkInhibitTracker = "SinkInhibitTracker";
const char *const YBVSinkInhibitVergence = "SinkInhibitVergence";
const char *const YBVSinkInhibitSaccade = "SinkInhibitSaccade";
const char *const YBVSinkEnableVor = "SinkEnableVor";
const char *const YBVSinkEnableTracker = "SinkEnableTracker";
const char *const YBVSinkEnableVergence = "SinkEnableVergence";
const char *const YBVSinkEnableSaccade = "SinkEnableSaccade";
const char *const YBVSinkSuppress = "SinkSuppress";
const char *const YBVSinkRelease = "SinkRelease";
const char *const YBVSinkDisplayStatus = "SinkDisplayStatus";

// grasprflx
const char *const YBVGraspRflxQuit = "GraspRflxQuit";
const char *const YBVGraspRflxClutch = "GraspRflxClutch";
const char *const YBVGraspRflxForceOpen = "GraspRflxForceOpen";
const char *const YBVGraspRflxInhibit = "GraspRflxInhibit";
const char *const YBVGraspRflxRelease = "GraspRflxRelease";

// handlocalization
const char *const YBVHandLocQuit = "HandLocQuit";

// hand kinematics
const char *const YBVHandKinQuit = "HandKinQuit";
const char *const YBVHandKinView = "HandKinView";
const char *const YBVHandKinFreeze = "HandKinFreeze";
const char *const YBVHandKinSetFile = "HandKinSetFile";
const char *const YBVHandKinSavePosture = "HandKinSavePosture";

// egomap
const char *const YBVEgoMapAdd = "EgoMapAdd";
const char *const YBVEgoMapRemove = "EgoMapRemove";
const char *const YBVEgoMapSetCurrent = "EgoMapSetCurrent";

// general
const char *const YBVExit = "GeneralExit";

// remote learner
const char *const YBVRLTrainNow = "RLTrainNow";
const char *const YBVRLTrainContinue = "RLTrainContinue";
const char *const YBVRLNEpoch = "RLNEpoch";
const char *const YBVRLBatchSize = "RLBatchSize";

// Key frames messages
const char *const YBVKFStart = "KFStart";
const char *const YBVKFStop = "KFStop";
const char *const YBVKFSetRefModel = "KFSetRefModel";
const char *const YBVKFCheckAgainst = "KFCheckAgainst";
const char *const YBVKFQuit = "KFQuit";
const char *const YBVKFDump = "KFDump";

const char *const YBVKFExplorationStart = "KFExplorationStart";
const char *const YBVKFExplorationQuit = "KFExplorationQuit";

// Visual attention messages
const char *const YBVVAStart = "VAStart";
const char *const YBVVAStop = "VAStop";
const char *const YBVVAQuit = "VAQuit";
const char *const YBVVADump = "VADump";

const char *const YBVVASet = "VASet";
const char *const YBVVAUpdateIORTable = "VAUpdateIORTable";
const char *const YBVVAResetIORTable = "VAResetIORTable";

// saccade control messages
const char *const YBVSaccadeSetTarget = "SaccadeSetTarget";
const char *const YBVSaccadeNewTarget = "SaccadeNewTarget";
const char *const YBVSaccadeNewImpTarget = "SaccadeNewImpTarget";
const char *const YBVSaccadeFixated = "SaccadeFixated";

// misc
const char *const YBVCollectPoints = "CollectPoints";

#endif
