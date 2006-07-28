/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialConsole.cpp
 *  Provides a text like console to access directly the Serial Handler. It
 * runs its own svc loop reading from the keyboard and puts the command into the
 * SerialHandler ACE_Message_Queue
 * @version 1.0
 * @date 26-Jun-06 4:03:17 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * */

/*
 * RCS-ID:$Id: SerialConsole.cpp,v 1.2 2006-07-28 12:39:00 beltran Exp $
 */
#include "SerialConsole.h"

/**
 *  SerialConsole constructor
 */
SerialConsole::SerialConsole ()
{
    _mykeyboardreader.setFather(this);
}  

SerialConsole::SerialConsole (char * delimiter)
{
    _mykeyboardreader.setDelimiter(delimiter);
}

/** 
 *  SerialConsole::~SerialConsole Destructor
 */
SerialConsole::~SerialConsole()
{
}

/** 
 *  SerialHandler::svc This is a loop reading for input from the internal
 *  queue.
 * @return 
 */
int SerialConsole::svc()
{
    // Activate the task (thread) that reads from the keyboard.
    _mykeyboardreader.activate();
    for (;;)
    {
        //wait for the answer
        ACE_Message_Block * response_block;
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Waiting Response\n")));
        getq(response_block);
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Response Received\n")));
        response_block->rd_ptr()[response_block->length()] = '\0';
        ACE_OS::printf("%s", response_block->rd_ptr());
    }
    return 0;
}
