#! /bin/sh

#
# this is a global script file for compiling YARP experiments
# options
#		<debug> compile DEBUG
#		<release> compile RELEASE, optimization ON
#		<clean>	clean everything, be careful.
# second argument -robot setup-
#		<eurobot>
#		<babybot>
#

EXECUTABLES="./armcontrol/
			 ./behaviorRnd/
			 ./headcontrol/
			 ./headsink/
			 ./headsmooth/
			 ./tracker/
			 ./headvergence/
			 ./attention/
			 ./teststereomatch/"	

All()
{
	for execut in $EXECUTABLES
	do
		echo "$PHRASE $execut"
		cd $execut

		if [ "$MODE" == "YARP_DEBUG=-g CFAST=" ]
		then
			make $MODE debug=1 optimize=0 $1
		else
			make $MODE debug=0 optimize=1 $1
		fi

		Check $?
		$INSTALL
		Check $?
		cd $YARP_ROOT/src/experiments/babybot/ 
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
	echo "A parameter is needed (debug, release, clean) + robotsetup"
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
	All $2

elif [ "$1" == "release" ]
then

	echo "release"
	MODE=""
	PHRASE="Building (release)"
	#install
	INSTALL="make install"
	All $2 

elif [ "$1" == "clean" ]
then

	echo "clean"
	MODE="clean"
	PHRASE="Cleaning"
	INSTALL=""
	All 

fi
