/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file UDPGenerator.cpp
 * @brief Sends received UDP packets.
 * @version 1.0
 * @date 02-Aug-06 5:59:01 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: UDPGenerator.cpp,v 1.1 2006-08-03 16:46:08 beltran Exp $
 */

#include "ace/OS_NS_string.h"
#include "ace/OS_main.h"
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"
#include "ace/Message_Block.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "UDPGenerator.h"

// Keep track of when we're done.
static int done = 0;

UDPGenerator::UDPGenerator (void)
  : completion_key_ ("UDPGenerator Completion Key"),
    act_ ("UDPGenerator ACT")
{
}

UDPGenerator::~UDPGenerator (void)
{
  sockDgram_.close ();
}

/** 
 * UDPGenerator::open
 * 
 * @param localport 
 * @param remoteport 
 * 
 * @return 
 */
int
UDPGenerator::open(const ACE_TCHAR *remotehost,
    u_short localport,
    u_short remoteport)
{
  ACE_DEBUG ((LM_DEBUG, "%N:%l:UDPGenerator::open_addr called\n"));

  // Create a local UDP socket to receive datagrams.
  
  ACE_INET_Addr localAddr(localport);
  if (this->sockDgram_.open (localAddr) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "ACE_SOCK_Dgram::open"), -1);

  // Initialize the asynchronous read.
  if (this->rd_.open (
          *this,
          this->sockDgram_.get_handle (),
          this->completion_key_,
          ACE_Proactor::instance ()) == -1
      )
      ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "ACE_Asynch_Read_Dgram::open"), -1);

  // Initialize the asynchronous read.
  if (this->wd_.open (
          *this,
          this->sockDgram_.get_handle (),
          this->completion_key_,
          ACE_Proactor::instance ()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "ACE_Asynch_Write_Dgram::open"), -1);

  int res = 0;
  res = readdatagram();
  res = writedatagram(remotehost, remoteport);

  return res;
}

int UDPGenerator::readdatagram()
{
  // Create a buffer to read into.  We are using scatter/gather to
  // read the message header and message body into 2 buffers

  // create a message block to read the message header
  ACE_Message_Block* msg = 0;
  ACE_NEW_RETURN (msg, ACE_Message_Block (1024), -1);

  // the next line sets the size of the header, even though we
  // allocated a the message block of 1k, by setting the size to 20
  // bytes then the first 20 bytes of the reveived datagram will be
  // put into this message block.
  msg->size (20); // size of header to read is 20 bytes

  // create a message block to read the message body
  ACE_Message_Block* body = 0;
  ACE_NEW_RETURN (body, ACE_Message_Block (1024), -1);
  // The message body will not exceed 1024 bytes, at least not in this test.

  // set body as the cont of msg.  This associates the 2 message
  // blocks so that a read will fill the first block (which is the
  // header) up to size(), and use the cont() block for the rest of
  // the data.  You can chain up to IOV_MAX message block using this
  // method.
  msg->cont (body);

  // ok lets do the asynch read
  size_t number_of_bytes_recvd = 0;

  int res = rd_.recv (
      msg,
      number_of_bytes_recvd,
      0,
      PF_INET,
      this->act_);
  
  switch (res)
  {
      case 0:
          // this is a good error.  The proactor will call our handler when the
          // read has completed.
          break;
      case 1:
          // actually read something, we will handle it in the handler callback
          ACE_DEBUG ((LM_DEBUG, "********************\n"));
          ACE_DEBUG ((LM_DEBUG,
                  "%s = %d\n",
                  "bytes recieved immediately",
                  number_of_bytes_recvd));
          ACE_DEBUG ((LM_DEBUG, "********************\n"));
          res = 0;
          break;
      case -1:
          // Something else went wrong.
          ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_Asynch_Read_Dgram::recv"));
          // the handler will not get called in this case so lets clean up our msg
          msg->release ();
          break;
      default:
          // Something undocumented really went wrong.
          ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_Asynch_Read_Dgram::recv"));
          msg->release ();
          break;
  }

  return res;
}

