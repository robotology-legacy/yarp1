#! /bin/sh

#
# this is a global script file for compiling YARP services
# options
#		<debug> compile DEBUG
#		<release> compile RELEASE, optimization ON
#		<clean>	clean everything, be careful.
#
#

EXECUTABLES="./nameserver/YARPNameService/		
			./nameserver/nameclient/		
			./porter/porter/	
			./porter/conditional_porter		
			./repeater/"	

All()
{
	for execut in $EXECUTABLES
	do
		echo "$PHRASE $execut"
		cd $execut
		make $MODE
		Check $?
		$INSTALL
		Check $?
		cd $YARP_ROOT/src/maintenance/services/
	done
}

Check()
{
	if [ $1 -eq 0 ]         # Test exit status of "cmp" command.
	then
		echo "COMMAND SUCCESSFUL"
	else  
		echo "ERROR DETECTED"
		exit 0
	fi
}

Nothing()
{
	echo "A parameter is needed (debug, release, clean)"
}

echo "Entering build process of YARP libraries..."

if [ "$1" ==  "" ]
then
	Nothing
elif [ "$1" == "debug" ]
then
	echo "debug"
	MODE="YARP_DEBUG=-g CFAST="
	PHRASE="Building (debug)"
	#install option
	INSTALL="make install"
	All 
elif [ "$1" == "release" ]
then
	echo "release"
	MODE=""
	PHRASE="Building (release)"
	#install
	INSTALL="make install"
	All 
elif [ "$1" == "clean" ]
then
	echo "clean"
	MODE="clean"
	PHRASE="Cleaning"
	INSTALL=""
	All 
fi
