
package yarp.os;

import java.io.*;
import java.util.*;

class ProtocolBlockWriter implements BlockWriter {
    private List content;
    private boolean textMode;

    public ProtocolBlockWriter() {
    }

    public void set(List content, boolean textMode) {
	this.content = content;
	this.textMode = textMode;
    }

    private void addContent(byte[] data) {
	content.add(data);
    }

    public void appendBlock(byte[] data) {
	addContent(data);
    }

    public void appendInt(int data) {
	addContent(NetType.netInt(data));
    }

    public void appendString(String data) {
	addContent(NetType.netString(data));
    }

    public void appendLine(String data) {
	appendString(data + "\n");
    }

    public boolean isTextMode() throws IOException {
	return textMode;
    }
}

