/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialHandler.cpp
 *  Contains the implementation of the Serial Handler
 * @version 1.0
 * @date 21-Jun-06 1:53:39 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * @todo Write a correct method of finalization of the system.
 */

/*
 * $Id: SerialHandler.cpp,v 1.1 2006-07-27 15:23:20 beltran Exp $
 */
#include <string.h>
#include "SerialHandler.h"

/** 
 *  SerialHandler::SerialHandler 
 */
SerialHandler::SerialHandler (void)
{
    ACE_TRACE("SerialHandler::SerialHandler");
}

/** 
 *  SerialHandler::~SerialHandler Destructor.
 * @todo close correctly all the streams
 */
SerialHandler::~SerialHandler (void)
{
    ACE_TRACE("SerialHandler::~SerialHandler");
}

/** 
 *  SerialHandler::open Opens the serial handler. 
 * @return 
 */
int SerialHandler::initialize(void) 
{
  ACE_TRACE("SerialHandler::initialize");

  u_short client_port = ACE_DEFAULT_SERVICE_PORT; /* This seems to be 20003*/

  // Initialize serial port
  _serialConnector.connect(_serial_dev, ACE_DEV_Addr ("COM4"), 0, ACE_Addr::sap_any, 0, 
      O_RDWR|FILE_FLAG_OVERLAPPED); 

  ACE_TTY_IO::Serial_Params myparams;
  myparams.baudrate   = 9600;
  myparams.xonlim     = 0;
  myparams.xofflim    = 0;
  myparams.parityenb  = true;
  myparams.paritymode = "FALSE";
  myparams.databits   = 8;
  myparams.stopbits   = 1;
  myparams.readmincharacters = 0;
  myparams.readtimeoutmsec   = 10000;

  if (_serial_dev.control (ACE_TTY_IO::SETPARAMS, &myparams) == -1)
      ACE_ERROR_RETURN ((LM_ERROR,
              ACE_TEXT ("%p control\n"),
              "COM1"), 1);

  // Asociate the ACE_Asynch_Read_Stream with the TTY serial device
  if (this->_serial_read_stream.open (*this, _serial_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("ACE_Asynch_Read_Stream::open")), -1);

  // Idem for ACE_Asynch_Write_Stream
  if (this->_serial_write_stream.open (*this, _serial_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
                ACE_TEXT("ACE_Asynch_Write_Stream::open")), -1);

  ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("SerialHandler::open: streams and Asynch Operations opened sucessfully\n")));
/*{{{*/
  // Start an asynchronous read stream
  ////if (this->initiate_read_stream () == -1)
  ////  return -1;
  
  /* Sending a ? to the serial {{{*/
  /*
  char message[]="?\r";
  ACE_Message_Block message_block(2);

  if (message_block.copy(message, 2)) {
        ACE_ERROR ((LM_ERROR,
                    ACE_TEXT ("Error copiando mensaje\n")));
  }

  //ssize_t bytes_written = _serial_dev.send_n ("hola\n", 5);
  if (this->_serial_write_stream.write (message_block, 2 ) == -1)
  {
      ACE_ERROR ((LM_ERROR, "%p\n", "Error escribiendo conyo"));
      return 0;
  }*/
  /*}}}*/
  // Acceptor configuration
  /*}}}*/
  
  //Start the keyboad reading thread
  activate();
  return 0;
}

/** 
 *  SerialHandler::svc Reads the queue and writes the message block in the
 * serial device
 * @return 
 */
int SerialHandler::svc()
{
    ACE_TRACE(ACE_TEXT("SerialHandler::svc"));

    for (;;)
    {
        ACE_Message_Block * serial_block;
        int result = getq(serial_block);

        if (result == -1)
        {
            ACE_DEBUG((LM_NOTICE, ACE_TEXT("%l SerialHandler::svc Problem Reading block from Queue\n")));
        } 
        else 
        {
            ACE_DEBUG((LM_NOTICE, ACE_TEXT("%l SerialHandler::svc Procesing Message_block\n")));
            //serial_block->rd_ptr()[serial_block->length()] = '\0';
            //ACE_OS::printf("Mensaje: %s\n", serial_block->rd_ptr());
            if (this->_serial_write_stream.write (*serial_block, serial_block->length()) == -1)
            {
                ACE_ERROR ((LM_ERROR, "%l \n", "Error writing in svc SerialHandler"));
                return 0;
            }
            //serial_block->release();
            //DGSTask * command_sender = ACE_reinterpret_cast(DGSTask *, serial_block->cont()->rd_ptr());
            SerialFeedbackData * feedback_data = ACE_reinterpret_cast(SerialFeedbackData *, serial_block->cont()->rd_ptr());
            DGSTask * command_sender = feedback_data->getCommandSender();
            command_sender->IsAlive();
        }
    }
    ACE_DEBUG((LM_NOTICE, ACE_TEXT("%N Line %l SerialHandler::svc Exiting\n")));
    return 0;
}

