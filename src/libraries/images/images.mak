# Microsoft Developer Studio Generated NMAKE File, Based on images.dsp
!IF "$(CFG)" == ""
CFG=images - Win32 FakeIpl Debug
!MESSAGE No configuration specified. Defaulting to images - Win32 FakeIpl Debug.
!ENDIF 

!IF "$(CFG)" != "images - Win32 Release" && "$(CFG)" != "images - Win32 Debug" && "$(CFG)" != "images - Win32 FakeIpl Debug" && "$(CFG)" != "images - Win32 FakeIpl Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "images - Win32 Release"

OUTDIR=.\obj\Release
INTDIR=.\obj\Release
# Begin Custom Macros
OutDir=.\obj\Release
# End Custom Macros

ALL : "$(OUTDIR)\imagesx.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPImage.obj"
	-@erase "$(INTDIR)\YARPImageUtils.obj"
	-@erase "$(OUTDIR)\imagesx.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /D "_LIB" /D "NDEBUG" /D "WIN32" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\images.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\imagesx.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPImage.obj" \
	"$(INTDIR)\YARPImageUtils.obj"

"$(OUTDIR)\imagesx.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\obj\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\imagesx.lib"
   copy .\include\*.h ..\..\..\include
	lib .\obj\Release\imagesx.lib ..\ipl\lib\ipl.lib /out:.\obj\Release\images.lib
	copy .\obj\Release\images.lib ..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "images - Win32 Debug"

OUTDIR=.\obj\Debug
INTDIR=.\obj\Debug
# Begin Custom Macros
OutDir=.\obj\Debug
# End Custom Macros

ALL : "$(OUTDIR)\imagesDBx.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPImage.obj"
	-@erase "$(INTDIR)\YARPImageUtils.obj"
	-@erase "$(OUTDIR)\imagesDBx.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /D "_LIB" /D "_DEBUG" /D "WIN32" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\images.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\imagesDBx.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPImage.obj" \
	"$(INTDIR)\YARPImageUtils.obj"

"$(OUTDIR)\imagesDBx.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\obj\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\imagesDBx.lib"
   copy .\include\*.h ..\..\..\include
	lib .\obj\Debug\imagesDBx.lib ..\ipl\lib\ipl.lib /out:.\obj\Debug\imagesDB.lib
	copy .\obj\Debug\imagesDB.lib ..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "images - Win32 FakeIpl Debug"

OUTDIR=.\obj\Debug
INTDIR=.\obj\Debug
# Begin Custom Macros
OutDir=.\obj\Debug
# End Custom Macros

ALL : "$(OUTDIR)\images_fakeipl_DBx.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPImage.obj"
	-@erase "$(INTDIR)\YARPImageUtils.obj"
	-@erase "$(OUTDIR)\images_fakeipl_DBx.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /I ".\fakeipl" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__FAKEIPL__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\images.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\images_fakeipl_DBx.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPImage.obj" \
	"$(INTDIR)\YARPImageUtils.obj"

"$(OUTDIR)\images_fakeipl_DBx.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\obj\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\images_fakeipl_DBx.lib"
   copy .\include\*.h ..\..\..\include
	lib .\obj\Debug\images_fakeipl_DBx.lib .\fakeipl\fakeiplDB.lib /out:.\obj\Debug\imagesDBf.lib
	copy .\obj\Debug\imagesDBf.lib ..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "images - Win32 FakeIpl Release"

OUTDIR=.\obj\Release
INTDIR=.\obj\Release
# Begin Custom Macros
OutDir=.\obj\Release
# End Custom Macros

ALL : "$(OUTDIR)\images_fakeipl_x.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPImage.obj"
	-@erase "$(INTDIR)\YARPImageUtils.obj"
	-@erase "$(OUTDIR)\images_fakeipl_x.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /I ".\fakeipl" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__FAKEIPL__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\images.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\images_fakeipl_x.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPImage.obj" \
	"$(INTDIR)\YARPImageUtils.obj"

"$(OUTDIR)\images_fakeipl_x.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\obj\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\images_fakeipl_x.lib"
   copy .\include\*.h ..\..\..\include
	lib .\obj\Release\images_fakeipl_x.lib .\fakeipl\fakeipl.lib /out:.\obj\Release\imagesf.lib
	copy .\obj\Release\imagesf.lib ..\..\..\lib\winnt
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
!IF EXISTS("images.dep")
!INCLUDE "images.dep"
!ELSE 
!MESSAGE Warning: cannot find "images.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "images - Win32 Release" || "$(CFG)" == "images - Win32 Debug" || "$(CFG)" == "images - Win32 FakeIpl Debug" || "$(CFG)" == "images - Win32 FakeIpl Release"
SOURCE=.\src\YARPImage.cpp

"$(INTDIR)\YARPImage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPImageUtils.cpp

"$(INTDIR)\YARPImageUtils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

