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
 * @date 01-Aug-06 2:01:21 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions: 
 * Version 1.0: Based on C++NP1 sender.cpp
 */

/*
 * RCS-ID:$Id: sender.cpp,v 1.3 2006-08-02 15:30:37 beltran Exp $
 */

#include "ace/OS_NS_sys_time.h"
#include "ace/CDR_Stream.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Connector.h"
#include "ace/SOCK_Stream.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Record.h"
#include "ace/OS_NS_unistd.h"

// FUZZ: disable check_for_streams_include
#include "ace/streams.h"

#if defined (ACE_WIN32) && (!defined (ACE_HAS_STANDARD_CPP_LIBRARY) || \
                            (ACE_HAS_STANDARD_CPP_LIBRARY == 0))
#  include <stdio.h>
#else
#  include <string>
#endif
#include "glovedata.h"
#include "sender.h"
extern const int HEADER_SIZE;
extern const int __finalmessagelength;

int operator<< (ACE_OutputCDR &cdr, const DataGloveData &glove_data)
{
  size_t msglen = glove_data.getMessageLength();
  // Insert each field from <glove_data> into the output CDR stream.
  cdr.write_long_array(glove_data.thumb, 3);
  cdr << ACE_CDR::ULong (glove_data.palmArch);
  cdr << ACE_CDR::ULong (glove_data.wristPitch);
  cdr << ACE_CDR::ULong (glove_data.wristYaw);
  cdr << ACE_CDR::ULong (msglen);
  cdr.write_char_array (finalmessage, __finalmessagelength);
  return cdr.good_bit ();
}

int
Sender::send(const DataGloveData &glovedata)
{
    ACE_DEBUG ((LM_DEBUG, "Sender::initiate_write called\n"));
    const size_t max_payload_size =
        4 //boolean alignment flag
        + 4 //payload length 
        + glovedata.length // Data Glove data length
        + ACE_CDR::MAX_ALIGNMENT; //pading

    ACE_OutputCDR payload (max_payload_size);
    payload << glovedata;

    ACE_CDR::ULong length = payload.total_length();

    ACE_OutputCDR header (ACE_CDR::MAX_ALIGNMENT + 8);
    header << ACE_OutputCDR::from_boolean (ACE_CDR_BYTE_ORDER);
    header << ACE_CDR::ULong(length);

    iovec iov[2];
    iov[0].iov_base = header.begin()->rd_ptr();
    iov[0].iov_len  = HEADER_SIZE;
    iov[1].iov_base = payload.begin()->rd_ptr();
    iov[1].iov_len  = length;

    return peer_.sendv_n(iov,2);
}

