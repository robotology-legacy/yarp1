
package yarp.os;

import java.io.*;

public class BottleContent implements Content {
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
	System.out.println("Bottle should read");
	int len = proto.expectInt();
	System.out.println("> name len is " + len);
	byte[] b = proto.expectBlock(len);
	System.out.println("> name is " + new String(b));
	int dataLen = proto.expectInt();
	System.out.println("> data len is " + dataLen);
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

