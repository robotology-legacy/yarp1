@echo off

rem
rem
rem this is a global script file for compiling YARP daemons
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem
rem
rem


echo Entering build process of YARP daemons...

if "%1"=="" goto end
if "%1"=="debug" goto debug
if "%1"=="release" goto release


:clean 
echo Cleaning all...

cd .\framegrab\
msdev framegrab.dsw /MAKE "framegrab - Win32 Debug" /CLEAN
msdev framegrab.dsw /MAKE "framegrab - Win32 Release" /CLEAN
cd ..\forcesensor\
msdev forcesensor.dsw /MAKE "forcesensor - Win32 Debug" /CLEAN
msdev forcesensor.dsw /MAKE "forcesensor - Win32 Release" /CLEAN
cd ..\touch\
msdev touch.dsw /MAKE "touch - Win32 Debug" /CLEAN
msdev touch.dsw /MAKE "touch - Win32 Release" /CLEAN
cd ..\soundgrab\
msdev soundgrab.dsw /MAKE "soundgrab - Win32 Debug" /CLEAN
msdev soundgrab.dsw /MAKE "soundgrab - Win32 Release" /CLEAN
cd ..\

goto end


:debug
echo Compiling all with DEBUG enabled.

cd .\framegrab\
msdev framegrab.dsw /MAKE "framegrab - Win32 Debug" /BUILD
cd ..\forcesensor\
msdev forcesensor.dsw /MAKE "forcesensor - Win32 Debug" /BUILD
cd ..\touch\
msdev touch.dsw /MAKE "touch - Win32 Debug" /BUILD
cd ..\soundgrab\
msdev soundgrab.dsw /MAKE "soundgrab - Win32 Debug" /BUILD
cd ..\

goto end

:release

echo Compiling all with RELEASE enabled.

cd .\framegrab\
msdev framegrab.dsw /MAKE "framegrab - Win32 Release" /BUILD
cd ..\forcesensor\
msdev forcesensor.dsw /MAKE "forcesensor - Win32 Release" /BUILD
cd ..\touch\
msdev touch.dsw /MAKE "touch - Win32 Release" /BUILD
cd ..\soundgrab\
msdev soundgrab.dsw /MAKE "soundgrab - Win32 Release" /BUILD
cd ..\

goto end

:end
echo Building process completed.
