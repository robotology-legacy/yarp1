# Microsoft Developer Studio Generated NMAKE File, Based on tools.dsp
!IF "$(CFG)" == ""
CFG=tools - Win32 FakeIpl Debug
!MESSAGE No configuration specified. Defaulting to tools - Win32 FakeIpl Debug.
!ENDIF 

!IF "$(CFG)" != "tools - Win32 Release" && "$(CFG)" != "tools - Win32 Debug" && "$(CFG)" != "tools - Win32 FakeIpl Debug" && "$(CFG)" != "tools - Win32 FakeIpl Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tools.mak" CFG="tools - Win32 FakeIpl Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tools - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tools - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tools - Win32 FakeIpl Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "tools - Win32 FakeIpl Release" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tools - Win32 Release"

OUTDIR=.\..\obj\Release
INTDIR=.\..\obj\Release
# Begin Custom Macros
OutDir=.\..\obj\Release
# End Custom Macros

ALL : "$(OUTDIR)\tools.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARP3DHistogram.obj"
	-@erase "$(INTDIR)\YARPBlobDetector.obj"
	-@erase "$(INTDIR)\YARPColorConverter.obj"
	-@erase "$(INTDIR)\YARPDIBConverter.obj"
	-@erase "$(INTDIR)\YARPHistoSegmentation.obj"
	-@erase "$(INTDIR)\YARPImageFile.obj"
	-@erase "$(INTDIR)\YARPIntegralImage.obj"
	-@erase "$(INTDIR)\YARPLogpolar.obj"
	-@erase "$(OUTDIR)\tools.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tools.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPBlobDetector.obj" \
	"$(INTDIR)\YARPColorConverter.obj" \
	"$(INTDIR)\YARPDIBConverter.obj" \
	"$(INTDIR)\YARPImageFile.obj" \
	"$(INTDIR)\YARPIntegralImage.obj" \
	"$(INTDIR)\YARPLogpolar.obj" \
	"$(INTDIR)\YARPHistoSegmentation.obj" \
	"$(INTDIR)\YARP3DHistogram.obj"

"$(OUTDIR)\tools.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\obj\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\tools.lib"
   copy .\*.h ..\..\..\..\include
	lib ..\obj\Release\imagesx.lib ..\..\ipl\lib\ipl.lib ..\obj\Release\tools.lib ..\..\..\..\lib\winnt\LogPolarSmallSDK.lib /out:..\obj\Release\images.lib
	copy ..\obj\Release\images.lib ..\..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "tools - Win32 Debug"

OUTDIR=.\..\obj\Debug
INTDIR=.\..\obj\Debug
# Begin Custom Macros
OutDir=.\..\obj\Debug
# End Custom Macros

ALL : "$(OUTDIR)\toolsDB.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARP3DHistogram.obj"
	-@erase "$(INTDIR)\YARPBlobDetector.obj"
	-@erase "$(INTDIR)\YARPColorConverter.obj"
	-@erase "$(INTDIR)\YARPDIBConverter.obj"
	-@erase "$(INTDIR)\YARPHistoSegmentation.obj"
	-@erase "$(INTDIR)\YARPImageFile.obj"
	-@erase "$(INTDIR)\YARPIntegralImage.obj"
	-@erase "$(INTDIR)\YARPLogpolar.obj"
	-@erase "$(OUTDIR)\toolsDB.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\toolsDB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPBlobDetector.obj" \
	"$(INTDIR)\YARPColorConverter.obj" \
	"$(INTDIR)\YARPDIBConverter.obj" \
	"$(INTDIR)\YARPImageFile.obj" \
	"$(INTDIR)\YARPIntegralImage.obj" \
	"$(INTDIR)\YARPLogpolar.obj" \
	"$(INTDIR)\YARPHistoSegmentation.obj" \
	"$(INTDIR)\YARP3DHistogram.obj"

"$(OUTDIR)\toolsDB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\obj\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\toolsDB.lib"
   copy .\*.h ..\..\..\..\include
	lib ..\obj\Debug\imagesDBx.lib ..\..\ipl\lib\ipl.lib ..\obj\Debug\toolsDB.lib ..\..\..\..\lib\winnt\LogPolarSmallSDKDB.lib /out:..\obj\Debug\imagesDB.lib
	copy ..\obj\Debug\imagesDB.lib ..\..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "tools - Win32 FakeIpl Debug"

OUTDIR=.\..\obj\Debug
INTDIR=.\..\obj\Debug
# Begin Custom Macros
OutDir=.\..\obj\Debug
# End Custom Macros

