/* File: prototype.h */

#ifndef __PROTOTYPE_H
#define __PROTOTYPE_H

#include "port.h"
#include "arch.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************/
/* Name mappings for DSP code portability             */
/******************************************************/

/* void setnostat (void); */
#define setnosat   archSetNoSat

/* void setstat32 (void); */
#define setsat32   archSetSat32

/* void Stop (void); */
#define Stop        archStop

/* void Trap (void); */
#define Trap        archTrap

/* void Wait (void); */
#define Wait        archWait

/* void EnableInt (void); */
#define EnableInt   archEnableInt

/* void DisableInt (void); */
#define DisableInt  archDisableInt

#ifdef ITU_INTRINSICS 

#define MAX_32 (Word32)0x7fffffffL
#define MIN_32 (Word32)0x80000000L

#define MAX_16 (Word16)0x7fff
#define MIN_16 (Word16)0x8000

/******************************************************* 

   Predefined basic intrinsics. 

   Builtin support for these functions will be implemented 
   in the CodeWarrior C compiler code generator in Release 3.0.

   The intrinsic functions are defined in the compiler
   defined functions name space. They are redefined here
   according to the ETSI naming convention.

 ******************************************************/

/************************************/
/* Fractional arithmetic primitives */
/************************************/

#define add             __add
#define sub             __sub
#define abs_s           __abs
#define mult            __mult
#define mult_r          __mult_r
#define negate          __negate
#define extract_h       __extract_h
#define round           __round
#define extract_l       __extract_l
#define shl             __shl
#define shr             __shr
#define div_s           __div

/*****************************************/
/* Long Fractional arithmetic primitives */
/*****************************************/

#define L_add           _L_add
#define L_sub           _L_sub
#define L_negate        _L_negate
#define L_abs           __labs
#define mac_r           __mac_r
#define msu_r           __msu_r
#define L_mult          _L_mult
#define L_mac           _L_mac
#define L_msu           _L_msu
#define L_shl           _L_shl
#define L_shr           _L_shr
#define shr_r           __shr_r
#define L_deposit_l     _L_deposit_l
#define norm_s          __norm_s
#define norm_l          __norm_l

/* defined by MetroWerks but not by ITU */
#define div_ls          __div_ls
#define L_mult_ls       _L_mult_ls

/* To resolve bug in MetroWerks CW 3.5 */
Word32 L_shr_r(Word32 L_var1, Word16 var2);
Word32 L_deposit_h(Word16 var1);

#else

/* Use ITU C code */
#include "basic_op.h"

#endif


#ifdef __cplusplus
}
#endif

#endif
