This directory contains some experimental modules for the obrero robot.

=========================================================================
=========================================================================
=========================================================================
=== MACHINES INVOLVED
===

"obrero" is currently associated with the following machines:

  obrero.csail.mit.edu
    A debian laptop
    Hand control runs here (module "touch")
    
  conspire1.csail.mit.edu
    A debian desktop
    Auditory processing runs here (module "listen")
    Perceptual integration runs here (module "collisionDetector")
    Behavioral integration runs here (module "collisionCreator")

  conspire2.csail.mit.edu
    A debian desktop
    YARP name service runs here (module "yarp-service")
    Arm control runs here (module "sweep")

  conspire0.csail.mit.edu
    A Mac mini
    L/Mars will run here


=========================================================================
=========================================================================
=========================================================================
=== MESSAGE FORMAT
===

Messages between modules right now are very simple.

MESSAGES:

 expect integer ID
   // COLLISION MEASURES
   ID=0 expect a float (sound)
   ID=1 expect a float (tactile)
        expect a float (finger force thumb "middle sensor (not tip)")
        expect a float (finger force index "middle sensor (not tip)")
        expect a float (finger force middle "middle sensor (not tip)")
   ID=2 <not used>

   // ARM COMMAND
   ID=3 expect integer VAL
        VAL=0 nothing
        VAL=1 arm should move
        VAL=2 arm should stop
        VAL=3 arm should go up to ready position (and wait for 1)
	  VAL=4 arm should go back to resting position

   // COLLISION DETECTION
   ID=5 expect integer VAL
        VAL=0 nothing
        VAL=100 collision may have occured within last few seconds

   // HAND COMMAND
   ID=6 expect integer Cmd
        Cmd=0  nothing
        Cmd=1  Thumb up
	Cmd=2  Start Tapping
	Cmd=3  Stop Tapping

=========================================================================
=========================================================================
=========================================================================
=== MODULES
===

Suggested order in which to start modules:
  yarp-service
  touch
  listen
  collisionDetector
  collisionCreator
  connections

*************************************************************************
*** MODULE "touch"

This module interfaces with the hand.
On OBRERO (the laptop), do:
  cd /afs/csail.mit.edu/group/lbr/platform/yarp/src/experiments/obrero
  cd touch
  ./ReadGUI --yarp

This program hangs every time the robot is power cycled, and must
be restarted by hand.


*************************************************************************
*** MODULE "listen"

This module interfaces with the microphone on the camcorder.
On CONSPIRE1, do:
  cd /afs/csail.mit.edu/group/lbr/platform/yarp/src/experiments/obrero
  cd listen
  ./dvgrab-shell

This program hangs every time the camera is power cycled, and must
be restarted by hand.


*************************************************************************
*** MODULE "collisionDetector"

This module collects perceptual data.
On CONSPIRE1, do:
  cd /afs/csail.mit.edu/group/lbr/platform/yarp/src/experiments/obrero
  cd collisionDetector
  ../bin/linux/collisionDetector

In a separate terminal, do:
  cd /afs/csail.mit.edu/group/lbr/platform/yarp/src/experiments/obrero
  ./connect
to connect modules "listen" and "touch" to this one.


*************************************************************************
*** MODULE "collisionCreator"

This module coordinates action.
On CONSPIRE1, do:
  cd /afs/csail.mit.edu/group/lbr/platform/yarp/src/experiments/obrero
  cd collisionCreator
  ../bin/linux/collisionCreator

In a separate terminal, do:
  cd /afs/csail.mit.edu/group/lbr/platform/yarp/src/experiments/obrero
  ./connect
  ./connect-arm
to connect modules "listen" and "touch" and "collisionDetector" and
"sweep".



*************************************************************************
*** MODULE "sweep"

This module controls the arm.  Currently runs from Lorenzo's repository.
Must set YARP_ROOT environment variable as follows:
  setenv YARP_ROOT ~lorenzo/work/yarp
  [or export YARP_ROOT=~lorenzo/work/yarp]
Source code is in:
  cd $YARP_ROOT/src/experiments/obrero/sweep
Compile, and run on CONSPIRE2 as:
  ../bin/linux/sweep
(remember that YARP_ROOT variable must be set)

A configuration file for this module is in $YARP_ROOT/conf/sweep.cfg


*************************************************************************
*** MODULE "yarp-service"

This module manages the YARP name space.
On CONSPIRE2, run:
  yarp-service --run
On another terminal, run:
  yarp-service --check
If this eventually reports "YARP seems okay" then things are good.

If you have problems, compare this file:
  $HOME/.yarp/conf/namer.conf
with this file:
  ~paulfitz/.yarp/conf/namer.conf
If they are different, copy the second one to the first one.
Alternatively, set your $YARP_ROOT variable to:
  setenv YARP_ROOT ~lorenzo/work/yarp
  [or export YARP_ROOT=~lorenzo/work/yarp]


*************************************************************************
*** MODULE "connections"

This isn't a real module - just a place to document how to
connect everything.

  cd /afs/csail.mit.edu/group/lbr/platform/yarp/src/experiments/obrero
  ./connect
  ./connect-arm

Should do the job.  You can look at the connect and connect-arm
scripts to see how to connect things piece by piece.
