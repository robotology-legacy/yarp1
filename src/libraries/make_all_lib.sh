#! /bin/sh

#
# this is a global script file for compiling YARP libraries
# options
#		<debug> compile DEBUG
#		<release> compile RELEASE, optimization ON
#		<clean>	clean everything, be careful.
#
#             %2 == <full> clean ACE and copies IPL libs.
#
#

All ()
{
if [ "$1" == "full" ]
then
	echo "$PHRASE ACE DLL."
	cd ./ACE_wrappers/ace/
	make $MODE 
	make yarpize
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
$RUN_BUILD_TABLES
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

#is this already done in the images makefile?
#cd ./images/tools/
#make $MODE
#cd ../../

echo "$PHRASE utils library."
cd ./utils/
make $MODE
Check $?
$INSTALL
cd ../

echo "Going to "Device Drivers.""
cd ../hardware/src/
make $MODE
Check $?
cd ../../libraries/

echo "$MODE motorcontrol"
cd ./motorcontrol/
make $MODE
Check $?
$INSTALL
cd ../
}

Framegrabber()
{
echo "$PHRASE YARPNameService"
cd $YARP_ROOT/src/maintenance/services/nameserver/YARPNameService/
make $MODE
Check $?
make
Check $?
$INSTALL
Check $?
cd $YARP_ROOT/src/libraries

echo "$PHRASE Porter"
cd $YARP_ROOT/src/maintenance/services/porter/porter/
make $MODE
Check $?
make
Check $?
$INSTALL
Check $?
cd $YARP_ROOT/src/libraries

echo "$PHRASE Framegrabber"
cd $YARP_ROOT/src/hardware/daemons/framegrab/
make $MODE
Check $?
make $ROBOT
Check $?
$INSTALL
Check $?
cd $YARP_ROOT/src/libraries

echo "$PHRASE Viewer"
cd $YARP_ROOT/src/applications/viewers/camview/qnx6/
make $MODE
Check $?
make
Check $?
$INSTALL
Check $?
cd $YARP_ROOT/src/libraries
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
        MODE="debug"
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
elif [ "$1" == "framegrab" ]
then
        echo "Compiling Nameserver + porter + framegrabber + visualizer"
        MODE="clean"
        PHRASE="Compiling/installing"
        INSTALL="make install"
        INSTALL_IPL=""
        RUN_BUILD_TABLES=""
	ROBOT="eurobot"
        Framegrabber
fi

