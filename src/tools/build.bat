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
for /D %%i in (*) do (
	for %%j in (%%i\*) do (
		if "%%~xj" == ".dsp" msdev %%j /MAKE "%%~nj - Win32 Debug" /CLEAN ))
goto end


:debug
for /D %%i in (*) do (
	for %%j in (%%i\*) do (
		if "%%~xj" == ".dsp" msdev %%j /MAKE "%%~nj - Win32 Debug" /BUILD ))
goto end


:release
for /D %%i in (*) do (
	for %%j in (%%i\*) do (
		if "%%~xj" == ".dsp" msdev %%j /MAKE "%%~nj - Win32 Release" /BUILD ))
goto end


:install
for /D %%i in (*) do (
	copy %%i\obj\%%i.exe %YARP_ROOT%\bin\winnt\ )
goto end


:error_not_defined
echo YARP_ROOT is not defined, exiting
goto end


:error
echo Syntax error: please make sure you know how this script works!
goto end


:end
echo Build process completed!