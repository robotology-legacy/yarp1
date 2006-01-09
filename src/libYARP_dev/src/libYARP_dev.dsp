# Microsoft Developer Studio Project File - Name="libYARP_dev" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libYARP_dev - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_dev.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_dev.mak" CFG="libYARP_dev - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libYARP_dev - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libYARP_dev - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libYARP_dev - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib/winnt/"
# PROP Intermediate_Dir "../obj/winnt/release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\include\winnt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/winnt/libYARP_dev_x.lib"

!ELSEIF  "$(CFG)" == "libYARP_dev - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib/winnt/"
# PROP Intermediate_Dir "../obj/winnt/debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\include\winnt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/winnt/libYARP_devd_x.lib"

!ENDIF 

# Begin Target

# Name "libYARP_dev - Win32 Release"
# Name "libYARP_dev - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\YARPControlBoardUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\yarp\YARPADCUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPControlBoardUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPDataGloveUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPDeviceDriver.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPFrameGrabberUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPGenericControlBoard.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPGenericDataGlove.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPGenericForceSensor.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPGenericGrabber.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPGenericPresSens.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPGenericSoundGrabber.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPGenericTracker.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPPresSensUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSoundCardUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPTrackerUtils.h
# End Source File
# End Group
# Begin Group "valuecan"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\valuecan\winnt\yarp\YARPValueCanDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\valuecan\winnt\yarp\YARPValueCanDeviceDriver.h
# End Source File
# End Group
# Begin Group "androidworld"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\androidworld\winnt\yarp\YARPAndroidDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\androidworld\winnt\yarp\YARPAndroidDeviceDriver.h
# End Source File
# End Group
# Begin Group "galil"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\galil\winnt\yarp\YARPGalilDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\galil\winnt\yarp\YARPGalilDeviceDriver.h
# End Source File
# End Group
# Begin Group "jr3"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\jr3\winnt\yarp\YARPJR3DeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\jr3\winnt\yarp\YARPJR3DeviceDriver.h
# End Source File
# End Group
# Begin Group "mei"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\mei\winnt\yarp\YARPMEIDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\mei\winnt\yarp\YARPMEIDeviceDriver.h
# End Source File
# End Group
# Begin Group "nidaq"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\nidaq\winnt\yarp\YARPNIDAQDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\nidaq\winnt\yarp\YARPNIDAQDeviceDriver.h
# End Source File
# End Group
# Begin Group "picolo"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\picolo\winnt\yarp\YARPPicoloDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\picolo\winnt\yarp\YARPPicoloDeviceDriver.h
# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\sound\winnt\yarp\YARPSoundDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\sound\winnt\yarp\YARPSoundDeviceDriver.h
# End Source File
# End Group
# Begin Group "null"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\null\winnt\yarp\YARPNullDeviceDriver.h
# End Source File
# End Group
# Begin Group "null_grabber"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\null_grabber\winnt\yarp\YARPNullGrabberDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\null_grabber\winnt\yarp\YARPNullGrabberDeviceDriver.h
# End Source File
# End Group
# Begin Group "cyberglove"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\cyberglove\winnt\yarp\YARPCyberGloveDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\cyberglove\winnt\yarp\YARPCyberGloveDeviceDriver.h
# End Source File
# End Group
# Begin Group "fob"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\fob\winnt\yarp\YARPFoBDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\fob\winnt\yarp\YARPFoBDeviceDriver.h
# End Source File
# End Group
# Begin Group "joypres"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\joypres\winnt\yarp\YARPJoyPresDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\joypres\winnt\yarp\YARPJoyPresDeviceDriver.h
# End Source File
# End Group
# Begin Group "esdcan"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\esdcan\winnt\yarp\YARPEsdCanDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\esdcan\winnt\yarp\YARPEsdCanDeviceDriver.h
# End Source File
# End Group
# Begin Group "esddaq"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\esddaq\winnt\yarp\YARPEsdDaqDeviceDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\esddaq\winnt\yarp\YARPEsdDaqDeviceDriver.h
# End Source File
# End Group
# End Target
# End Project