int UDPGenerator::writedatagram(const ACE_TCHAR * remotehost,
    u_short remoteport)
{
  // We are using scatter/gather to send the message header and
  // message body using 2 buffers

  // create a message block for the message header
  ACE_Message_Block* msg = 0;
  ACE_NEW_RETURN(msg, ACE_Message_Block(100), -1);
  const char rawMsg [] = "To be or not to be.";
  // Copy buf into the Message_Block and update the wr_ptr ().
  msg->copy(rawMsg, ACE_OS::strlen(rawMsg) + 1);

  // create a message block for the message body
  ACE_Message_Block* body = 0;
  ACE_NEW_RETURN(body, ACE_Message_Block(100), -1);
  ACE_OS::memset(body->wr_ptr(), 'X', 100);
  body->wr_ptr(100); // always remember to update the wr_ptr()

  // set body as the cont of msg.  This associates the 2 message blocks so
  // that a send will send the first block (which is the header) up to
  // length(), and use the cont() to get the next block to send.  You can
  // chain up to IOV_MAX message block using this method.
  msg->cont(body);

  // do the asynch send
  size_t number_of_bytes_sent = 0;
  ACE_INET_Addr serverAddr(remoteport, remotehost);
  int res = this->wd_.send(
      msg, number_of_bytes_sent, 
      0, 
      serverAddr, 
      this->act_);

  switch (res)
    {
    case 0:
      // this is a good error.  The proactor will call our handler when the
      // send has completed.
      break;
    case 1:
      // actually sent something, we will handle it in the handler callback
      ACE_DEBUG ((LM_DEBUG, "********************\n"));
      ACE_DEBUG ((LM_DEBUG,
                  "%s = %d\n",
                  "bytes sent immediately",
                  number_of_bytes_sent));
      ACE_DEBUG ((LM_DEBUG, "********************\n"));
      res = 0;
      break;
    case -1:
      // Something else went wrong.
      ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_Asynch_Write_Dgram::recv"));
      // the handler will not get called in this case so lets clean up our msg
      msg->release ();
      break;
    default:
      // Something undocumented really went wrong.
      ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_Asynch_Write_Dgram::recv"));
      msg->release ();
      break;
    }
  return res;
}

void
UDPGenerator::handle_read_dgram (const ACE_Asynch_Read_Dgram::Result &result)
{
  ACE_DEBUG ((LM_DEBUG, "handle_read_dgram called\n"));

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_read", result.bytes_to_read ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_INET_Addr peerAddr;
  result.remote_address (peerAddr);
  ACE_DEBUG ((LM_DEBUG, "%s = %s:%d\n", "peer_address", peerAddr.get_host_addr (), peerAddr.get_port_number ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "flags", result.flags ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "act", result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "completion_key", result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));

  if (result.success () && result.bytes_transferred () != 0)
    {
      // loop through our message block and print out the contents
      for (const ACE_Message_Block* msg = result.message_block(); msg != 0; msg = msg->cont ())
        { // use msg->length() to get the number of bytes written to the message
          // block.
          ACE_DEBUG ((LM_DEBUG, "Buf=[size=<%d>", msg->length ()));
          for (u_long i = 0; i < msg->length(); ++i)
            ACE_DEBUG ((LM_DEBUG,
                        "%c", (msg->rd_ptr())[i]));
          ACE_DEBUG ((LM_DEBUG, "]\n"));
        }
    }

  ACE_DEBUG ((LM_DEBUG, "Receive completed\n"));

  // No need for this message block anymore.
  result.message_block ()->release ();
  readdatagram();

  // Note that we are done with the test.
  done++;
}

void
UDPGenerator::handle_write_dgram (const ACE_Asynch_Write_Dgram::Result &result)
{
  ACE_DEBUG ((LM_DEBUG,
              "handle_write_dgram called\n"));

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "flags", result.flags ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "act", result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "completion_key", result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));


  ACE_DEBUG ((LM_DEBUG, "Sender completed\n"));

  // No need for this message block anymore.
  result.message_block ()->release ();

  // Note that we are done with the test.
  done++;
}

