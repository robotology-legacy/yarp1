
#ifndef YARPBottleCodes_INC
#define YARPBottleCodes_INC

// Automatically generated file -- do not edit directly.
// Generated in directory /home/mb/cdp/src/YARP/conf/vocab

// labels
enum {
	YBLabelNULL,
	YBLabelMotor
};

enum {
	YBTypeChar,
	YBTypeInt,
	YBTypeDouble,
	YBTypeYVector,
	YBTypeVocab,
	YBTypeString
};

enum {
	YBVIsAlive,
	YBVArmNewCmd,
	YBVArmDone,
	YBVArmRndStart,
	YBVArmRndStop,
	YBVArmShake,
	YBVArmRest,
	YBVArmRestDone,
	YBVArmForceResting,
	YBVArmInhibitResting,
	YBVArmZeroG,
	YBVArmQuit,
	YBVHandNewCmd,
	YBVHandDone,
	YBVHandShake,
	YBVHandResetEncoders,
	YBVHandQuit,
	YBVHeadNewCmd,
	YBVHeadStop,
	YBVHeadQuit,
	YBVExit
};

template <class T>
const char *ybc_label(T x) {
  switch (x) {
    case YBVExit: return "exit";
	case YBVIsAlive: return "is alive?";
	case YBVArmNewCmd: return "arm new cmd";
	case YBVArmDone: return "arm done";
	case YBVArmRest: return "arm start resting sequence";
	case YBVArmRestDone: return "arm rest done";
	case YBVArmRndStart: return "arm rnd start";
	case YBVArmRndStop: return "arm rnd stop";
	case YBVArmShake: return "arm shake";
	case YBVArmForceResting: return "arm force resting";
	case YBVArmInhibitResting: return "arm inhibit resting";
	case YBVArmZeroG: return "arm zero g mode";
	case YBVArmQuit: return "arm quit";
	case YBVHandShake: return "hand shake";
	case YBVHandNewCmd: return "hand new cmd";
	case YBVHandDone: return "hand done";
	case YBVHandResetEncoders: return "hand reset encoders";
	case YBVHandQuit: return "hand quit";
	case YBVHeadNewCmd: return "head new cmd";
	case YBVHeadStop: return "head stop cmd";
	case YBVHeadQuit: return "head quit";
  }
  return "UNKNOWN";
};


#endif
