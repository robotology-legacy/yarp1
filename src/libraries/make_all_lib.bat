@echo off

rem
rem
rem this is a global script file for compiling YARP libraries
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

echo Cleaning ACE DLL.
cd .\ACE_wrappers\ace\
nmake /f ace_dll.mak CFG="ACE DLL - Win32 Debug" clean
nmake /f ace_dll.mak CFG="ACE DLL - Win32 Release" clean
cd ..\..\
echo Cleaning Math.
cd .\math\
nmake /f math.mak CFG="math - Win32 Debug" clean
nmake /f math.mak CFG="math - Win32 Release" clean
cd ..\
echo Cleaning LogPolar library.
cd .\logpolar\LogPolarSmallSDK\
nmake /f LogPolarSmallSDK.mak CFG="LogPolarSmallSDK - Win32 Debug" clean
nmake /f LogPolarSmallSDK.mak CFG="LogPolarSmallSDK - Win32 Release" clean
cd ..\..\
echo Cleaning BuildTables exe.
cd .\logpolar\BuildTablesSmall\
nmake /f BuildTablesSmall.mak CFG="BuildTablesSmall - Win32 Debug" clean
nmake /f BuildTablesSmall.mak CFG="BuildTablesSmall - Win32 Release" clean
cd ..\..\
echo Cleaning os_services library.
cd .\os_services\make_winnt\os_services\
nmake /f os_services.mak CFG="os_services - Win32 Debug" clean
nmake /f os_services.mak CFG="os_services - Win32 Release" clean
cd ..\..\..\
echo Cleaning images libraries.
cd .\images\tools\
nmake /f tools.mak CFG="tools - Win32 Debug" clean
nmake /f tools.mak CFG="tools - Win32 Release" clean
cd ..\..\
echo Cleaning utils library.
cd .\utils\
nmake /f utils.mak CFG="utils - Win32 Debug" clean
nmake /f utils.mak CFG="utils - Win32 Release" clean
cd ..\

echo Going to "Device Drivers."
cd ..\hardware\src\
nmake /f alldrivers.mak CFG="alldrivers - Win32 Debug" clean
nmake /f alldrivers.mak CFG="alldrivers - Win32 Release" clean
cd ..\..\libraries\

echo Cleaning motorcontrol
cd .\motorcontrol\
nmake /f motorcontrol.mak CFG="motorcontrol - Win32 Debug" clean
nmake /f motorcontrol.mak CFG="motorcontrol - Win32 Release" clean
cd ..\
 
goto end


:debug

echo Compiling DEBUG...

echo Building ACE DLL.
cd .\ACE_wrappers\ace\
nmake /f ace_dll.mak CFG="ACE DLL - Win32 Debug"
cd ..\..\
echo Building Math.
cd .\math\
nmake /f math.mak CFG="math - Win32 Debug"
cd ..\
echo Building LogPolar library.
cd .\logpolar\LogPolarSmallSDK\
nmake /f LogPolarSmallSDK.mak CFG="LogPolarSmallSDK - Win32 Debug"
cd ..\..\
echo Building BuildTables exe.
cd .\logpolar\BuildTablesSmall\
nmake /f BuildTablesSmall.mak CFG="BuildTablesSmall - Win32 Debug"
cd ..\..\
echo Building os_services library.
cd .\os_services\make_winnt\os_services\
nmake /f os_services.mak CFG="os_services - Win32 Debug"
cd ..\..\..\
echo Building images libraries.
cd .\ipl\
call install_ipl.bat
cd ..\
cd .\images\tools\
nmake /f tools.mak CFG="tools - Win32 Debug"
cd ..\..\
echo Building utils library.
cd .\utils\
nmake /f utils.mak CFG="utils - Win32 Debug"
cd ..\

echo Building "Device Drivers."
cd ..\hardware\src\
nmake /f alldrivers.mak CFG="alldrivers - Win32 Debug"
cd ..\..\libraries\

echo Building motorcontrol
cd .\motorcontrol\
nmake /f motorcontrol.mak CFG="motorcontrol - Win32 Debug"
cd ..\


goto end


:release

echo Compiling RELEASE...

echo Building ACE DLL.
cd .\ACE_wrappers\ace\
nmake /f ace_dll.mak CFG="ACE DLL - Win32 Release"
cd ..\..\
echo Building Math.
cd .\math\
nmake /f math.mak CFG="math - Win32 Release"
cd ..\
echo Building LogPolar library.
cd .\logpolar\LogPolarSmallSDK\
nmake /f LogPolarSmallSDK.mak CFG="LogPolarSmallSDK - Win32 Release"
cd ..\..\
echo Building BuildTables exe.
cd .\logpolar\BuildTablesSmall\
nmake /f BuildTablesSmall.mak CFG="BuildTablesSmall - Win32 Release"
cd ..\..\
echo Building os_services library.
cd .\os_services\make_winnt\os_services\
nmake /f os_services.mak CFG="os_services - Win32 Release"
cd ..\..\..\
echo Building images libraries.
cd .\ipl\
call install_ipl.bat
cd ..\
cd .\images\tools\
nmake /f tools.mak CFG="tools - Win32 Release"
cd ..\..\
echo Building utils library.
cd .\utils\
nmake /f utils.mak CFG="utils - Win32 Release"
cd ..\

echo Building "Device Drivers."
cd ..\hardware\src\
nmake /f alldrivers.mak CFG="alldrivers - Win32 Release"
cd ..\..\libraries\

echo Building motorcontrol
cd .\motorcontrol\
nmake /f motorcontrol.mak CFG="motorcontrol - Win32 Release"
cd ..\

goto end



:end

echo Build process completed.
