# Microsoft Developer Studio Project File - Name="libYARP_OS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libYARP_OS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_OS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_OS.mak" CFG="libYARP_OS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libYARP_OS - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libYARP_OS - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libYARP_OS - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\obj\winnt"
# PROP Intermediate_Dir "..\obj\winnt\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../include" /I "../../../include" /I "../../../include/winnt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_OS.lib"

!ELSEIF  "$(CFG)" == "libYARP_OS - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\winnt"
# PROP Intermediate_Dir "..\obj\winnt\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../../include" /I "../../../include/winnt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_OSd.lib"

!ENDIF 

# Begin Target

# Name "libYARP_OS - Win32 Release"
# Name "libYARP_OS - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BlockReceiver.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockSender.cpp
# End Source File
# Begin Source File

SOURCE=.\mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Port.cpp
# End Source File
# Begin Source File

SOURCE=.\RefCounted.cpp
# End Source File
# Begin Source File

SOURCE=.\Sendable.cpp
# End Source File
# Begin Source File

SOURCE=.\wide_nameloc.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPAll.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPBottle.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPIterativeStats.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPMultipartMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPNameClient.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPNameService.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPNativeNameService.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPNativeSyncComm.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPParseParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPPort.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPRefCount.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPScheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSocketDgram.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSocketMcast.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSocketMulti.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSocketNameService.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSocketSyncComm.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSyncComm.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPThread.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPThreadAux.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPTime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\yarp\begin_pack_for_net.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\debug.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\end_pack_for_net.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\wide_nameloc.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPAll.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPBool.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPBottle.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPBottleCodes.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPBottleContent.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPErrorCodes.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPIterativeStats.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPList.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPMultipartMessage.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPNameClient.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPNameID.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPNameID_defs.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPNameService.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPNativeNameService.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPNativeSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPNetworkTypes.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPParseParameters.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPPort.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPPortContent.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPRandom.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPRateThread.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPRefCount.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPScheduler.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSemaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSocket.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSocketDgram.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSocketMcast.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSocketMulti.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSocketNameService.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSocketSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPString.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPThread.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPTime.h
# End Source File
# End Group
# Begin Group "Private Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\include\yarp_private\BlockPrefix.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\BlockReceiver.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\BlockSender.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\debug.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\mesh.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\Port.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\RefCounted.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\Sendable.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\Sendables.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\YARPFragments.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp_private\YARPNameID_defs.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\build.pl
# End Source File
# End Target
# End Project
