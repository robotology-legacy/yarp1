#ifndef __CONFIGDEFINES_H
#define __CONFIGDEFINES_H

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
*
* Include user selected SDK components
*
****************************************************************************/
#include "appconfig.h"

/****************************************************************************
*
* Resolve include dependencies automatically
*
****************************************************************************/

#ifdef INCLUDE_BSP
  #ifndef INCLUDE_PLL
    #define INCLUDE_PLL
  #endif
  #ifndef INCLUDE_SIM
    #define INCLUDE_SIM
  #endif
  #ifndef INCLUDE_COP
    #define INCLUDE_COP
  #endif
  #ifndef INCLUDE_CORE
    #define INCLUDE_CORE
  #endif
  #ifndef INCLUDE_ITCN
    #define INCLUDE_ITCN
  #endif
#endif

/* Default all INCLUDE_IO_xxx to INCLUDE_xxx */
#ifdef INCLUDE_IO_ADC
  #ifndef INCLUDE_ADC
    #define INCLUDE_ADC
  #endif
#endif
  
#ifdef INCLUDE_IO_BUTTON
  #ifndef INCLUDE_BUTTON
    #define INCLUDE_BUTTON
  #endif
#endif

#ifdef INCLUDE_IO_CAN
  #ifndef INCLUDE_CAN
    #define INCLUDE_CAN
  #endif
#endif

#ifdef INCLUDE_IO_DAC
  #ifndef INCLUDE_DAC
    #define INCLUDE_DAC
  #endif
#endif

#ifdef INCLUDE_IO_DECODER
  #ifndef INCLUDE_DECODER
    #define INCLUDE_DECODER
  #endif
#endif

#ifdef INCLUDE_IO_FILEIO
  #ifndef INCLUDE_FILEIO
    #define INCLUDE_FILEIO
  #endif
#endif

#ifdef INCLUDE_IO_FLASH
  #ifndef INCLUDE_FLASH
    #define INCLUDE_FLASH
  #endif
#endif

#ifdef INCLUDE_IO_GPIO
  #ifndef INCLUDE_GPIO
    #define INCLUDE_GPIO
  #endif
#endif
  
#ifdef INCLUDE_IO_LED
  #ifndef INCLUDE_LED
    #define INCLUDE_LED
  #endif
#endif
  
#ifdef INCLUDE_IO_QUAD_TIMER
  #ifndef INCLUDE_QUAD_TIMER
    #define INCLUDE_QUAD_TIMER
  #endif
#endif
  
#ifdef INCLUDE_IO_PWM
  #ifndef INCLUDE_PWM
    #define INCLUDE_PWM
  #endif
#endif
  
#ifdef INCLUDE_IO_SCI
  #ifndef INCLUDE_SCI
    #define INCLUDE_SCI
  #endif
#endif

#ifdef INCLUDE_IO_SPI
  #ifndef INCLUDE_SPI
    #define INCLUDE_SPI
  #endif
#endif

#ifdef INCLUDE_IO_SWITCH
  #ifndef INCLUDE_SWITCH
    #define INCLUDE_SWITCH
  #endif
#endif



#ifdef INCLUDE_PCMASTER
  #ifndef INCLUDE_SCI
    #define INCLUDE_SCI
  #endif
#endif

#ifdef INCLUDE_LED
  #ifndef INCLUDE_GPIO
    #define INCLUDE_GPIO
  #endif
#endif

#ifdef INCLUDE_SWITCH
  #ifndef INCLUDE_GPIO
    #define INCLUDE_GPIO
  #endif
#endif

#ifdef INCLUDE_BUTTON
  #ifndef INCLUDE_TIMER
    #define INCLUDE_TIMER
  #endif
#endif

#ifdef INCLUDE_BRAKE
  #ifndef INCLUDE_GPIO
    #define INCLUDE_GPIO
  #endif
#endif

