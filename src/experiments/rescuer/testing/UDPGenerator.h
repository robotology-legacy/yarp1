/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file UDPGenerator.h
 * @brief the definitions of the UDPGenerator
 * @version 1.0
 * @date 02-Aug-06 6:00:25 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: UDPGenerator.h,v 1.2 2006-08-03 19:57:49 beltran Exp $
 */

#ifndef _UDPGENERATORH_ 
#define _UDPGENERATORH_

#include "ace/OS_NS_string.h"
#include "ace/OS_main.h"
#include "ace/Proactor.h"
#include "ace/Asynch_IO.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Dgram.h"
#include "ace/Message_Block.h"
#include "ace/Get_Opt.h"
#include "ace/Log_Msg.h"
#include "ace/CDR_Stream.h"
#include "glovedata.h"

class UDPGenerator : public ACE_Service_Handler
{
  // = TITLE
  //     This class will receive data from
  //     the network connection and dump it to a file.
public:
  // = Initialization and termination.
  UDPGenerator (void);
  ~UDPGenerator (void);

  int open(const ACE_INET_Addr &localAddr);
  int open(const ACE_TCHAR * remotehost,
      u_short localport,
      u_short remoteport);

protected:
  // These methods are called by the framework

  /** 
   * handle_read_dgram This method will be called when an asynchronous read
   * completes on a UDP socket.
   * 
   * @param result 
   */
  virtual void handle_read_dgram (const ACE_Asynch_Read_Dgram::Result &result);

  /** 
   * handle_write_dgram This is called when asynchronous writes from the dgram
   * socket completes.
   * 
   * @param result 
   */
  virtual void handle_write_dgram (const ACE_Asynch_Write_Dgram::Result &result);

  int readdatagram(int header_size);
  int gloveDgramWrite(const ACE_TCHAR * remotehost, u_short remoteport, const DataGloveData &glovedata);

  int writedatagram( const ACE_TCHAR * remotehost, u_short remoteport);
  int UDPGenerator::rescuerDgramWrite( const ACE_TCHAR * remotehost, u_short remoteport);

private:
  ACE_SOCK_Dgram sockDgram_;

  ACE_Asynch_Read_Dgram rd_;
  ACE_Asynch_Write_Dgram wd_;
  // rd (read dgram): for reading from a UDP socket.
  const char* completion_key_;
  const char* act_;
};

#endif

