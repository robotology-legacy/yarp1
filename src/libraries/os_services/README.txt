$Id: README.txt,v 1.30 2003-08-07 04:23:43 gmetta Exp $


=============
- As of April 2003 the os_services library is built on top of ACE. It assumes
the ACE zip files is unzipped in $YARP_ROOT/src/libraries/ACE_wrappers

- ACE is compiled using the config files (and project) contained in the CVS repository

- Other things as usual, YARP_ROOT must be defined

- ACE is 5.3.3, july 2003.

- not everything is neat and clean because of the many versions the code 
  evolved through, it should hopefully be reasonably efficient.


=============
Things under development:

1.2.5- bugs in fg, thread stuff?

1.1- there's still a possibility for the connection to fail in MCAST. This is because
	multiple connection messages might overlap. One mightn't get accepted, and silently
	dropped. Not sure about this story!


1.2- still troubles when closing ports smoothly.
1.2.1- proper shutdown of extra port threads.
	--- HOW TO DO IT: self connect to socket and send a close MSG!
1.2.2- vector use (ACE_Array) try testing the max_size instead of the actual size to 
	avoid re-allocating memory if already allocated.


1.3- IMPORTANT! specify the nic, for multiple parallel connections in MCAST.

1.4- use temp buffer on TCP communication, check efficiency (see UDP/MCAST model).

1.5- not all test SW compiles correctly.

-------------> 

3- initialization MFC+os_services might fail under some circumstances
	- solution: do explicit init and fini

7- can still get garbage if a recv fails, timeout or a connection is broken by terminating
	the sender. USE test12 to see this condition.

9- release version crashes on exit. USE test 13 to show this problem, and close the 
	receiver first.

11- missing htons here and there in the udp communication (fine w/ intel architecture).

13- noted a sigsegv on exit of the grabber application, perhaps while closing down the Port threads?
	- perhaps remove the SIGINT handling, it appears to upset the libc.

===
0- QNET
	Destroying channel is not implemented (where? destructor?).

6- add windows DLL to the repository (these are the DLL's installed by visual C++).
	- ok, still need to check whether everything is there.

***********
