# Microsoft Developer Studio Project File - Name="porter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=porter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "porter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "porter.mak" CFG="porter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "porter - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "porter - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "porter - Win32 Release"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f porter.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "porter.exe"
# PROP BASE Bsc_Name "porter.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f porter.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "porter.exe"
# PROP Bsc_Name "porter.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "porter - Win32 Debug"

# PROP BASE Use_MFC
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f porter.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "porter.exe"
# PROP BASE Bsc_Name "porter.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "porter.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "porter.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "porter - Win32 Release"
# Name "porter - Win32 Debug"

!IF  "$(CFG)" == "porter - Win32 Release"

!ELSEIF  "$(CFG)" == "porter - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\porter.cpp
# End Source File
# End Group
# End Target
# End Project
