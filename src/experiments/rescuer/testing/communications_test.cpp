/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file communications_test.cpp
 * @brief This is a test suit for the communications. It will implements
 * differente types on communcations for testing RESCUER system comms.
 * @version 1.0
 * @date 31-Jul-06 7:50:01 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 * Version 1.0: Originaly derived from ACE "examples/test_proactor.cpp" by Irfan Pyarali.
 */

/*
 * RCS-ID:$Id: communications_test.cpp,v 1.4 2006-08-03 16:46:08 beltran Exp $
 */
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

#include "receiver.h"
#include "glovedata.h"
#include "sender.h"
#include "UDPGenerator.h"

const int HEADER_SIZE = 8;
// Host that we're connecting to.
ACE_TCHAR *host = 0;

// Port that we're receiving connections on.
u_short port = ACE_DEFAULT_SERVER_PORT;
u_short remoteport = 0;

// File that we're sending.
const ACE_TCHAR *file = ACE_TEXT("communications_test.cpp");

// Name of the output file.
const ACE_TCHAR *dump_file = ACE_TEXT("output");

// Keep track of when we're done.
int done = 0;

// Size of each initial asynchronous <read> operation.
int initial_read_size = BUFSIZ;

// Whether the connection is udp or tcp
bool udp = false;


static int
parse_args (int argc, ACE_TCHAR *argv[])
{
  ACE_Get_Opt get_opt (argc, argv, ACE_TEXT("h:p:r:f:d:u"));
  int c;

  while ((c = get_opt ()) != EOF)
    switch (c)
      {
      case 'h':
        host = get_opt.opt_arg ();
        break;
      case 'p':
        port = ACE_OS::atoi (get_opt.opt_arg ());
        break;
      case 'r':
        remoteport = ACE_OS::atoi (get_opt.opt_arg());
        break;
      case 'f':
        file = get_opt.opt_arg ();
        break;
      case 'd':
        //dump_file = get_opt.opt_arg ();
        break;
      case 'u':
        udp = true;
        break;
      default:
        ACE_ERROR ((LM_ERROR, "%p.\n",
                    "usage :\n"
                    "-h <host>\n"
                    "-p <localport>\n"
					"-r <remoteport>\n"
                    "-f <file>\n"
                    "-d determines the use of udp\n"));
        return -1;
      }

  return 0;
}

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  if (parse_args (argc, argv) == -1)
    return -1;

  if ( udp = false) // Lets do a tcp/ip connection
  {
      Sender sender;

      // Note: acceptor parameterized by the Receiver.
      ACE_Asynch_Acceptor<Receiver> acceptor;

      // If passive side
      if (host == 0)
      {
          if (acceptor.open (ACE_INET_Addr (port)) == -1)
              return -1;

          int success = 1;

          while (success > 0  && !done)
              // Dispatch events via Proactor singleton.
              success = ACE_Proactor::instance ()->handle_events ();
      }
      // If active side
      //else if (sender.open (host, port) == -1)
      //  return -1;
      else 
      {
          ACE_INET_Addr reader_addr;
          int result;
          result = reader_addr.set ( port, host);
          ACE_SOCK_Connector connector;

          if (connector.connect (sender.peer (), reader_addr) < 0)
              ACE_ERROR_RETURN ((LM_ERROR, "%p\n", "connect()"), 1);
          DataGloveData glove_data;
          glove_data.initialize();
          while (1)
          {
              ACE_OS::printf("*****%s ******\n", finalmessage);
              sender.send(glove_data);
              ACE_OS::sleep(2);
          }
      }
  }
  else // We want an udp connection
  {
      UDPGenerator udpGenerator;

      // If passive side
      if (host == 0 || port == 0 || remoteport == 0 )
      {
          ACE_ERROR_RETURN((LM_ERROR,ACE_TEXT("Sorry, I need a remote host name\n")),-1);
          return -1;
      }
      // If active side
      else if (udpGenerator.open (host, port, remoteport) == -1)
          return -1;

      for (int success = 1;
          success > 0  && !done;
          )
          // Dispatch events via Proactor singleton.
          success = ACE_Proactor::instance ()->handle_events ();

      return 0;
  }

  ACE_DEBUG ((LM_DEBUG, "Ending the program\n"));
  return 0;
}

