
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
	YBVIsAlive,
	YBVHandNewCmd,
	YBVHandDone,
	YBVHandShake,
	YBVExit
};

template <class T>
const char *ybc_label(T x) {
  switch (x) {
    case YBVExit: return "exit";
	case YBVArmNewCmd: return "arm new cmd";
	case YBVArmDone: return "arm done";
	case YBVArmRndStart: return "arm rnd start";
	case YBVArmRndStop: return "arm rnd stop";
	case YBVHandNewCmd: return "hand new cmd";
	case YBVHandDone: return "hand done";
	case YBVHandShake: return "shake hand";
	case YBVIsAlive: return "is alive?";
  }
  return "UNKNOWN";
};


#endif
