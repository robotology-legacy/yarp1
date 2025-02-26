# Microsoft Developer Studio Project File - Name="libYARP_robot" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libYARP_robot - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_robot.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_robot.mak" CFG="libYARP_robot - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libYARP_robot - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libYARP_robot - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libYARP_robot - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\obj\winnt\release"
# PROP Intermediate_Dir "..\obj\winnt\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\babybot" /I "..\robotcub" /I "..\eurobot" /I "..\..\..\include" /I "..\..\..\include\winnt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_robot.lib"

!ELSEIF  "$(CFG)" == "libYARP_robot - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\winnt\debug"
# PROP Intermediate_Dir "..\obj\winnt\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\babybot" /I "..\robotcub" /I "..\eurobot" /I "..\..\..\include" /I "..\..\..\include\winnt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_robotd.lib"

!ENDIF 

# Begin Target

# Name "libYARP_robot - Win32 Release"
# Name "libYARP_robot - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\YARPKinematics.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPTrajectoryGen.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\yarp\YARPKinematics.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPRobotHardware.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPTrajectoryGen.h
# End Source File
# End Group
# Begin Group "robots"

# PROP Default_Filter ""
# Begin Group "babybot"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotArm.cpp
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotArm.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotForceSensor.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotGrabber.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotHand.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotHandParameters.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotHead.cpp
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotHead.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotHeadKin.cpp
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotHeadKin.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotInertialSensor.cpp
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotInertialSensor.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPBabybotSoundGrabber.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPControlBoardNetworkData.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPGalilOnBabybotHandAdapter.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPJR3Adapter.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPMEIOnBabybotArmAdapter.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPMEIOnBabybotHeadAdapter.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPNIDAQOnBabybotHandAdapter.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPPicoloOnBabybotAdapter.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPSoundOnBabybotAdapter.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPTouchBoard.h
# End Source File
# Begin Source File

SOURCE=..\babybot\yarp\YARPTouchBoardParameters.h
# End Source File
# End Group
# Begin Group "robotcub"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\robotcub\yarp\YARPCanOnRobotcubArmAdapter.h
# End Source File
# Begin Source File

SOURCE=..\robotcub\yarp\YARPCanOnRobotcubHeadAdapter.h
# End Source File
# Begin Source File

SOURCE=..\robotcub\yarp\YARPDragonflyGrabberOnRobotcubAdapter.h
# End Source File
# Begin Source File

SOURCE=..\robotcub\yarp\YARPNullGrabberOnRobotcubAdapter.h
# End Source File
# Begin Source File

SOURCE=..\robotcub\yarp\YARPRobotcubArm.h
# End Source File
# Begin Source File

SOURCE=..\robotcub\yarp\YARPRobotcubGrabber.h
# End Source File
# Begin Source File

SOURCE=..\robotcub\yarp\YARPRobotcubHead.h
# End Source File
# End Group
# Begin Group "null"

# PROP Default_Filter "cpp;h"
# End Group
# Begin Group "eurobot"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\eurobot\yarp\YARPEurobotArm.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPEurobotArm.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPEurobotGrabber.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPEurobotHead.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPEurobotHeadKin.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPEurobotHeadKin.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPGALILOnEurobotArmAdapter.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPGALILOnEurobotHeadAdapter.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\eurobot\yarp\YARPPicoloOnEurobotAdapter.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "mirror"

# PROP Default_Filter "h;cpp"
# Begin Source File

SOURCE=..\mirror\yarp\YARPCyberGloveOnMirrorAdapter.h
# End Source File
# Begin Source File

SOURCE=..\mirror\yarp\YARPFoBOnMirrorAdapter.h
# End Source File
# Begin Source File

SOURCE=..\mirror\yarp\YARPJoyPresOnMirrorAdapter.h
# End Source File
# Begin Source File

SOURCE=..\mirror\yarp\YARPMirrorDataGlove.h
# End Source File
# Begin Source File

SOURCE=..\mirror\yarp\YARPMirrorGrabber.h
# End Source File
# Begin Source File

SOURCE=..\mirror\yarp\YARPMirrorMagneticTracker.h
# End Source File
# Begin Source File

SOURCE=..\mirror\yarp\YARPMirrorPresSens.h
# End Source File
# Begin Source File

SOURCE=..\mirror\yarp\YARPPicoloOnMirrorAdapter.h
# End Source File
# End Group
# Begin Group "obrero"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\obrero\yarp\YARPObreroArm.cpp
# End Source File
# Begin Source File

SOURCE=..\obrero\yarp\YARPObreroArm.h
# End Source File
# Begin Source File

SOURCE=..\obrero\yarp\YARPObreroGrabber.h
# End Source File
# Begin Source File

SOURCE=..\obrero\yarp\YARPSCIOnObreroArmAdapter.h
# End Source File
# End Group
# Begin Group "james"

# PROP Default_Filter "cpp;h"
# Begin Source File

SOURCE=..\james\yarp\YARPCanOnJamesArmAdapter.h
# End Source File
# Begin Source File

SOURCE=..\james\yarp\YARPCanOnJamesHeadAdapter.h
# End Source File
# Begin Source File

SOURCE=..\james\yarp\YARPDragonflyGrabberOnJamesAdapter.h
# End Source File
# Begin Source File

SOURCE=..\james\yarp\YARPJamesArm.h
# End Source File
# Begin Source File

SOURCE=..\james\yarp\YARPJamesGrabber.h
# End Source File
# Begin Source File

SOURCE=..\james\yarp\YARPJamesHead.h
# End Source File
# End Group
# End Group
# End Target
# End Project
