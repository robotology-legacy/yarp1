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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /I "C:\Program Files\Intel\plsuite\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\winnt\images.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	copy .\temporary\*.h ..\..\..\include\temporary
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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /I "C:\Program Files\Intel\plsuite\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\winnt\imagesDB.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	copy .\temporary\*.h ..\..\..\include\temporary
# End Special Build Tool

!ELSEIF  "$(CFG)" == "images - Win32 FakeIpl Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "images___Win32_FakeIpl_Debug"
# PROP BASE Intermediate_Dir "images___Win32_FakeIpl_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj\FakeIpl_Debug"
# PROP Intermediate_Dir "obj\FakeIpl_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /I "C:\Program Files\Intel\plsuite\include" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /I ".\fakeipl\\" /I "C:\Program Files\Intel\plsuite\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /D "__FAKEIPL__" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\winnt\imagesDB.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\winnt\images_fake_DB.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	copy .\temporary\*.h ..\..\..\include\temporary
# End Special Build Tool

!ELSEIF  "$(CFG)" == "images - Win32 FakeIpl Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "images___Win32_FakeIpl_Release"
# PROP BASE Intermediate_Dir "images___Win32_FakeIpl_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\FakeIpl_Release"
# PROP Intermediate_Dir "obj\FakeIpl_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /I "C:\Program Files\Intel\plsuite\include" /D "NDEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /I ".\fakeipl" /I "C:\Program Files\Intel\plsuite\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WIN__" /D "__WIN_MSVC__" /D "__FAKEIPL__" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\lib\winnt\images.lib"
# ADD LIB32 /nologo /out:"..\..\..\lib\winnt\images_fake.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
PostBuild_Cmds=copy .\include\*.h ..\..\..\include	copy .\temporary\*.h ..\..\..\include\temporary
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

SOURCE=.\src\crl_detect_faces.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CRLFaceFinderTool.cpp
# End Source File
# Begin Source File

SOURCE=.\src\face_detect_cog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\features_asym_scan.cpp
# End Source File
# Begin Source File

SOURCE=.\src\iimage_cog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IntegralImageTool.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTconvolve.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTerror.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTklt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTklt_util.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTpnmio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTpyramid.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTselectGoodFeatures.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTstoreFeatures.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTtrackFeatures.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KLTwriteFeatures.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPBinarizeFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPBlobFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPBoxer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPClrHistogram.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPColorConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPColorSaliency.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPColorSegmentation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPDIBConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPDisparity.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPFftFeatures.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPFirstOrderFlow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPFlowVerify.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPGaussianFeatures.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPImage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPImageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPImageUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPITagFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPlogpolar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPLpShifter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPOrientation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPSimpleOperations.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPSkinFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPSusan.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPTagFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPTemporalSmooth.cpp
# End Source File
# Begin Source File

SOURCE=.\src\YARPVelocityField.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\crl_detect_faces.h
# End Source File
# Begin Source File

SOURCE=.\include\CRLFaceFinderTool.h
# End Source File
# Begin Source File

SOURCE=.\include\face_detect_asym.h
# End Source File
# Begin Source File

SOURCE=.\include\IntegralImageTool.h
# End Source File
# Begin Source File

SOURCE=.\include\KLTbase.h
# End Source File
# Begin Source File

SOURCE=.\include\KLTconvolve.h
# End Source File
# Begin Source File

SOURCE=.\include\KLTerror.h
# End Source File
# Begin Source File

SOURCE=.\include\KLTklt.h
# End Source File
# Begin Source File

SOURCE=.\include\KLTklt_util.h
# End Source File
# Begin Source File

SOURCE=.\include\KLTpnmio.h
# End Source File
# Begin Source File

SOURCE=.\include\KLTpyramid.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBinarizeFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBlobFinder.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBlurFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPBoxer.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPClrHistogram.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPColorConverter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPColorSaliency.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPColorSegmentation.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPDIBConverter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPDisparity.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPFftFeatures.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPFilters.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPFirstDerivativeT.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPFirstOrderFlow.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPFlowVerify.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPGaussianFeatures.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImage.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImageDraw.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImageFile.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImageServices.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPImageUtils.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPITagFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPKernelFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPlogpolar.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPLpKernelFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPLpSeparableFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPLpShifter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPOpticFlowBM.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPOrientation.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPSimpleOperations.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPSkinFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPSusan.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPTagFilter.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPTemporalSmooth.h
# End Source File
# Begin Source File

SOURCE=.\include\YARPVelocityField.h
# End Source File
# End Group
# Begin Group "Temporary"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\temporary\YARPConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\temporary\YARPConverter.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\README.txt
# End Source File
# End Target
# End Project
