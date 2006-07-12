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
 * ===================================================================================*/

#define ACE_NTRACE 0
/*
 * $Id: DGSDriver.cpp,v 1.2 2006-07-12 19:48:44 beltran Exp $
 */
#include "SerialHandler.h"
#include "SerialConsole.h"
#include "DGSDriver.h "

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
    //ACE_TRACE("main");
    //if (parse_args (argc, argv) == -1)
    //return -1;

    //ACE_INET_Addr client_addr_;
    //My_Asynch_Acceptor<SerialHandler> acceptor;
    /****   
    ACE_LOG_MSG->priority_mask ( LM_NOTICE
        , ACE_Log_Msg::PROCESS);
     ****/

    //ACE_LOG_MSG->open(argv[0],ACE_Log_Msg::SYSLOG, ACE_TEXT("syslogTest"));
    ACE_DEBUG((LM_INFO, ACE_TEXT("%IYes, I did it\n")));
	
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
