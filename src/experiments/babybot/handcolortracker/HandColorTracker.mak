# Microsoft Developer Studio Generated NMAKE File, Based on HandColorTracker.dsp
!IF "$(CFG)" == ""
CFG=HandColorTracker - Win32 Debug
!MESSAGE No configuration specified. Defaulting to HandColorTracker - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HandColorTracker - Win32 Release" && "$(CFG)" != "HandColorTracker - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HandColorTracker.mak" CFG="HandColorTracker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HandColorTracker - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "HandColorTracker - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "HandColorTracker - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\HandColorTracker.exe"


CLEAN :
	-@erase "$(INTDIR)\HandColorTracker.obj"
	-@erase "$(INTDIR)\HandDirectKinematics.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\HandColorTracker.exe"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HandColorTracker.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontrol.lib math.lib utils.lib winmm.lib images.lib ace.lib os_services.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\HandColorTracker.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\HandColorTracker.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\HandColorTracker.obj" \
	"$(INTDIR)\HandDirectKinematics.obj"

"$(OUTDIR)\HandColorTracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\HandColorTracker.exe"
   copy .\release\handcolortracker.exe ..\..\..\..\bin\winnt\handcolortracker.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "HandColorTracker - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\HandColorTracker.exe"


CLEAN :
	-@erase "$(INTDIR)\HandColorTracker.obj"
	-@erase "$(INTDIR)\HandDirectKinematics.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\HandColorTracker.exe"
	-@erase "$(OUTDIR)\HandColorTracker.ilk"
	-@erase "$(OUTDIR)\HandColorTracker.pdb"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HandColorTracker.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=imagesdb.lib mathdb.lib motorcontroldb.lib mathdb.lib utilsdb.lib alldriversdb.lib winmm.lib os_servicesDB.lib ipl.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\HandColorTracker.pdb" /debug /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\HandColorTracker.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\HandColorTracker.obj" \
	"$(INTDIR)\HandDirectKinematics.obj"

"$(OUTDIR)\HandColorTracker.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=install executable
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\HandColorTracker.exe"
   copy .\debug\handcolortracker.exe ..\..\..\..\bin\winnt\handcolortracker.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("HandColorTracker.dep")
!INCLUDE "HandColorTracker.dep"
!ELSE 
!MESSAGE Warning: cannot find "HandColorTracker.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "HandColorTracker - Win32 Release" || "$(CFG)" == "HandColorTracker - Win32 Debug"
SOURCE=.\HandColorTracker.cpp

"$(INTDIR)\HandColorTracker.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\HandDirectKinematics.cpp

"$(INTDIR)\HandDirectKinematics.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

