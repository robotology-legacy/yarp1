@echo off

rem
rem
rem compiling libYARP_dev
rem
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem
rem %2 not needed.
rem

echo Entering build process of YARP device driver libraries...

if "%1"=="" goto error
rem if "%2"=="" goto error
if NOT DEFINED YARP_ROOT goto error_not_defined 

if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="install" goto install

:clean 
echo Cleaning...
cd .\src
msdev libYARP_dev.dsw /MAKE "libYARP_dev - Win32 Debug" /CLEAN
msdev libYARP_dev.dsw /MAKE "libYARP_dev - Win32 Release" /CLEAN
cd ..
goto end


:debug
cd .\src
msdev libYARP_dev.dsw /MAKE "libYARP_dev - Win32 Debug" /BUILD
cd ..
goto end


:release
cd .\src
msdev libYARP_dev.dsw /MAKE "libYARP_dev - Win32 Release" /BUILD
cd ..
goto end


:install
echo Installing YARP device driver library to default install directory.
echo *
echo *
echo *

if NOT EXIST .\lib\winnt\libYARP_dev.lib goto notcompiled
if NOT EXIST .\lib\winnt\libYARP_devd.lib goto notcompiled

echo *
echo Now entering subdirectories...
echo *
echo *

for /D %%i in (*) do if EXIST %%i\winnt\build.bat call %%i\winnt\build.bat %1 %YARP_ROOT%

echo *
echo Now copying common inclue files...
echo *
echo *

copy .\include\yarp\*.h %YARP_ROOT%\include\yarp\

echo *
echo Now preparing the libraries...
echo *
echo *

echo make library...

lib .\lib\winnt\libYARP_dev.lib .\valuecan\winnt\dd_orig\lib\mCan.lib /out:.\lib\winnt\libYARP_dev_x.lib
lib .\lib\winnt\libYARP_devd.lib .\valuecan\winnt\dd_orig\lib\mCand.lib /out:.\lib\winnt\libYARP_devd_x.lib

copy .\lib\winnt\libYARP_dev_x.lib %YARP_ROOT%\lib\winnt\libYARP_dev.lib
copy .\lib\winnt\libYARP_devd_x.lib %YARP_ROOT%\lib\winnt\libYARP_devd.lib

goto end


:error_not_defined
echo YARP_ROOT is not defined, exiting
goto end


:error
echo Syntax error: please make sure you know how this script works!
goto end


:notcompiled
echo Error: compile the library before installing it.
goto end


:end
echo Build process completed!