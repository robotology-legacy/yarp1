
package yarp.os;

import java.io.*;

/**
 * Class representing one way to send and receive YARP messages.
 * YARP communication occurs through a series of defined phases
 * that make it easy to duplicate output.  Any process that goes
 * through the same phases could be added as a YARP Carrier.
 */
interface Carrier extends ShiftStream {

    Carrier create();
    void start(Address address) throws IOException;
    void open(Address address, ShiftStream previous) throws IOException;
    void close() throws IOException;

    String getName();
    boolean checkHeader(byte[] header);
    byte[] getHeader();

    void setParameters(byte[] header);

    // sender
    void prepareSend(Protocol proto) throws IOException;
    boolean sendHeader(Protocol proto) throws IOException;
    void expectReplyToHeader(Protocol proto) throws IOException;    
    boolean sendIndex(Protocol proto) throws IOException;
    // actual data is currently always sent the same way - this need not
    // be so

    // receiver
    boolean expectExtraHeader(Protocol proto) throws IOException;
    boolean respondToHeader(Protocol proto) throws IOException;
    boolean expectIndex(Protocol proto) throws IOException;
    boolean expectSenderSpecifier(Protocol proto) throws IOException;
    boolean sendAck(Protocol proto) throws IOException;

    // is there a way to simplify the above phases?
    // at least eliminate Protocol for something weaker (Route,ShiftStream,...)
    // could merge expectextraheader and respondtoheader
    // could merge sendheader and expectreplytoheader

    // sender: sendHeader+expectReply; sendIndex; sendPayload+expectAck
    // rcver:  expectHeader+respond; expeckIndex; expectPayload+sendAck

    boolean isActive() throws IOException;
    boolean isConnectionless();
    boolean canAccept();
    boolean canOffer();
    boolean isTextMode();

    // not ready - use protocol.requireack
    boolean requireAck();

    TwoWayStream getStreams();
    TwoWayStream takeStreams();

    String toString();
}

