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
# PROP Output_Dir ".\obj\Release"
# PROP Intermediate_Dir ".\obj\Release"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:".\lib\winnt\utils.lib"
# SUBTRACT LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=installing lib
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	copy .\lib\winnt\utils.lib ..\..\..\lib\winnt\utils.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

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
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\lib\winnt\utilsdb.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	copy .\lib\winnt\utilsdb.lib ..\..\..\lib\winnt\utilsdb.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "utils - Win32 Release"
# Name "utils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\YARPBottle.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPBPNNet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPConfigFile.cpp
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

SOURCE=.\src\YARPLU.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPParseParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPPidFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPRecursiveLS.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPSVD.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPTwoDKalmanFilter.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\YARPBehavior.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBottle.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBottleCodes.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBottleContent.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBPNNet.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPConfigFile.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPFft.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPFSM.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPLogFile.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPLowPassFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPParseParameters.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPPidFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPRecursiveLS.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPRepeater.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPTwoDKalmanFilter.h
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
