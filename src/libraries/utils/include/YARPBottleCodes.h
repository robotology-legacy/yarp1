
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
	YBVArmNewCmd,
	YBVArmDone,
	YBVArmRndStart,
	YBVArmRndStop,
	YBVArmShake,
	YBVArmRest,
	YBVArmRestDone,
	YBVIsAlive,
	YBVHandNewCmd,
	YBVHandDone,
	YBVHandShake,
	YBVHandResetEncoders,
	YBVExit
};

template <class T>
const char *ybc_label(T x) {
  switch (x) {
    case YBVExit: return "exit";
	case YBVArmNewCmd: return "arm -- new cmd";
	case YBVArmDone: return "arm -- done";
	case YBVArmRest: return "arm -- start resting sequence";
	case YBVArmRestDone: return "arm -- rest done";
	case YBVArmRndStart: return "arm -- rnd start";
	case YBVArmRndStop: return "arm -- rnd stop";
	case YBVArmShake: return "arm -- shake";
	case YBVHandNewCmd: return "hand -- new cmd";
	case YBVHandDone: return "hand -- done";
	case YBVHandShake: return "hand -- shake";
	case YBVIsAlive: return "is alive?";
	case YBVHandResetEncoders: return "hand -- reset encoders";
  }
  return "UNKNOWN";
};


#endif
