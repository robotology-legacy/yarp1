/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSNetworkHandler.cpp
 * @brief Implementation of the DGSNetworkHandler
 * @version 1.0
 * @date 17-Jul-06 6:39:14 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGSNetworkHandler.cpp,v 1.1 2006-07-27 16:07:02 beltran Exp $
 */
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_sys_socket.h>
#include <ace/SOCK_Stream.h>

#include "DGSNetworkHandler.h"
#include "SerialFeedbackData.h"

/** 
 * DGSNetworkHandler::~DGSNetworkHandler Destructor.
 */
DGSNetworkHandler::~DGSNetworkHandler () {
  reader_.cancel ();
  ACE_OS::closesocket (handle ());
  if (mblk_ != 0) mblk_->release ();
  mblk_ = 0;
  acceptor_->remove (this);
}

/** 
 * DGSNetworkHandler::open
 * 
 * @param new_handle 
 * @param  
 */
void DGSNetworkHandler::open
  (ACE_HANDLE new_handle, ACE_Message_Block &)
{

  // Link the handle with the reader stream
  reader_.open (*this, new_handle, 0, proactor ());
  // Link the handle with the writer stream
  writer_.open (*this, new_handle, 0, proactor ());

  this->activate(); ///Activate the svc message queue reading.
  
  ACE_NEW_NORETURN (mblk_, ACE_Message_Block (BLOCK_SIZE));
  // Align the Message Block for a CDR stream
  //ACE_CDR::mb_align (mblk_);
  reader_.read (*mblk_, LOG_HEADER_SIZE);
}

/** 
 * DGSNetworkHandler::handle_read_stream
 * 
 * @param result 
 */
void DGSNetworkHandler::handle_read_stream
    (const ACE_Asynch_Read_Stream::Result &result) 
{
    if (!result.success () || result.bytes_transferred () == 0)
    { 
        ACE_DEBUG((LM_ERROR, ACE_TEXT("%N Line %l DGSNetworkHandler::handle_read_stream Problems receiving data from socket\n")));
        delete this;
    }
    else 
    {
        ////mblk_->rd_ptr()[mblk_->length()] = '\0';
        ////ACE_OS::printf("%s", mblk_->wr_ptr() - result.bytes_transferred());
        //Detect the carriage return
        if ( ACE_OS::strchr(mblk_->rd_ptr(),'\n') != NULL)
        {
            mblk_->rd_ptr()[mblk_->length()] = '\r';

            //Compose a message block putting the necesary feedback information.
            SerialFeedbackData * feedback_data = 0;
            ACE_NEW_NORETURN( feedback_data, SerialFeedbackData());
            feedback_data->setCommandSender(this);
            feedback_data->setSerialResponseDelimiter("=>");

            ACE_Message_Block * feedback_block = 0;
            ACE_NEW_NORETURN ( feedback_block, ACE_Message_Block( ACE_reinterpret_cast( char *, feedback_data)));

            //glue both block message_block and feedback_block
            mblk_->cont(feedback_block);
            commands_consumer->putq(mblk_) ;

            ///@todo to be sure that the consumer is erasing the message blocks
            //Create a new message block
            ACE_NEW_NORETURN (mblk_, ACE_Message_Block (BLOCK_SIZE));
        }
        
        reader_.read (*mblk_, BLOCK_SIZE);
    }
}

/** 
 * DGSNetworkHandler::handle_write_stream
 * 
 * @param result 
 */
void DGSNetworkHandler::handle_write_stream
    (const ACE_Asynch_Read_Stream::Result &result)
{
    ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l DGSNetworkHandler::handle_write_stream I have sent back data\n")));
    result.message_block().release();
}

/** 
 * DGSNetworkHandler::svc Reads from the message queue and sends it back to the
 * network client.
 * 
 * @return 
 */
int DGSNetworkHandler::svc()
{
    ACE_TRACE("DGSNetworkHandler::svc");
    ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l DGSNetworkHandler::svc Starting SVC\n"))); 

    for (;;)
    {
        ACE_Message_Block * response_block;
        getq(response_block);
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%N Line %l DGSNetworkHandler::svc Getting a message\n"))); 
        writer_.write( *response_block, response_block->length());
        ACE_DEBUG((LM_DEBUG, ACE_TEXT("%N Line %l DGSNetworkHandler::svc Sending a message back\n"))); 
        //response_block->release();
    }
    ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l DGSNetworkHandler::svc Finising SVC\n")));
    return 0;
}

