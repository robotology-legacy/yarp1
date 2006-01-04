
package yarp.os;

import java.io.IOException;

public class IntegerContent implements Content {
    public Integer content = new Integer(0);
    public Content create() {
	return new IntegerContent();
    }
    public void read(BlockReader proto) throws IOException {
	content = new Integer(proto.expectInt());
    }
    public void write(BlockWriter proto) throws IOException {
	proto.appendInt(content.intValue());
    }
    public void release() {
    }
    public Object object() {
	return content;
    }
}

