# Microsoft Developer Studio Project File - Name="common_winnt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=common_winnt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "common_winnt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "common_winnt.mak" CFG="common_winnt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "common_winnt - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "common_winnt - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "common_winnt - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../sys_include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /Z<none> /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /pdb:none /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "common_winnt - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /GX /Od /I "z:\YARP\src\os_services\include" /I "z:\YARP\src\os_services\sys_include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ  /c
# SUBTRACT CPP /Z<none> /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /pdb:none /machine:I386
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "common_winnt - Win32 Release"
# Name "common_winnt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\BlockReceiver.cpp
# End Source File
# Begin Source File

SOURCE=..\BlockSender.cpp
# End Source File
# Begin Source File

SOURCE=..\mesh.cpp
# End Source File
# Begin Source File

SOURCE=..\Port.cpp
# End Source File
# Begin Source File

SOURCE=..\RefCounted.cpp
# End Source File
# Begin Source File

SOURCE=..\Sendable.cpp
# End Source File
# Begin Source File

SOURCE=..\socklib.c
# End Source File
# Begin Source File

SOURCE=..\TinySocket.cpp
# End Source File
# Begin Source File

SOURCE=..\YARPNameService.cpp
# End Source File
# Begin Source File

SOURCE=..\YARPPort.cpp
# End Source File
# Begin Source File

SOURCE=..\YARPSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\YARPSocketNameService.cpp
# End Source File
# Begin Source File

SOURCE=..\YARPSocketSyncComm.cpp
# End Source File
# Begin Source File

SOURCE=..\YARPSyncComm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\socklib.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ThreadInput.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TinySocket.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPBool.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPImage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPMultipartMessage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPNameID.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPNameService.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPNativeNameService.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPNativeSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPNetworkTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPPort.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPPortContent.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPRefCount.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPScheduler.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPSemaphore.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPSocketNameService.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPSocketSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPString.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPThread.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPTime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
