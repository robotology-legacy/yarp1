
package yarp.os;

import java.io.*;

interface Carrier extends ShiftStream {

    Carrier create();
    void start(Address address) throws IOException;
    void open(Address address, ShiftStream previous) throws IOException;
    void close() throws IOException;

    String getName();
    // int getSpecifier();
    boolean checkHeader(byte[] header);
    byte[] getHeader();

    void setParameters(byte[] header);

    boolean sendHeader(Protocol proto) throws IOException;
    void prepareSend(Protocol proto) throws IOException;
    boolean expectExtraHeader(Protocol proto) throws IOException;
    boolean expectIndex(Protocol proto) throws IOException;
    boolean sendIndex(Protocol proto) throws IOException;
    boolean sendAck(Protocol proto) throws IOException;
    boolean respondToHeader(Protocol proto) throws IOException;
    boolean expectSenderSpecifier(Protocol proto) throws IOException;
    void expectReplyToHeader(Protocol proto) throws IOException;    

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

