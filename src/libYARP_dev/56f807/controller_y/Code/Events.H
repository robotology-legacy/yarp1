/** ###################################################################
**     Filename  : Events.H
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

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

void TI1_OnInterrupt(void);
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


void CAN1_OnFullRxBuffer(void);
/*
** ===================================================================
**     Event       :  CAN1_OnFullRxBuffer (module Events)
**
**     From bean   :  CAN1 [MotorolaCAN]
**     Description :
**         This event is called when receive buffer is full after
**         successful reception of a message.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/* END Events */
#endif /* __Events_H*/

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 03.32 for 
**     the Motorola 56800 series of microcontrollers.
**
** ###################################################################
*/
