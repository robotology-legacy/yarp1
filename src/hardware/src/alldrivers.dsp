# Microsoft Developer Studio Project File - Name="alldrivers" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=alldrivers - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "alldrivers.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "alldrivers.mak" CFG="alldrivers - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "alldrivers - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "alldrivers - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "alldrivers - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\obj\Release"
# PROP Intermediate_Dir ".\obj\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\common" /I "..\..\..\include" /I "..\..\..\include\sys" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\yarpdrivers.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=installing library
PostBuild_Cmds=copy .\mei\winnt\*.h ..\..\..\include	copy .\galil\common\*.h ..\..\..\include	copy .\nidaq\winnt\*.h ..\..\..\include	copy .\picolo\winnt\*.h ..\..\..\include	copy .\common\*.h ..\..\..\include	copy .\jr3\winnt\*.h ..\..\..\include	copy ..\lib\winnt\*.lib ..\..\..\lib\winnt	copy .\androidworld\common\*.h ..\..\..\include	make_lib.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "alldrivers - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\obj\Debug"
# PROP Intermediate_Dir ".\obj\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\common" /I "..\..\..\include" /I "..\..\..\include\sys" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\yarpdriversdb.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=installing library
PostBuild_Cmds=copy .\mei\winnt\*.h ..\..\..\include	copy .\galil\common\*.h ..\..\..\include	copy .\nidaq\winnt\*.h ..\..\..\include	copy .\picolo\winnt\*.h ..\..\..\include	copy .\common\*.h ..\..\..\include	copy .\jr3\winnt\*.h ..\..\..\include	copy ..\lib\winnt\*.lib ..\..\..\lib\winnt	copy .\androidworld\common\*.h ..\..\..\include	make_lib_db.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "alldrivers - Win32 Release"
# Name "alldrivers - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\androidworld\common\YARPAndroidDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\common\YARPControlBoardUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\galil\common\YARPGalilDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\jr3\winnt\YARPJR3DeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\mei\winnt\YARPMEIDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\nidaq\winnt\YARPNIDAQDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=.\picolo\winnt\YARPPicoloDeviceDriver.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common\YARPADCUtils.h
# End Source File
# Begin Source File

SOURCE=.\androidworld\common\YARPAndroidDeviceDriver.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPControlBoardUtils.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPDeviceDriver.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPFrameGrabberUtils.h
# End Source File
# Begin Source File

SOURCE=.\galil\common\YARPGalilDeviceDriver.h
# End Source File
# Begin Source File

SOURCE=.\jr3\winnt\YARPJR3DeviceDriver.h
# End Source File
# Begin Source File

SOURCE=.\mei\winnt\YARPMEIDeviceDriver.h
# End Source File
# Begin Source File

SOURCE=.\nidaq\winnt\YARPNIDAQDeviceDriver.h
# End Source File
# Begin Source File

SOURCE=.\picolo\winnt\YARPPicoloDeviceDriver.h
# End Source File
# End Group
# End Target
# End Project
