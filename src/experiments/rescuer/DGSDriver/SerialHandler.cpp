/** =================================================================================
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file SerialHandler.cpp
 * @brief Contains the implementation of the Serial Handler
 * @version 1.0
 * @date 21-Jun-06 1:53:39 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * ===================================================================================*/

/*
 * $Id: SerialHandler.cpp,v 1.1 2006-06-21 17:32:24 beltran Exp $
 */
#include "SerialHandler.h"

/** --------------------------------------------------------------------------
 * @brief SerialHandler::SerialHandler 
 ----------------------------------------------------------------------------*/
SerialHandler::SerialHandler (void)
{
    ACE_TRACE("SerialHandler::SerialHandler");
    flag = 0;
    counter = 0;
}

/** --------------------------------------------------------------------------
 * @brief SerialHandler::~SerialHandler Destructor.
 * @todo close correctly all the streams
 ----------------------------------------------------------------------------*/
SerialHandler::~SerialHandler (void)
{
    ACE_TRACE("SerialHandler::~SerialHandler");
    reader_.cancel();
}

/** --------------------------------------------------------------------------
 * @brief SerialHandler::open Opens the serial handler. 
 * @return 
 ----------------------------------------------------------------------------*/
int SerialHandler::initialize(void) 
{
  ACE_TRACE("SerialHandler::initialize");
  u_short client_port = ACE_DEFAULT_SERVICE_PORT; /* This seems to be 20003*/

  // Initialize serial port
  con.connect(read_dev, ACE_DEV_Addr ("COM4"), 0, ACE_Addr::sap_any, 0, 
      O_RDWR|FILE_FLAG_OVERLAPPED); 

  ACE_TTY_IO::Serial_Params myparams;
  myparams.baudrate = 9600;
  myparams.xonlim   = 0;
  myparams.xofflim  = 0;
  myparams.readmincharacters = 0;
  myparams.readtimeoutmsec   = 10000;
  myparams.parityenb  = true;
  myparams.paritymode = "FALSE";
  myparams.databits = 8;
  myparams.stopbits = 1;

  if (read_dev.control (ACE_TTY_IO::SETPARAMS, &myparams) == -1)
      ACE_ERROR_RETURN ((LM_ERROR,
              ACE_TEXT ("%p control\n"),
              "COM1"), 1);

  // Asociate the ACE_Asynch_Read_File with the TTY serial device
  if (this->rf_.open (*this, read_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
            ACE_TEXT("ACE_Asynch_Read_File::open")), -1);

  // Idem for ACE_Asynch_Write_File
  if (this->wf_.open (*this, read_dev.get_handle()) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"),
                ACE_TEXT("ACE_Asynch_Write_File::open")), -1);

  ACE_DEBUG ((LM_DEBUG, 
          ACE_TEXT("SerialHandler::open: Files and Asynch Operations opened sucessfully\n")));

  // Start an asynchronous read file
  if (this->initiate_read_file () == -1)
    return -1;
  
  /* Sending a ? to the serial {{{*/
  /*
  char message[]="?\r";
  ACE_Message_Block message_block(2);

  if (message_block.copy(message, 2)) {
        ACE_ERROR ((LM_ERROR,
                    ACE_TEXT ("Error copiando mensaje\n")));
  }

  //ssize_t bytes_written = read_dev.send_n ("hola\n", 5);
  if (this->wf_.write (message_block, 2 ) == -1)
  {
      ACE_ERROR ((LM_ERROR, "%p\n", "Error escribiendo conyo"));
      return 0;
  }*/
  /*}}}*/
  // Acceptor configuration
  
  //Start the keyboad reading thread
  activate();
  return 0;
}

/** --------------------------------------------------------------------------
 * @brief SerialHandler::svc
 * @return 
 ----------------------------------------------------------------------------*/
int SerialHandler::svc()
{
    for (;;)
    {
        std::string user_input;
        //std::getline(cin,user_input, '\n');
        int c = getchar();
        while ( c != '\n')
        {
            user_input+= c;
            c = getchar();
        }
        if (user_input.size() == 0) continue;
        if (user_input == "quit") 
            ACE_OS::printf("ok, received quit\n");
        user_input = user_input + "\r";

        ACE_Message_Block message_block(user_input.size());

        if (message_block.copy(user_input.c_str(), user_input.size())) {
            ACE_ERROR ((LM_ERROR,
                    ACE_TEXT ("Error copiando mensaje\n")));
        }

        //ssize_t bytes_written = read_dev.send_n ("hola\n", 5);
        if (this->wf_.write (message_block, user_input.size()) == -1)
        {
            ACE_ERROR ((LM_ERROR, "%p\n", "Error escribiendo conyo"));
            return 0;
        }
    }
}

