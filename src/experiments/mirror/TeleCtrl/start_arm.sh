#! /usr/bin/bash

# this script plainly connects the arm, and uses then sendCmd
# to operate it. TeleCtrl is NOT involved, nor is the MASTER setup.

# safety measure: silently try and disconnect everything before anything else
yarp-connect /motorcmd/o !/repeater/i >/dev/null 2>&1
yarp-connect /armcontrol/behavior/o !/repeater/i >/dev/null 2>&1
yarp-connect /repeater/o !/armcontrol/behavior/i >/dev/null 2>&1

pushd $HOME/DEVELOPMENT/yarp/scripts/eurobot >/dev/null 2>&1

  # start head control & sendCmd (headcontrol + repeater)
  su -c "xterm -title ARMCONTROL -geometry 80x50-0+0 -e on -f danae armcontrol &"
  xterm -title REPEATER -geometry 80x24+0+0 -e repeater --name /repeater --protocol tcp &
  xterm -title SENDCMD -geometry 80x24+0-0 -e sendCmd &

  sleep 5

  # connect armcontrol to repeater and sendcmd to repeater
  yarp-connect /repeater/o /armcontrol/behavior/i
  yarp-connect /armcontrol/behavior/o /repeater/i
  yarp-connect /motorcmd/o /repeater/i

popd >/dev/null 2>&1

echo
echo Connections made.
