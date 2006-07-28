/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialConsole.h
 *  Provides a text like console to access directly the Serial Handler. It
 * runs its own svc loop reading from the keyboard and puts the command into the
 * SerialHandler ACE_Message_Queue.
 * @version 1.0
 * @date 26-Jun-06 4:06:17 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: SerialConsole.h,v 1.3 2006-07-28 12:39:00 beltran Exp $
 */
#ifndef __SERIALCONSOLEH__
#define __SERIALCONSOLEH__

#include <stdio.h>
#include <string>
#include <ace/String_Base.h>
#include "DGSTask.h"
#include "SerialFeedbackData.h"
#include "KeyboardReader.h"

/** 
 * @class SerialConsole
 *  Provides console like access to the serial port.
 */
class SerialConsole:public DGSTask
{

public:
    SerialConsole ();  /* constructor */
    /** 
     * SerialConsole Alternative constructor that accepts a string to
     * determinate the end of a response. This can be so in some serial
     * devices like the Barrett Hand that issues a "=>" when a command
     * response is finish.
     * 
     * @param delimiter The string containing the delimiter. 
     */
    SerialConsole (char * delimiter); /* alternative constructor */
    ~SerialConsole(); /*Destructor */
    
    /** 
     *  svc Internal console loop. Read keyboar commands
     * 
     * @return 
     */
    virtual int svc();
    
    /** 
     *  setCommandsConsumer Set the pointer to the Serial handler
     * accepting console messages
     * 
     * @param consumer The pointer to the hander implementing a message queue
     * that can be used to input messages using the "put" function
     * 
     * @return 
     */
    int setCommandsConsumer(DGSTask * consumer){ _mykeyboardreader.setCommandsConsumer(consumer); return 0;};

    /** 
     *  IsAlive Prints a messages signaling that is alive
     * 
     * @return 
     */
    virtual int IsAlive(){ ACE_DEBUG((LM_INFO, ACE_TEXT("SerialConsole: I am Alive!\n"))); return 0;};

protected:

private:
    KeyboardReader _mykeyboardreader;
}; /*   end of class  SerialConsole   */

#endif
