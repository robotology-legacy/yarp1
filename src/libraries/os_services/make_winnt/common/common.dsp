# Microsoft Developer Studio Project File - Name="common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "common.mak" CFG="common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "common - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "common - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "../../include" /I "../../sys_include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WIN__" /D "__WIN_MSVC__" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../sys_include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WIN__" /D "__WIN_MSVC__" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "common - Win32 Release"
# Name "common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\common.cpp
# End Source File
# Begin Source File

SOURCE=.\common.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Sys_include"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\sys_include\BlockPrefix.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\BlockReceiver.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\BlockSender.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\Port.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\RefCounted.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\Sendable.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\Sendables.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\socklib.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\ThreadInput.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\TinySocket.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\wide_nameloc.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\YARPFragments.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\YARPNameID_defs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\common.ico
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "Common Files"

# PROP Default_Filter "*.cpp, *.c"
# Begin Source File

SOURCE=..\..\common\BlockReceiver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\BlockSender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\mesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Port.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\RefCounted.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\Sendable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\socklib.c
# End Source File
# Begin Source File

SOURCE=..\..\common\TinySocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPAll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPNameService.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPPort.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPRefCount.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSocketNameService.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSocketSyncComm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSyncComm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPThreadAux.cpp
# End Source File
# End Group
# Begin Group "Specific Files"

# PROP Default_Filter "*.cpp, *.c"
# Begin Source File

SOURCE=..\..\winnt\YARPMultipartMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winnt\YARPNativeNameService.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winnt\YARPScheduler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winnt\YARPSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winnt\YARPThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winnt\YARPTime.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
