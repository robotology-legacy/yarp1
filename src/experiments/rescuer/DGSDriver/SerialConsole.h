/** =================================================================================
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialConsole.h
 * @brief Provides a text like console to access directly the Serial Handler. It
 * runs its own svc loop reading from the keyboard and puts the command into the
 * SerialHandler ACE_Message_Queue
 * @version 1.0
 * @date 26-Jun-06 4:06:17 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * ===================================================================================*/

/*
 * RCS-ID:$Id: SerialConsole.h,v 1.1 2006-07-13 16:53:08 beltran Exp $
 */
#ifndef __SERIALCONSOLEH__
#define __SERIALCONSOLEH__

#include <stdio.h>
#include <string>
#include "DGSTask.h"
#include "DGSDriver.h"

/** ===================================================================================
 * @class SerialConsole
 * @brief Provides console like access to the serial port
 =====================================================================================*/
class SerialConsole:public DGSTask
{

public:
    SerialConsole ();  /* constructor */
    ~SerialConsole(); /*Destructor */
    
    /** --------------------------------------------------------------------------
     * @brief svc Internal console loop. Read keyboar commands
     * 
     * @return 
     ----------------------------------------------------------------------------*/
    virtual int svc();
    
    /** --------------------------------------------------------------------------
     * @brief setCommandsConsumer Set the pointer to the Serial handler
     * accepting console messages
     * 
     * @param consumer The pointer to the hander implementing a message queue
     * that can be used to input messages using the "put" function
     * 
     * @return 
     ----------------------------------------------------------------------------*/
    int setCommandsConsumer(DGSTask * consumer){ commands_consumer = consumer; return 0;};

    /** --------------------------------------------------------------------------
     * @brief IsAlive Prints a messages signaling that is alive
     * 
     * @return 
     ----------------------------------------------------------------------------*/
    virtual int IsAlive(){ ACE_DEBUG((LM_INFO, ACE_TEXT("SerialConsole: I am Alive!\n"))); return 0;};

protected:

private:
    DGSTask * commands_consumer;
}; /* -----  end of class  SerialConsole  ----- */

#endif
