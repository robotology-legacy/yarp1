@echo off

rem
rem
rem this is a global script file for compiling YARP daemons
rem options
rem		<release> compile release
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem
rem
rem


echo Entering build process of YARP babybot experiments

if "%1"=="" goto end
if "%1"=="release" goto release
if "%1"=="debug" goto debug


:clean 
echo Cleaning all...

cd .\armcontrol\
msdev armcontrol.dsw /MAKE "armcontrol - Win32 Debug" /CLEAN
msdev armcontrol.dsw /MAKE "armcontrol - Win32 Release" /CLEAN
cd ..\armtrigger\
msdev armtrigger.dsw /MAKE "armtrigger - Win32 Debug" /CLEAN
msdev armtrigger.dsw /MAKE "armtrigger - Win32 Release" /CLEAN
cd ..\attention\
msdev attention.dsw /MAKE "attention - Win32 Debug" /CLEAN
msdev attention.dsw /MAKE "attention - Win32 Release" /CLEAN
cd ..\behaviorRnd\
msdev behaviorRnd.dsw /MAKE "behaviorRnd - Win32 Debug" /CLEAN
msdev behaviorRnd.dsw /MAKE "behaviorRnd - Win32 Release" /CLEAN
cd ..\bottlesender\
msdev bottlesender.dsw /MAKE "bottlesender - Win32 Debug" /CLEAN
msdev bottlesender.dsw /MAKE "bottlesender - Win32 Release" /CLEAN
cd ..\checkfixation\
msdev checkfixation.dsw /MAKE "checkfixation - Win32 Debug" /CLEAN
msdev checkfixation.dsw /MAKE "checkfixation - Win32 Release" /CLEAN
cd ..\collectpoints\
msdev collectpoints.dsw /MAKE "collectpoints - Win32 Debug" /CLEAN
msdev collectpoints.dsw /MAKE "collectpoints - Win32 Release" /CLEAN
cd ..\datacollector\
msdev datacollector.dsw /MAKE "datacollector - Win32 Debug" /CLEAN
msdev datacollector.dsw /MAKE "datacollector - Win32 Release" /CLEAN
cd ..\egomap\
msdev egomap.dsw /MAKE "egomap - Win32 Debug" /CLEAN
msdev egomap.dsw /MAKE "egomap - Win32 Release" /CLEAN
cd ..\grasprflx\
msdev grasprflx.dsw /MAKE "grasprflx - Win32 Debug" /CLEAN
msdev grasprflx.dsw /MAKE "grasprflx - Win32 Release" /CLEAN
cd ..\handcolortracker\
msdev HandColorTracker.dsw /MAKE "HandColorTracker - Win32 Debug" /CLEAN
msdev HandColorTracker.dsw /MAKE "HandColorTracker - Win32 Release" /CLEAN
cd ..\handcontrol\
msdev handcontrol.dsw /MAKE "handcontrol - Win32 Debug" /CLEAN
msdev handcontrol.dsw /MAKE "handcontrol - Win32 Release" /CLEAN
cd ..\handlocalization\
msdev handlocalization.dsw /MAKE "handlocalization - Win32 Debug" /CLEAN
msdev handlocalization.dsw /MAKE "handlocalization - Win32 Release" /CLEAN
cd ..\handtrackerswitch\
msdev handtrackerswitch.dsw /MAKE "handtrackerswitch - Win32 Debug" /CLEAN
msdev handtrackerswitch.dsw /MAKE "handtrackerswitch - Win32 Release" /CLEAN
cd ..\headcontrol\
msdev headcontrol.dsw /MAKE "headcontrol - Win32 Debug" /CLEAN
msdev headcontrol.dsw /MAKE "headcontrol - Win32 Release" /CLEAN
cd ..\headsaccades\
msdev headsaccades.dsw /MAKE "headsaccades - Win32 Debug" /CLEAN
msdev headsaccades.dsw /MAKE "headsaccades - Win32 Release" /CLEAN
cd ..\headsink\
msdev headsink.dsw /MAKE "headsink - Win32 Debug" /CLEAN
msdev headsink.dsw /MAKE "headsink - Win32 Release" /CLEAN
cd ..\headsmooth\
msdev headsmooth.dsw /MAKE "headsmooth - Win32 Debug" /CLEAN
msdev headsmooth.dsw /MAKE "headsmooth - Win32 Release" /CLEAN
cd ..\headvergence\
msdev headvergence.dsw /MAKE "headvergence - Win32 Debug" /CLEAN
msdev headvergence.dsw /MAKE "headvergence - Win32 Release" /CLEAN
cd ..\reaching\
msdev reaching.dsw /MAKE "reaching - Win32 Debug" /CLEAN
msdev reaching.dsw /MAKE "reaching - Win32 Release" /CLEAN
cd ..\remotelearn\
msdev remotelearn.dsw /MAKE "remotelearn - Win32 Debug" /CLEAN
msdev remotelearn.dsw /MAKE "remotelearn - Win32 Release" /CLEAN
cd ..\remotelearn\remotelearnclient
msdev remotelearnclient.dsw /MAKE "remotelearnclient - Win32 Debug" /CLEAN
msdev remotelearnclient.dsw /MAKE "remotelearnclient - Win32 Release" /CLEAN
cd ..
cd ..\sendCmd\
msdev sendCmd.dsw /MAKE "sendCmd - Win32 Debug" /CLEAN
msdev sendCmd.dsw /MAKE "sendCmd - Win32 Release" /CLEAN
cd ..\seqgrabber\
msdev seqgrabber.dsw /MAKE "seqgrabber - Win32 Debug" /CLEAN
msdev seqgrabber.dsw /MAKE "seqgrabber - Win32 Release" /CLEAN
cd ..\teststereomatch\
msdev teststereomatch.dsw /MAKE "teststereomatch - Win32 Debug" /CLEAN
msdev teststereomatch.dsw /MAKE "teststereomatch - Win32 Release" /CLEAN
cd ..\tracker\
msdev tracker.dsw /MAKE "tracker - Win32 Debug" /CLEAN
msdev tracker.dsw /MAKE "tracker - Win32 Release" /CLEAN
cd ..\vergence\
msdev vergence.dsw /MAKE "vergence - Win32 Debug" /CLEAN
msdev vergence.dsw /MAKE "vergence - Win32 Release" /CLEAN
cd ..\visualattention\
msdev visualattention.dsw /MAKE "visualattention - Win32 Debug" /CLEAN
msdev visualattention.dsw /MAKE "visualattention - Win32 Release" /CLEAN
cd ..\vor\
msdev vor.dsw /MAKE "vor - Win32 Debug" /CLEAN
msdev vor.dsw /MAKE "vor - Win32 Release" /CLEAN
cd..