ALL : "$(OUTDIR)\toolsDBf.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARP3DHistogram.obj"
	-@erase "$(INTDIR)\YARPBlobDetector.obj"
	-@erase "$(INTDIR)\YARPColorConverter.obj"
	-@erase "$(INTDIR)\YARPDIBConverter.obj"
	-@erase "$(INTDIR)\YARPHistoSegmentation.obj"
	-@erase "$(INTDIR)\YARPImageFile.obj"
	-@erase "$(INTDIR)\YARPIntegralImage.obj"
	-@erase "$(INTDIR)\YARPLogpolar.obj"
	-@erase "$(OUTDIR)\toolsDBf.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__FAKEIPL__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\toolsDBf.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPBlobDetector.obj" \
	"$(INTDIR)\YARPColorConverter.obj" \
	"$(INTDIR)\YARPDIBConverter.obj" \
	"$(INTDIR)\YARPImageFile.obj" \
	"$(INTDIR)\YARPIntegralImage.obj" \
	"$(INTDIR)\YARPLogpolar.obj" \
	"$(INTDIR)\YARPHistoSegmentation.obj" \
	"$(INTDIR)\YARP3DHistogram.obj"

"$(OUTDIR)\toolsDBf.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\obj\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\toolsDBf.lib"
   copy .\*.h ..\..\..\..\include
	lib ..\obj\Debug\imagesDBf.lib ..\..\ipl\ipl.lib ..\obj\Debug\toolsDBf.lib ..\..\..\..\lib\winnt\LogPolarSmallSDKDB.lib /out:..\obj\Debug\imagesDBf.lib
	copy ..\obj\Debug\imagesDBf.lib ..\..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "tools - Win32 FakeIpl Release"

OUTDIR=.\..\obj\Release
INTDIR=.\..\obj\Release
# Begin Custom Macros
OutDir=.\..\obj\Release
# End Custom Macros

ALL : "$(OUTDIR)\toolsf.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARP3DHistogram.obj"
	-@erase "$(INTDIR)\YARPBlobDetector.obj"
	-@erase "$(INTDIR)\YARPColorConverter.obj"
	-@erase "$(INTDIR)\YARPDIBConverter.obj"
	-@erase "$(INTDIR)\YARPHistoSegmentation.obj"
	-@erase "$(INTDIR)\YARPImageFile.obj"
	-@erase "$(INTDIR)\YARPIntegralImage.obj"
	-@erase "$(INTDIR)\YARPLogpolar.obj"
	-@erase "$(OUTDIR)\toolsf.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__FAKEIPL__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\toolsf.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPBlobDetector.obj" \
	"$(INTDIR)\YARPColorConverter.obj" \
	"$(INTDIR)\YARPDIBConverter.obj" \
	"$(INTDIR)\YARPImageFile.obj" \
	"$(INTDIR)\YARPIntegralImage.obj" \
	"$(INTDIR)\YARPLogpolar.obj" \
	"$(INTDIR)\YARPHistoSegmentation.obj" \
	"$(INTDIR)\YARP3DHistogram.obj"

"$(OUTDIR)\toolsf.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\obj\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\toolsf.lib"
   copy .\*.h ..\..\..\..\include
	lib ..\obj\Release\imagesf.lib ..\..\ipl\ipl.lib ..\obj\Release\toolsf.lib ..\..\..\..\lib\winnt\LogPolarSmallSDK.lib /out:..\obj\Release\imagesf.lib
	copy ..\obj\Release\imagesf.lib ..\..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("tools.dep")
!INCLUDE "tools.dep"
!ELSE 
!MESSAGE Warning: cannot find "tools.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tools - Win32 Release" || "$(CFG)" == "tools - Win32 Debug" || "$(CFG)" == "tools - Win32 FakeIpl Debug" || "$(CFG)" == "tools - Win32 FakeIpl Release"
SOURCE=.\YARP3DHistogram.cpp

"$(INTDIR)\YARP3DHistogram.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPBlobDetector.cpp

"$(INTDIR)\YARPBlobDetector.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPColorConverter.cpp

"$(INTDIR)\YARPColorConverter.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPDIBConverter.cpp

"$(INTDIR)\YARPDIBConverter.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPHistoSegmentation.cpp

"$(INTDIR)\YARPHistoSegmentation.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPImageFile.cpp

"$(INTDIR)\YARPImageFile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPIntegralImage.cpp

"$(INTDIR)\YARPIntegralImage.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPLogpolar.cpp

"$(INTDIR)\YARPLogpolar.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

