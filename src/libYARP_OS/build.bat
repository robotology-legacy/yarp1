@echo off

rem
rem
rem compiling libYARP_OS
rem
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem
rem %2 not needed.
rem

echo Entering build process of YARP libraries...

if "%1"=="" goto error
rem if "%2"=="" goto error
if NOT DEFINED YARP_ROOT goto error_not_defined 

if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="install" goto install

:clean 
echo Cleaning...
cd .\src
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Debug" /CLEAN
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Release" /CLEAN
cd ..
goto end


:debug
cd .\src
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Debug" /BUILD
cd ..
goto end


:release
cd .\src
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Release" /BUILD
cd ..
goto end


:install
echo Installing YARP libraries to default install directory.
echo Later this might change to something else.
copy .\include\yarp\*.h %YARP_ROOT%\include\yarp\
copy .\lib\winnt\*.lib %YARP_ROOT%\lib\winnt\
goto end


:error_not_defined
echo YARP_ROOT is not defined, exiting
goto end


:error
echo Syntax error: please make sure you know how this script works!
goto end

:end
echo Build process completed!