@echo off

rem
rem
rem compiling ACE dll (assuming 5.4.1).
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
echo Assuming ACE version 5.4.1, it might not work on other releases.


if "%1"=="" goto error
if "%2"=="" goto error
if NOT DEFINED YARP_ROOT goto error_not_defined 

if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="install" goto install

:clean 
echo Cleaning...
cd .\src
msdev ace.dsw /MAKE "ACE DLL - Win32 Debug" /CLEAN
msdev ace.dsw /MAKE "ACE DLL - Win32 Release" /CLEAN
cd ..
goto end


:debug
echo Preparing the ACE config file (assuming WIN32).
pushd .
copy %YARP_ROOT%\include\winnt\ace\config.h %2\ace\config.h
cd %2\ace\
msdev ace.dsw /MAKE "ACE DLL - Win32 Debug" /BUILD
del config.h
popd
goto end


:release
echo Preparing the ACE config file (assuming WIN32).
pushd .
copy %YARP_ROOT%\include\winnt\ace\config.h %2\ace\config.h
cd %2\ace\
msdev ace.dsw /MAKE "ACE DLL - Win32 Release" /BUILD
del config.h
popd
goto end

:install
echo Installing ACE for compiling YARP libraries.
echo WARNING: Not doing the full installation for now!
pushd .
cd %2\ace\
echo Copying .h files.
copy *.h %YARP_ROOT%\include\ace\
echo Copying .inl files.
copy *.inl %YARP_ROOT%\include\ace\
echo Copying .i files.
copy *.i %YARP_ROOT%\include\ace\
echo Copying .cpp files.
copy *.cpp %YARP_ROOT%\include\ace\
echo Copying os_include files.
xcopy /E /Y .\os_include\* %YARP_ROOT%\include\ace\os_include\
echo Copying libraries.
copy ..\bin\*.dll %YARP_ROOT%\bin\winnt\
copy *.lib %YARP_ROOT%\lib\winnt\
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

