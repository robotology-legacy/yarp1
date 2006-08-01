/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file sender.h
 * @brief Contain the sender defintion.
 * @version 1.0
 * @date 31-Jul-06 7:55:08 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: sender.h,v 1.1 2006-08-01 09:20:13 beltran Exp $
 */

#ifndef _SENDERH_ 
#define _SENDERH_

#include "ace/OS_NS_string.h"
#include "ace/OS_main.h"
#include "ace/Service_Config.h"
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/Asynch_IO_Impl.h"
#include "ace/Asynch_Acceptor.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/CDR_Stream.h"
#include "ace/Message_Block.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_sys_stat.h"

#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"
#include "glovedata.h"

extern int done;
extern const ACE_TCHAR *file;

class Sender : public ACE_Handler
{
  // = TITLE
  //     The class will be created by <main>.  After connecting to the
  //     host, this class will then read data from a file and send it
  //     to the network connection.
public:
  Sender (void);
  ~Sender (void);
  int open (const ACE_TCHAR *host,
            u_short port);
  ACE_HANDLE handle (void) const;
  void handle (ACE_HANDLE);

protected:
  // These methods are called by the freamwork

  virtual void handle_transmit_file (const ACE_Asynch_Transmit_File::Result &result);
  // This is called when asynchronous transmit files complete
  virtual void handle_write_stream (const ACE_Asynch_Write_Stream::Result &result);
  // This is called when asynchronous writes from the socket complete
  virtual void handle_read_file (const ACE_Asynch_Read_File::Result &result);
  // This is called when asynchronous reads from the socket complete

private:
  int transmit_file (void);
  // Transmit the entire file in one fell swoop.

  int initiate_read_file (void);
  // Initiate an asynchronous file read.

  ACE_SOCK_Stream stream_;
  // Network I/O handle

  ACE_Asynch_Write_Stream ws_;
  // ws (write stream): for writing to the socket

  ACE_Asynch_Read_File rf_;
  // rf (read file): for writing from the file

  ACE_Asynch_Transmit_File tf_;
  // Transmit file.

  ACE_HANDLE input_file_;
  // File to read from

  u_long file_offset_;
  // Current file offset

  u_long file_size_;
  // File size

  ACE_Message_Block welcome_message_;
  // Welcome message

  ACE_Asynch_Transmit_File::Header_And_Trailer header_and_trailer_;
  // Header and trailer which goes with transmit_file

  int stream_write_done_;
  int transmit_file_done_;
  // These flags help to determine when to close down the event loop
};

#endif

