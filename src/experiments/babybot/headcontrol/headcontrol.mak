# Microsoft Developer Studio Generated NMAKE File, Based on headcontrol.dsp
!IF "$(CFG)" == ""
CFG=headcontrol - Win32 Debug
!MESSAGE No configuration specified. Defaulting to headcontrol - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "headcontrol - Win32 Release" && "$(CFG)" != "headcontrol - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "headcontrol.mak" CFG="headcontrol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "headcontrol - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "headcontrol - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "headcontrol - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\headcontrol.exe"


CLEAN :
	-@erase "$(INTDIR)\HeadBehavior.obj"
	-@erase "$(INTDIR)\headcontrol.obj"
	-@erase "$(INTDIR)\HeadThread.obj"
	-@erase "$(INTDIR)\HState.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\headcontrol.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\headcontrol.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontrol.lib math.lib utils.lib alldrivers.lib winmm.lib os_services.lib ipl.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\headcontrol.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /out:"$(OUTDIR)\headcontrol.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\HeadBehavior.obj" \
	"$(INTDIR)\headcontrol.obj" \
	"$(INTDIR)\HeadThread.obj" \
	"$(INTDIR)\HState.obj"

"$(OUTDIR)\headcontrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\headcontrol.exe"
   copy .\Release\headcontrol.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "headcontrol - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\headcontrol.exe"


CLEAN :
	-@erase "$(INTDIR)\HeadBehavior.obj"
	-@erase "$(INTDIR)\headcontrol.obj"
	-@erase "$(INTDIR)\HeadThread.obj"
	-@erase "$(INTDIR)\HState.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\headcontrol.exe"
	-@erase "$(OUTDIR)\headcontrol.ilk"
	-@erase "$(OUTDIR)\headcontrol.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\headcontrol.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontroldb.lib mathdb.lib utilsdb.lib alldriversdb.lib winmm.lib os_servicesDB.lib ipl.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\headcontrol.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /out:"$(OUTDIR)\headcontrol.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\HeadBehavior.obj" \
	"$(INTDIR)\headcontrol.obj" \
	"$(INTDIR)\HeadThread.obj" \
	"$(INTDIR)\HState.obj"

"$(OUTDIR)\headcontrol.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\headcontrol.exe"
   copy .\Debug\headcontrol.exe ..\..\..\..\bin\winnt
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
!IF EXISTS("headcontrol.dep")
!INCLUDE "headcontrol.dep"
!ELSE 
!MESSAGE Warning: cannot find "headcontrol.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "headcontrol - Win32 Release" || "$(CFG)" == "headcontrol - Win32 Debug"
SOURCE=.\HeadBehavior.cpp

"$(INTDIR)\HeadBehavior.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\headcontrol.cpp

"$(INTDIR)\headcontrol.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HeadThread.cpp

"$(INTDIR)\HeadThread.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HState.cpp

"$(INTDIR)\HState.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

