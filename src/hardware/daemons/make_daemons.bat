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
nmake /f framegrab.mak CFG="framegrab - Win32 Debug" clean
nmake /f framegrab.mak CFG="framegrab - Win32 Release" clean
cd ..\forcesensor\
nmake /f forcesensor.mak CFG="forcesensor - Win32 Debug" clean
nmake /f forcesensor.mak CFG="forcesensor - Win32 Release" clean
cd ..\touch\
nmake /f touch.mak CFG="touch - Win32 Debug" clean
nmake /f touch.mak CFG="touch - Win32 Release" clean
cd ..\

goto end


:debug
echo Compiling all with DEBUG enabled.

cd .\framegrab\
nmake /f framegrab.mak CFG="framegrab - Win32 Debug" 
cd ..\forcesensor\
nmake /f forcesensor.mak CFG="forcesensor - Win32 Debug" 
cd ..\touch\
nmake /f touch.mak CFG="touch - Win32 Debug"
cd ..\

goto end

:release

echo Compiling all with RELEASE enabled.

cd .\framegrab\
nmake /f framegrab.mak CFG="framegrab - Win32 Release" 
cd ..\forcesensor\
nmake /f forcesensor.mak CFG="forcesensor - Win32 Release" 
cd ..\touch\
nmake /f touch.mak CFG="touch - Win32 Release"
cd ..\

goto end

:end
echo Building process completed.
