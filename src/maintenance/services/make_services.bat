@echo off

rem
rem
rem this is a global script file for compiling YARP services
rem options
rem		<debug> compile DEBUG
rem		<release> compile RELEASE, optimization ON
rem		<clean>	clean everything, be careful.
rem
rem
rem


echo Entering build process of YARP services...

if "%1"=="" goto end
if "%1"=="debug" goto debug
if "%1"=="release" goto release


:clean 
echo Cleaning all...

cd .\porter\porter\
nmake /f porter.mak CFG="porter - Win32 Debug" clean
nmake /f porter.mak CFG="porter - Win32 Release" clean
cd ..\conditional_porter\
nmake /f conditional_porter.mak CFG="conditional_porter - Win32 Debug" clean
nmake /f conditional_porter.mak CFG="conditional_porter - Win32 Release" clean
cd ..\..\

cd .\nameserver\YARPNameService\
nmake /f YARPNameService.mak CFG="YARPNameService - Win32 Debug" clean
nmake /f YARPNameService.mak CFG="YARPNameService - Win32 Release" clean
cd ..\nameclient\
nmake /f NameClient.mak CFG="NameClient - Win32 Debug" clean
nmake /f NameClient.mak CFG="NameClient - Win32 Release" clean
cd ..\..\

cd .\repeater\
nmake /f repeater.mak CFG="repeater - Win32 Debug" clean
nmake /f repeater.mak CFG="repeater - Win32 Release" clean
cd ..\

cd .\qnxlike\on
nmake /f on.mak CFG="on - Win32 Debug" clean
nmake /f on.mak CFG="on - Win32 Release" clean
cd ..\shutdown
nmake /f shutdown.mak CFG="shutdown - Win32 Debug" clean
nmake /f shutdown.mak CFG="shutdown - Win32 Release" clean
cd ..\sin
nmake /f sin.mak CFG="sin - Win32 Debug" clean
nmake /f sin.mak CFG="sin - Win32 Release" clean
cd ..\slay
nmake /f slay.mak CFG="slay - Win32 Debug" clean
nmake /f slay.mak CFG="slay - Win32 Release" clean
cd ..\..\

goto end


:debug
echo compiling all with DEBUG enabled.

cd .\porter\porter\
nmake /f porter.mak CFG="porter - Win32 Debug"
cd ..\conditional_porter\
nmake /f conditional_porter.mak CFG="conditional_porter - Win32 Debug"
cd ..\..\

cd .\nameserver\YARPNameService\
nmake /f YARPNameService.mak CFG="YARPNameService - Win32 Debug"
cd ..\nameclient\
nmake /f NameClient.mak CFG="NameClient - Win32 Debug"
cd ..\..\

cd .\repeater\
nmake /f repeater.mak CFG="repeater - Win32 Debug"
cd ..\

cd .\qnxlike\on
nmake /f on.mak CFG="on - Win32 Debug"
cd ..\shutdown
nmake /f shutdown.mak CFG="shutdown - Win32 Debug"
cd ..\sin
nmake /f sin.mak CFG="sin - Win32 Debug"
cd ..\slay
nmake /f slay.mak CFG="slay - Win32 Debug"
cd ..\..\

goto end

:release

echo compiling all with RELEASE enabled.

cd .\porter\porter\
nmake /f porter.mak CFG="porter - Win32 Release"
cd ..\conditional_porter\
nmake /f conditional_porter.mak CFG="conditional_porter - Win32 Release"
cd ..\..\

cd .\nameserver\YARPNameService\
nmake /f YARPNameService.mak CFG="YARPNameService - Win32 Release"
cd ..\nameclient\
nmake /f NameClient.mak CFG="NameClient - Win32 Release"
cd ..\..\

cd .\repeater\
nmake /f repeater.mak CFG="repeater - Win32 Release"
cd ..\

cd .\qnxlike\on
nmake /f on.mak CFG="on - Win32 Release"
cd ..\shutdown
nmake /f shutdown.mak CFG="shutdown - Win32 Release"
cd ..\sin
nmake /f sin.mak CFG="sin - Win32 Release"
cd ..\slay
nmake /f slay.mak CFG="slay - Win32 Release"
cd ..\..\

goto end

:end
echo Building process completed.
