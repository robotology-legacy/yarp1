# Microsoft Developer Studio Generated NMAKE File, Based on behaviorRnd.dsp
!IF "$(CFG)" == ""
CFG=behaviorRnd - Win32 Debug
!MESSAGE No configuration specified. Defaulting to behaviorRnd - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "behaviorRnd - Win32 Release" && "$(CFG)" != "behaviorRnd - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "behaviorRnd.mak" CFG="behaviorRnd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "behaviorRnd - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "behaviorRnd - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "behaviorRnd - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\behaviorRnd.exe"


CLEAN :
	-@erase "$(INTDIR)\behaviorRnd.obj"
	-@erase "$(INTDIR)\rndBehavior.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\behaviorRnd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\behaviorRnd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\behaviorRnd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=utils.lib math.lib os_services.lib ace.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\behaviorRnd.pdb" /machine:I386 /out:"$(OUTDIR)\behaviorRnd.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\behaviorRnd.obj" \
	"$(INTDIR)\rndBehavior.obj"

"$(OUTDIR)\behaviorRnd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=insalling application
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\behaviorRnd.exe"
   copy .\Release\behaviorRnd.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "behaviorRnd - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\behaviorRnd.exe"


CLEAN :
	-@erase "$(INTDIR)\behaviorRnd.obj"
	-@erase "$(INTDIR)\rndBehavior.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\behaviorRnd.exe"
	-@erase "$(OUTDIR)\behaviorRnd.ilk"
	-@erase "$(OUTDIR)\behaviorRnd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\behaviorRnd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\behaviorRnd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=utilsDB.lib mathDB.lib os_servicesDB.lib aced.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\behaviorRnd.pdb" /debug /machine:I386 /out:"$(OUTDIR)\behaviorRnd.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\behaviorRnd.obj" \
	"$(INTDIR)\rndBehavior.obj"

"$(OUTDIR)\behaviorRnd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=installing application
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\behaviorRnd.exe"
   copy .\Debug\behaviorRnd.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("behaviorRnd.dep")
!INCLUDE "behaviorRnd.dep"
!ELSE 
!MESSAGE Warning: cannot find "behaviorRnd.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "behaviorRnd - Win32 Release" || "$(CFG)" == "behaviorRnd - Win32 Debug"
SOURCE=.\behaviorRnd.cpp

"$(INTDIR)\behaviorRnd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rndBehavior.cpp

"$(INTDIR)\rndBehavior.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

