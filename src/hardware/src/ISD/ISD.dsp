# Microsoft Developer Studio Project File - Name="ISD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=ISD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ISD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ISD.mak" CFG="ISD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ISD - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "ISD - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "ISD - Win32 Release"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f ISD.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ISD.exe"
# PROP BASE Bsc_Name "ISD.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f ISD.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "ISD.exe"
# PROP Bsc_Name "ISD.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "ISD - Win32 Debug"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f ISD.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "ISD.exe"
# PROP BASE Bsc_Name "ISD.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "NMAKE /f ISD.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "ISD.exe"
# PROP Bsc_Name "ISD.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "ISD - Win32 Release"
# Name "ISD - Win32 Debug"

!IF  "$(CFG)" == "ISD - Win32 Release"

!ELSEIF  "$(CFG)" == "ISD - Win32 Debug"

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

SOURCE=.\daemon\Makefile
# End Source File
# Begin Source File

SOURCE=.\daemon\sys_msg.c
# End Source File
# Begin Source File

SOURCE=.\daemon\usemsg.txt
# End Source File
# Begin Source File

SOURCE=.\daemon\wraps.c
# End Source File
# End Group
# Begin Group "client"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\client\Makefile
# End Source File
# Begin Source File

SOURCE=.\client\wrappers.cc
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\include\isd-err.h"
# End Source File
# Begin Source File

SOURCE=".\include\isd-int.h"
# End Source File
# Begin Source File

SOURCE=".\include\isd-msg.h"
# End Source File
# Begin Source File

SOURCE=.\include\isd.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\HOW_TO_ADD_FUNCTIONS
# End Source File
# Begin Source File

SOURCE=.\Makefile
# End Source File
# Begin Source File

SOURCE=.\README
# End Source File
# Begin Source File

SOURCE=.\RESETING_THE_GYRO
# End Source File
# End Target
# End Project
