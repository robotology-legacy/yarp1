# Microsoft Developer Studio Project File - Name="ueid2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=ueid2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ueid2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ueid2.mak" CFG="ueid2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ueid2 - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "ueid2 - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "ueid2 - Win32 Release"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f ueid2.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ueid2.exe"
# PROP BASE Bsc_Name "ueid2.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f ueid2.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "ueid2.exe"
# PROP Bsc_Name "ueid2.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "ueid2 - Win32 Debug"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f ueid2.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ueid2.exe"
# PROP BASE Bsc_Name "ueid2.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "ueid2.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "ueid2.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "ueid2 - Win32 Release"
# Name "ueid2 - Win32 Debug"

!IF  "$(CFG)" == "ueid2 - Win32 Release"

!ELSEIF  "$(CFG)" == "ueid2 - Win32 Debug"

!ENDIF 

# Begin Group "daemon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\daemon\extern.h
# End Source File
# Begin Source File

SOURCE=.\daemon\init.c
# End Source File
# Begin Source File

SOURCE=.\daemon\main.c
# End Source File
# Begin Source File

SOURCE=.\daemon\sys_msg.c
# End Source File
# Begin Source File

SOURCE=.\daemon\wraps.c
# End Source File
# End Group
# Begin Group "client"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\client\wrappers.c
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\include\pd-int.h"
# End Source File
# Begin Source File

SOURCE=.\include\pdfw_def.h
# End Source File
# Begin Source File

SOURCE=.\include\pdfw_if.h
# End Source File
# Begin Source File

SOURCE=.\include\pdpcidef.h
# End Source File
# Begin Source File

SOURCE=.\include\powerdaq.h
# End Source File
# Begin Source File

SOURCE=".\include\ueid-err.h"
# End Source File
# Begin Source File

SOURCE=".\include\ueid-int.h"
# End Source File
# Begin Source File

SOURCE=".\include\ueid-msg.h"
# End Source File
# Begin Source File

SOURCE=.\include\ueid.h
# End Source File
# Begin Source File

SOURCE=.\include\win2qnx.h
# End Source File
# End Group
# Begin Group "test"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\test\init.c
# End Source File
# End Group
# End Target
# End Project
