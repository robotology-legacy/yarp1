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
if [ "%2" == "full" ]
then
	echo "$PHRASE ACE DLL."
	cd ./ACE_wrappers/ace/
	make $MODE 
	cd ../../
fi
echo "$PHRASE Math."
cd ./math/
make $MODE
$INSTALL 
cd ../
echo "$PHRASE LogPolar library."
cd ./logpolar/
make $MODE
$INSTALL 
cd ../
echo "$PHRASE BuildTables executable"
cd ./logpolar/BuildTablesSmall/
make $MODE
$RUN_BUILD_TABLES
cd ../../
echo "$PHRASE os_services library"
cd ./os_services/
make $MODE
$INSTALL
cd ../
echo "$PHRASE images libraries."
cd ./ipl/
$INSTALL_IPL
cd ../
cd ./images/
make $MODE
$INSTALL
cd ../
#is this already done in the images makefile?
#cd ./images/tools/
#make $MODE
#cd ../../
echo "$PHRASE utils library."
cd ./utils/
make $MODE
$INSTALL
cd ../
echo "Going to "Device Drivers.""
cd ../hardware/src/
make $MODE
cd ../../libraries/
echo "$MODE motorcontrol"
cd ./motorcontrol/
make $MODE
$INSTALL
cd ../
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
        All
elif [ "$1" == "release" ]
then
        echo "release"
        MODE=""
        PHRASE="Building (release)"
        #install
        INSTALL="make install"
        INSTALL_IPL="./install_fake_ipl"
        RUN_BUILD_TABLES="./BuildTablesSmall $YARP_ROOT/conf/"
        All
elif [ "$1" == "clean" ]
then
        echo "clean"
        MODE="clean"
        PHRASE="Cleaning"
        INSTALL=""
        INSTALL_IPL=""
        RUN_BUILD_TABLES=""
        All
fi

