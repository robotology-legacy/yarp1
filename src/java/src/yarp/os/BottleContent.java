
package yarp.os;

import java.io.*;

public class BottleContent implements Content, ContentCreator {
    private byte[] data;
    private Bottle bot = new Bottle();

    public int rawSize() {
	if (data==null) return 0;
	return data.length;
    }

    public Content create() {
	return new BottleContent();
    }
    public void read(Protocol proto) {
	try {
	    System.out.println("Bottle should read");
	    int len = proto.expectInt();
	    System.out.println("> name len is " + len);
	    byte[] b = proto.expectBlock(len);
	    System.out.println("> name is " + new String(b));
	    int dataLen = proto.expectInt();
	    System.out.println("> data len is " + dataLen);
	    data = proto.expectBlock(dataLen);
	    bot.set(data);

	} catch (IOException e) {
	    System.err.println("Problem reading bottle");
	}
    }
    public void write(Protocol proto) {	
	byte[] data = bot.get();
	String name = "void";
	int len = data.length;
	proto.addContent(NetType.netInt(name.length()+1));
	proto.addContent(NetType.netString(name));
	proto.addContent(NetType.netInt(len));
	proto.addContent(bot.get());
    }
    public void release() {
    }

    public Object content() {
	return bot;
    }
}

