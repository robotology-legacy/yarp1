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
 * RCS-ID:$Id: receiver.cpp,v 1.1 2006-08-01 09:20:13 beltran Exp $
 */

#include "receiver.h"



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
  //ccc if (message_block.length () != 0)
    //ccc {
      //ccc // Duplicate the message block so that we can keep it around.
      //ccc ACE_Message_Block &duplicate =
        //ccc *message_block.duplicate ();
        //ccc 
      //ccc // Fake the result so that we will get called back.
      //ccc ACE_Asynch_Read_Stream_Result_Impl *fake_result =
        //ccc ACE_Proactor::instance ()->create_asynch_read_stream_result (*this,
                                                                     //ccc this->handle_,
                                                                     //ccc duplicate,
                                                                     //ccc initial_read_size,
                                                                     //ccc 0,
                                                                     //ccc ACE_INVALID_HANDLE,
                                                                     //ccc 0,
                                                                     //ccc 0);
                                                                     //ccc 
      //ccc size_t bytes_transferred = message_block.length ();
      //ccc 
      //ccc // <complete> for Accept would have already moved the <wr_ptr>
      //ccc // forward. Update it to the beginning position.
      //ccc duplicate.wr_ptr (duplicate.wr_ptr () - bytes_transferred);
      //ccc 
      //ccc // This will call the callback.
      //ccc fake_result->complete (message_block.length (),
                             //ccc 1,
                             //ccc 0);
                             //ccc 
      //ccc // Zap the fake result.
      //ccc delete fake_result;
    //ccc }
  //ccc else
    // Otherwise, make sure we proceed. Initiate reading the socket
    // stream.
    if (this->initiate_read_stream () == -1)
      return;
}

int
Receiver::initiate_read_stream (void)
{
  // Create a new <Message_Block>.  Note that this message block will
  // be used both to <read> data asynchronously from the socket and to
  // <write> data asynchronously to the file.
  ACE_Message_Block *mb = 0;
  ACE_NEW_RETURN (mb,
                  ACE_Message_Block (BUFSIZ + 1),
                  -1);

  // Inititiate read
  if (this->rs_.read (*mb,
                      mb->size () - 1) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_Asynch_Read_Stream::read"),
                      -1);
  return 0;
}

void
Receiver::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result)
{
  ACE_DEBUG ((LM_DEBUG,
              "handle_read_stream called\n"));

  // Reset pointers.
  result.message_block ().rd_ptr ()[result.bytes_transferred ()] = '\0';

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

  if (result.success () && result.bytes_transferred () != 0)
    {
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
  else
    {
      ACE_DEBUG ((LM_DEBUG,
                  "Receiver completed\n"));

      // No need for this message block anymore.
      result.message_block ().release ();

      // Note that we are done with the test.
      done = 1;

      // We are done: commit suicide.
      delete this;
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
