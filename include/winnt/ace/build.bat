@echo off

rem
rem
rem compiling ACE dll.
rem
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem		<install> copies include files in some appropriate directory.
rem
rem		%2 is the path where ACE was unpacked.
rem

echo Entering build process of ACE libraries...

if "%1"=="" goto error
if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="install" goto install

if "%2"=="" goto error

if NOT DEFINED YARP_ROOT goto error_not_defined 

:clean 
echo Cleaning...
cd ./src
msdev ace.dsw /MAKE "ACE - Win32 Debug" /CLEAN
msdev ace.dsw /MAKE "ACE - Win32 Release" /CLEAN
cd ..
goto end


:debug
echo Preparing the ACE config file (assuming WIN32).
pushd .
cp %YARP_ROOT%\include\winnt\ace\config.h %2\ace\config.h
cd %2\ace\
msdev ace.dsw /MAKE "ACE - Win32 Debug" /BUILD
del config.h
popd
goto end


:release
echo Preparing the ACE config file (assuming WIN32).
pushd .
cp %YARP_ROOT%\include\winnt\ace\config.h %2\ace\config.h
cd %2\ace\
msdev ace.dsw /MAKE "ACE - Win32 Release" /BUILD
del config.h
popd
goto end

:install
echo Installing ACE for compiling YARP libraries.
echo WARNING: Not doing the full installation for now!
pushd .
cd %2\ace\
cp *.h %YARP_ROOT%\include\ace\
cp *.inl %YARP_ROOT%\include\ace\
cp *.i %YARP_ROOT%\include\ace\
cp *.cpp %YARP_ROOT%\include\ace\
cd ..\lib\
cp *.dll %YARP_ROOT%\bin\winnt\
cp *.lib %YARP_ROOT%\lib\winnt\
popd
goto end

:error
echo Syntax error: please make sure you know how this script works!
goto end

:error_not_defined
echo YARP_ROOT is not defined, exiting
goto end

:end
echo Build process completed!

