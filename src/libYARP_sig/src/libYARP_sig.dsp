# Microsoft Developer Studio Project File - Name="libYARP_sig" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libYARP_sig - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_sig.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libYARP_sig.mak" CFG="libYARP_sig - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libYARP_sig - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libYARP_sig - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libYARP_sig - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\obj\release"
# PROP Intermediate_Dir "..\obj\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\include\winnt" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_sig.lib"

!ELSEIF  "$(CFG)" == "libYARP_sig - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\obj\debug"
# PROP Intermediate_Dir "..\obj\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\include\winnt" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\winnt\libYARP_sigd.lib"

!ENDIF 

# Begin Target

# Name "libYARP_sig - Win32 Release"
# Name "libYARP_sig - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cvbase.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPImage.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPImageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPImageUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPSimpleOperations.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\yarp\cvbase.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPFilters.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPImage.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPImageDraw.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPImageFile.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPImagePort.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPImagePortContent.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPImages.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPImageUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSimpleOperations.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSound.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPSoundPortContent.h
# End Source File
# Begin Source File

SOURCE=..\include\yarp\YARPVisMap.h
# End Source File
# End Group
# End Target
# End Project
