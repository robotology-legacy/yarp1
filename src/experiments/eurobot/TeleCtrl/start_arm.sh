#! /usr/bin/bash

# this script plainly connects the arm, and uses then sendCmd
# to operate it. TeleCtrl is NOT involved, nor is the MASTER setup.

# safety measure: try and disconnect everything before anything else
yarp-connect /motorcmd/o !/repeater/i
yarp-connect /armcontrol/behavior/o !/repeater/i
yarp-connect /repeater/o !/armcontrol/behavior/i

pushd $HOME/DEVELOPMENT/yarp/scripts/eurobot

  # start head control & sendCmd (headcontrol + repeater)
  xterm -e repeater --name /repeater --protocol tcp &
  xterm -e su -c "on -f danae armcontrol" &
  xterm -e sendCmd &

  # wait for user to press a key
  echo Press any key after you have entered all passwords...
  read

  # connect armcontrol to repeater and sendcmd to repeater
  yarp-connect /repeater/o /armcontrol/behavior/i
  yarp-connect /armcontrol/behavior/o /repeater/i
  yarp-connect /motorcmd/o /repeater/i

popd
