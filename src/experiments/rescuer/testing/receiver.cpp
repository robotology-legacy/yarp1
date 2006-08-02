/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file receiver.cpp
 * @brief 
 * @version 1.0
 * @date 31-Jul-06 7:53:17 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: receiver.cpp,v 1.3 2006-08-02 15:30:37 beltran Exp $
 */

#include <stdio.h>
#include <string>
#include <ace/CDR_Stream.h>
#include "receiver.h"
#include "glovedata.h"
const ACE_TCHAR * finalmessage = "GloveDataSend";
const int __finalmessagelength = 13;

int operator>> (ACE_InputCDR &cdr, DataGloveData &glove_data)
{
  //char * _message = (char *)malloc((__finalmessagelength+1) *sizeof(char));
  ACE_TCHAR _message[512];
  ACE_CDR::ULong msglength;

  ACE_INT32 _thumb[3];
  cdr.read_long_array(_thumb,3);
  ACE_CDR::ULong _palmArch;
  cdr >> _palmArch;
  ACE_CDR::ULong _wristPitch;
  cdr >> _wristPitch;
  ACE_CDR::ULong _wristYaw;
  cdr >> _wristYaw;
  cdr >> msglength;

  ACE_OS::memcpy(glove_data.thumb, _thumb, sizeof(ACE_INT32)*3);
  glove_data.palmArch   = _palmArch;
  glove_data.wristPitch = _wristPitch;
  glove_data.wristYaw   = _wristYaw;

  cdr.read_char_array (_message, msglength);
  _message[msglength] = '\0';

  glove_data.dump();
  ACE_OS::printf("%s **END MESSAGE*\n",_message);
  //free(_message);
  return cdr.good_bit ();
}

Receiver::Receiver (void)
  : dump_file_ (ACE_INVALID_HANDLE),
    handle_ (ACE_INVALID_HANDLE)
{
}

Receiver::~Receiver (void)
{
  ACE_OS::close (this->dump_file_);
  ACE_OS::closesocket (this->handle_);
}

void
Receiver::open (ACE_HANDLE handle,
                ACE_Message_Block &message_block)
{
  ACE_DEBUG ((LM_DEBUG,
              "%N:%l:Receiver::open called\n"));

  // New connection, so initiate stuff.

  // Cache the new connection
  this->handle_ = handle;

  // File offset starts at zero
  this->file_offset_ = 0;

  // Open dump file (in OVERLAPPED mode)
  this->dump_file_ = ACE_OS::open ("output",
                                   O_CREAT | O_RDWR | O_TRUNC | FILE_FLAG_OVERLAPPED);
  if (this->dump_file_ == ACE_INVALID_HANDLE)
    {
      ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_OS::open"));
      return;
    }

  // Initiate <ACE_Asynch_Write_File>.
  if (this->wf_.open (*this,
                      this->dump_file_) == -1)
    {
      ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_Asynch_Write_File::open"));
      return;
    }

  // Initiate <ACE_Asynch_Read_Stream>.
  if (this->rs_.open (*this, this->handle_) == -1)
    {
      ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_Asynch_Read_Stream::open"));
      return;
    }

  // Fake the result and make the <handle_read_stream> get
  // called. But, not, if there is '0' is transferred.
  ////CCC if (message_block.length () != 0)
  ////CCC   {
  ////CCC     // Duplicate the message block so that we can keep it around.
  ////CCC     ACE_Message_Block &duplicate =
  ////CCC       *message_block.duplicate ();
  ////CCC       
  ////CCC     // Fake the result so that we will get called back.
  ////CCC     ACE_Asynch_Read_Stream_Result_Impl *fake_result =
  ////CCC       ACE_Proactor::instance ()->create_asynch_read_stream_result (*this,
  ////CCC                                                                    this->handle_,
  ////CCC                                                                    duplicate,
  ////CCC                                                                    0,
  ////CCC                                                                    0,
  ////CCC                                                                    ACE_INVALID_HANDLE,
  ////CCC                                                                    0,
  ////CCC                                                                    0);
  ////CCC                                                                    
  ////CCC     size_t bytes_transferred = message_block.length ();
  ////CCC     
  ////CCC     // <complete> for Accept would have already moved the <wr_ptr>
  ////CCC     // forward. Update it to the beginning position.
  ////CCC     duplicate.wr_ptr (duplicate.wr_ptr () - bytes_transferred);
  ////CCC     
  ////CCC     // This will call the callback.
  ////CCC     fake_result->complete (message_block.length (),
  ////CCC                            1,
  ////CCC                            0);
  ////CCC                            
  ////CCC     // Zap the fake result.
  ////CCC     delete fake_result;
  ////CCC   }
  ////CCC else
    // Otherwise, make sure we proceed. Initiate reading the socket
    // stream.
    if (this->initiate_read_stream () == -1)
      return;
     /////this->rs_.read(message_block, 8 );
}