goto end


:debug
echo Compiling all with debug enabled.

cd .\armcontrol\
msdev armcontrol.dsw /MAKE "armcontrol - Win32 Debug" /BUILD
cd ..\armtrigger\
msdev armtrigger.dsw /MAKE "armtrigger - Win32 Debug" /BUILD
cd ..\attention\
msdev attention.dsw /MAKE "attention - Win32 Debug" /BUILD
cd ..\behaviorRnd\
msdev behaviorRnd.dsw /MAKE "behaviorRnd - Win32 Debug" /BUILD
cd ..\bottlesender\
msdev bottlesender.dsw /MAKE "bottlesender - Win32 Debug" /BUILD
cd ..\checkfixation\
msdev checkfixation.dsw /MAKE "checkfixation - Win32 Debug" /BUILD
cd ..\collectpoints\
msdev collectpoints.dsw /MAKE "collectpoints - Win32 Debug" /BUILD
cd ..\datacollector\
msdev datacollector.dsw /MAKE "datacollector - Win32 Debug" /BUILD
cd ..\egomap\
msdev egomap.dsw /MAKE "egomap - Win32 Debug" /BUILD
cd ..\grasprflx\
msdev grasprflx.dsw /MAKE "grasprflx - Win32 Debug" /BUILD
cd ..\handcolortracker\
msdev HandColorTracker.dsw /MAKE "HandColorTracker - Win32 Debug" /BUILD
cd ..\handcontrol\
msdev handcontrol.dsw /MAKE "handcontrol - Win32 Debug" /BUILD
cd ..\handlocalization\
msdev handlocalization.dsw /MAKE "handlocalization - Win32 Debug" /BUILD
cd ..\handtrackerswitch\
msdev handtrackerswitch.dsw /MAKE "handtrackerswitch - Win32 Debug" /BUILD
cd ..\headcontrol\
msdev headcontrol.dsw /MAKE "headcontrol - Win32 Debug" /BUILD
cd ..\headsaccades\
msdev headsaccades.dsw /MAKE "headsaccades - Win32 Debug" /BUILD
cd ..\headsink\
msdev headsink.dsw /MAKE "headsink - Win32 Debug" /BUILD
cd ..\headsmooth\
msdev headsmooth.dsw /MAKE "headsmooth - Win32 Debug" /BUILD
cd ..\headvergence\
msdev headvergence.dsw /MAKE "headvergence - Win32 Debug" /BUILD
cd ..\reaching\
msdev reaching.dsw /MAKE "reaching - Win32 Debug" /BUILD
cd ..\remotelearn\
msdev remotelearn.dsw /MAKE "remotelearn - Win32 Debug" /BUILD
cd ..\remotelearn\remotelearnclient
msdev remotelearnclient.dsw /MAKE "remotelearnclient - Win32 Debug" /BUILD
cd ..
cd ..\sendCmd\
msdev sendCmd.dsw /MAKE "sendCmd - Win32 Debug" /BUILD
cd ..\seqgrabber\
msdev seqgrabber.dsw /MAKE "seqgrabber - Win32 Debug" /BUILD
cd ..\teststereomatch\
msdev teststereomatch.dsw /MAKE "teststereomatch - Win32 Debug" /BUILD
cd ..\tracker\
msdev tracker.dsw /MAKE "tracker - Win32 Debug" /BUILD
cd ..\vergence\
msdev vergence.dsw /MAKE "vergence - Win32 Debug" /BUILD
cd ..\visualattention\
msdev visualattention.dsw /MAKE "visualattention - Win32 Debug" /BUILD
cd ..\vor\
msdev vor.dsw /MAKE "vor - Win32 Debug" /BUILD
cd..

