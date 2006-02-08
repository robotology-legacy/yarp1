
===============================================================================

This will become, in time, a replacement for libYARP_OS, bringing that library 
into conformance with the YARP2 specification document available in draft form 
at:

  http://yarp0.sourceforge.net/specs/yarp-protocol-2-0.pdf

-- paulfitz, Genoa Italy, Thursday Jan 26, 2006

===============================================================================

DEVELOPER'S GUIDE:

This directory holds an almost-complete implementation of the YARP2 
specification, at:

  http://yarp0.sourceforge.net/specs/yarp-protocol-2-0.pdf

Here is an inventory:

  include/
    Header files for the implementation

  src/
    Source code for the implementation - compiles to libYARP_OS2.[a|lib]

  src-test/
    Unit tests - compile to libYARP_OS2_test.[a|lib]

  test/

    Contains a single application, which when called this way:
      "<application> regression [OPTIONAL-CLASS-NAME]"
    runs unit tests.

    Otherwise, the application follows the YARP2 specification for
    the yarp companion utility.

    In the remainder of this document, we assume this <application> is
    compiled to an executable called "yarp".  This is true currently
    on linux; on windows it currently compiles to "harness.exe"

YARP2 contains a partial implementation of YARP1 classes.  For simple
programs that link libYARP_OS2.[a|lib], they can simply link now 
against libYARP_OS.[a|lib] and (hopefully) continue to work
almost unchanged -- except all ports will now conform to YARP2.  
This is not well tested though.

For this YARP1 support, the YARP1 header files are needed.  If this
support is not desired, all source files that include YARP1 header
files can safely be removed (just update src-test/TestList.h to remove
compatibility tests).


===============================================================================

YARP1 to YARP2 - what is new?

The yarp network utility functions have been aggregated into a single
executable, "yarp" (currently called harness.exe on windows, sorry...)

Here we give the new equivalents of the command-line interface to yarp:

  "yarp-service --run" ...... "yarp server"

  "yarp-service --check" ...... "yarp check"

  "yarp-connect /source /target" ...... "yarp connect /source /target"

  "yarp-connect /source \!/target" ...... "yarp disconnect /source /target"

  "yarp-read /read" ....... "yarp read /read"

  "yarp-write /write [/read]" ....... "yarp write /write [/read]"

You need to have the name server running somewhere on your system,
as in YARP1.  If you don't set up a configuration file saying where
it is, or if that file is inaccurate, other programs using yarp
will scan for it using multi-cast.

In YARP1, the programmer chose the communication protocol when creating
a port.  In YARP2, this is a run-time property.  You can type:

  yarp connect /source /target udp
or
  yarp connect /source udp://target

to say that the connection to /target should use UDP.

In YARP1, the programmer chose the communication protocol when creating
a port.  In YARP2, this is a run-time property.  You can type:

  yarp connect /source udp://net=127/target

(or any unambiguous prefix like net=127.0.0.1, net=127.0 and so on)
to say that the connection to /target should be on the 127.0.0.1 network
interface.

You can also "log on" to ports and the name server, and rummage around
in them -- see the specification document.


===============================================================================

INTERNAL NOTES:

State of development:
  + tcp connections work - YARP1 compatible
  + udp connections work - YARP1 compatible
  + text connections work
  + port infrastructure
  + partial companion

Compatibility files:
  + src/YARPPort.cpp
  + src/YARPSemaphore.cpp
  + src/YARPTime.cpp
  + src/YARPThread.cpp
  + src-test/HeaderCompatibilityTest.cpp [listed in src-test/TestList.h]
give a little bit of YARPPort support.  YARP1 headers need to be available
for this to compile and run.

===============================================================================

IMPLEMENTATION OF PORTS AND CONNECTIONS

What are all these strange classes?  Carriers, Connections, Ports?
This section works through examples that are ONLY of interest
to developers and NOT users of the library.


CASE 1: Creating an output connection
=====================================

Suppose you want to communicate with an input port "/read" from your
code, and do it from scratch, using the implementation classes.

First thing you need to do is find the address of that port,
if you don't already know it.

   #include <yarp/NameClient.h>
   using namespace yarp;
   ...

   // get a client for talking to the yarp name server
   NameClient& nc = NameClient::getNameClient()
   Address addr = nc.queryName("/read");
   if (!addr.isValid()) { /* failure */ }

Once you have an address, you can connect as follows:

   #include <yarp/Carriers.h>
   ...

   OutputProtocol *out = Carriers::connect(addr);
   if (out == NULL) { /* failure */ }

Do you want to talk in tcp, mcast, udp, ...?  Pick one.
And pick a name to call yourself, say "external".
Then complete setting up the connection with:

   out->open(Route("external","/read","tcp"));

Now you can start to send some data.  For example,
a bottle message.

   #include <yarp/PortCommand.h>
   #include <yarp/Bottle.h>
   ...

   PortCommand pc('d',""); // data tag
   Bottle bot;
   bot.addInt(0);
   bot.addString("hello");
   BufferedBlockWriter writer(false);
   pc.writeBlock(writer);
   bot.writeBlock(writer);
   out->write(writer);

This will tell the receiving port that the message is "data",
and then pass the bottle content to it.  Now let's
close our connection:

   out->close();
   delete out;

The only class mentioned here that knows about specific protocols
is "Carriers".



CASE 2: Creating an input connection
====================================

The above example relied on having a port on the receiving end.
We could make a crude approximation to handle just this
single connection as follows.

First, we decide on an address to use.  We can let the
name server decide that instead:

   #include <yarp/NameClient.h>
   using namespace yarp;
   ...

   // get a client for talking to the yarp name server
   NameClient& nc = NameClient::getNameClient()
   Address addr = nc.registerName("/read");
   if (!addr.isValid()) { /* failure */ }

This will allocate an address for us if one doesn't already exist.
Now we can acquire a "Face", an object that will listen on that
address:

   #include <yarp/Carriers.h>
   ...

   Face *face = Carriers::listen(addr);
   if (face == NULL) { /* failure */ }

The Face will generally be a TcpFace, which listens to a 
specified socket.  We could have requested a particular kind
of protocol in our call to registerName.

Now, we read from the Face, which will sit and wait for a connection:

   InputProtocol *in = face->read();
   if (in == NULL) { /* failure */ }

Once we've got something, we need to initialize the connection as
follows:

   in->open("/read");

We pass along the name we've registered with.  This done, we can
start to actually read data.  Real ports are quite flexible in
what they read; we'll just deal with the output from Case 1 above:


   #include <yarp/PortCommand.h>
   #include <yarp/Bottle.h>
   ...

   PortCommand pc;
   Bottle bot;
   BlockReader& reader = in->beginRead();
   pc.readBlock(reader);
   bot.readBlock(reader);
   in->endRead();

Now pc and bot should have the values chosen by the sender in Case 1.
Now we can close everything down:

   in->close();
   delete in;
   face->close();
   delete face;

Note that we did not implement a full YARP2 compliant port this way.
The easiest way to do that is this way:

   PortCore core;
   core.listen(addr);
   core.start();

===============================================================================

