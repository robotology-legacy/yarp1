
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

State of development:
  + tcp connections work - YARP1 compatible
  + udp connections work - YARP1 compatible
  + text connections work
  + port infrastructure
  + partial companion

Compatibility files:
  + src/YARPPort.cpp
  + src-test/HeaderCompatibilityTest.cpp [listed in src-test/TestList.h]
give a little bit of YARPPort support.  YARP1 headers need to be available
for this to compile and run.

===============================================================================
