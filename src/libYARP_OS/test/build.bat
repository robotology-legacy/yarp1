@echo off

rem
rem
rem compiling libYARP_OS test code
rem
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything
rem
rem
rem

echo Entering build process of YARP libraries...

if "%1"=="" goto error
if NOT DEFINED YARP_ROOT goto error_not_defined 

if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="install" goto install


:clean 
echo Cleaning...
msdev test.dsw /MAKE "demo00 - Win32 Debug" /CLEAN
msdev test.dsw /MAKE "demo00 - Win32 Release" /CLEAN
msdev test.dsw /MAKE "demo01 - Win32 Debug" /CLEAN
msdev test.dsw /MAKE "demo01 - Win32 Release" /CLEAN
goto end


:debug
msdev test.dsw /MAKE "demo00 - Win32 Debug" /BUILD
msdev test.dsw /MAKE "demo01 - Win32 Debug" /BUILD
goto end


:release
msdev test.dsw /MAKE "demo00 - Win32 Release" /BUILD
msdev test.dsw /MAKE "demo01 - Win32 Release" /BUILD
goto end


:install
echo Doing nothing as yet
goto end


:error_not_defined
echo YARP_ROOT is not defined, exiting
goto end


:error
echo Syntax error: please make sure you know how this script works!
goto end


:end
echo Build process completed!