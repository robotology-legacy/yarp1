/* vim:tw=78:sw=4:ts=4: */

/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialHandler.h
 *  Contains the serial handler using the Proactor framework
 * @version 1.0
 * @date 21-Jun-06 1:06:18 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * $Id: SerialHandler.h,v 1.8 2006-07-27 10:23:27 beltran Exp $
 */

#ifndef __SERIALHANDLERH__
#define __SERIALHANDLERH__

///#include <stdio.h>
///#include <string>
#include <ace/streams.h>
#include <ace/Log_Msg.h>
#include <ace/OS.h>
#include <ace/Proactor.h>
#include <ace/Asynch_IO.h>
#include <ace/Asynch_IO_Impl.h>
#include <ace/Message_Block.h>
#include <ace/OS_main.h>
#include <ace/TTY_IO.h>
#include <ace/Task.h>
#include <ace/CDR_Stream.h>
#include <ace/DEV_Connector.h>
#include "DGSDriver.h"
#include "DGSTask.h"
#include "SerialFeedbackData.h"

/** 
 * @class SerialHandler
 *  Implements the Serial Adaptor
 */
class SerialHandler : public DGSTask, public ACE_Service_Handler
{

public:
    // Constructor and destructor
    SerialHandler (void);
    ~SerialHandler (void);

    /** 
     *  initialize starts the inizialization of the serial device and attach it
     * to the reading streams.
     * 
     * @return 
     */
    int initialize(void);

    /** 
     *  svc This is the internal loop of the ACETask, it read from the
     * standart input. 
     * 
     * @return 
     */
    virtual int svc();

protected:
    // = These methods are called by the framework.

    /** 
     *  handle_read_stream
     * 
     * @param result 
     */
    virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);
    // This is called when asynchronous reads from the socket complete.

    /** 
     *  handle_write_stream
     * 
     * @param result 
     */
    virtual void handle_write_stream (const ACE_Asynch_Write_Stream::Result &result);
    // This is called when asynchronous writes from the socket complete.

private:
    //int initiate_read_stream(void);
    // Initiate an asynchronous <read> operation on the socket

    /** 
     *  initiate_read_stream 
     * 
     * @return 
     */
    int initiate_read_stream (ACE_Message_Block *);

    /**
     *  Parameters.
     */

    /** 
     *  The stream for reading from the serial line
     */
    ACE_Asynch_Read_Stream _serial_read_stream; 

    /** 
     *  The stream for writing to the serial line
     */
    ACE_Asynch_Write_Stream _serial_write_stream;

    /** 
     *  The TTY device for the serial line
     */
    ACE_TTY_IO _serial_dev; 
    /** 
     *  The connector used to connect to the serial line
     */
    ACE_DEV_Connector _serialConnector;
    int flag;
};

#endif /* __SERIALHANDLER__ */ 
