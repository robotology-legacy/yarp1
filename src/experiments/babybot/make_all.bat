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
nmake /f armcontrol.mak CFG="armcontrol - Win32 Debug" clean
nmake /f armcontrol.mak CFG="armcontrol - Win32 Release" clean
cd ..\armtrigger\
nmake /f armtrigger.mak CFG="armtrigger - Win32 Debug" clean
nmake /f armtrigger.mak CFG="armtrigger - Win32 Release" clean
cd ..\attention\
nmake /f attention.mak CFG="attention - Win32 Debug" clean
nmake /f attention.mak CFG="attention - Win32 Release" clean
cd ..\behaviorRnd\
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Debug" clean
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Release" clean
cd ..\bottlesender\
nmake /f bottlesender.mak CFG="bottlesender - Win32 Debug" clean
nmake /f bottlesender.mak CFG="bottlesender - Win32 Release" clean
cd ..\checkfixation\
nmake /f checkfixation.mak CFG="checkfixation - Win32 Debug" clean
nmake /f checkfixation.mak CFG="checkfixation - Win32 Release" clean
cd ..\collectpoints\
nmake /f collectpoints.mak CFG="collectpoints - Win32 Debug" clean
nmake /f collectpoints.mak CFG="collectpoints - Win32 Release" clean
cd ..\datacollector\
nmake /f datacollector.mak CFG="datacollector - Win32 Debug" clean
nmake /f datacollector.mak CFG="datacollector - Win32 Release" clean
cd ..\egomap\
nmake /f egomap.mak CFG="egomap - Win32 Debug" clean
nmake /f egomap.mak CFG="egomap - Win32 Release" clean
cd ..\grasprflx\
nmake /f grasprflx.mak CFG="grasprflx - Win32 Debug" clean
nmake /f grasprflx.mak CFG="grasprflx - Win32 Release" clean
cd ..\handcolortracker\
nmake /f HandColorTracker.mak CFG="HandColorTracker - Win32 Debug" clean
nmake /f HandColorTracker.mak CFG="HandColorTracker - Win32 Release" clean
cd ..\handcontrol\
nmake /f handcontrol.mak CFG="handcontrol - Win32 Debug" clean
nmake /f handcontrol.mak CFG="handcontrol - Win32 Release" clean
cd ..\handlocalization\
nmake /f handlocalization.mak CFG="handlocalization - Win32 Debug" clean
nmake /f handlocalization.mak CFG="handlocalization - Win32 Release" clean
cd ..\handtrackerswitch\
nmake /f handtrackerswitch.mak CFG="handtrackerswitch - Win32 Debug" clean
nmake /f handtrackerswitch.mak CFG="handtrackerswitch - Win32 Release" clean
cd ..\headcontrol\
nmake /f headcontrol.mak CFG="headcontrol - Win32 Debug" clean
nmake /f headcontrol.mak CFG="headcontrol - Win32 Release" clean
cd ..\headsaccades\
nmake /f headsaccades.mak CFG="headsaccades - Win32 Debug" clean
nmake /f headsaccades.mak CFG="headsaccades - Win32 Release" clean
cd ..\headsink\
nmake /f headsink.mak CFG="headsink - Win32 Debug" clean
nmake /f headsink.mak CFG="headsink - Win32 Release" clean
cd ..\headsmooth\
nmake /f headsmooth.mak CFG="headsmooth - Win32 Debug" clean
nmake /f headsmooth.mak CFG="headsmooth - Win32 Release" clean
cd ..\headvergence\
nmake /f headvergence.mak CFG="headvergence - Win32 Debug" clean
nmake /f headvergence.mak CFG="headvergence - Win32 Release" clean
cd ..\reaching\
nmake /f reaching.mak CFG="reaching - Win32 Debug" clean
nmake /f reaching.mak CFG="reaching - Win32 Release" clean
cd ..\remotelearn\
nmake /f remotelearn.mak CFG="remotelearn - Win32 Debug" clean
nmake /f remotelearn.mak CFG="remotelearn - Win32 Release" clean
cd ..\remotelearn\remotelearnclient
nmake /f remotelearnclient.mak CFG="remotelearnclient - Win32 Debug" clean
nmake /f remotelearnclient.mak CFG="remotelearnclient - Win32 Release" clean
cd ..
cd ..\sendCmd\
nmake /f sendCmd.mak CFG="sendCmd - Win32 Debug" clean
nmake /f sendCmd.mak CFG="sendCmd - Win32 Release" clean
cd ..\seqgrabber\
nmake /f seqgrabber.mak CFG="seqgrabber - Win32 Debug" clean
nmake /f seqgrabber.mak CFG="seqgrabber - Win32 Release" clean
cd ..\teststereomatch\
nmake /f teststereomatch.mak CFG="teststereomatch - Win32 Debug" clean
nmake /f teststereomatch.mak CFG="teststereomatch - Win32 Release" clean
cd ..\tracker\
nmake /f tracker.mak CFG="tracker - Win32 Debug" clean
nmake /f tracker.mak CFG="tracker - Win32 Release" clean
cd ..\vergence\
nmake /f vergence.mak CFG="vergence - Win32 Debug" clean
nmake /f vergence.mak CFG="vergence - Win32 Release" clean
cd ..\visualattention\
nmake /f visualattention.mak CFG="visualattention - Win32 Debug" clean
nmake /f visualattention.mak CFG="visualattention - Win32 Release" clean
cd ..\vor\
nmake /f vor.mak CFG="vor - Win32 Debug" clean
nmake /f vor.mak CFG="vor - Win32 Release" clean
cd..

