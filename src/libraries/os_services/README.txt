$Id: README.txt,v 1.33 2003-08-28 11:50:55 babybot Exp $


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

1.1- there's still a possibility for the connection to fail in MCAST. This is because
	multiple connection messages might overlap. One mightn't get accepted, and silently
	dropped. Not sure about this story!

1.2 - future improvement. What about using select() instead of having multiple receiving 
	threads? It would be a matter of rewriting the InputSocketMulti class!

1.4- use temp buffer on TCP communication, check efficiency (see UDP/MCAST model).

1.5- not all test SW compiles correctly?

1.6- remove the symbol UPDATED_PORT. Not really needed since the architecture changed a lot.

LINUX:
-- requires multi-if code!
-- MCAST troubles... not clear why.

-------------> 

7- can still get garbage if a recv fails, timeout or a connection is broken by terminating
	the sender. USE test12 to see this condition.

9- release version crashes on exit. USE test 13 to show this problem, and close the 
	receiver first.

===
0- QNET
	Destroying channel is not implemented (where? destructor?).

6- add windows DLL to the repository (these are the DLL's installed by visual C++).
	- ok, still need to check whether everything is there.

***********
