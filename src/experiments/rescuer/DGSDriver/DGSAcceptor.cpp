/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSAcceptor.cpp
 * @brief Contains the implementation of the Acceptor.
 * @version 1.0
 * @date 17-Jul-06 5:22:29 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGSAcceptor.cpp,v 1.3 2006-07-27 11:06:08 beltran Exp $
 */

#include "DGSAcceptor.h"

/** 
 * DGSAcceptor::close
 */
void DGSAcceptor::close (void)
{
  ACE_Unbounded_Set_Iterator<DGSNetworkHandler *>
    iter (clients_.begin ());
  DGSNetworkHandler **ih;
  while (iter.next (ih))
    delete *ih;
}

/** 
 * DGSAcceptor::make_handler
 * 
 * @return 
 */
DGSNetworkHandler * DGSAcceptor::make_handler (void)
{
  DGSNetworkHandler *ih;
  ACE_NEW_RETURN (ih, DGSNetworkHandler(this, commands_consumer ,
          console_consumer), 0);
  if (clients_.insert (ih) == -1)
    { delete ih; return 0; }
  return ih;
}
