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
	pterm su -c "framegrab -b 0 +left -l -f" &
}

Right()
{
	pterm su -c "framegrab -b 1 +right -l -f" &
}

All()
{
	pterm su -c "framegrab -b 0 +left -l -f -o 4" &
	pterm su -c "framegrab -b 1 +right -l -f -o -4" &
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
