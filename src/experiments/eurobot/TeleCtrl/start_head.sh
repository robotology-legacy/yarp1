#! /usr/bin/bash

# this script plainly connects the head, and uses then sendCmd
# to operate it. TeleCtrl is NOT involved, nor is the MASTER setup.

# safety measure: try and disconnect everything before anything else
yarp-connect /motorcmd/o !/repeater/i
yarp-connect /headcontrol/behavior/o !/repeater/i
yarp-connect /repeater/o !/headcontrol/behavior/i

pushd $HOME/DEVELOPMENT/yarp/scripts/eurobot

  # start head control & sendCmd (headcontrol + repeater)
  xterm -e repeater --name /repeater --protocol tcp &
  xterm -e su -c headcontrol &
  xterm -e sendCmd &

  # wait for user to press a key
  echo Press any key after you have entered all passwords and calibrated the head...
  read

  # connect headcontrol to repeater and sendcmd to repeater
  yarp-connect /repeater/o /headcontrol/behavior/i
  yarp-connect /headcontrol/behavior/o /repeater/i
  yarp-connect /motorcmd/o /repeater/i

popd
