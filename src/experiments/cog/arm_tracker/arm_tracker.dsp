# Microsoft Developer Studio Project File - Name="arm_tracker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=arm_tracker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "arm_tracker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "arm_tracker.mak" CFG="arm_tracker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "arm_tracker - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "arm_tracker - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "arm_tracker - Win32 Release"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f arm_tracker.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "arm_tracker.exe"
# PROP BASE Bsc_Name "arm_tracker.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f arm_tracker.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "arm_tracker.exe"
# PROP Bsc_Name "arm_tracker.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "arm_tracker - Win32 Debug"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f arm_tracker.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "arm_tracker.exe"
# PROP BASE Bsc_Name "arm_tracker.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "arm_tracker.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "arm_tracker.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "arm_tracker - Win32 Release"
# Name "arm_tracker - Win32 Debug"

!IF  "$(CFG)" == "arm_tracker - Win32 Release"

!ELSEIF  "$(CFG)" == "arm_tracker - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\arm_tracker.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\YARPTracker.h
# End Source File
# End Group
# End Target
# End Project