/** 
 *  SerialHandler::initiate_read_stream
 * @return 
 */
int
SerialHandler::initiate_read_stream (ACE_Message_Block * response_data_block)
{
  ACE_TRACE("SerialHandler::initiate_read_stream");

  // Create Message_Block
  ACE_Message_Block *mb = 0;
  ACE_NEW_RETURN (mb, ACE_Message_Block (BUFSIZ + 1), -1);

  //glue both block block and the response_data_block that contains all the
  //necessary data to detect the response condition and the feedback Task
  mb->cont(response_data_block);

  // Inititiate an asynchronous read from the stream
  if (this->_serial_read_stream.read (*mb, mb->size () - 1) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("ACE_Asynch_Read_Stream::read")), -1);

  ACE_DEBUG ((LM_DEBUG, ACE_TEXT("SerialHandler:initiate_read_stream: Asynch Read stream issued sucessfully\n")));/*{{{*/
/*}}}*/
  return 0;
}

/** 
 *  SerialHandler::handle_read_stream
 * @param result 
 */
void
SerialHandler::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result)
{
    //ACE_TRACE("SerialHandler::handle_read_stream");
  ACE_DEBUG ((LM_DEBUG,ACE_TEXT( "handle_read_stream called\n")));

  /*{{{
  //ACE_DEBUG ((LM_DEBUG, "********************\n"));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_read", result.bytes_to_read ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  //ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  //ACE_DEBUG ((LM_DEBUG, "********************\n"));
  }}}*/
  // Watch out if you need to enable this... the ACE_Log_Record::MAXLOGMSGLEN/*{{{*/
  // value controls to max length of a log record, and a large output
  // buffer may smash it.
//#if 0
  //ACE_DEBUG ((LM_INFO, "%s",
              //result.message_block ().rd_ptr ()));/*}}}*/
              /*{{{
  ////ACE_OS::printf("%s", result.message_block().rd_ptr());
  // Start an asynchronous read stream
  ////this->initiate_read_stream ();
  }}}*/
  SerialFeedbackData * feedback_data = ACE_reinterpret_cast(SerialFeedbackData *, result.message_block().cont()->rd_ptr());

  // Check if the => command from the barrett is present in the readed buffer
  if ( feedback_data->checkSerialResponseEnd(result.message_block().rd_ptr(), 0))
  {
      DGSTask * command_sender = feedback_data->getCommandSender();
      result.message_block().cont()->release();
      result.message_block().cont(NULL);
      command_sender->putq(&result.message_block());
  }
  else
  {
      // Inititiate an asynchronous read from the stream to read the rest of the
      // response
      if (this->_serial_read_stream.read (result.message_block(), result.message_block().size() - 1) == -1)
          ACE_ERROR ((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("ACE_Asynch_Read_Stream::read")));
  }
}

/** 
 *  SerialHandler::handle_write_stream
 * @param result 
 */
void SerialHandler::handle_write_stream (const ACE_Asynch_Write_Stream::Result &result)
{
    ACE_TRACE("SerialHandler::handle_write_stream");
  ACE_DEBUG ((LM_INFO, ACE_TEXT("handle_write_stream called\n")));

  // Reset pointers/*{{{*/
  ////result.message_block ().rd_ptr (result.message_block ().rd_ptr () - result.bytes_transferred ());

  ////result.message_block ().rd_ptr ()[result.bytes_transferred ()] = '\0';

  ////ACE_DEBUG ((LM_DEBUG, "********************\n"));/*}}}*/
  /*{{{*/
  ACE_DEBUG ((LM_INFO, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
  ///ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_INFO, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
  ACE_DEBUG ((LM_INFO, ACE_TEXT("Getting the command sender pointer\n")));
  /*}}}*/

  ACE_Message_Block * response_data_block = result.message_block().cont();

  result.message_block().cont(NULL);
  result.message_block().release();
  
  this->initiate_read_stream (response_data_block);
  ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (u_long) result.act ()));/*{{{*/
  ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success ()));
  ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (u_long) result.completion_key ()));
  ////ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error ()));
  ////ACE_DEBUG ((LM_DEBUG, "********************\n"));
  // Watch out if you need to enable this... the ACE_Log_Record::MAXLOGMSGLEN
  // value controls to max length of a log record, and a large output
  // buffer may smash it.
#if 0
  ACE_DEBUG ((LM_DEBUG, "%s = %s\n",
              "message_block",
              result.message_block ().rd_ptr ()));
#endif  /* 0 *//*}}}*/
}/*{{{*/
/*}}}*/
