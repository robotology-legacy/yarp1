#! /bin/sh

#
# Script to run the cameras
#	camerasrun <right> --> runs right camera
#	camerasrun <left>  --> runs left camera
#	camerasrun	   --> runs both cameras
# 	
#
#

Left()
{
	pterm su -c "on -f morpheus grabber --b 0 --name /left" &
}

Right()
{
	pterm su -c "on -f morpheus grabber --b 1 --name /right" &
}

All()
{
	pterm su -c "on -f morpheus grabber --b 0 --name /left -o -12" &
	pterm su -c "on -f morpheus grabber --b 1 --name /right -o 12" &
}

if [ "$1" ==  "" ]
then
	All
elif [ "$1" == "right" ]
then
	Right
elif [ "$1" == "left" ]
then
	Left
fi
