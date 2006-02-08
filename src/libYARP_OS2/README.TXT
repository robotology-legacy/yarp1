
===============================================================================

This library is under development and should not be used.

I repeat: don't use it.  Move along.  There's nothing to see here.  These are 
not the droids you're looking for.

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
