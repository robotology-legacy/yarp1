# Microsoft Developer Studio Generated NMAKE File, Based on math.dsp
!IF "$(CFG)" == ""
CFG=math - Win32 Debug
!MESSAGE No configuration specified. Defaulting to math - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "math - Win32 Release" && "$(CFG)" != "math - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "math.mak" CFG="math - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "math - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "math - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "math - Win32 Release"

OUTDIR=.\obj\Release
INTDIR=.\obj\Release

ALL : "..\..\..\lib\winnt\math.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPLU.obj"
	-@erase "$(INTDIR)\YARPMatrix.obj"
	-@erase "$(INTDIR)\YARPRobotMath.obj"
	-@erase "$(INTDIR)\YARPSVD.obj"
	-@erase "..\..\..\lib\winnt\math.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\math.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\lib\winnt\math.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPMatrix.obj" \
	"$(INTDIR)\YARPRobotMath.obj" \
	"$(INTDIR)\YARPSVD.obj" \
	"$(INTDIR)\YARPLU.obj"

"..\..\..\lib\winnt\math.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\lib\winnt\math.lib"
   copy *.h ..\..\..\include
	copy *.inl ..\..\..\include
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "math - Win32 Debug"

OUTDIR=.\obj\Debug
INTDIR=.\obj\Debug

ALL : "..\..\..\lib\winnt\mathDB.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPLU.obj"
	-@erase "$(INTDIR)\YARPMatrix.obj"
	-@erase "$(INTDIR)\YARPRobotMath.obj"
	-@erase "$(INTDIR)\YARPSVD.obj"
	-@erase "..\..\..\lib\winnt\mathDB.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\math.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\lib\winnt\mathDB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPMatrix.obj" \
	"$(INTDIR)\YARPRobotMath.obj" \
	"$(INTDIR)\YARPSVD.obj" \
	"$(INTDIR)\YARPLU.obj"

"..\..\..\lib\winnt\mathDB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\..\lib\winnt\mathDB.lib"
   copy *.h ..\..\..\include
	copy *.inl ..\..\..\include
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
!IF EXISTS("math.dep")
!INCLUDE "math.dep"
!ELSE 
!MESSAGE Warning: cannot find "math.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "math - Win32 Release" || "$(CFG)" == "math - Win32 Debug"
SOURCE=.\YARPLU.cpp

"$(INTDIR)\YARPLU.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPMatrix.cpp

"$(INTDIR)\YARPMatrix.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPRobotMath.cpp

"$(INTDIR)\YARPRobotMath.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPSVD.cpp

"$(INTDIR)\YARPSVD.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

