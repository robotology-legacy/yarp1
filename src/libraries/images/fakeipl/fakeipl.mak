# Microsoft Developer Studio Generated NMAKE File, Based on fakeipl.dsp
!IF "$(CFG)" == ""
CFG=fakeipl - Win32 Debug
!MESSAGE No configuration specified. Defaulting to fakeipl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "fakeipl - Win32 Release" && "$(CFG)" != "fakeipl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "fakeipl.mak" CFG="fakeipl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "fakeipl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fakeipl - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "fakeipl - Win32 Release"

OUTDIR=.\..\obj\Release
INTDIR=.\..\obj\Release

ALL : ".\fakeipl.lib"


CLEAN :
	-@erase "$(INTDIR)\FakeIpl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\fakeipl.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fakeipl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\fakeipl.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FakeIpl.obj"

".\fakeipl.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\fakeipl.lib"
   copy *.h ..\..\..\..\include
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "fakeipl - Win32 Debug"

OUTDIR=.\..\obj\Debug
INTDIR=.\..\obj\Debug

ALL : ".\fakeiplDB.lib"


CLEAN :
	-@erase "$(INTDIR)\FakeIpl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase ".\fakeiplDB.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\fakeipl.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\fakeiplDB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FakeIpl.obj"

".\fakeiplDB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\fakeiplDB.lib"
   copy *.h ..\..\..\..\include
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
!IF EXISTS("fakeipl.dep")
!INCLUDE "fakeipl.dep"
!ELSE 
!MESSAGE Warning: cannot find "fakeipl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "fakeipl - Win32 Release" || "$(CFG)" == "fakeipl - Win32 Debug"
SOURCE=.\FakeIpl.cpp

"$(INTDIR)\FakeIpl.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

