/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSDriver.cpp
 *  Contains the driver for the Dexterous Grasping System (Three finger
 * gripper)
 * @version 1.0
 * @date 20-Jun-06 1:10:28 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab.
 * Revisions:.
 * V1.0: Tries to make an independed SerialHandler and SerialConsole and
 * communicated them through Ace message queues
 * @todo Fix syncronization problems in message queues
 * @todo Fix visualization problems in the doxygen documentation
 */
/**
 * Defines the current "general" version of the DGSDRiver
 */
#define VERSION 1.0
/**
 * Controls if the trace is activated
 */
#define ACE_NTRACE 0
/*
 * $Id: DGSDriver.cpp,v 1.9 2006-07-27 11:06:08 beltran Exp $
 */
#include "SerialHandler.h"
#include "SerialConsole.h"
#include "DGSAcceptor.h"

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


    ACE_LOG_MSG->priority_mask ( LM_INFO | LM_DEBUG | LM_NOTICE, ACE_Log_Msg::PROCESS);
    //ACE_LOG_MSG->open(argv[0],ACE_Log_Msg::SYSLOG, ACE_TEXT("syslogTest"));
    //ACE_LOG_MSG->open(argv[0],ACE_Log_Msg::SILENT, ACE_TEXT("syslogTest"));

    ACE_OS::printf(ACE_TEXT("***********************************************************\n"));
    ACE_OS::printf(ACE_TEXT("*     Welcome to Dexterous Grasping System Module         *\n"));
    ACE_OS::printf(ACE_TEXT("*                      Version %1.2f                       *\n"),VERSION);
    ACE_OS::printf(ACE_TEXT("***********************************************************\n"));
    ACE_OS::printf(ACE_TEXT("                                                           \n"));


    u_short cld_port = ACE_DEFAULT_SERVICE_PORT;
    ACE_INET_Addr cld_addr_;

    if (cld_addr_.set (cld_port) == -1)
        return -1;


    SerialHandler serialhandler;
    if (serialhandler.initialize () == -1)
        return -1;


    SerialConsole serialConsole;
    serialConsole.setCommandsConsumer(&serialhandler); 
    if (serialConsole.activate() == -1)
        return -1;
    int success = 1;

    DGSAcceptor acceptor_;
    acceptor_.setCommandsConsumer(&serialhandler);
    acceptor_.setConsoleConsumer(&serialConsole);

    if (acceptor_.open (cld_addr_) == -1) return -1;
    ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l main DGSAcceptor Activated\n")));

    // dispatch events
    success = !(ACE_Proactor::run_event_loop () == -1);

    return success ? 0 : 1;
}
