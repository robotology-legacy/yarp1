
package yarp.os;

public class BottleContent implements Content, ContentCreator {
    public Content create() {
	return new BottleContent();
    }
    public void read(Protocol proto) {
	// have to figure out bottle protocol again
    }
    public void write(Protocol proto) {
    }
    public void release() {
    }
}

