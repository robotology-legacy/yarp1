/*
 * functions contained here are fixes for the motorola bugs.
 * to use them you must comment out the original generated code
 * or alternatevely comment this out if you like to keep the buggy version.
 *
 */
 
/* MODULE Cpu. */
/* 
 * in case you add new beans you
 * must add the proper .h by hand here.
 *
 */
#include "AS1.h"
#include "CAN1.h"
#include "QD1.h"
#include "QD2.h"
#include "IFsh1.h"
#include "PWMC1.h"
#include "PWMC2.h"
#include "TI1.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Events.h"
#include "Cpu.h"

/*
** ===================================================================
**     Method      :  _EntryPoint (bean 56F807)
**
**     Description :
**         This method is internal. It is used by Processor Expert
**         only.
** ===================================================================
*/
extern void init_56800_(void);         /* Forward declaration of external startup function declared in file FSTART.ASM */
void _EntryPoint(void)
{
  long i;
  
  /*** ### 56F807 "Cpu" init code ... ***/
  /*** PE initialization code after reset ***/
  /* System clock initialization */
  setRegBit(PLLCR, PLLPD);             /* Disable PLL */
  setReg(PLLDB, 19);                   /* Set the clock prescalers */
  setRegBit(PLLCR, LCKON);             /* Enable lock detector */
  clrRegBit(PLLCR, PLLPD);             /* Enable PLL */
  while(!getRegBit(PLLSR, LCK0)){}     /* Wait for PLL lock */
  
  /* extra delay since the timing is wrong */
  /* this is not a real fix, it works though */
  for (i = 0; i < 10000; i++)
  	asm(nop);
  	
  setRegBitGroup(PLLCR, ZSRC, 2);      /* Select clock source from postscaler */
  /*** End of PE initialization code after reset ***/

  asm(JMP init_56800_);                /* Jump to C startup code */
}


/*
** ===================================================================
**     Method      :  QD1_SetPosition (bean QuadratureDecoder)
**
**     Description :
**         This method sets position counter.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Position        - Value stored in position
**                           counter.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte QD1_SetPosition(dword Position)
{
  setReg(DEC0_UIR, (word)(Position>>16));
  setReg(DEC0_LIR, (word)(Position));
  setRegBit(DEC0_DECCR, SWIP);
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  QD2_SetPosition (bean QuadratureDecoder)
**
**     Description :
**         This method sets position counter.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Position        - Value stored in position
**                           counter.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte QD2_SetPosition(dword Position)
{
  setReg(DEC1_UIR,(word)(Position>>16)); 	/* Set Upper Position Counter Register */
  setReg(DEC1_LIR,(word)(Position));  		/* Set Lower Position Counter Register */
  setRegBit(DEC1_DECCR, SWIP);
  return ERR_OK;                       /* OK */
}

//#if 0
/*
** ===================================================================
**     Method      :  QD1_GetPosition (bean QuadratureDecoder)
**
**     Description :
**         This method gets position counter. Position value is
**         captured toi the difference hold register and difference
**         counter is cleared.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Position        - Pointer to value stored in
**                           position counter.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte QD1_GetPosition(dword *Position)
{
  *Position = getReg(DEC0_LPOS)|(((dword)getReg(DEC0_UPOS))<<16); /* Get Lower hold and Upper Position Counter Registers */
  return ERR_OK;                       /* OK */
}

//#endif

/*
** ===================================================================
**     Method      :  QD2_GetPosition (bean QuadratureDecoder)
**
**     Description :
**         This method gets position counter. Position value is
**         captured toi the difference hold register and difference
**         counter is cleared.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Position        - Pointer to value stored in
**                           position counter.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte QD2_GetPosition(dword *Position)
{
  *Position = getReg(DEC1_LPOS)|(dword)getReg(DEC1_UPOS<<16); /* Get Lower hold and Upper Position Counter Registers */
  return ERR_OK;                       /* OK */
}
