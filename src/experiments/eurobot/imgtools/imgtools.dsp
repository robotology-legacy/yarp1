# Microsoft Developer Studio Project File - Name="imgtools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=imgtools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "imgtools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "imgtools.mak" CFG="imgtools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "imgtools - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "imgtools - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "imgtools - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\obj\winnt\release"
# PROP Intermediate_Dir ".\obj\winnt\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\..\include" /I "..\..\..\..\include\winnt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\lib\winnt\imgtools.lib"

!ELSEIF  "$(CFG)" == "imgtools - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\obj\winnt\debug"
# PROP Intermediate_Dir ".\obj\winnt\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\..\include" /I "..\..\..\..\include\winnt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\lib\winnt\imgtoolsd.lib"

!ENDIF 

# Begin Target

# Name "imgtools - Win32 Release"
# Name "imgtools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\YARP3DHistogram.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPBlobDetector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPColorConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPConicFitter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPDisparity.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPHistoSegmentation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPImageMoments.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPIntegralImage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\yarp\YARP3DHistogram.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPBlobDetector.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPColorConverter.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPConicFitter.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPDisparity.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPHistoSegmentation.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPImageMoments.h
# End Source File
# Begin Source File

SOURCE=.\include\yarp\YARPIntegralImage.h
# End Source File
# End Group
# End Target
# End Project
