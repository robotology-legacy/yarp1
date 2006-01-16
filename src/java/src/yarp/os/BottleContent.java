
package yarp.os;

import java.io.*;

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
	    bot.fromString(line);
	    log.println("GOT bottle [" + bot + "]");
	}
    }

    public void write(BlockWriter proto) throws IOException {	
	if (!proto.isTextMode()) {
	    byte[] data2 = bot.get();
	    String name = "void";
	    int len = data2.length;
	    proto.appendInt(name.length()+1);
	    proto.appendString(name);
	    proto.appendInt(len);
	    proto.appendBlock(bot.get());
	} else {
	    proto.appendString(bot.toString()+"\n");
	}
    }

    public Object object() {
	return bot;
    }
}