goto end


:debug
echo Compiling all with debug enabled.

cd .\armcontrol\
nmake /f armcontrol.mak CFG="armcontrol - Win32 Debug"
cd ..\armtrigger\
nmake /f armtrigger.mak CFG="armtrigger - Win32 Debug"
cd ..\attention\
nmake /f attention.mak CFG="attention - Win32 Debug"
cd ..\behaviorRnd\
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Debug"
cd ..\bottlesender\
nmake /f bottlesender.mak CFG="bottlesender - Win32 Debug"
cd ..\checkfixation\
nmake /f checkfixation.mak CFG="checkfixation - Win32 Debug"
cd ..\collectpoints\
nmake /f collectpoints.mak CFG="collectpoints - Win32 Debug"
cd ..\datacollector\
nmake /f datacollector.mak CFG="datacollector - Win32 Debug"
cd ..\egomap\
nmake /f egomap.mak CFG="egomap - Win32 Debug"
cd ..\grasprflx\
nmake /f grasprflx.mak CFG="grasprflx - Win32 Debug"
cd ..\handcolortracker\
nmake /f HandColorTracker.mak CFG="HandColorTracker - Win32 Debug"
cd ..\handcontrol\
nmake /f handcontrol.mak CFG="handcontrol - Win32 Debug"
cd ..\handlocalization\
nmake /f handlocalization.mak CFG="handlocalization - Win32 Debug"
cd ..\handtrackerswitch\
nmake /f handtrackerswitch.mak CFG="handtrackerswitch - Win32 Debug"
cd ..\headcontrol\
nmake /f headcontrol.mak CFG="headcontrol - Win32 Debug"
cd ..\headsaccades\
nmake /f headsaccades.mak CFG="headsaccades - Win32 Debug"
cd ..\headsink\
nmake /f headsink.mak CFG="headsink - Win32 Debug"
cd ..\headsmooth\
nmake /f headsmooth.mak CFG="headsmooth - Win32 Debug"
cd ..\headvergence\
nmake /f headvergence.mak CFG="headvergence - Win32 Debug"
cd ..\reaching\
nmake /f reaching.mak CFG="reaching - Win32 Debug"
cd ..\remotelearn\
nmake /f remotelearn.mak CFG="remotelearn - Win32 Debug"
cd ..\remotelearn\remotelearnclient
nmake /f remotelearnclient.mak CFG="remotelearnclient - Win32 Debug"
cd ..
cd ..\sendCmd\
nmake /f sendCmd.mak CFG="sendCmd - Win32 Debug"
cd ..\seqgrabber\
nmake /f seqgrabber.mak CFG="seqgrabber - Win32 Debug"
cd ..\teststereomatch\
nmake /f teststereomatch.mak CFG="teststereomatch - Win32 Debug"
cd ..\tracker\
nmake /f tracker.mak CFG="tracker - Win32 Debug"
cd ..\vergence\
nmake /f vergence.mak CFG="vergence - Win32 Debug"
cd ..\visualattention\
nmake /f visualattention.mak CFG="visualattention - Win32 Debug"
cd ..\vor\
nmake /f vor.mak CFG="vor - Win32 Debug"
cd..

