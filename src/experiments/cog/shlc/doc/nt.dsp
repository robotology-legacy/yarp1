# Microsoft Developer Studio Project File - Name="nt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=nt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "nt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nt.mak" CFG="nt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nt - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "nt - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "nt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f nt.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "nt.exe"
# PROP BASE Bsc_Name "nt.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f nt.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "nt.exe"
# PROP Bsc_Name "nt.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f nt.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "nt.exe"
# PROP BASE Bsc_Name "nt.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "nt.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "nt.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "nt - Win32 Release"
# Name "nt - Win32 Debug"

!IF  "$(CFG)" == "nt - Win32 Release"

!ELSEIF  "$(CFG)" == "nt - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\classes.cc
# End Source File
# Begin Source File

SOURCE=..\src\commands.cc
# End Source File
# Begin Source File

SOURCE=..\src\file.cc
# End Source File
# Begin Source File

SOURCE=..\src\flow.cc
# End Source File
# Begin Source File

SOURCE=..\src\initialize.cc
# End Source File
# Begin Source File

SOURCE=..\src\secret.cc
# End Source File
# Begin Source File

SOURCE=..\src\secret_command.cc
# End Source File
# Begin Source File

SOURCE=..\src\stats.cc
# End Source File
# Begin Source File

SOURCE=..\src\YARPGoodActions.cc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\classes.h
# End Source File
# Begin Source File

SOURCE=..\src\displayimg.h
# End Source File
# Begin Source File

SOURCE=..\src\portnames.h
# End Source File
# Begin Source File

SOURCE=..\src\shlc.h
# End Source File
# Begin Source File

SOURCE=..\src\YARPGoodActions.h
# End Source File
# Begin Source File

SOURCE=..\src\YARPTracker.h
# End Source File
# End Group
# End Target
# End Project
