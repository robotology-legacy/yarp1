#! /bin/sh

#
# this is a global script file for compiling YARP libraries
# options
#		<debug> compile DEBUG
#		<release> compile RELEASE, optimization ON
#		<clean>	clean everything, be careful.
#
#               %2 == <full> clean/build ACE.
#
#

All ()
{
if [ "$1" == "full" ]
then
	echo "$PHRASE ACE DLL."
	cd ./ACE_wrappers/ace/

	if [ "$MODE" == "YARP_DEBUG=-g CFAST=" ]
	then
		make debug=1 optimize=0
		make yarpize
	elif [ "$MODE" == "clean" ]
	then
		make clean
	else
		make optimize=1 debug=0
		make yarpize
	fi

# SPECIAL for ACE
	cd ../../
fi

echo "$PHRASE Math."
cd ./math/
make $MODE
Check $?
$INSTALL 
cd ../

echo "$PHRASE LogPolar library."
cd ./logpolar/
make $MODE
Check $?
$INSTALL 
cd ../

echo "$PHRASE BuildTables executable"
cd ./logpolar/BuildTablesSmall/
make $MODE
Check $?
$INSTALL
# $RUN_BUILD_TABLES
cd ../../

echo "$PHRASE os_services library"
cd ./os_services/
make $MODE
Check $?
$INSTALL
cd ../

echo "$PHRASE images libraries."
cd ./ipl/
$INSTALL_IPL
cd ../
cd ./images/
make $MODE
Check $?
$INSTALL
cd ../

echo "$PHRASE utils library."
cd ./utils/
make $MODE
Check $?
$INSTALL
cd ../

#
#
# No device drivers defined for Linux yet...
#
#
#echo "Going to "Device Drivers.""
#cd ../hardware/src/
#make $MODE
#Check $?
#cd ../../libraries/

#echo "$MODE motorcontrol"
#cd ./motorcontrol/
#make $MODE
#Check $?
#$INSTALL
#cd ../
#
#

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
echo "Use:" $0 "(debug, release, clean) [full]"
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
        INSTALL_IPL="./install_fake_ipl"
        RUN_BUILD_TABLES="./BuildTablesSmall $YARP_ROOT/conf/"
        All $2

elif [ "$1" == "release" ]
then

        echo "release"
        MODE=""
        PHRASE="Building (release)"
        #install
        INSTALL="make install"
        INSTALL_IPL="./install_fake_ipl"
        RUN_BUILD_TABLES="./BuildTablesSmall $YARP_ROOT/conf/"
        All $2

elif [ "$1" == "clean" ]
then

        echo "clean"
        MODE="clean"
        PHRASE="Cleaning"
        INSTALL=""
        INSTALL_IPL=""
        RUN_BUILD_TABLES=""
        All $2

fi

