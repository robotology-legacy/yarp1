
package yarp.os;

abstract class Portlet extends Thread {

    public abstract void close();

    public abstract void run();

}
