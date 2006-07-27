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
 * RCS-ID:$Id: SerialConsole.cpp,v 1.3 2006-07-27 10:23:27 beltran Exp $
 */
#include "SerialConsole.h"

/**
 *  SerialConsole constructor
 */
SerialConsole::SerialConsole ()
{
}  

/** 
 *  SerialConsole::~SerialConsole Destructor
 */
SerialConsole::~SerialConsole()
{
}

/** 
 *  SerialHandler::svc Reads from the keyboard
 * @return 
 */
int SerialConsole::svc()
{
    for (;;)
    {
        std::string user_input;
        int c = getchar();
        while ( c != '\n')
        {
            user_input+= c;
            c = getchar();
        }
        if (user_input.size() == 0) continue;

        if (user_input == "quit") 
            ACE_OS::printf("ok, received quit\n");
        // A return is need for the Barrett hand
        user_input = user_input + "\r";

        //Compose the Ace_Message_Block with user data
        ACE_Message_Block * message_block = 0;
        ACE_NEW_RETURN( message_block, ACE_Message_Block(user_input.size()),-1);

        if (message_block->copy(user_input.c_str(), user_input.size())) 
            ACE_ERROR ((LM_ERROR,"%p%l", ACE_TEXT ("%I%N%l Error coping user message block\n")));

        //Compose a message block putting the pointer to this serial console (to
        //be used by the serial handler to send back data)
        SerialFeedbackData * feedback_data = 0;
        ACE_NEW_RETURN( feedback_data, SerialFeedbackData(),-1);
        feedback_data->setCommandSender(this);
        feedback_data->setSerialResponseDelimiter("=>");

        ACE_Message_Block * pointer_block = 0;
        ACE_NEW_RETURN ( pointer_block, ACE_Message_Block( ACE_reinterpret_cast( char *, feedback_data)), -1);
        //glue both block message_block and pointer_block
        message_block->cont(pointer_block);
        
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Sending MessageBlock\n")));
        commands_consumer->putq(message_block);
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Message Block Send\n")));

        //wait for the answer
        ACE_Message_Block * response_block;
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Waiting Response\n")));
        getq(response_block);
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Response Received\n")));
        response_block->rd_ptr()[response_block->length()] = '\0';
        ACE_OS::printf("%s", response_block->rd_ptr());
    }
}
