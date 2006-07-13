/** =================================================================================
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSDriver.cpp
 * @brief Contains the driver for the Dexterous Grasping System (Three finger
 * gripper)
 * @version 1.0
 * @date 20-Jun-06 1:10:28 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab.
 * Revisions:.
 * V1.0: Tries to make an independed SerialHandler and SerialConsole and
 * communicated them through Ace message queues
 * ===================================================================================*/
#define VERSION 1.0
#define ACE_NTRACE 0
/*
 * $Id: DGSDriver.cpp,v 1.3 2006-07-13 16:45:57 beltran Exp $
 */
#include "SerialHandler.h"
#include "SerialConsole.h"

/** --------------------------------------------------------------------------
 * @brief ACE_TMAIN The main DGC loop.
 * Starts the systems and creates and initializes all other components. 
 * 
 * @param argc The number of entered parameters
 * @param argv The array containing the input parameters
 * 
 * @return 
 ----------------------------------------------------------------------------*/
int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
    //ACE_TRACE("main");
    //if (parse_args (argc, argv) == -1)
    //return -1;

       
    ACE_LOG_MSG->priority_mask ( LM_NOTICE, ACE_Log_Msg::PROCESS);
    //ACE_LOG_MSG->open(argv[0],ACE_Log_Msg::SYSLOG, ACE_TEXT("syslogTest"));

    ACE_printf(ACE_TEXT("%I***********************************************************\n"));
    ACE_printf(ACE_TEXT("%I*     Welcome to Dexterous Grasping System Module         *\n"));
    ACE_printf(ACE_TEXT("%I*                      Version %1.2f                      *\n"),VERSION);
    ACE_printf(ACE_TEXT("%I***********************************************************\n"));
    ACE_printf(ACE_TEXT("%I                                                           \n"));
	
    SerialHandler serialhandler;
    if (serialhandler.initialize () == -1)
        return -1;

    SerialConsole serialConsole;
    serialConsole.setCommandsConsumer(&serialhandler); 
    if (serialConsole.activate() == -1)
        return -1;
    int success = 1;

    // dispatch events
    success = !(ACE_Proactor::run_event_loop () == -1);

    return success ? 0 : 1;
}
