# Microsoft Developer Studio Project File - Name="images" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=images - Win32 FakeIpl Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "images.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "images.mak" CFG="images - Win32 FakeIpl Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "images - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "images - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "images - Win32 FakeIpl Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "images - Win32 FakeIpl Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "images - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\Release"
# PROP Intermediate_Dir "obj\Release"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /D "_LIB" /D "NDEBUG" /D "WIN32" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"obj\Release\imagesx.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	lib .\obj\Release\imagesx.lib ..\ipl\lib\ipl.lib /out:.\obj\Release\images.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "images - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\Debug"
# PROP Intermediate_Dir "obj\Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"obj\Debug\imagesDBx.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	lib .\obj\Debug\imagesDBx.lib ..\ipl\lib\ipl.lib /out:.\obj\Debug\imagesDB.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "images - Win32 FakeIpl Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "images___Win32_FakeIpl_Debug"
# PROP BASE Intermediate_Dir "images___Win32_FakeIpl_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\Debug"
# PROP Intermediate_Dir "obj\Debug"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /I "C:\Program Files\Intel\plsuite\include" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /I ".\fakeipl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__FAKEIPL__" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\winnt\imagesDB.lib"
# ADD LIB32 /nologo /out:"obj\Debug\images_fakeipl_DBx.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	lib .\obj\Debug\images_fakeipl_DBx.lib .\fakeipl\fakeiplDB.lib /out:.\obj\Debug\imagesDBf.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "images - Win32 FakeIpl Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "images___Win32_FakeIpl_Release"
# PROP BASE Intermediate_Dir "images___Win32_FakeIpl_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\Release"
# PROP Intermediate_Dir "obj\Release"
# PROP Target_Dir ""
LINK32=link.exe -lib
MTL=midl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /I "C:\Program Files\Intel\plsuite\include" /D "NDEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /I ".\fakeipl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__FAKEIPL__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\winnt\images.lib"
# ADD LIB32 /nologo /out:"obj\Release\images_fakeipl_x.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	lib .\obj\Release\images_fakeipl_x.lib .\fakeipl\fakeipl.lib /out:.\obj\Release\imagesf.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "images - Win32 Release"
# Name "images - Win32 Debug"
# Name "images - Win32 FakeIpl Debug"
# Name "images - Win32 FakeIpl Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\YARPImage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPImageUtils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\YARPFilters.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImage.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImageDraw.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImagePort.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImagePortContent.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImageUtils.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPSound.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPSoundPortContent.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\README.txt
# End Source File
# End Target
# End Project
