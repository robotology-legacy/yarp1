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

cd .\mapper\
msdev mapper.dsw /MAKE "mapper - Win32 Debug" /CLEAN
msdev mapper.dsw /MAKE "mapper - Win32 Release" /CLEAN
cd ..\

cd .\porter\porter\
msdev porter.dsp /MAKE "porter - Win32 Debug" /CLEAN
msdev porter.dsp /MAKE "porter - Win32 Release" /CLEAN
cd ..\conditional_porter\
msdev conditional_porter.dsp /MAKE "conditional_porter - Win32 Debug" /CLEAN
msdev conditional_porter.dsp /MAKE "conditional_porter - Win32 Release" /CLEAN
cd ..\..\

cd .\nameserver\YARPNameService\
msdev YARPNameService.dsp /MAKE "YARPNameService - Win32 Debug" /CLEAN
msdev YARPNameService.dsp /MAKE "YARPNameService - Win32 Release" /CLEAN
cd ..\nameclient\
msdev NameClient.dsp /MAKE "NameClient - Win32 Debug" /CLEAN
msdev NameClient.dsp /MAKE "NameClient - Win32 Release" /CLEAN
cd ..\..\

cd .\repeater\
msdev repeater.dsw /MAKE "repeater - Win32 Debug" /CLEAN
msdev repeater.dsw /MAKE "repeater - Win32 Release" /CLEAN
cd ..\

cd .\qnxlike\on
msdev on.dsp /MAKE "on - Win32 Debug" /CLEAN
msdev on.dsp /MAKE "on - Win32 Release" /CLEAN
cd ..\shutdown
msdev shutdown.dsp /MAKE "shutdown - Win32 Debug" /CLEAN
msdev shutdown.dsp /MAKE "shutdown - Win32 Release" /CLEAN
cd ..\sin
msdev sin.dsp /MAKE "sin - Win32 Debug" /CLEAN
msdev sin.dsp /MAKE "sin - Win32 Release" /CLEAN
cd ..\slay
msdev slay.dsp /MAKE "slay - Win32 Debug" /CLEAN
msdev slay.dsp /MAKE "slay - Win32 Release" /CLEAN
cd ..\..\

goto end


:debug
echo compiling all with DEBUG enabled.

cd .\mapper\
msdev mapper.dsw /MAKE "mapper - Win32 Debug" /BUILD
cd ..\

cd .\porter\porter\
msdev porter.dsp /MAKE "porter - Win32 Debug" /BUILD
cd ..\conditional_porter\
msdev conditional_porter.dsp /MAKE "conditional_porter - Win32 Debug" /BUILD
cd ..\..\

cd .\nameserver\YARPNameService\
msdev YARPNameService.dsp /MAKE "YARPNameService - Win32 Debug" /BUILD
cd ..\nameclient\
msdev NameClient.dsp /MAKE "NameClient - Win32 Debug" /BUILD
cd ..\..\

cd .\repeater\
msdev repeater.dsw /MAKE "repeater - Win32 Debug" /BUILD
cd ..\

cd .\qnxlike\on
msdev on.dsp /MAKE "on - Win32 Debug" /BUILD
cd ..\shutdown
msdev shutdown.dsp /MAKE "shutdown - Win32 Debug" /BUILD
cd ..\sin
msdev sin.dsp /MAKE "sin - Win32 Debug" /BUILD
cd ..\slay
msdev slay.dsp /MAKE "slay - Win32 Debug" /BUILD
cd ..\..\

goto end

:release

echo compiling all with RELEASE enabled.

cd .\mapper\
msdev mapper.dsw /MAKE "mapper - Win32 Release" /BUILD
cd ..\

cd .\porter\porter\
msdev porter.dsp /MAKE "porter - Win32 Release" /BUILD
cd ..\conditional_porter\
msdev conditional_porter.dsp /MAKE "conditional_porter - Win32 Release" /BUILD
cd ..\..\

cd .\nameserver\YARPNameService\
msdev YARPNameService.dsp /MAKE "YARPNameService - Win32 Release" /BUILD
cd ..\nameclient\
msdev NameClient.dsp /MAKE "NameClient - Win32 Release" /BUILD
cd ..\..\

cd .\repeater\
msdev repeater.dsw /MAKE "repeater - Win32 Release" /BUILD
cd ..\

cd .\qnxlike\on
msdev on.dsp /MAKE "on - Win32 Release" /BUILD
cd ..\shutdown
msdev shutdown.dsp /MAKE "shutdown - Win32 Release" /BUILD
cd ..\sin
msdev sin.dsp /MAKE "sin - Win32 Release" /BUILD
cd ..\slay
msdev slay.dsp /MAKE "slay - Win32 Release" /BUILD
cd ..\..\

goto end

:end
echo Building process completed.
