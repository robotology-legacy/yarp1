/*
 * ad.h
 * 	AD sampling interface.
 */

#ifndef __adh__
#define __adh__

/**
 * \file ad.h 
 * ad.h contains the interface definition to the AD converter.
 * Current implementation is made to acquire from a single channel only.
 */

#include "dsp56f807.h"

void AD_interruptCC(void);
byte AD_measure(bool wait);
byte AD_enableIntTrigger(void);
byte AD_stopAcquisition(void);
byte AD_getValue16(word *values);
byte AD_getChannel16(byte i, word *value);
void AD_init(void);
void AD_onEnd(void);


#endif /* ifndef __adh__ */
