@echo off

rem
rem
rem compiling libYARP_OS services
rem
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything
rem
rem
rem

echo Entering build process of YARP services...

if "%1"=="" goto error
if NOT DEFINED YARP_ROOT goto error_not_defined 

if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="install" goto install


:clean 
echo Cleaning...
msdev tools.dsw /MAKE "yarp_service - Win32 Debug" /CLEAN
msdev tools.dsw /MAKE "yarp_service - Win32 Release" /CLEAN
msdev tools.dsw /MAKE "yarp_read - Win32 Debug" /CLEAN
msdev tools.dsw /MAKE "yarp_read - Win32 Release" /CLEAN
msdev tools.dsw /MAKE "yarp_write - Win32 Debug" /CLEAN
msdev tools.dsw /MAKE "yarp_write - Win32 Release" /CLEAN
msdev tools.dsw /MAKE "yarp_connect - Win32 Debug" /CLEAN
msdev tools.dsw /MAKE "yarp_connect - Win32 Release" /CLEAN
goto end


:debug
msdev tools.dsw /MAKE "yarp_service - Win32 Debug" /BUILD
msdev tools.dsw /MAKE "yarp_read - Win32 Debug" /BUILD
msdev tools.dsw /MAKE "yarp_write - Win32 Debug" /BUILD
msdev tools.dsw /MAKE "yarp_connect - Win32 Debug" /BUILD
goto end


:release
msdev tools.dsw /MAKE "yarp_service - Win32 Release" /BUILD
msdev tools.dsw /MAKE "yarp_read - Win32 Release" /BUILD
msdev tools.dsw /MAKE "yarp_write - Win32 Release" /BUILD
msdev tools.dsw /MAKE "yarp_connect - Win32 Release" /BUILD
goto end


:install
copy .\yarp-service\obj\yarp-service.exe %YARP_ROOT%\bin\winnt\
copy .\yarp-read\obj\yarp-read.exe %YARP_ROOT%\bin\winnt\
copy .\yarp-write\obj\yarp-write.exe %YARP_ROOT%\bin\winnt\
copy .\yarp-connect\obj\yarp-connect.exe %YARP_ROOT%\bin\winnt\
goto end


:error_not_defined
echo YARP_ROOT is not defined, exiting
goto end


:error
echo Syntax error: please make sure you know how this script works!
goto end


:end
echo Build process completed!