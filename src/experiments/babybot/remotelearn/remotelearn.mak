# Microsoft Developer Studio Generated NMAKE File, Based on remotelearn.dsp
!IF "$(CFG)" == ""
CFG=remotelearn - Win32 Debug
!MESSAGE No configuration specified. Defaulting to remotelearn - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "remotelearn - Win32 Release" && "$(CFG)" != "remotelearn - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "remotelearn.mak" CFG="remotelearn - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "remotelearn - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "remotelearn - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "remotelearn - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\remotelearn.exe"


CLEAN :
	-@erase "$(INTDIR)\learner.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\remotelearn.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\remotelearn.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\remotelearn.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=utils.lib math.lib os_services.lib ace.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\remotelearn.pdb" /machine:I386 /out:"$(OUTDIR)\remotelearn.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\learner.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\remotelearn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\remotelearn.exe"
   copy .\Release\remotelearn.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "remotelearn - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\remotelearn.exe"


CLEAN :
	-@erase "$(INTDIR)\learner.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\remotelearn.exe"
	-@erase "$(OUTDIR)\remotelearn.ilk"
	-@erase "$(OUTDIR)\remotelearn.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\remotelearn.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\remotelearn.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=utilsDB.lib mathDB.lib os_servicesDB.lib aced.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\remotelearn.pdb" /debug /machine:I386 /out:"$(OUTDIR)\remotelearn.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\learner.obj" \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\remotelearn.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\remotelearn.exe"
   copy .\Debug\remotelearn.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("remotelearn.dep")
!INCLUDE "remotelearn.dep"
!ELSE 
!MESSAGE Warning: cannot find "remotelearn.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "remotelearn - Win32 Release" || "$(CFG)" == "remotelearn - Win32 Debug"
SOURCE=.\learner.cpp

"$(INTDIR)\learner.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

