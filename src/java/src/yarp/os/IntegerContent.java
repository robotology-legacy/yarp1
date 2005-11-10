
package yarp.os;

public class IntegerContent implements Content, ContentCreator {
    public Integer content = new Integer(0);
    public Content create() {
	return new IntegerContent();
    }
    public void read(Protocol proto) {
    }
    public void write(Protocol proto) {
    }
    public void release() {
    }
}

