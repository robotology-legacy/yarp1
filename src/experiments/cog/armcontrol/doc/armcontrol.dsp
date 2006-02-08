# Microsoft Developer Studio Project File - Name="armcontrol" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=armcontrol - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "armcontrol.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "armcontrol.mak" CFG="armcontrol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "armcontrol - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "armcontrol - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "armcontrol - Win32 Release"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f armcontrol.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "armcontrol.exe"
# PROP BASE Bsc_Name "armcontrol.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f armcontrol.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "armcontrol.exe"
# PROP Bsc_Name "armcontrol.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "armcontrol - Win32 Debug"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f armcontrol.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "armcontrol.exe"
# PROP BASE Bsc_Name "armcontrol.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "armcontrol.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "armcontrol.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "armcontrol - Win32 Release"
# Name "armcontrol - Win32 Debug"

!IF  "$(CFG)" == "armcontrol - Win32 Release"

!ELSEIF  "$(CFG)" == "armcontrol - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\commands.cc
# End Source File
# Begin Source File

SOURCE=..\src\initialize.cc
# End Source File
# Begin Source File

SOURCE=..\src\terminal.cc
# End Source File
# Begin Source File

SOURCE=..\src\trend.cc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\armcontrol.h
# End Source File
# Begin Source File

SOURCE=..\src\classes.h
# End Source File
# End Group
# End Target
# End Project
