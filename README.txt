$Id: README.txt,v 1.5 2002-09-23 20:45:09 gmetta Exp $

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



For historical reasons the YARP root is usually called YARP (some of
the applications do not really check $YARP_ROOT variable and assume
some weird absolute path.
The CVS repository is called yarp (lowercase). Temporary solution is 
to make a soft link (ln -s).



pasa Jul 2001. Directory tree description.

src: YARP libraries source code
include: global YARP include files
lib: global YARP lib repositories
bin: executables. grabber, daemons, applications.
scripts: scripts repository
conf: configuration files.


within src:
	libraries: the libraries.
	hardware: device drivers and daemons.
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


pasa Sep 2002. 
CVS repository, many things need to change.
