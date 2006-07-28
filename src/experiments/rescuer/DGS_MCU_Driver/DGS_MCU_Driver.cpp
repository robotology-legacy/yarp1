/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGS_MCU_Driver.cpp
 * @brief 
 * @version 1.0
 * @date 27-Jul-06 5:06:42 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGS_MCU_Driver.cpp,v 1.1 2006-07-28 12:41:57 beltran Exp $
 */

/**
 * Defines the current "general" version of the DGS_MCU_Driver
 */
#define VERSION 1.0
/**
 * Controls if the trace is activated
 */
#define ACE_NTRACE 0
/*
 * $Id: DGS_MCU_Driver.cpp,v 1.1 2006-07-28 12:41:57 beltran Exp $
 */
#include "SerialHandler.h"
#include "SerialConsole.h"
#include "DGS_MCU_Acceptor.h"
#include "DGSCyberGlove.h"

/** 
 *  ACE_TMAIN The main DGC loop.
 * Starts the systems and creates and initializes all other components. 
 * 
 * @param argc The number of entered parameters
 * @param argv The array containing the input parameters
 * 
 * @return 
 */
int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
    //ACE_TRACE("main");
    //if (parse_args (argc, argv) == -1)
    //return -1;

    //ACE_LOG_MSG->priority_mask ( LM_INFO | LM_DEBUG | LM_NOTICE, ACE_Log_Msg::PROCESS);
    //ACE_LOG_MSG->open(argv[0],ACE_Log_Msg::SYSLOG, ACE_TEXT("syslogTest"));
    ACE_LOG_MSG->open(argv[0],ACE_Log_Msg::SILENT, ACE_TEXT("syslogTest"));

    ACE_OS::printf(ACE_TEXT("***********************************************************\n"));
    ACE_OS::printf(ACE_TEXT("*     Welcome to Dexterous Grasping System Driver         *\n"));
    ACE_OS::printf(ACE_TEXT("*                 Mobile Control Unit (MCU)               *\n"));
    ACE_OS::printf(ACE_TEXT("*                      Version %1.2f                       *\n"),VERSION);
    ACE_OS::printf(ACE_TEXT("***********************************************************\n"));
    ACE_OS::printf(ACE_TEXT("                                                           \n"));

    //xxx u_short cld_port = ACE_DEFAULT_SERVICE_PORT;
    //xxx ACE_INET_Addr cld_addr_;

    //xxx if (cld_addr_.set (cld_port) == -1)
        //xxx return -1;

    SerialHandler serialhandler;
    if (serialhandler.initialize () == -1)
        return -1;

    //SerialConsole serialConsole("=>");
    SerialConsole serialConsole;
    serialConsole.setCommandsConsumer(&serialhandler); 
    if (serialConsole.activate() == -1)
        return -1;
    int success = 1;

    //xxx DGS_MCU_Acceptor acceptor_;
    //xxx acceptor_.setCommandsConsumer(&serialhandler);
    //xxx acceptor_.setConsoleConsumer(&serialConsole);

    //xxx if (acceptor_.open (cld_addr_) == -1) return -1;
    //xxx ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l main DGSAcceptor Activated\n")));

    // dispatch events
    success = !(ACE_Proactor::run_event_loop () == -1);

    return success ? 0 : 1;
}
