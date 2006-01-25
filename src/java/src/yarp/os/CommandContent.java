
package yarp.os;

import java.io.IOException;

class CommandContent implements Content {
    static Logger log = Logger.get();

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
	int cmd = 0;
	byte[] arg = new byte[] {0};
	ch = 0;
	str = "";
	if (proto.isTextMode()) {
	    String txt = proto.expectLine();
	    if (txt.length()>0) {
		cmd = txt.charAt(0);
		arg = NetType.netString(txt);
		ch = (char)cmd;
		str = txt;
		return;
	    }
	} else {
	    if (proto.getSize()>=8) {
		log.println("waiting for 8");
		byte[] b = proto.expectBlock(8);
		log.println("got 8");
		cmd = (int)b[5];
		log.println("command is " + cmd);
		if (b[4]==(byte)'~') {
		    if (proto.getSize()>0 && cmd==0) {
			arg = proto.expectBlock(-1);
			if (cmd==0) {
			    cmd = ((int)arg[0]);
			    log.println("Subbing cmd " + cmd);
			}
		    }
		} else {
		    log.warning("Bad message");
		}
	    }
	    if (cmd!=0) {
		log.println("Got command [" + ((char)cmd) + "]");
		String src = NetType.netString(arg);
		ch = (char)cmd;
		str = src;
	    }
	}
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
		proto.appendString(str + "\n");
	    }
	}
    }

    public Object object() {
	if (ch!='\0') {
	    return "" + ch;
	} else {
	    return str;
	}
    }
}

