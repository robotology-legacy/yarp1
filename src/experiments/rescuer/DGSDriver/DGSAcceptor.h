/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGSAcceptor.h
 * @brief Contains the DGSAcceptor that receives incoming connection to the
 * DGSDriver.
 * @version 1.0
 * @date 17-Jul-06 5:14:57 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGSAcceptor.h,v 1.1 2006-07-17 18:17:43 beltran Exp $
 */

#ifndef _DGSAcceptorh_ 
#define _DGSAcceptorh_

#include <ace/Asynch_Acceptor.h>
#include <ace/Asynch_IO.h>
#include <ace/Unbounded_Set.h>
#include "DGSNetworkHandler.h"

class DGSNetworkHandler;


class DGSAcceptor
: public ACE_Asynch_Acceptor<DGSNetworkHandler> {
public:
    /** 
     * Cancel accept and close all clients
     */
    void close (void);

    // Remove handler from client set.
    /** 
     * Remove handler from client set.
     * 
     * @param ih The correspoding DGSNetworkHandler.
     */
    void remove (DGSNetworkHandler *ih)
    { clients_.remove (ih); }

protected:
    /** 
     * Service handler factory method.
     * 
     * @return 
     */
    virtual DGSNetworkHandler *make_handler (void);

    /** 
     * Set of all connected clients.
     */
    ACE_Unbounded_Set<DGSNetworkHandler *> clients_;
};
#endif

