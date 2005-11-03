#ifndef __WAVETABLE_H
#define __WAVETABLE_H

#include "fir.h"

//  Compile with oversampling or plain oscillator
#define OVERSAMPLING_OSCILLATOR   1

typedef struct _TRMWavetable {
    TRMFIRFilter *FIRFilter;
    double *wavetable;

    int tableDiv1;
    int tableDiv2;
    double tnLength;
    double tnDelta;

    double basicIncrement;
    double currentPosition;
} TRMWavetable;

TRMWavetable *TRMWavetableCreate(int waveform, double tp, double tnMin, double tnMax, double sampleRate);
void TRMWavetableFree(TRMWavetable *wavetable);

void TRMWavetableUpdate(TRMWavetable *wavetable, double amplitude);
double TRMWavetableOscillator(TRMWavetable *wavetable, double frequency);

#endif
