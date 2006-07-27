/* vim:set tw=78: set sw=4: set ts=4: */
/** 
 * 
 *
 *             RESCUER - IST-2003-511492 (c) 2004-2008 
 *
 *   Improvement of the Emergency Risk Management through Secure Mobile
 *   Mechatronic Support to Bomb Disposal and Rescue Operations
 *
 * @file DGS_MMU_Acceptor.h
 * @brief Contains the DGS_MMU_Acceptor that receives incoming connection to the
 * DGSDriver.
 * @version 1.0
 * @date 17-Jul-06 5:14:57 PM ora solare Europa occidentale
 * @author Carlos Beltran Gonzalez (Carlos), cbeltran@dist.unige.it
 * @author Lira-Lab
 * Revisions:
 */

/*
 * RCS-ID:$Id: DGS_MMU_Acceptor.h,v 1.1 2006-07-27 16:07:02 beltran Exp $
 */

#ifndef _DGS_MMU_Acceptorh_ 
#define _DGS_MMU_Acceptorh_

#include <ace/Asynch_Acceptor.h>
#include <ace/Asynch_IO.h>
#include <ace/Unbounded_Set.h>
#include "DGSNetworkHandler.h"

class DGSNetworkHandler;


/** 
 * @class DGS_MMU_Acceptor listens for connections in a given port creating the
 * corresponding Network handler.
 * @todo In the future the acceptor should be able to create different types of
 * networks hardlers based on a first handshaking packet. To be defined. This
 * should be part of a Rescuer protocol.
 */
class DGS_MMU_Acceptor : public ACE_Asynch_Acceptor<DGSNetworkHandler> {
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

    /** 
     * setCommandsConsumer Set the consumer of the received commands.
     * 
     * @param consumer A pointer to a task with a Messages Block queue.
     * 
     * @return 
     */
    int setCommandsConsumer(DGSTask * consumer){ commands_consumer = consumer; return 0;};

    /** 
     * setConsoleConsumer Set the console that is visualicing the answers from
     * the serial consumer.
     * 
     * @param consumer 
     * 
     * @return 
     */
    int setConsoleConsumer(DGSTask * consumer){ console_consumer = consumer; return 0;};
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

    DGSTask * commands_consumer; /// A pointer to the DGSTask accepting commands.
    DGSTask * console_consumer; /// A pointer to the DGSTask accepting console responses.
};
#endif

