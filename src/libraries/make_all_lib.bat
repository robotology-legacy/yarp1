@echo off

rem
rem
rem this is a global script file for compiling YARP libraries
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem
rem             %2 == <full> clean ACE and copies IPL libs.
rem
rem

echo Entering build process of YARP libraries...

if "%1"=="" goto end
if "%1"=="debug" goto debug
if "%1"=="release" goto release


:clean 

if not "%2"=="full" goto skipace

echo Cleaning ACE DLL.
cd .\ACE_wrappers\ace\
msdev ace.dsw /MAKE "ACE DLL - Win32 Debug" /CLEAN
msdev ace.dsw /MAKE "ACE DLL - Win32 Release" /CLEAN
cd ..\..\

:skipace
echo Cleaning Math.
cd .\math\
msdev math.dsw /MAKE "math - Win32 Debug" /CLEAN
msdev math.dsw /MAKE "math - Win32 Release" /CLEAN
cd ..\
echo Cleaning LogPolar library.
cd .\logpolar\LogPolarSmallSDK\
msdev LogPolarSmallSDK.dsp /MAKE "LogPolarSmallSDK - Win32 Debug" /CLEAN
msdev LogPolarSmallSDK.dsp /MAKE "LogPolarSmallSDK - Win32 Release" /CLEAN
cd ..\..\
echo Cleaning BuildTables exe.
cd .\logpolar\BuildTablesSmall\
msdev BuildTablesSmall.dsp /MAKE "BuildTablesSmall - Win32 Debug" /CLEAN
msdev BuildTablesSmall.dsp /MAKE "BuildTablesSmall - Win32 Release" /CLEAN
cd ..\..\
echo Cleaning os_services library.
cd .\os_services\make_winnt\os_services\
msdev os_services.dsp /MAKE "os_services - Win32 Debug" /CLEAN
msdev os_services.dsp /MAKE "os_services - Win32 Release" /CLEAN
cd ..\..\..\
echo Cleaning images libraries.
cd .\images\
msdev images.dsw /MAKE "images - Win32 Debug" /CLEAN
msdev images.dsw /MAKE "images - Win32 Release" /CLEAN
cd ..\
cd .\images\tools\
msdev tools.dsp /MAKE "tools - Win32 Debug" /CLEAN
msdev tools.dsp /MAKE "tools - Win32 Release" /CLEAN
cd ..\..\
echo Cleaning utils library.
cd .\utils\
msdev utils.dsw /MAKE "utils - Win32 Debug" /CLEAN
msdev utils.dsw /MAKE "utils - Win32 Release" /CLEAN
cd ..\

echo Going to "Device Drivers."
cd ..\hardware\src\
msdev alldrivers.dsw /MAKE "alldrivers - Win32 Debug" /CLEAN
msdev alldrivers.dsw /MAKE "alldrivers - Win32 Release" /CLEAN
cd ..\..\libraries\

echo Cleaning motorcontrol
cd .\motorcontrol\
msdev motorcontrol.dsw /MAKE "motorcontrol - Win32 Debug" /CLEAN
msdev motorcontrol.dsw /MAKE "motorcontrol - Win32 Release" /CLEAN
cd ..\
 
goto end


:debug

echo Compiling DEBUG...

if not "%2"=="full" goto skipaced
 
echo Building ACE DLL.
cd .\ACE_wrappers\ace\
msdev ace.dsw /MAKE "ACE DLL - Win32 Debug" /BUILD
cd ..\..\

:skipaced

echo Building Math.
cd .\math\
msdev math.dsw /MAKE "math - Win32 Debug" /BUILD
cd ..\
echo Building LogPolar library.
cd .\logpolar\LogPolarSmallSDK\
msdev LogPolarSmallSDK.dsp /MAKE "LogPolarSmallSDK - Win32 Debug" /BUILD
cd ..\..\
echo Building BuildTables exe.
cd .\logpolar\BuildTablesSmall\
msdev BuildTablesSmall.dsp /MAKE "BuildTablesSmall - Win32 Debug" /BUILD
cd ..\..\
echo Building os_services library.
cd .\os_services\make_winnt\os_services\
msdev os_services.dsp /MAKE "os_services - Win32 Debug" /BUILD
cd ..\..\..\
echo Building images libraries.

if not "%2"=="full" goto skipipld

cd .\ipl\
call install_ipl.bat
cd ..\

:skipipld

cd .\images\
msdev images.dsw /MAKE "images - Win32 Debug" /BUILD
cd ..\
cd .\images\tools\
msdev tools.dsp /MAKE "tools - Win32 Debug" /BUILD
cd ..\..\
echo Building utils library.
cd .\utils\
msdev utils.dsw /MAKE "utils - Win32 Debug" /BUILD
cd ..\

echo Building "Device Drivers."
cd ..\hardware\src\
msdev alldrivers.dsw /MAKE "alldrivers - Win32 Debug" /BUILD
cd ..\..\libraries\

echo Building motorcontrol
cd .\motorcontrol\
msdev motorcontrol.dsw /MAKE "motorcontrol - Win32 Debug" /BUILD
cd ..\


goto end


:release

echo Compiling RELEASE...

if not "%2"=="full" goto skipacer

echo Building ACE DLL.
cd .\ACE_wrappers\ace\
msdev ace.dsw /MAKE "ACE DLL - Win32 Release" /BUILD
cd ..\..\

:skipacer

echo Building Math.
cd .\math\
msdev math.dsw /MAKE "math - Win32 Release" /BUILD
cd ..\
echo Building LogPolar library.
cd .\logpolar\LogPolarSmallSDK\
msdev LogPolarSmallSDK.dsp /MAKE "LogPolarSmallSDK - Win32 Release" /BUILD
cd ..\..\
echo Building BuildTables exe.
cd .\logpolar\BuildTablesSmall\
msdev BuildTablesSmall.dsp /MAKE "BuildTablesSmall - Win32 Release" /BUILD
cd ..\..\
echo Building os_services library.
cd .\os_services\make_winnt\os_services\
msdev os_services.dsp /MAKE "os_services - Win32 Release" /BUILD
cd ..\..\..\
echo Building images libraries.

if not "%2"=="full" goto skipiplr

cd .\ipl\
call install_ipl.bat
cd ..\

:skipiplr

cd .\images\
msdev images.dsw /MAKE "images - Win32 Release" /BUILD
cd ..\
cd .\images\tools\
msdev tools.dsp /MAKE "tools - Win32 Release" /BUILD
cd ..\..\
echo Building utils library.
cd .\utils\
msdev utils.dsw /MAKE "utils - Win32 Release" /BUILD
cd ..\

echo Building "Device Drivers."
cd ..\hardware\src\
msdev alldrivers.dsw /MAKE "alldrivers - Win32 Release" /BUILD
cd ..\..\libraries\

echo Building motorcontrol
cd .\motorcontrol\
msdev motorcontrol.dsw /MAKE "motorcontrol - Win32 Release" /BUILD
cd ..\

goto end



:end

echo Build process completed.
