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
cd ..\behaviorRnd\
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Debug" clean
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Release" clean
cd ..\handcolortracker\
nmake /f HandColorTracker.mak CFG="HandColorTracker - Win32 Debug" clean
nmake /f HandColorTracker.mak CFG="HandColorTracker - Win32 Release" clean
cd ..\handcontrol\
nmake /f handcontrol.mak CFG="handcontrol - Win32 Debug" clean
nmake /f handcontrol.mak CFG="handcontrol - Win32 Release" clean
cd ..\handlocalization\
nmake /f handlocalization.mak CFG="handlocalization - Win32 Debug" clean
nmake /f handlocalization.mak CFG="handlocalization - Win32 Release" clean
cd ..\sendCmd\
nmake /f sendCmd.mak CFG="sendCmd - Win32 Debug" clean
nmake /f sendCmd.mak CFG="sendCmd - Win32 Release" clean
cd ..\grasprflx\
nmake /f grasprflx.mak CFG="grasprflx - Win32 Debug" clean
nmake /f grasprflx.mak CFG="grasprflx - Win32 Release" clean
cd ..\

goto end


:debug
echo Compiling all with debug enabled.

cd .\armcontrol\
nmake /f armcontrol.mak CFG="armcontrol - Win32 Debug"
cd ..\behaviorRnd\
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Debug"
cd ..\handcolortracker\
nmake /f handcolortracker.mak CFG="HandColorTracker - Win32 Debug"
cd ..\handcontrol\
nmake /f handcontrol.mak CFG="handcontrol - Win32 Debug"
cd ..\handlocalization\
nmake /f handlocalization.mak CFG="handlocalization - Win32 Debug"
cd ..\sendCmd\
nmake /f sendCmd.mak CFG="sendCmd - Win32 Debug"
cd ..\grasprflx\
nmake /f grasprflx.mak CFG="grasprflx - win32 Debug"
cd ..

goto end

:release

echo Compiling all with RELEASE enabled.

cd .\armcontrol\
nmake /f armcontrol.mak CFG="armcontrol - Win32 Release"
cd ..\behaviorRnd\
nmake /f behaviorRnd.mak CFG="behaviorRnd - Win32 Release"
cd ..\handcolortracker\
nmake /f handcolortracker.mak CFG="HandColorTracker - Win32 Release"
cd ..\handcontrol\
nmake /f handcontrol.mak CFG="handcontrol - Win32 Release"
cd ..\handlocalization\
nmake /f handlocalization.mak CFG="handlocalization - Win32 Release"
cd ..\sendCmd\
nmake /f sendCmd.mak CFG="sendCmd - Win32 Release"
cd ..\grasprflx\
nmake /f grasprflx.mak CFG="grasprflx - win32 Release"
cd ..

goto end

:end
echo Building process completed.
