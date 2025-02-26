# Microsoft Developer Studio Project File - Name="TeleCtrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=TeleCtrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TeleCtrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TeleCtrl.mak" CFG="TeleCtrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TeleCtrl - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TeleCtrl - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TeleCtrl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\..\include\winnt" /I "..\..\..\..\include" /I "..\mirrorCollector" /I ".\NR" /I "..\lib_learner" /I "c:\yarp" /I "c:\yarp\include" /I "c:\yarp\include\winnt" /I "c:\yarp\src\experiments" /I "NR" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 lib_learner.lib libYARP_math.lib libYARP_sig.lib libYARP_sig_logpolar.lib libYARP_OS.lib libYARP_dev.lib libYARP_robot.lib ace.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\..\..\lib\winnt"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing release version...
PostBuild_Cmds=copy release\*.exe ..\..\..\..\bin\winnt
# End Special Build Tool

!ELSEIF  "$(CFG)" == "TeleCtrl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\lib_learner" /I "c:\yarp" /I "c:\yarp\include" /I "c:\yarp\include\winnt" /I "c:\yarp\src\experiments" /I "NR" /D "LIBSVM_DEBUG" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 lib_learnerd.lib libYARP_mathd.lib libYARP_sigd.lib libYARP_sig_logpolard.lib libYARP_OSd.lib libYARP_devd.lib libYARP_robotd.lib aced.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\..\lib\winnt"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing debug version...
PostBuild_Cmds=copy debug\*.exe ..\..\..\..\bin\winnt
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "TeleCtrl - Win32 Release"
# Name "TeleCtrl - Win32 Debug"
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ArmPosControlThread.h
# End Source File
# Begin Source File

SOURCE=.\ArmVelControlThread.h
# End Source File
# Begin Source File

SOURCE=.\Calibration.h
# End Source File
# Begin Source File

SOURCE=.\ControlThread.h
# End Source File
# Begin Source File

SOURCE=.\GazeControlThread.h
# End Source File
# Begin Source File

SOURCE=.\HandControlThread.h
# End Source File
# Begin Source File

SOURCE=.\TeleCtrl.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ArmPosControlThread.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmVelControlThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GazeControlThread.cpp
# End Source File
# Begin Source File

SOURCE=.\HandControlThread.cpp
# End Source File
# Begin Source File

SOURCE=.\TeleCtrl.cpp
# End Source File
# End Group
# End Target
# End Project
