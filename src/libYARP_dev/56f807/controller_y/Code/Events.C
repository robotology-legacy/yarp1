/** ###################################################################
**     Filename  : Events.C
**     Project   : controller_y
**     Processor : 56F807
**     Beantype  : Events
**     Version   : Driver 01.00
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 12/14/2004, 2:50 PM
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         TI1_OnInterrupt - void TI1_OnInterrupt(void);
**
**     (c) Copyright UNIS, spol. s r.o. 1997-2002
**     UNIS, spol. s r.o.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/
/* MODULE Events */

/*Including used modules for compilling procedure*/
#include "Cpu.h"
#include "Events.h"
#include "AS1.h"
#include "CAN1.h"
#include "QD1.h"
#include "QD2.h"
#include "IFsh1.h"
#include "PWMC1.h"
#include "PWMC2.h"
#include "TI1.h"

/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "Controller.h"
#include "DSP807_serial.h"

/* from Controller.c */
extern bool _wait;

/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     From bean   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the bean is enabled - "Enable" and the events are
**         enabled - "EnableEvent").
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#pragma interrupt called
void TI1_OnInterrupt(void)
{
	//if (!_wait) DSP_SendData ("*", 1);
	_wait = false;
}

/* END Events */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 03.32 for 
**     the Motorola 56800 series of microcontrollers.
**
** ###################################################################
*/