/** --------------------------------------------------------------------------
 * @brief SerialHandler::initiate_read_file
 * @return 
 ----------------------------------------------------------------------------*/
int
SerialHandler::initiate_read_file (void)
{
    //ACE_TRACE("SerialHandler::initiate_read_file");
  // Create Message_Block
  ACE_Message_Block *mb = 0;
  ACE_NEW_RETURN (mb, ACE_Message_Block (BUFSIZ + 1), -1);

  // Inititiate an asynchronous read from the file
  if (this->rf_.read (*mb,
		      mb->size () - 1) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("ACE_Asynch_Read_File::read")), -1);

  ACE_DEBUG ((LM_DEBUG, ACE_TEXT("SerialHandler:initiate_read_file: Asynch Read File issued sucessfully\n")));

  return 0;
}

/** --------------------------------------------------------------------------
 * @brief SerialHandler::handle_read_file
 * @param result 
 ----------------------------------------------------------------------------*/
void
SerialHandler::handle_read_file (const ACE_Asynch_Read_File::Result &result)
{
    counter++;
    //ACE_TRACE("SerialHandler::handle_read_file");
  ACE_DEBUG ((LM_DEBUG,ACE_TEXT( "handle_read_file called\n")));

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
  // Watch out if you need to enable this... the ACE_Log_Record::MAXLOGMSGLEN/*{{{*/
  // value controls to max length of a log record, and a large output
  // buffer may smash it.
//#if 0
  //ACE_DEBUG ((LM_INFO, "%s",
              //result.message_block ().rd_ptr ()));/*}}}*/
  ACE_OS::printf("%s", result.message_block().rd_ptr());
  // Start an asynchronous read file
  this->initiate_read_file ();
/*{{{*/
  /*
  if ((result.bytes_transferred() == 0) && (flag == 0)) {

      ACE_DEBUG ((LM_INFO, ACE_TEXT("Writing ? message\n")));
      char message[]="PGET PTEMP\r";
      ACE_Message_Block message_block(11);

      if (message_block.copy(message, 11)) {
          ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("Error copiando mensaje\n")));
      }

      //ssize_t bytes_written = read_dev.send_n ("hola\n", 5);
      if (this->wf_.write (message_block, 11 ) == -1)
      {
          ACE_ERROR ((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("Error escribiendo conyo")));
          return;
      }
      flag = 1;
  }
  */

  ////if (result.success ())
  ////  {
  ////    // Read successful: write this to the file.
  ////    if (this->wf_.write (result.message_block (),
  ////  		   result.bytes_transferred ()) == -1)
  ////  {
  ////    ACE_ERROR ((LM_ERROR, "%p\n", "ACE_Asynch_Write_File::write"));
  ////    return;
  ////  }
  ////  }
/*}}}*/
}

/** --------------------------------------------------------------------------
 * @brief SerialHandler::handle_write_file
 * @param result 
 ----------------------------------------------------------------------------*/
void SerialHandler::handle_write_file (const ACE_Asynch_Write_File::Result &result)
{
    ACE_TRACE("SerialHandler::handle_write_file");
  ACE_DEBUG ((LM_INFO, ACE_TEXT("handle_write_File called\n")));

  // Reset pointers/*{{{*/
  ////result.message_block ().rd_ptr (result.message_block ().rd_ptr () - result.bytes_transferred ());

  ////result.message_block ().rd_ptr ()[result.bytes_transferred ()] = '\0';

  ////ACE_DEBUG ((LM_DEBUG, "********************\n"));/*}}}*/
  ACE_DEBUG ((LM_INFO, "%s = %d\n", "bytes_to_write", result.bytes_to_write ()));
  ///ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle ()));
  ACE_DEBUG ((LM_INFO, "%s = %d\n", "bytes_transfered", result.bytes_transferred ()));
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
  ///ACE_Proactor::end_event_loop ();
}

