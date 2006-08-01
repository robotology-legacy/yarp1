/*
** test_proactor.h,v 1.1 2004/05/06 16:50:31 shuston Exp
*/

#ifndef _TEST_PROACTOR_H
#define _TEST_PROACTOR_H

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
#include "ace/Message_Block.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_sys_stat.h"

#include "ace/OS_NS_unistd.h"
#include "ace/OS_NS_fcntl.h"

extern int done;
extern const int HEADER_SIZE;
extern const ACE_TCHAR *dump_file;

class Receiver : public ACE_Service_Handler
{
  // = TITLE
  //     The class will be created by <ACE_Asynch_Acceptor> when new
  //     connections arrive.  This class will then receive data from
  //     the network connection and dump it to a file.
public:
  // = Initialization and termination.
  Receiver (void);
  ~Receiver (void);

  virtual void open (ACE_HANDLE handle,
                     ACE_Message_Block &message_block);
  // This is called after the new connection has been accepted.

protected:
  // These methods are called by the framework

  virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);
  // This is called when asynchronous <read> operation from the socket
  // complete.

  virtual void handle_write_file (const ACE_Asynch_Write_File::Result &result);
  // This is called when an asynchronous <write> to the file
  // completes.

private:
  int initiate_read_stream (void);
  // Initiate an asynchronous <read> operation on the socket.

  ACE_Asynch_Read_Stream rs_;
  // rs (read stream): for reading from a socket.

  ACE_HANDLE dump_file_;
  // File for dumping data.

  ACE_Asynch_Write_File wf_;
  // wf (write file): for writing to a file.

  u_long file_offset_;
  // Offset for the file.

  ACE_HANDLE handle_;
  // Handle for IO to remote peer.
};

#endif /* _TEST_PROACTOR_H */
