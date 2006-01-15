
package yarp.os;

import java.io.*;
import java.util.*;
import java.util.regex.*;

public class BottleContent implements Content {
    private static Logger log = Logger.get();

    private byte[] data;
    private Bottle bot = new Bottle();

    private int rawSize() {
	if (data==null) return 0;
	return data.length;
    }

    public Content create() {
	return new BottleContent();
    }

    public void read(BlockReader proto) throws IOException {
	log.println("Bottle should read");
	if (!proto.isTextMode()) {
	    int len = proto.expectInt();
	    log.println("> name len is " + len);
	    byte[] b = proto.expectBlock(len);
	    log.println("> name is " + new String(b));
	    int dataLen = proto.expectInt();
	    log.println("> data len is " + dataLen);
	    data = proto.expectBlock(dataLen);
	    bot.set(data);
	} else {
	    // there is no requirement to have a special text mode 
	    // representation, but it is nice for Bottle
	    String line = proto.expectLine();
	    Pattern p = Pattern.compile(" ");
	    String[] parts = p.split(line); // in future need to handle quoting
	    bot.clear();
	    for (int i=0; i<parts.length; i++) {
		String str = parts[i];
		if (str.length()>0) {
		    char ch = str.charAt(0);
		    if (ch>='0'&&ch<='9') {
			bot.add(new Integer(Integer.valueOf(str).intValue()));
		    } else {
			bot.add(str);
		    }
		}
	    }
	    log.println("GOT bottle [" + bot + "]");
	}
    }

    public void write(BlockWriter proto) throws IOException {	
	byte[] data2 = bot.get();
	String name = "void";
	int len = data2.length;
	proto.appendInt(name.length()+1);
	proto.appendString(name);
	proto.appendInt(len);
	proto.appendBlock(bot.get());
    }

    public Object object() {
	return bot;
    }
}

