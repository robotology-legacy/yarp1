$Id: README.txt,v 1.1 2003-04-29 22:38:09 gmetta Exp $


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

1- UDP, MCAST code?

2- speed up by removing all reply to message issues (sort of mandatory for MCAST anyway).
2.1- also sending header + data in a single gather wide send should improve perf. especially
	since the Nagle's algorithm is disabled now.

3- what to do with the QNX4 code.

4- how to integrate the QNX6 code.

