
package yarp.os;

import java.io.*;
import java.net.*;

public class SocketShiftStream implements ShiftStream {
    Socket socket;

    public SocketShiftStream(Socket socket) {
	this.socket = socket;
    }

    public void close() throws IOException {
	socket.close();
    }

    public InputStream getInputStream() throws IOException {
	return socket.getInputStream();
    }

    public OutputStream getOutputStream() throws IOException {
	return socket.getOutputStream();
    }
}

