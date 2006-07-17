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
 * RCS-ID:$Id: DGSNetworkHandler.cpp,v 1.1 2006-07-17 18:17:43 beltran Exp $
 */
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_sys_socket.h>
#include <ace/SOCK_Stream.h>

#include "DGSNetworkHandler.h"

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
  (ACE_HANDLE new_handle, ACE_Message_Block &) {
  reader_.open (*this, new_handle, 0, proactor ());
  
  ACE_NEW_NORETURN
    (mblk_, ACE_Message_Block (BLOCK_SIZE));
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
    (const ACE_Asynch_Read_Stream::Result &result) {
  if (!result.success () || result.bytes_transferred () == 0)
  { 
      ACE_DEBUG((LM_ERROR, ACE_TEXT("%N Line %l DGSNetworkHandler::handle_read_stream Problems receiving data from socket\n")));
    delete this;
  }
  else 
  {
    reader_.read (*mblk_, BLOCK_SIZE);
    ACE_OS::printf("%s", mblk_->rd_ptr());
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
}

