/* */

#ifndef ___DSP807_SERIAL_H
#define ___DSP807_SERIAL_H

/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* Including used modules for compilling procedure */
#include "AS1.h"
/* end */

void DSP_SendDWordAsChars (dword data);
void DSP_SendDWordAsCharsDec (dword data);
void DSP_SendWord16AsChars (Word16 data);
void DSP_SendUWord16AsChars (UWord16 data);
void DSP_SendByteAsChars (byte data);
char DSP_HexToChar (byte value);
int  DSP_atoi (char *strAddr, int strSize);
long DSP_atol (char *strAddr, int strSize);
int  DSP_strlen (char *strAddr, int strSize);
void DSP_SendData (char *dataAddress, int dataSize);
void DSP_SendDataEx (char *dataAddress);
void DSP_ReceiveData (char *dataAddress, int dataSize);
int DSP_ReceiveDataEx (char *dataAddress, int dataSize, bool echo);

#endif 