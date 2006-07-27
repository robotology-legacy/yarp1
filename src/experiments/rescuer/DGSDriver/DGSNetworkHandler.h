/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSNetworkHandler.h
 * @brief Declaration of DGCNetworkHandler.
 * @version 1.0
 * @date 17-Jul-06 5:41:26 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGSNetworkHandler.h,v 1.6 2006-07-27 11:06:08 beltran Exp $
 */

#ifndef _DGSNetworkHandlerh_
#define _DGSNetworkHandlerh_

#include <ace/Asynch_IO.h>
#include <ace/Task.h>
#include <ace/Message_Block.h>
#include "DGSTask.h"
#include "DGSAcceptor.h"

/** 
 * Determines the size of the reading block.
 */
#define BLOCK_SIZE 100
class DGSAcceptor;

/** 
 * @class DGSNetworkHandler Takes care of routing console commands coming from a telnet like
 * connection. 
 * @todo Change the name into a more a appropriate name.
 */
class DGSNetworkHandler : public DGSTask, public ACE_Service_Handler
{
public:
    /** 
     * DGSNetworkHandler Constructor.
     * 
     * @param *acc It is the acceptor that has created this.
     */
    DGSNetworkHandler (DGSAcceptor *acc = 0, DGSTask * com_cons = NULL,
        DGSTask * cons_cons = NULL): acceptor_ (acc), commands_consumer(com_cons),
    console_consumer(cons_cons), mblk_ (0) {}

    virtual ~DGSNetworkHandler ();

    // Called by ACE_Asynch_Acceptor when a client connects.
    /** 
     * open Called by the ACE_Asynch_Acceptor whe a client connects.
     * 
     * @param new_handle The handle of the opening stream
     * @param message_block 
     */
    virtual void open (ACE_HANDLE new_handle,
        ACE_Message_Block &message_block);


protected:
    /** 
     * This header size is from the example.
     * @todo Use a similar constant to fix the DGS Header size
     */
    enum { LOG_HEADER_SIZE = 8 };    /// Length of CDR header
    DGSAcceptor *acceptor_;          /// Our creator
    ACE_Message_Block *mblk_;        /// Block to receive log record
    ACE_Asynch_Read_Stream reader_;  /// Async reading stream
    ACE_Asynch_Write_Stream writer_; /// Asynch writing stream
    DGSTask * commands_consumer;     /// The consumer of command received
    DGSTask * console_consumer;      /// An external console consumer

    /** 
     * handle_read_stream Handle input form loggin clients
     * 
     * @param result 
     */
    virtual void handle_read_stream
        (const ACE_Asynch_Read_Stream::Result &result);

    /** 
     * handle_write_stream Handle the asynchronous writing
     * 
     * @param result 
     */
    virtual void handle_write_stream
        (const ACE_Asynch_Read_Stream::Result &result);

    /** 
     * svc The svc task reading from the message queue
     * 
     * @return 
     */
    virtual int svc();
};
#endif

