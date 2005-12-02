#! /usr/bin/bash

# safety measure: try and disconnect everything before anything else
# head
yarp-connect /repeater/o !/headcontrol/behavior/i
yarp-connect /headcontrol/behavior/o !/repeater/i
# arm
yarp-connect /armcontrol/behavior/o !/repeater/i
yarp-connect /repeater/o !/armcontrol/behavior/i
# eyes
yarp-connect /left/o:img !/lview/i:img
yarp-connect /right/o:img !/rview/i:img

pushd $HOME/DEVELOPMENT/yarp/scripts/eurobot

  # start repeater
  xterm -e repeater --name /repeater --protocol tcp &
  # start head control (headcontrol)
  xterm -e su -c headcontrol &
  # start arm control (armcontrol on danae)
  xterm -e su -c "on -f danae armcontrol" &
  # start eyes control (two instances of grabber, on different boards)
  xterm -e su -c "on -f morpheus grabber --b 0 --name /left -o -12 --protocol tcp" &
  xterm -e su -c "on -f morpheus grabber --b 1 --name /right -o 12 --protocol tcp" &

  # wait for user to press a key
  echo Press any key after you have entered all passwords and calibrated the head...
  read

  # connect headcontrol to repeater
  yarp-connect /repeater/o /headcontrol/behavior/i
  yarp-connect /headcontrol/behavior/o /repeater/i
  # connect armcontrol to repeater
  yarp-connect /repeater/o /armcontrol/behavior/i
  yarp-connect /armcontrol/behavior/o /repeater/i
  # connect grabbers to camviews
  yarp-connect /left/o:img /lview/i:img
  yarp-connect /right/o:img /rview/i:img

popd
