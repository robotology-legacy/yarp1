# Microsoft Developer Studio Project File - Name="MotorCtrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=MotorCtrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MotorCtrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MotorCtrl.mak" CFG="MotorCtrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MotorCtrl - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "MotorCtrl - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "MotorCtrl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f MotorCtrl.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "MotorCtrl.exe"
# PROP BASE Bsc_Name "MotorCtrl.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "NMAKE /f MotorCtrl.mak"
# PROP Rebuild_Opt "/a"
# PROP Target_File "MotorCtrl.exe"
# PROP Bsc_Name "MotorCtrl.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "MotorCtrl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f MotorCtrl.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "MotorCtrl.exe"
# PROP BASE Bsc_Name "MotorCtrl.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f "MotorCtrl.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "MotorCtrl.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "MotorCtrl - Win32 Release"
# Name "MotorCtrl - Win32 Debug"

!IF  "$(CFG)" == "MotorCtrl - Win32 Release"

!ELSEIF  "$(CFG)" == "MotorCtrl - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\ArmControl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CogGaze.cpp
# End Source File
# Begin Source File

SOURCE=..\src\HeadControl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\YARPMapCameras.cpp
# End Source File
# Begin Source File

SOURCE=..\src\YARPPlanarReaching.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\ArmControl.h
# End Source File
# Begin Source File

SOURCE=..\include\ArmControl.inl
# End Source File
# Begin Source File

SOURCE=..\include\CogGaze.h
# End Source File
# Begin Source File

SOURCE=..\include\HeadControl.h
# End Source File
# Begin Source File

SOURCE=..\include\HeadControl.inl
# End Source File
# Begin Source File

SOURCE=..\include\InertialControl.h
# End Source File
# Begin Source File

SOURCE=..\include\MotorCtrl.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPMapCameras.h
# End Source File
# Begin Source File

SOURCE=..\include\YARPPlanarReaching.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\README
# End Source File
# End Target
# End Project
