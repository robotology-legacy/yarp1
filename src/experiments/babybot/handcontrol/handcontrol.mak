# Microsoft Developer Studio Generated NMAKE File, Based on handcontrol.dsp
!IF "$(CFG)" == ""
CFG=handcontrol - Win32 Debug
!MESSAGE No configuration specified. Defaulting to handcontrol - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "handcontrol - Win32 Release" && "$(CFG)" != "handcontrol - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "handcontrol.mak" CFG="handcontrol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "handcontrol - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "handcontrol - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "handcontrol - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\handcontrol.exe"


CLEAN :
	-@erase "$(INTDIR)\HandBehavior.obj"
	-@erase "$(INTDIR)\handcontrol.obj"
	-@erase "$(INTDIR)\HandFSMStates.obj"
	-@erase "$(INTDIR)\handthread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\handcontrol.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\handcontrol.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontrol.lib math.lib utils.lib alldrivers.lib winmm.lib os_services.lib ipl.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\handcontrol.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\handcontrol.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\HandBehavior.obj" \
	"$(INTDIR)\handcontrol.obj" \
	"$(INTDIR)\HandFSMStates.obj" \
	"$(INTDIR)\handthread.obj"

"$(OUTDIR)\handcontrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\handcontrol.exe"
   copy .\release\handcontrol.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "handcontrol - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\handcontrol.exe"


CLEAN :
	-@erase "$(INTDIR)\HandBehavior.obj"
	-@erase "$(INTDIR)\handcontrol.obj"
	-@erase "$(INTDIR)\HandFSMStates.obj"
	-@erase "$(INTDIR)\handthread.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\handcontrol.exe"
	-@erase "$(OUTDIR)\handcontrol.ilk"
	-@erase "$(OUTDIR)\handcontrol.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\handcontrol.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mathdb.lib motorcontroldb.lib mathdb.lib utilsdb.lib alldriversdb.lib winmm.lib os_servicesDB.lib ipl.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\handcontrol.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\handcontrol.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\HandBehavior.obj" \
	"$(INTDIR)\handcontrol.obj" \
	"$(INTDIR)\HandFSMStates.obj" \
	"$(INTDIR)\handthread.obj"

"$(OUTDIR)\handcontrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\handcontrol.exe"
   copy .\debug\handcontrol.exe ..\..\..\..\bin\winnt
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
!IF EXISTS("handcontrol.dep")
!INCLUDE "handcontrol.dep"
!ELSE 
!MESSAGE Warning: cannot find "handcontrol.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "handcontrol - Win32 Release" || "$(CFG)" == "handcontrol - Win32 Debug"
SOURCE=.\HandBehavior.cpp

"$(INTDIR)\HandBehavior.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\handcontrol.cpp

"$(INTDIR)\handcontrol.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HandFSMStates.cpp

"$(INTDIR)\HandFSMStates.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\handthread.cpp

"$(INTDIR)\handthread.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

