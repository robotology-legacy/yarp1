
package yarp.os;

import java.io.IOException;

// there's a problem with the content idea and immutable types

class IntegerContent implements Content {
    static private Logger log = Logger.get();

    public Integer content = new Integer(0);

    public IntegerContent() {}

    public IntegerContent(int x) {
	content = new Integer(x);
    }

    public Content create() {
	return new IntegerContent();
    }
    public void read(BlockReader proto) throws IOException {
	log.println("*** reading integer");
	content = new Integer(proto.expectInt());
	log.println("*** read integer " + content);
    }
    public void write(BlockWriter proto) throws IOException {
	log.println("*** writing integer");
	proto.appendInt(content.intValue());
	log.println("*** wrote integer " + content);
    }
    public Object object() {
	return content;
    }
}

