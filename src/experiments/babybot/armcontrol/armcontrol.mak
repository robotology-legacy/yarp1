# Microsoft Developer Studio Generated NMAKE File, Based on armcontrol.dsp
!IF "$(CFG)" == ""
CFG=armcontrol - Win32 Debug
!MESSAGE No configuration specified. Defaulting to armcontrol - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "armcontrol - Win32 Release" && "$(CFG)" != "armcontrol - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "armcontrol.mak" CFG="armcontrol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "armcontrol - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "armcontrol - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "armcontrol - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\armcontrol.exe"


CLEAN :
	-@erase "$(INTDIR)\ArmBehavior.obj"
	-@erase "$(INTDIR)\armcontrol.obj"
	-@erase "$(INTDIR)\ArmFSMStates.obj"
	-@erase "$(INTDIR)\armthread.obj"
	-@erase "$(INTDIR)\AState.obj"
	-@erase "$(INTDIR)\RandomThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPGravityEstimator.obj"
	-@erase "$(OUTDIR)\armcontrol.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\armcontrol.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontrol.lib math.lib utils.lib alldrivers.lib winmm.lib os_services.lib ipl.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\armcontrol.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\armcontrol.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\ArmBehavior.obj" \
	"$(INTDIR)\armcontrol.obj" \
	"$(INTDIR)\ArmFSMStates.obj" \
	"$(INTDIR)\armthread.obj" \
	"$(INTDIR)\AState.obj" \
	"$(INTDIR)\RandomThread.obj" \
	"$(INTDIR)\YARPGravityEstimator.obj"

"$(OUTDIR)\armcontrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\armcontrol.exe"
   copy .\release\armcontrol.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "armcontrol - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\armcontrol.exe"


CLEAN :
	-@erase "$(INTDIR)\ArmBehavior.obj"
	-@erase "$(INTDIR)\armcontrol.obj"
	-@erase "$(INTDIR)\ArmFSMStates.obj"
	-@erase "$(INTDIR)\armthread.obj"
	-@erase "$(INTDIR)\AState.obj"
	-@erase "$(INTDIR)\RandomThread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPGravityEstimator.obj"
	-@erase "$(OUTDIR)\armcontrol.exe"
	-@erase "$(OUTDIR)\armcontrol.ilk"
	-@erase "$(OUTDIR)\armcontrol.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\armcontrol.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontroldb.lib mathdb.lib utilsdb.lib alldriversdb.lib winmm.lib os_servicesDB.lib ipl.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\armcontrol.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\armcontrol.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\ArmBehavior.obj" \
	"$(INTDIR)\armcontrol.obj" \
	"$(INTDIR)\ArmFSMStates.obj" \
	"$(INTDIR)\armthread.obj" \
	"$(INTDIR)\AState.obj" \
	"$(INTDIR)\RandomThread.obj" \
	"$(INTDIR)\YARPGravityEstimator.obj"

"$(OUTDIR)\armcontrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\armcontrol.exe"
   copy .\debug\armcontrol.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("armcontrol.dep")
!INCLUDE "armcontrol.dep"
!ELSE 
!MESSAGE Warning: cannot find "armcontrol.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "armcontrol - Win32 Release" || "$(CFG)" == "armcontrol - Win32 Debug"
SOURCE=.\ArmBehavior.cpp

"$(INTDIR)\ArmBehavior.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\armcontrol.cpp

"$(INTDIR)\armcontrol.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ArmFSMStates.cpp

"$(INTDIR)\ArmFSMStates.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\armthread.cpp

"$(INTDIR)\armthread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AState.cpp

"$(INTDIR)\AState.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\RandomThread.cpp

"$(INTDIR)\RandomThread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPGravityEstimator.cpp

"$(INTDIR)\YARPGravityEstimator.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

