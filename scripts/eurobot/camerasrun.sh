#! /bin/sh

#
# Script to run the cameras
#	camerasrun <right> --> runs right camera
#	camerasrun <left>  --> runs left camera
#	camerasrun		   --> runs both cameras
# 	
#
#

Left()
{
	pterm su -c "on -f morpheus framegrab -b 0 +left -l -f" &
	pterm camview +view001 -l -w 256 -h 256 & 
}

Right()
{
	pterm su -c "on -f morpheus framegrab -b 1 +right -l -f" &
	pterm camview +view002 -l -w 256 -h 256 &  
}

All()
{
	pterm su -c "on -f morpheus framegrab -b 0 +left -l -f" &
	pterm su -c "on -f morpheus framegrab -b 1 +right -l -f" &

	pterm camview +view001 -l -w 256 -h 256 & 
	pterm camview +view002 -l -w 256 -h 256 &  
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
