$Id: README.txt,v 1.11 2003-05-31 06:31:38 gmetta Exp $


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

1- remove old grabber stuff.

7- can still get garbage if a recv fails, timeout or a connection is broken by terminating
	the sender. USE test12 to see this condition.

9- release version crashes on exit. USE test 13 to show this problem, and close the 
	receiver first.

10- IMPORTANT! specify the nic, for multiple parallel connections in MCAST.

11- missing htons here and there in the udp communication (fine w/ intel architecture).

===
0- QNET
	Destroying channel is not implemented (where? destructor?).

0.5- implement check_out from name server.


3- what to do with the QNX4 code.

5- return error if trying a connection on the wrong protocol.

6- add windows DLL to the repository (these are the DLL's installed by visual C++).

***********
7- CRAZY but GORGEOUS, multiple protocols from the same port. ACE allows also streams in
	shared memory (very efficient).