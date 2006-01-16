
package yarp.os;

import java.io.IOException;

class CommandContent implements Content {
    public Integer content = new Integer(0);
    
    private char ch;
    private String str;

    public CommandContent() {
    }

    public CommandContent(char ch, String str) {
	this.ch = ch;
	this.str = str;
    }

    public Content create() {
	return new CommandContent();
    }

    public void read(BlockReader proto) throws IOException {
	// no reading offered yet
	System.exit(1);
    }

    public void write(BlockWriter proto) throws IOException {
	Logger.get().println("Command content");
	if (!proto.isTextMode()) {
	    int len = 0;
	    if (str!=null) {
		len = str.length()+1;
	    }
	    byte[] hdr = NetType.append(NetType.netInt(len),
					new byte[] { '~', (byte)ch, 0, 1});
	    proto.appendBlock(hdr);
	    if (str!=null) {
		proto.appendString(str);
	    }
	} else {
	    if (ch!='\0') {
		proto.appendString("" + ch + "\n");
	    } else {
		proto.appendString(str);
	    }
	}
    }

    public Object object() {
	return this;
    }
}

