#! /bin/sh

#
# Script for cogvis experiment
#

Cam()
{
	pterm su -c "on -f morpheus framegrab -b 0 +left" &
	pterm su -c "on -f morpheus framegrab -b 1 +right" &
}

Arm()
{
	pterm su -c "on -f danae armcontrol" &
	pterm repeater -name repeater/ &
}
Connect()
{
	#images
	porter /left/o:img /leftimage:i 
	porter /right/o:img /rightimage:i 
	# arm staff
	porter /armcontrol/o:status /armstatus/i
	porter /motorcmd/o /repeater/i
	porter /repeater/o /armcontrol/behavior/i
	porter /armcontrol/behavior/o /repeater/i
}
Disconnect()
{
	#images
	porter /left/o:img !/leftimage:i 
	porter /right/o:img !/rightimage:i 
	# arm staff
	porter /armcontrol/o:status !/armstatus/i
	porter /motorcmd/o !/repeater/i
	porter /repeater/o !/armcontrol/behavior/i
	porter /armcontrol/behavior/o !/repeater/i
}

if [ "$1" ==  "" ]
then
	echo "Running Cam & Arm"
	Cam
	Arm
elif [ "$1" == "cam" ]
then
	Cam	
elif [ "$1" == "arm" ]
then
	Arm
elif [ "$1" == "connect" ]
then
	Connect
elif [ "$1" == "disconnect" ]
then
	Disconnect
fi
