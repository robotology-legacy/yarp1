# Microsoft Developer Studio Generated NMAKE File, Based on mCAN.dsp
!IF "$(CFG)" == ""
CFG=mCAN - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mCAN - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mCAN - Win32 Release" && "$(CFG)" != "mCAN - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mCAN.mak" CFG="mCAN - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mCAN - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mCAN - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mCAN - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mCAN.lib"


CLEAN :
	-@erase "$(INTDIR)\mCan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mCAN.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mCAN.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mCAN.lib" 
LIB32_OBJS= \
	"$(INTDIR)\mCan.obj"

"$(OUTDIR)\mCAN.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\mCAN.lib"
   copy .\Release\mCAN.lib .
	copy *.h ..\include
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "mCAN - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mCAN.lib"


CLEAN :
	-@erase "$(INTDIR)\mCan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mCAN.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mCAN.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mCAN.lib" 
LIB32_OBJS= \
	"$(INTDIR)\mCan.obj"

"$(OUTDIR)\mCAN.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\mCAN.lib"
   copy .\Debug\mCANd.lib .
	copy *.h ..\include
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mCAN.dep")
!INCLUDE "mCAN.dep"
!ELSE 
!MESSAGE Warning: cannot find "mCAN.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mCAN - Win32 Release" || "$(CFG)" == "mCAN - Win32 Debug"
SOURCE=.\mCan.cpp

"$(INTDIR)\mCan.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

