@echo off

rem
rem this is for install only.
rem don't call this script directly.
rem
rem


if not "%1"=="install" goto nothingdone
if "%2"=="" goto nothingdone

:install
echo Installing the ValueCan device driver for YARP.
echo Pls, make sure this is what you need for your hardware.
pushd .
cd .\valuecan\winnt

copy .\yarp\*.h %2\include\yarp\

popd
goto done

:nothingdone
echo Nothing done for ValueCan.

:done
