$Id: README.txt,v 1.21 2003-07-06 23:25:45 gmetta Exp $


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


1- troubles when disconnecting/reconnecting mcast ports, grabber, etc.
1.1- check proper termination of connections/threads - too many complaints.
	- at least when the main thread is closing gracefully.
	- it might need to call an extra termination function and wait...
	- see also point 9.
1.2- something wrong when starting grabber + viewer on hermes, it tends to slow
	down over time (if connecting and disconnecting many times).

2- extra commands for the name server.

3- initialization MFC+os_services might fail under some circumstances
	- solution: do explicit init and fini

5- implement shared mem communication protocol.

6.2- reply messages, exploit iovec structure, maintaining compatibility w/ YARP multipart msg.

7- can still get garbage if a recv fails, timeout or a connection is broken by terminating
	the sender. USE test12 to see this condition.

9- release version crashes on exit. USE test 13 to show this problem, and close the 
	receiver first.

10- IMPORTANT! specify the nic, for multiple parallel connections in MCAST.

11- missing htons here and there in the udp communication (fine w/ intel architecture).

12- check out from name server.

13- noted a sigsegv on exit of the grabber application, perhaps while closing down the Port threads?
	- perhaps remove the SIGINT handling, it appears to upset the libc.

===
0- QNET
	Destroying channel is not implemented (where? destructor?).

0.5- implement check_out from name server.


3- what to do with the QNX4 code.

5- return error if trying a connection on the wrong protocol.

6- add windows DLL to the repository (these are the DLL's installed by visual C++).

***********
