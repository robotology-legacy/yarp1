/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file sender.h
 * @brief 
 * @version 1.0
 * @date 01-Aug-06 2:06:16 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: sender.h,v 1.2 2006-08-01 20:53:27 beltran Exp $
 */

#ifndef _SENDERH_ 
#define _SENDERH_

class Sender {
private:
  ACE_SOCK_Stream peer_;

public:
  ACE_SOCK_Stream &peer () { return peer_; }

  int send (const DataGloveData &glove_data);

  ~Sender () { peer_.close (); }
};

#endif

