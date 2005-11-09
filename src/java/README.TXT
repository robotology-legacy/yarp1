
This is the beginnings of a Java interface to YARP.  It is not yet
functional.  However, already it is probably a better source for
understanding the basic communication protocol than the C++ code.  Only
the TCP component of the protocol has been touched on so far though.

Hopefully we will write the protocol down explicitly at some point :-)

-paulfitz

======================================================================

To build the project, type:
  ant
[if you are on a debian machine, you can apt-get install ant (it is
in contrib)]

Then you can run it with:
  java -jar dist/lib/*.jar

Not that much will happen right now if you do this.