int
Receiver::initiate_read_stream (void)
{
  // Create a new <Message_Block>.  Note that this message block will
  // be used both to <read> data asynchronously from the socket and to
  // <write> data asynchronously to the file.
  ACE_DEBUG ((LM_DEBUG, "initiate_read_stream called\n"));
  ACE_Message_Block *mb = 0;
  ACE_NEW_RETURN (mb, ACE_Message_Block (ACE_DEFAULT_CDR_BUFSIZE), -1);
  ACE_CDR::mb_align(mb);

  // Inititiate read
  if (this->rs_.read (*mb, HEADER_SIZE) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "ACE_Asynch_Read_Stream::read"), -1);
  return 0;
}

void
Receiver::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result)
{
  ACE_DEBUG ((LM_DEBUG, "handle_read_stream called\n"));

  // Reset pointers.
  ////result.message_block ().rd_ptr ()[result.bytes_transferred ()] = '\0';

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_read", result.bytes_to_read ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));
#if 0
  // This can overrun the ACE_Log_Msg buffer and do bad things.
  // Re-enable it at your risk.
  ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "message_block", result.message_block ().rd_ptr ()));
#endif /* 0 */

  if (!result.success() || result.bytes_transferred() == 0)
  {
      delete this;
  }
  else if (result.bytes_transferred() < result.bytes_to_read())
  {
     rs_.read( result.message_block(), result.bytes_to_read() - result.bytes_transferred() ); 
  }
  else if ( result.message_block().length() == HEADER_SIZE)
  {
      ACE_InputCDR cdr (& (result.message_block()));

      ACE_CDR::Boolean byte_order;
      cdr >> ACE_InputCDR::to_boolean (byte_order);
      cdr.reset_byte_order(byte_order);

      ACE_CDR::ULong length;
      cdr >> length;

      result.message_block().size(length + HEADER_SIZE);
      rs_.read (result.message_block(),length);
  }
  else
  {
      ACE_InputCDR cdr (& (result.message_block()));

      ACE_CDR::Boolean byte_order;
      cdr >> ACE_InputCDR::to_boolean (byte_order);
      cdr.reset_byte_order(byte_order);

      ACE_CDR::ULong length;
      cdr >> length;
      DataGloveData glovedata;
      cdr >> glovedata;
      // Successful read: write the data to the file asynchronously.
      // Note how we reuse the <ACE_Message_Block> for the writing.
      // Therefore, we do not delete this buffer because it is handled
      // in <handle_write_stream>.
      if (this->wf_.write (result.message_block (),
              result.bytes_transferred (),
              this->file_offset_) == -1)
      {
          ACE_ERROR ((LM_ERROR,
                  "%p\n",
                  "ACE_Asynch_Write_File::write"));
          return;
      }

      // Initiate new read from the stream.
      if (this->initiate_read_stream () == -1)
          return;
  }
}

void
Receiver::handle_write_file (const ACE_Asynch_Write_File::Result &result)
{
  ACE_DEBUG ((LM_DEBUG, "handle_write_file called\n"));

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));

  result.message_block ().release ();

  if (result.success ())
    // Write successful:  Increment file offset
    this->file_offset_ += result.bytes_transferred ();

  // This code is not robust enough to deal with short file writes
  // (which hardly ever happen) ;-)
  ACE_ASSERT (result.bytes_to_write () == result.bytes_transferred ());
}
