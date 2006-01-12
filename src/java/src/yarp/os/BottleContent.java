
package yarp.os;

import java.io.*;

public class BottleContent implements Content {
    private static Logger log = Logger.get();

    private byte[] data;
    private Bottle bot = new Bottle();

    public int rawSize() {
	if (data==null) return 0;
	return data.length;
    }

    public Content create() {
	return new BottleContent();
    }

    public void read(BlockReader proto) throws IOException {
	log.println("Bottle should read");
	int len = proto.expectInt();
	log.println("> name len is " + len);
	byte[] b = proto.expectBlock(len);
	log.println("> name is " + new String(b));
	int dataLen = proto.expectInt();
	log.println("> data len is " + dataLen);
	data = proto.expectBlock(dataLen);
	bot.set(data);
    }

    public void write(BlockWriter proto) throws IOException {	
	byte[] data = bot.get();
	String name = "void";
	int len = data.length;
	proto.appendInt(name.length()+1);
	proto.appendString(name);
	proto.appendInt(len);
	proto.appendBlock(bot.get());
    }
    public void release() {
    }

    public Object object() {
	return bot;
    }
}

