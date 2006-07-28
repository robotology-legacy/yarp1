/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file KeyboardReader.h
 * @brief Reads from the keyboard and send its to a commands consumer.
 * @version 1.0
 * @date 28-Jul-06 1:05:58 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: KeyboardReader.h,v 1.1 2006-07-28 12:39:00 beltran Exp $
 */

#ifndef _KEYBOARDREADERH_ 
#define _KEYBOARDREADERH_
#include <ace/Task.h>
#include "SerialFeedbackData.h"
#include "DGSTask.h"

/** 
 * @class KeyboardReader
 * @brief Reads from the keyboard, compose a Message block and sends it to a
 * commands consumer.
 */
class KeyboardReader:public ACE_Task_Base
{
public:

    KeyboardReader ();  /* constructor */
    virtual int svc();
    void setFather(DGSTask * myfather);
    void setDelimiter(char * delimiter);
    void setCommandsConsumer(DGSTask * commandsconsumer);

protected:

private:
    DGSTask * _myFather;
    ACE_String_Base<char> * _delimiter_string;   /// The delimiter string that detects the end of a serial response.
    DGSTask * commands_consumer; /// The consumer of commands.
}; 


#endif

