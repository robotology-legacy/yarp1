
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
	YBVExit
};

template <class T>
const char *ybc_label(T x) {
  switch (x) {
    case YBVExit: return "exit";
  }
  return "UNKNOWN";
};


#endif
