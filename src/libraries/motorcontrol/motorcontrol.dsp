# Microsoft Developer Studio Project File - Name="motorcontrol" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=motorcontrol - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "motorcontrol.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "motorcontrol.mak" CFG="motorcontrol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "motorcontrol - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "motorcontrol - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "motorcontrol - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\obj\Release"
# PROP Intermediate_Dir ".\obj\Release"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\common" /I ".\babybot" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\lib\winnt\motorcontrol.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\babybot\*.h ..\..\..\include	copy .\common\*.h ..\..\..\include	copy .\lib\winnt\motorcontrol.lib ..\..\..\lib\winnt
# End Special Build Tool

!ELSEIF  "$(CFG)" == "motorcontrol - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\obj\Debug"
# PROP Intermediate_Dir ".\obj\Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\common" /I ".\babybot" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /X /YX
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\lib\winnt\motorcontroldb.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=installing lib
PostBuild_Cmds=copy .\common\*.h ..\..\..\include	copy .\babybot\*.h ..\..\..\include	copy .\lib\winnt\motorcontroldb.lib ..\..\..\lib\winnt
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "motorcontrol - Win32 Release"
# Name "motorcontrol - Win32 Debug"
# Begin Group "common"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\common\YARPControlBoardNetworkData.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPForceSensor.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPGenericControlBoard.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPGenericGrabber.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPGenericSoundGrabber.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPJR3Adapter.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPKinematics.cpp
# End Source File
# Begin Source File

SOURCE=.\common\YARPKinematics.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPTouchBoard.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPTouchBoardParameters.h
# End Source File
# Begin Source File

SOURCE=.\common\YARPTrajectoryGen.cpp
# End Source File
# Begin Source File

SOURCE=.\common\YARPTrajectoryGen.h
# End Source File
# End Group
# Begin Group "eurobot"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\eurobot\YARPEurobotArm.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\eurobot\YARPEurobotArm.h
# End Source File
# Begin Source File

SOURCE=.\eurobot\YARPEurobotGrabber.h
# End Source File
# Begin Source File

SOURCE=.\eurobot\YARPEurobotHead.h
# End Source File
# Begin Source File

SOURCE=.\eurobot\YARPGALILOnEurobotArmAdapter.h
# End Source File
# Begin Source File

SOURCE=.\eurobot\YARPGALILOnEurobotHeadAdapter.h
# End Source File
# Begin Source File

SOURCE=.\eurobot\YARPPicoloOnEurobotAdapter.h
# End Source File
# End Group
# Begin Group "babybot"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\babybot\YARPBabybotArm.cpp
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotArm.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotGrabber.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotHand.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotHandParameters.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotHead.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotHeadKin.cpp
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotHeadKin.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotInertialSensor.cpp
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPBabybotInertialSensor.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPGalilOnBabybotHandAdapter.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPMEIOnBabybotArmAdapter.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPMEIOnBabybotHeadAdapter.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPNIDAQOnBabybotHandAdapter.h
# End Source File
# Begin Source File

SOURCE=.\babybot\YARPPicoloOnBabybotAdapter.h
# End Source File
# End Group
# Begin Group "robotcub"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\robotcub\YARPCanOnRobotcubHeadAdapter.h
# End Source File
# End Group
# End Target
# End Project