goto end

:release

echo Compiling all with RELEASE enabled.

cd .\armcontrol\
nmake /f armcontrol.mak CFG="armcontrol - Win32 Release"
cd ..\armtrigger\
nmake /f armtrigger.mak CFG="armtrigger - Win32 Release"
cd ..\attention\
nmake /f attention.mak CFG="attention - Win32 Release"
cd ..\behaviorRnd\
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Release"
cd ..\bottlesender\
nmake /f bottlesender.mak CFG="bottlesender - Win32 Release"
cd ..\checkfixation\
nmake /f checkfixation.mak CFG="checkfixation - Win32 Release"
cd ..\collectpoints\
nmake /f collectpoints.mak CFG="collectpoints - Win32 Release"
cd ..\datacollector\
nmake /f datacollector.mak CFG="datacollector - Win32 Release"
cd ..\egomap\
nmake /f egomap.mak CFG="egomap - Win32 Release"
cd ..\grasprflx\
nmake /f grasprflx.mak CFG="grasprflx - Win32 Release"
cd ..\handcolortracker\
nmake /f HandColorTracker.mak CFG="HandColorTracker - Win32 Release"
cd ..\handcontrol\
nmake /f handcontrol.mak CFG="handcontrol - Win32 Release"
cd ..\handlocalization\
nmake /f handlocalization.mak CFG="handlocalization - Win32 Release"
cd ..\handtrackerswitch\
nmake /f handtrackerswitch.mak CFG="handtrackerswitch - Win32 Release"
cd ..\headcontrol\
nmake /f headcontrol.mak CFG="headcontrol - Win32 Release"
cd ..\headsaccades\
nmake /f headsaccades.mak CFG="headsaccades - Win32 Release"
cd ..\headsink\
nmake /f headsink.mak CFG="headsink - Win32 Release"
cd ..\headsmooth\
nmake /f headsmooth.mak CFG="headsmooth - Win32 Release"
cd ..\headvergence\
nmake /f headvergence.mak CFG="headvergence - Win32 Release"
cd ..\reaching\
nmake /f reaching.mak CFG="reaching - Win32 Release"
cd ..\remotelearn\
nmake /f remotelearn.mak CFG="remotelearn - Win32 Release"
cd ..\remotelearn\remotelearnclient
nmake /f remotelearnclient.mak CFG="remotelearnclient - Win32 Release"
cd ..
cd ..\sendCmd\
nmake /f sendCmd.mak CFG="sendCmd - Win32 Release"
cd ..\seqgrabber\
nmake /f seqgrabber.mak CFG="seqgrabber - Win32 Release"
cd ..\teststereomatch\
nmake /f teststereomatch.mak CFG="teststereomatch - Win32 Release"
cd ..\tracker\
nmake /f tracker.mak CFG="tracker - Win32 Release"
cd ..\vergence\
nmake /f vergence.mak CFG="vergence - Win32 Release"
cd ..\visualattention\
nmake /f visualattention.mak CFG="visualattention - Win32 Release"
cd ..\vor\
nmake /f vor.mak CFG="vor - Win32 Release"
cd..

goto end

:end
echo Building process completed.
