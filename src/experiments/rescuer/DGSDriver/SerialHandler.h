/** =================================================================================
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialHandler.h
 * @brief Contains the serial handler using the Proactor framework
 * @version 1.0
 * @date 21-Jun-06 1:06:18 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * ===================================================================================*/

/*
 * $Id: SerialHandler.h,v 1.1 2006-06-21 17:32:24 beltran Exp $
 */

#ifndef __SERIALHANDLERH__
#define __SERIALHANDLERH__

#include <stdio.h>
#include <string>
#include "ace/streams.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/Asynch_IO_Impl.h"
#include "ace/Message_Block.h"
#include "ace/OS_main.h"
#include "ace/TTY_IO.h"
#include "ace/Task.h"
#include "ace/CDR_Stream.h"
#include "ace/DEV_Connector.h"

/** --------------------------------------------------------------------------
 * @class SerialHandler
 * @brief Implements the Serial Adaptor
 ----------------------------------------------------------------------------*/
class SerialHandler : public ACE_Task<ACE_NULL_SYNCH>, public ACE_Service_Handler
{

public:
    // Constructor and destructor
    SerialHandler (void);
    ~SerialHandler (void);

    /** --------------------------------------------------------------------------
     * @brief initialize starts the inizialization of the serial device and attach it
     * to the file reading streams.
     * 
     * @return 
     ----------------------------------------------------------------------------*/
    int initialize(void);

    /** --------------------------------------------------------------------------
     * @brief svc This is the internal loop of the ACETask, it read from the
     * standart input. 
     * 
     * @return 
     ----------------------------------------------------------------------------*/
    virtual int svc();

protected:
    // = These methods are called by the framework.

    /** --------------------------------------------------------------------------
     * @brief handle_read_file
     * 
     * @param result 
     ----------------------------------------------------------------------------*/
    virtual void handle_read_file (const ACE_Asynch_Read_File::Result &result);
    // This is called when asynchronous reads from the socket complete.

    /** --------------------------------------------------------------------------
     * @brief handle_write_file
     * 
     * @param result 
     ----------------------------------------------------------------------------*/
    virtual void handle_write_file (const ACE_Asynch_Write_File::Result &result);
    // This is called when asynchronous writes from the socket complete.

private:
    //int initiate_read_stream(void);
    // Initiate an asynchronous <read> operation on the socket

    /** --------------------------------------------------------------------------
     * @brief initiate_read_file
     * 
     * @return 
     ----------------------------------------------------------------------------*/
    int initiate_read_file (void);


    /**
     *  Parameters.
     */

    /** --------------------------------------------------------------------------
     * @brief 
     ----------------------------------------------------------------------------*/
    ACE_Asynch_Read_File rf_;
    // rf (read file): for writing from the file.

    /** --------------------------------------------------------------------------
     * @brief ws (write File): for writing to the file.
     ----------------------------------------------------------------------------*/
    ACE_Asynch_Write_File wf_;

    /** --------------------------------------------------------------------------
     * @brief 
     ----------------------------------------------------------------------------*/
    ACE_TTY_IO read_dev;

    ACE_Asynch_Read_Stream reader_;
    ACE_Asynch_Write_Stream writer_;

    /** --------------------------------------------------------------------------
     * @brief 
     ----------------------------------------------------------------------------*/
    ACE_DEV_Connector con;
    int flag;
    int counter;
};

#endif /* __SERIALHANDLER__ */ 
