
Before you do anything else.
YARP assumes you have correctly set up the following variables:

YARP_ROOT=the complete path of the directory this file is in
PATH=$YARP_ROOT/bin/<OS> ...
where <OS> is one of qnx, winnt, linux 

While most of the code compiles correctly on different platforms,
there are exceptions (e.g. the device drivers).
Also we assumed:
-Microsoft Visual C++ on NT
-Watcom 10.6 on QNX
-gcc (?) on LINUX



pasa Jul 2001. Directory tree description.

src: YARP libraries source code
include: global YARP include files
lib: global YARP lib repositories
bin: executables. grabber, daemons, applications.
scripts: scripts repository
conf: configuration files.


within src:
	libraries: the new libraries.
	hardware.old: device drivers and daemons.
	motorcontrol.old: new but not YARPed yet motor control lib (head).
	applications: client apps to the YARP lib (source code).

within include:
	stl: standard template lib
	temporary: temp stuff to help porting to YARP
	sys: system strange stuff like the IPL headers (mei, framegrabber .h should perhaps go here)
	conf: configuration include files (sort of global defines?)

/usr/YARP/:
	more data and conf files.

/etc/yarp.conf:
	configuration file.


