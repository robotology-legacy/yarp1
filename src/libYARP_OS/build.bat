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
rem
rem

echo Entering build process of YARP libraries...

if "%1"=="" goto end
if "%1"=="debug" goto debug
if "%1"=="release" goto release


:clean 
echo Cleaning...
cd ./src
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Debug" /CLEAN
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Release" /CLEAN
cd ..
goto end


:debug
cd ./src
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Debug" /BUILD
cd ..
goto end


:release
cd ./src
msdev libYARP_OS.dsw /MAKE "libYARP_OS - Win32 Release" /BUILD
cd ..
goto end


:end
echo Build process completed!