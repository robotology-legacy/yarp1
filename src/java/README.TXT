
This is the beginnings of a native Java interface to YARP.
The TCP protocol works and is interoperable with C++ YARP2.
The Bottle object is interoperable with C++ YARP2 but just 
can contain integers and strings.

The MCAST/UDP implementations have not been updated to the current YARP2 
standard. This code is after getting a bit out of date, sorry!

-paulfitz

======================================================================

Requirements:

  some form of java that support 1.4.2 or greater - gcj works, sun works, ...
  ant
  junit (if you don't want this, delete all ClassnameTest.java files)

======================================================================

To build the project, type:
  ant
[if you are on a debian machine, you can apt-get install ant (it is
in contrib)]

Strangely, there is a makefile for calling ant, and running different 
things.

It is simplest if yarp-service is running.  Then in one terminal you
could do:
  make write
in another
  make read
and in another
  make connect

Then things typed in the first terminal should appear in the second.

All this is basically the same as:
  yarp-write /write
  yarp-read /read
  yarp-connect /write /read

======================================================================

The code can also be compiled if you have gcj installed (on debian,
type "apt-get install gcj" -- make sure it is at least version 4).

Type:
  make gcj

Then the executable should appear, named "jyarp"