goto end

:release

echo Compiling all with RELEASE enabled.

cd .\armcontrol\
msdev armcontrol.dsw /MAKE "armcontrol - Win32 Release" /BUILD
cd ..\armtrigger\
msdev armtrigger.dsw /MAKE "armtrigger - Win32 Release" /BUILD
cd ..\attention\
msdev attention.dsw /MAKE "attention - Win32 Release" /BUILD
cd ..\behaviorRnd\
msdev behaviorRnd.dsw /MAKE "behaviorRnd - Win32 Release" /BUILD
cd ..\bottlesender\
msdev bottlesender.dsw /MAKE "bottlesender - Win32 Release" /BUILD
cd ..\checkfixation\
msdev checkfixation.dsw /MAKE "checkfixation - Win32 Release" /BUILD
cd ..\collectpoints\
msdev collectpoints.dsw /MAKE "collectpoints - Win32 Release" /BUILD
cd ..\datacollector\
msdev datacollector.dsw /MAKE "datacollector - Win32 Release" /BUILD
cd ..\egomap\
msdev egomap.dsw /MAKE "egomap - Win32 Release" /BUILD
cd ..\grasprflx\
msdev grasprflx.dsw /MAKE "grasprflx - Win32 Release" /BUILD
cd ..\handcolortracker\
msdev HandColorTracker.dsw /MAKE "HandColorTracker - Win32 Release" /BUILD
cd ..\handcontrol\
msdev handcontrol.dsw /MAKE "handcontrol - Win32 Release" /BUILD
cd ..\handlocalization\
msdev handlocalization.dsw /MAKE "handlocalization - Win32 Release" /BUILD
cd ..\handtrackerswitch\
msdev handtrackerswitch.dsw /MAKE "handtrackerswitch - Win32 Release" /BUILD
cd ..\headcontrol\
msdev headcontrol.dsw /MAKE "headcontrol - Win32 Release" /BUILD
cd ..\headsaccades\
msdev headsaccades.dsw /MAKE "headsaccades - Win32 Release" /BUILD
cd ..\headsink\
msdev headsink.dsw /MAKE "headsink - Win32 Release" /BUILD
cd ..\headsmooth\
msdev headsmooth.dsw /MAKE "headsmooth - Win32 Release" /BUILD
cd ..\headvergence\
msdev headvergence.dsw /MAKE "headvergence - Win32 Release" /BUILD
cd ..\reaching\
msdev reaching.dsw /MAKE "reaching - Win32 Release" /BUILD
cd ..\remotelearn\
msdev remotelearn.dsw /MAKE "remotelearn - Win32 Release" /BUILD
cd ..\remotelearn\remotelearnclient
msdev remotelearnclient.dsw /MAKE "remotelearnclient - Win32 Release" /BUILD
cd ..
cd ..\sendCmd\
msdev sendCmd.dsw /MAKE "sendCmd - Win32 Release" /BUILD
cd ..\seqgrabber\
msdev seqgrabber.dsw /MAKE "seqgrabber - Win32 Release" /BUILD
cd ..\teststereomatch\
msdev teststereomatch.dsw /MAKE "teststereomatch - Win32 Release" /BUILD
cd ..\tracker\
msdev tracker.dsw /MAKE "tracker - Win32 Release" /BUILD
cd ..\vergence\
msdev vergence.dsw /MAKE "vergence - Win32 Release" /BUILD
cd ..\visualattention\
msdev visualattention.dsw /MAKE "visualattention - Win32 Release" /BUILD
cd ..\vor\
msdev vor.dsw /MAKE "vor - Win32 Release" /BUILD
cd..

goto end

:end
echo Building process completed.
