$Id: README.txt,v 1.25 2003-07-29 02:26:52 gmetta Exp $


=============
- As of April 2003 the os_services library is built on top of ACE. It assumes
the ACE zip files is unzipped in $YARP_ROOT/src/libraries/ACE_wrappers

- ACE is compiled using the config files (and project) contained in the CVS repository

- Other things as usual, YARP_ROOT must be defined

- ACE is 5.3, april 2003.

- not everything is neat and clean because of the many versions the code 
  evolved through, it should hopefully be reasonably efficient.


=============
Things under development:

1- troubles when disconnecting/reconnecting mcast ports, grabber, etc. LORENZO!

1.1- remove long_timeout. Dangerous.

1.2- use temp buffer on socket communication, check efficiency (see UDP/MCAST model).

*****************>>>>>>>
-> Still problems on returning, many different issues.
-> Troubles with shared memory (crashes badly). 


1.3- proper shutdown of extra port threads.


3- initialization MFC+os_services might fail under some circumstances
	- solution: do explicit init and fini

7- can still get garbage if a recv fails, timeout or a connection is broken by terminating
	the sender. USE test12 to see this condition.

9- release version crashes on exit. USE test 13 to show this problem, and close the 
	receiver first.

10- IMPORTANT! specify the nic, for multiple parallel connections in MCAST.

11- missing htons here and there in the udp communication (fine w/ intel architecture).

13- noted a sigsegv on exit of the grabber application, perhaps while closing down the Port threads?
	- perhaps remove the SIGINT handling, it appears to upset the libc.

===
0- QNET
	Destroying channel is not implemented (where? destructor?).

3- what to do with the QNX4 code.

6- add windows DLL to the repository (these are the DLL's installed by visual C++).
	- ok, still need to check whether everything is there.

***********