#ifdef INCLUDE_FILEIO
  #ifndef INCLUDE_SCI
    #define INCLUDE_SCI
  #endif
  
  #ifndef INCLUDE_MEMORY
    #define INCLUDE_MEMORY
  #endif
#endif

#ifdef INCLUDE_DAC
  #ifndef INCLUDE_SPI
    #define INCLUDE_SPI
  #endif
#endif

#ifdef INCLUDE_TIMER
  #ifndef INCLUDE_QUAD_TIMER
    #define INCLUDE_QUAD_TIMER
  #endif
#endif

#ifdef INCLUDE_IO

  #ifdef INCLUDE_ADC  
    #ifndef INCLUDE_IO_ADC
      #define INCLUDE_IO_ADC
    #endif
  #endif
  
  #ifdef INCLUDE_BUTTON
    #ifndef INCLUDE_IO_BUTTON
      #define INCLUDE_IO_BUTTON
    #endif
  #endif

  #ifdef INCLUDE_CAN  
    #ifndef INCLUDE_IO_CAN
      #define INCLUDE_IO_CAN
    #endif
  #endif
  
  #ifdef INCLUDE_DAC
    #ifndef INCLUDE_IO_DAC
      #define INCLUDE_IO_DAC
    #endif
  #endif
  
  #ifdef INCLUDE_DECODER
    #ifndef INCLUDE_IO_DECODER
      #define INCLUDE_IO_DECODER
    #endif
  #endif
  
  #ifdef INCLUDE_FILEIO
    #ifndef INCLUDE_IO_FILEIO
      #define INCLUDE_IO_FILEIO
    #endif
  #endif

  #ifdef INCLUDE_FLASH
    #ifndef INCLUDE_IO_FLASH
      #define INCLUDE_IO_FLASH
    #endif
  #endif

  #ifdef INCLUDE_GPIO
    #ifndef INCLUDE_IO_GPIO
      #define INCLUDE_IO_GPIO
    #endif
  #endif
  
  #ifdef INCLUDE_LED
    #ifndef INCLUDE_IO_LED
      #define INCLUDE_IO_LED
    #endif
  #endif
  
  #ifdef INCLUDE_PWM
    #ifndef INCLUDE_IO_PWM
      #define INCLUDE_IO_PWM
    #endif
  #endif

  #ifdef INCLUDE_QUAD_TIMER
    #ifndef INCLUDE_IO_QUAD_TIMER
      #define INCLUDE_IO_QUAD_TIMER
    #endif
  #endif
  
  #ifdef INCLUDE_SCI
    #ifndef INCLUDE_IO_SCI
      #define INCLUDE_IO_SCI
    #endif
  #endif

  #ifdef INCLUDE_SPI
    #ifndef INCLUDE_IO_SPI
      #define INCLUDE_IO_SPI
    #endif
  #endif

  #ifdef INCLUDE_SWITCH
    #ifndef INCLUDE_IO_SWITCH
      #define INCLUDE_IO_SWITCH
    #endif
  #endif
#endif

#if defined(INCLUDE_IO)               \
  || defined(INCLUDE_IO_ADC)        \
  || defined(INCLUDE_IO_BUTTON)     \
  || defined(INCLUDE_IO_CAN)        \
  || defined(INCLUDE_IO_DAC)        \
  || defined(INCLUDE_IO_DECODER)    \
  || defined(INCLUDE_IO_FILEIO)     \
  || defined(INCLUDE_IO_FLASH)      \
  || defined(INCLUDE_IO_GPIO)       \
  || defined(INCLUDE_IO_LED)        \
  || defined(INCLUDE_IO_PWM)        \
  || defined(INCLUDE_IO_QUAD_TIMER) \
  || defined(INCLUDE_IO_SCI)        \
  || defined(INCLUDE_IO_SPI)        \
  || defined(INCLUDE_IO_SWITCH)
    #ifndef INCLUDE_IO_IO
      #define INCLUDE_IO_IO
    #endif
#endif

#endif
