# Microsoft Developer Studio Project File - Name="os_services" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=os_services - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "os_services.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "os_services.mak" CFG="os_services - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "os_services - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "os_services - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "os_services - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../include" /I "../../sys_include" /I "../../../../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
PostBuild_Cmds=COPY Release\os_services.lib ..\..\lib\winnt\os_services.lib	COPY Release\os_services.lib ..\..\..\..\..\lib\winnt\os_services.lib	COPY ..\..\include\*.h ..\..\..\..\..\include	COPY ..\..\sys_include\*.h ..\..\..\..\..\include
# End Special Build Tool

!ELSEIF  "$(CFG)" == "os_services - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../sys_include" /I "../../../../../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
PostBuild_Cmds=COPY Debug\os_services.lib ..\..\lib\winnt\os_servicesDB.lib	COPY Debug\os_services.lib ..\..\..\..\..\lib\winnt\os_servicesDB.lib	COPY ..\..\include\*.h ..\..\..\..\..\include	COPY ..\..\sys_include\*.h ..\..\..\..\..\include
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "os_services - Win32 Release"
# Name "os_services - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\winnt\YARPMultipartMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\winnt\YARPNativeNameService.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\begin_pack_for_net.h
# End Source File
# Begin Source File

SOURCE=..\..\include\end_pack_for_net.h
# End Source File
# Begin Source File

SOURCE=..\..\include\mesh.h
# End Source File
# Begin Source File

SOURCE=..\..\include\strng.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPAll.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPBool.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPErrorCodes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPIterativeStats.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPList.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPMultipartMessage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPNameClient.h
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

SOURCE=..\..\include\YARPRateThread.h
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

SOURCE=..\..\include\YARPSocketDgram.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPSocketMcast.h
# End Source File
# Begin Source File

SOURCE=..\..\include\YARPSocketMulti.h
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
# Begin Group "Common Files"

# PROP Default_Filter ""
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

SOURCE=..\..\common\wide_nameloc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPAll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPIterativeStats.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPNameClient.cpp
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

SOURCE=..\..\common\YARPScheduler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSocketDgram.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSocketMcast.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPSocketMulti.cpp
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

SOURCE=..\..\common\YARPThread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPThreadAux.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\YARPTime.cpp
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

SOURCE=..\..\sys_include\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\Port.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\RefCounted.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\Sendable.h
# End Source File
# Begin Source File

SOURCE=..\..\sys_include\Sendables.h
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
# Begin Source File

SOURCE=..\..\README.txt
# End Source File
# End Target
# End Project
