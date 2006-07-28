/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file KeyboardReader.c
 * @brief 
 * @version 1.0
 * @date 28-Jul-06 1:11:31 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: KeyboardReader.cpp,v 1.1 2006-07-28 12:39:00 beltran Exp $
 */
#include <stdio.h>
#include <string>
#include "KeyboardReader.h"

/**
 * @brief KeyboardReader
 */
KeyboardReader::KeyboardReader ()
{
    _delimiter_string = 0;
}  

/** 
 * KeyboardReader::setFather
 * 
 * @param myfather 
 */
void KeyboardReader::setFather(DGSTask * myfather)
{
    _myFather = myfather;
}

/** 
 * KeyboardReader::setDelimiter Set the delimeter of the response.
 * 
 * @param delimiter 
 */
void KeyboardReader::setDelimiter(char * delimiter)
{
    ACE_NEW_NORETURN(_delimiter_string, ACE_String_Base<char>(delimiter));
}

/** 
 * KeyboardReader::setCommandsConsumer
 * 
 * @param commandsconsumer 
 */
void KeyboardReader::setCommandsConsumer(DGSTask * commandsconsumer)
{
    commands_consumer = commandsconsumer;
}

/** 
 * KeyboardReader::svc Reads input from the keyboard.
 * 
 * @return 
 */
int KeyboardReader::svc ()
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
        feedback_data->setCommandSender(_myFather);
        if ( _delimiter_string != 0 ) 
            feedback_data->setSerialResponseDelimiter((char *)_delimiter_string->c_str());

        ACE_Message_Block * pointer_block = 0;
        ACE_NEW_RETURN ( pointer_block, ACE_Message_Block( ACE_reinterpret_cast( char *, feedback_data)), -1);
        //glue both block message_block and pointer_block
        message_block->cont(pointer_block);
        
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Sending MessageBlock\n")));
        commands_consumer->putq(message_block);
        ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialConsole::svc Message Block Send\n")));
    }
}
