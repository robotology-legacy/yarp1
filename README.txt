$Id: README.txt,v 1.10 2003-04-10 15:01:24 gmetta Exp $

Before you do anything else.
YARP assumes you have correctly set up the following variables (ALSO WINNT):

YARP_ROOT=the complete path of the directory this file is in
PATH=$YARP_ROOT/bin/<OS> ...
where <OS> is one of qnx, qnx6, winnt, linux 


While most of the code compiles correctly on different platforms,
there are exceptions (e.g. the device drivers).
Also we assumed:
-Microsoft Visual C++ on NT
-Watcom 10.6 on QNX4.25
-gcc on QNX6
-gcc (?) on LINUX


There should be a least a README.TXT in each directory so we can check out the
directory structure leaving out the "deleted" directories. See CVS behavior.

There is a naming.txt file with a proposal for the naming conventions.


For historical reasons the YARP root is usually called YARP (some of
the applications do not really check $YARP_ROOT variable and assume
some weird absolute path.
The CVS repository is called yarp (lowercase). Temporary solution is 
to make a soft link (ln -s).



Globally defined sysmbols for conditional compilation:
They are defined in Makefile.template (which is included by local mkfiles)
and should be defined __WIN__ in VisualStudio project.

__WIN32__  identifies NT, sometimes mixed with WIN32 (what about __WIN__?)
__QNX__    identifies QNX 4.25, QNX 6 not supported yet
__QNX4__   perhaps this will turn out to be useful
__QNX6__   identifies QNX 6, under development
__LINUX__  linux, using gcc


In WIN32 project wide definitions like __WIN32__ is done by including
<conf/YARPConfig.h>, this in turn includes conf/YARPConfigWin32.h.
YARPConfig.h is not in the repository but simply obtained from 
$YARP_ROOT/conf/YARPConfigTemplate.h, copy it and modify.


pasa Jul 2001. Directory tree description.
pasa Apr 2003. 


src: YARP libraries source code
include: global YARP include files
lib: global YARP lib repositories
bin: executables. grabber, daemons, applications.
scripts: scripts repository
conf: configuration files.
docs: documentation repository.


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

$YARP_ROOT/include/ace
	ACE files are copied here after compiling the lib, also cpp are copied, some are
	required to provide implementation to template classes.


pasa Sep 2002. 
CVS repository, many things need to change.

pasa Nov 2002.


ACE:
	included into YARP at some low level. Libraries should compile and copy (install)
	under $YARP_ROOT/lib/$OS

	things are not completely sorted out under NT.
	include files for ACE are not copied under include:
	- add something like -I$YARP_ROOT/libraries/ACE_wrappers
	- include as #include <ace/xxxx.h>


ACE Apr 2003:
	ACE is not in the repository for licensing reasons.

	-ACE must be unzipped under the appropriate dir subdir of libraries
	-then check out the project files (winnt) and the config.h file which has been
	adapted to compile under YARP. Very minor change actually.

	- DONT FORGET, add path of the DLL to environment variables.
	- $YARP_ROOT/bin/winnt
