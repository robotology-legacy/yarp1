# Microsoft Developer Studio Project File - Name="utils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=utils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak" CFG="utils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "utils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\obj\winnt\release"
# PROP Intermediate_Dir ".\obj\winnt\release"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\..\include" /I "..\..\..\..\include\winnt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:".\lib\winnt\utils.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\obj\winnt\debug"
# PROP Intermediate_Dir ".\obj\winnt\debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\..\include" /I "..\..\..\..\include\winnt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\lib\winnt\utilsd.lib"

!ENDIF 

# Begin Target

# Name "utils - Win32 Release"
# Name "utils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\YARPBPNNet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPDebugUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPFft.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPLogFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPLowPassFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPPidFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPRecursiveLS.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPRnd.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPRndUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPTwoDKalmanFilter.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\yarp\YARPBabyBottle.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPBehavior.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPBPNNet.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPDebugUtils.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPFft.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPFSM.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPLogFile.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPLowPassFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPPidFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPRecursiveLS.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPRepeater.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPRnd.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPRndUtils.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPTwoDKalmanFilter.h
# End Source File
# End Group
# Begin Group "mbp"

# PROP Default_Filter ".c;.cpp;.h"
# Begin Source File

SOURCE=.\mbp\mbp.cpp
# End Source File
# Begin Source File

SOURCE=.\mbp\mbp.h
# End Source File
# Begin Source File

SOURCE=.\mbp\Mm.c
# End Source File
# Begin Source File

SOURCE=.\mbp\Mm.h
# End Source File
# Begin Source File

SOURCE=.\mbp\Random.c
# End Source File
# Begin Source File

SOURCE=.\mbp\Random.h
# End Source File
# End Group
# End Target
# End Project
