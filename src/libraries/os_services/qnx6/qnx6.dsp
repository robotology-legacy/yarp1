# Microsoft Developer Studio Project File - Name="qnx6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=qnx6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qnx6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qnx6.mak" CFG="qnx6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qnx6 - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "qnx6 - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "qnx6 - Win32 Release"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f qnx6.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "qnx6.exe"
# PROP BASE Bsc_Name "qnx6.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "qnx6.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "qnx6.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "qnx6 - Win32 Debug"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f qnx6.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "qnx6.exe"
# PROP BASE Bsc_Name "qnx6.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "qnx6.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "qnx6.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "qnx6 - Win32 Release"
# Name "qnx6 - Win32 Debug"

!IF  "$(CFG)" == "qnx6 - Win32 Release"

!ELSEIF  "$(CFG)" == "qnx6 - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\YARPMultipartMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPNativeNameService.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPNativeSyncComm.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPScheduler.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPThread.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPTime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\begin_pack_for_net.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\BlockPrefix.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\BlockReceiver.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\BlockSender.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\debug.h
# End Source File
# Begin Source File

SOURCE=..\include\end_pack_for_net.h
# End Source File
# Begin Source File

SOURCE=..\include\mesh.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\Port.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\RefCounted.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\Sendable.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\Sendables.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\socklib.h
# End Source File
# Begin Source File

SOURCE=..\include\strng.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\ThreadInput.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\TinySocket.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\wide_nameloc.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPAll.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPBool.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPErrorCodes.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\YARPFragments.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPMultipartMessage.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPNameID.h
# End Source File
# Begin Source File

SOURCE=..\sys_include\YARPNameID_defs.h
# End Source File
# Begin Source File

SOURCE=.\YARPNameManager.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPNameService.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPNativeNameService.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPNativeSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPNetworkTypes.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPPort.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPPortContent.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPRateThread.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPRefCount.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPScheduler.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPSemaphore.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPSocket.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPSocketNameService.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPSocketSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPString.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPSyncComm.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPThread.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPTime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
