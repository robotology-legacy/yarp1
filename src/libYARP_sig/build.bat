@echo off

rem
rem
rem compiling libYARP_sig
rem
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem
rem %2 not needed.
rem

echo Entering build process of YARP signal libraries...

if "%1"=="" goto error
rem if "%2"=="" goto error
if NOT DEFINED YARP_ROOT goto error_not_defined 

if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="install" goto install

:clean 
echo Cleaning...
cd .\src
msdev libYARP_sig.dsw /MAKE "libYARP_sig - Win32 Debug" /CLEAN
msdev libYARP_sig.dsw /MAKE "libYARP_sig - Win32 Release" /CLEAN
cd ..
goto end


:debug
cd .\src
msdev libYARP_sig.dsw /MAKE "libYARP_sig - Win32 Debug" /BUILD
cd ..
goto end


:release
cd .\src
msdev libYARP_sig.dsw /MAKE "libYARP_sig - Win32 Release" /BUILD
cd ..
goto end


:install
echo Installing YARP signal processing libraries to default install directory.
echo.
copy .\include\yarp\*.h %YARP_ROOT%\include\yarp\
copy .\lib\winnt\*.lib %YARP_ROOT%\lib\winnt\
copy.
goto end


:error_not_defined
echo YARP_ROOT is not defined, exiting
goto end


:error
echo Syntax error: please make sure you know how this script works!
goto end

:end
echo Build process completed!