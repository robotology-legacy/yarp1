/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file sender.cpp
 * @brief 
 * @version 1.0
 * @date 31-Jul-06 7:54:18 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: sender.cpp,v 1.1 2006-08-01 09:20:13 beltran Exp $
 */

#include "sender.h"

int operator<< (ACE_OutputCDR &cdr, const DataGloveData &glove_data)
{
  size_t msglen = glove_data.length;
  // Insert each field from <glove_data> into the output CDR stream.
  cdr << ACE_CDR::ULong (msglen);
  cdr.write_long_array(glove_data.thumb, 3);
  cdr << ACE_CDR::ULong (glove_data.palmArch);
  cdr << ACE_CDR::ULong (glove_data.wristPitch);
  cdr << ACE_CDR::ULong (glove_data.wristYaw);
  cdr.write_char_array (finalmessage, __finalmessagelength);
  return cdr.good_bit ();
}

Sender::Sender (void)
  : input_file_ (ACE_INVALID_HANDLE),
    file_offset_ (0),
    file_size_ (0),
    stream_write_done_ (0),
    transmit_file_done_ (0)
{
  // Moment of inspiration... :-)
  static const char *data = "Welcome to Irfan World! Irfan RULES here !!\n";
  this->welcome_message_.init (data,
                               ACE_OS::strlen (data));
  this->welcome_message_.wr_ptr (ACE_OS::strlen (data));
}

Sender::~Sender (void)
{
  this->stream_.close ();
}

ACE_HANDLE
Sender::handle (void) const
{
  return this->stream_.get_handle ();
}

void
Sender::handle (ACE_HANDLE handle)
{
  this->stream_.set_handle (handle);
}

int
Sender::open (const ACE_TCHAR *host,
               u_short port)
{
  // Initialize stuff

  // Open input file (in OVERLAPPED mode)
  this->input_file_ =
    ACE_OS::open (file, GENERIC_READ | FILE_FLAG_OVERLAPPED);
  if (this->input_file_ == ACE_INVALID_HANDLE)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_OS::open"), -1);

  // Find file size
  this->file_size_ =
    ACE_OS::filesize (this->input_file_);

  // Connect to remote host
  ACE_INET_Addr address (port, host);
  ACE_SOCK_Connector connector;
  if (connector.connect (this->stream_,
                         address) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_SOCK_Connector::connect"),
                      -1);

  // Open ACE_Asynch_Write_Stream
  if (this->ws_.open (*this) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_Asynch_Write_Stream::open"),
                      -1);

  // Open ACE_Asynch_Read_File
  if (this->rf_.open (*this, this->input_file_) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_Asynch_Read_File::open"),
                      -1);

  // Start an asynchronous transmit file
  if (this->transmit_file () == -1)
    return -1;

  // Start an asynchronous read file
  if (this->initiate_read_file () == -1)
    return -1;

  return 0;
}

int
Sender::transmit_file (void)
{
  // Open file (in SEQUENTIAL_SCAN mode)
  ACE_HANDLE file_handle =
    ACE_OS::open (file, GENERIC_READ | FILE_FLAG_SEQUENTIAL_SCAN);
  if (file_handle == ACE_INVALID_HANDLE)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_OS::open"),
                      -1);

  // Open ACE_Asynch_Transmit_File
  if (this->tf_.open (*this) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_Asynch_Transmit_File::open"),
                      -1);

  // Header and trailer data for the file.
  // @@ What happens if header and trailer are the same?
  this->header_and_trailer_.header_and_trailer (&this->welcome_message_,
                                                this->welcome_message_.length (),
                                                &this->welcome_message_,
                                                this->welcome_message_.length ());

  // Send the entire file in one fell swoop!
  if (this->tf_.transmit_file (file_handle,
                               &this->header_and_trailer_) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_Asynch_Transmit_File::transmit_file"),
                      -1);

  return 0;
}

void
Sender::handle_transmit_file (const ACE_Asynch_Transmit_File::Result &result)
{
  ACE_DEBUG ((LM_DEBUG,
              "handle_transmit_file called\n"));

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "socket", result.socket ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "file", result.file ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_per_send", result.bytes_per_send ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "flags", result.flags ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));

  // Done with file
  ACE_OS::close (result.file ());

  this->transmit_file_done_ = 1;
  if (this->stream_write_done_)
    done = 1;
}

int
Sender::initiate_read_file (void)
{
  // Create a new <Message_Block>.  Note that this message block will
  // be used both to <read> data asynchronously from the file and to
  // <write> data asynchronously to the socket.
  ACE_Message_Block *mb = 0;
  ACE_NEW_RETURN (mb,
                  ACE_Message_Block (BUFSIZ + 1),
                  -1);

  // Inititiate an asynchronous read from the file
  if (this->rf_.read (*mb,
                      mb->size () - 1,
                      this->file_offset_) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "%p\n",
                       "ACE_Asynch_Read_File::read"),
                      -1);
  return 0;
}

void
Sender::handle_read_file (const ACE_Asynch_Read_File::Result &result)
{
  ACE_DEBUG ((LM_DEBUG,
              "handle_read_file called\n"));

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
  //ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "message_block", result.message_block ().rd_ptr ()));

  if (result.success ())
    {
      // Read successful: increment offset and write data to network.
      // Note how we reuse the <ACE_Message_Block> for the writing.
      // Therefore, we do not delete this buffer because it is handled
      // in <handle_write_stream>.

      this->file_offset_ += result.bytes_transferred ();
      if (this->ws_.write (result.message_block (),
                           result.bytes_transferred ()) == -1)
        {
          ACE_ERROR ((LM_ERROR,
                      "%p\n",
                      "ACE_Asynch_Write_Stream::write"));
          return;
        }

      if (this->file_size_ > this->file_offset_)
        {
          // Start an asynchronous read file.
          if (initiate_read_file () == -1)
            return;
        }
    }
}

void
Sender::handle_write_stream (const ACE_Asynch_Write_Stream::Result &result)
{
  ACE_DEBUG ((LM_DEBUG,
              "handle_write_stream called\n"));

  // Reset pointers.
  result.message_block ().rd_ptr (result.message_block ().rd_ptr () - result.bytes_transferred ());

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));
#if 0
  ACE_DEBUG ((LM_DEBUG, "%s = %s\n", "message_block", result.message_block ().rd_ptr ()));
#endif

  if (result.success ())
    {
      // Partial write to socket
      int unsent_data = result.bytes_to_write () - result.bytes_transferred ();
      if (unsent_data != 0)
        {
          // Reset pointers
          result.message_block ().rd_ptr (result.bytes_transferred ());

          // Duplicate the message block and retry remaining data
          if (this->ws_.write (*result.message_block ().duplicate (),
                               unsent_data) == -1)
            {
              ACE_ERROR ((LM_ERROR,
                          "%p\n",
                          "ACE_Asynch_Write_Stream::write"));
              return;
            }
        }
      else if (!(this->file_size_ > this->file_offset_))
        {
          this->stream_write_done_ = 1;
          if (this->transmit_file_done_)
            done = 1;
        }
    }

  // Release message block.
  result.message_block ().release ();
}
