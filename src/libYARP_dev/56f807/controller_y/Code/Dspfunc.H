/* File: dspfunc.h */

#ifndef __DSPFUNC_H
#define __DSPFUNC_H

#ifndef SDK_LIBRARY
  #include "configdefines.h"

  #ifndef INCLUDE_DSPFUNC
    #error INCLUDE_DSPFUNC must be defined in appconfig.h to initialize the DSP Function Library
  #endif
#endif

/*
   This include file is the master include file for the
   DSP568xx Digital Signal Processing Function Library -
   Fractional Algorithms for C and Assembly Programmers.
*/

/***************************
 Foundational Include Files
****************************/
#include "port.h"
#include "arch.h"
#include "prototype.h"
#include "dfr16priv.h"   /* MIEL_PRASAD */


/***************************
 Basic Fractional Math
****************************/
//#include "mfr16.h"
//#include "mfr32.h"

/***************************
 Single Dimension Array Functions
****************************/
//#include "afr16.h"
//#include "afr32.h"

/***************************
 Vector Functions
****************************/
//#include "vfr16.h"

/***************************
 Matrix Functions
****************************/
//#include "xfr16.h"

/***************************
 Trigonometric Functions
****************************/
//#include "tfr16.h"

/* Table lookup method via integer delta */
typedef struct tfr16_sSineWaveGenIDTL
{
  Word16 PrivateData[5]; /* Private data for the IDTL sine generation function */
}tfr16_tSineWaveGenIDTL;

/* Table lookup method via real delta */
typedef struct tfr16_sSineWaveGenRDTL
{
  Word16 PrivateData[4]; /* Private data for the RDTL sine generation function */
}tfr16_tSineWaveGenRDTL;

/* Table lookup method via real delta with interpolation */
typedef struct tfr16_sSineWaveGenRDITL
{
  Word16 PrivateData[5]; /* Private data for the RDITL sine generation function */
}tfr16_tSineWaveGenRDITL;

/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sSineWaveGenRDITLQ
{
  Word16 PrivateData[5]; /* Private data for the RDITLQ sine generation function */
}tfr16_tSineWaveGenRDITLQ;

/* Digital oscillator method */
typedef struct tfr16_sSineWaveGenDOM
{
  Word16 PrivateData[3]; /* Private data for the Digital Oscillator method */
}tfr16_tSineWaveGenDOM;

/* Polynomial approximation method */
typedef struct tfr16_sSineWaveGenPAM
{
  Word16 PrivateData[4]; /* Private data for the polynomial approximation method */
}tfr16_tSineWaveGenPAM;

/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sWaveGenRDITLQ
{
  Word16 PrivateData[4]; /* Private data for the RDITLQ wave generation function */
}tfr16_tWaveGenRDITLQ;

/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sSinPIxLUT
{
  Word16 PrivateData[3]; /* Private data for the SinPIxLUT function */
}tfr16_tSinPIxLUT;

/* Table lookup method via real delta with interpolation, quarter of a sine LUT */
typedef struct tfr16_sCosPIxLUT
{
  Word16 PrivateData[3]; /* Private data for the CosPIxLUT function */
}tfr16_tCosPIxLUT;

/***************************
 Signal Processing Functions
****************************/
//#include "dfr16.h"

/*********************************************************
* Flags for Fast Fourier Transform (FFT) Functions
**********************************************************/
#ifndef FFT_DEFAULT_OPTIONS
#define FFT_DEFAULT_OPTIONS               0  /* Default all options bits to 0     */
#define FFT_SCALE_RESULTS_BY_N            1  /* Unconditionally scale by N        */
#define FFT_SCALE_RESULTS_BY_DATA_SIZE    2  /* Scale according to data sizes     */
#define FFT_INPUT_IS_BITREVERSED          4  /* Default to normal (linear) input  */
#define FFT_OUTPUT_IS_BITREVERSED         8  /* Default to normal (linear) output */
#endif

/********************************************************
* Data Sructure for Real FFT and Real Inverse FFT
*********************************************************/
typedef struct {
        Frac16 z0;
        Frac16 zNDiv2;
        CFrac16 cz[1];
        } dfr16_sInplaceCRFFT;

/*******************************************************
* 16-bit Fractional FIR Filters
*******************************************************/
typedef struct dfr16_sFirStruct {
  Frac16   * pC;                 /* Coefficients for the filter */
  Frac16   * pHistory;           /* Memory for the filter history buffer */
  UWord16    Private[6];
} dfr16_tFirStruct;

#define dfr16_tFirDecStruct dfr16_tFirStruct
#define dfr16_tFirIntStruct dfr16_tFirStruct

/*******************************************************
* 16-bit Fractional IIR Filters
*******************************************************/
#define FILT_STATES_PER_BIQ 2
#define FILT_COEF_PER_BIQ   5
//typedef struct dfr16_sIirStruct dfr16_tIirStruct;

/*******************************************************
* 16-bit Fractional Correlations
*******************************************************/
#ifndef CORR_RAW
#define CORR_RAW      0  /* Select Raw correlation     */
#define CORR_BIAS     1  /* Select Bias correlation    */
#define CORR_UNBIAS   2  /* Select Unbias correlation  */
#endif


/***************************
 Functions to Workaround CW problems
****************************/

unsigned long impyuu (unsigned short unsigA, unsigned short unsigB);
long          impysu (short          sig,    unsigned short unsig);


void dspfuncInitialize(void);

#endif
