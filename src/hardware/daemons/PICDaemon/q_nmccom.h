//---------------------------------------------------------------------------
#ifndef nmccomH
#define nmccomH
//---------------------------------------------------------------------------
#include "CommonDefinitions.h"

//Function prototypes:
 ADCMOD * AdcNewMod();
 char AdcGetStat(byte addr);

//Initialization and shutdown
//extern "C" "C" WINAPI __declspec(dllexport) int NmcInit(char *portname, unsigned int baudrate);
 int  NmcInit(char *portname, unsigned int baudrate);
 void InitVars(void);
 char NmcSendCmd(byte addr, byte cmd, char *datastr, byte n, byte stataddr);
 void FixSioError(byte addr);
 void NmcShutdown(void);

//Module type independant commands (supported by all module types)
 char NmcSetGroupAddr(byte addr, byte groupaddr, char leader);
 char NmcDefineStatus(byte addr, byte statusitems);
 char NmcReadStatus(byte addr, byte statusitems);
 char NmcSynchOutput(byte groupaddr, byte leaderaddr);
 char NmcChangeBaud(byte groupaddr, unsigned int baudrate);
 char NmcSynchInput(byte groupaddr, byte leaderaddr);
 char NmcNoOp(byte addr);
 char NmcHardReset(byte addr);

//Retrieve module type independant data from a module's data structure
 byte NmcGetStat(byte addr);
 byte NmcGetStatItems(byte addr);
 byte NmcGetModType(byte addr);
 byte NmcGetModVer(byte addr);
 byte NmcGetGroupAddr(byte addr);
 char NmcGroupLeader(byte addr);




#endif