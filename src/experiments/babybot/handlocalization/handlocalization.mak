# Microsoft Developer Studio Generated NMAKE File, Based on handlocalization.dsp
!IF "$(CFG)" == ""
CFG=handlocalization - Win32 Debug
!MESSAGE No configuration specified. Defaulting to handlocalization - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "handlocalization - Win32 Release" && "$(CFG)" != "handlocalization - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "handlocalization.mak" CFG="handlocalization - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "handlocalization - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "handlocalization - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "handlocalization - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\handlocalization.exe"


CLEAN :
	-@erase "$(INTDIR)\findhand.obj"
	-@erase "$(INTDIR)\handlocalization.obj"
	-@erase "$(INTDIR)\hlbehavior.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\zerocrossing.obj"
	-@erase "$(OUTDIR)\handlocalization.exe"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\handlocalization.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=images.lib motorcontrol.lib math.lib utils.lib alldrivers.lib winmm.lib os_services.lib ipl.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\handlocalization.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\handlocalization.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\findhand.obj" \
	"$(INTDIR)\handlocalization.obj" \
	"$(INTDIR)\hlbehavior.obj" \
	"$(INTDIR)\zerocrossing.obj"

"$(OUTDIR)\handlocalization.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\handlocalization.exe"
   copy .\release\handlocalization.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "handlocalization - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\handlocalization.exe"


CLEAN :
	-@erase "$(INTDIR)\findhand.obj"
	-@erase "$(INTDIR)\handlocalization.obj"
	-@erase "$(INTDIR)\hlbehavior.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zerocrossing.obj"
	-@erase "$(OUTDIR)\handlocalization.exe"
	-@erase "$(OUTDIR)\handlocalization.ilk"
	-@erase "$(OUTDIR)\handlocalization.pdb"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\handlocalization.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=imagesdb.lib mathdb.lib motorcontroldb.lib mathdb.lib utilsdb.lib alldriversdb.lib winmm.lib os_servicesDB.lib ipl.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\handlocalization.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\handlocalization.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\findhand.obj" \
	"$(INTDIR)\handlocalization.obj" \
	"$(INTDIR)\hlbehavior.obj" \
	"$(INTDIR)\zerocrossing.obj"

"$(OUTDIR)\handlocalization.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\handlocalization.exe"
   copy .\debug\handlocalization.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("handlocalization.dep")
!INCLUDE "handlocalization.dep"
!ELSE 
!MESSAGE Warning: cannot find "handlocalization.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "handlocalization - Win32 Release" || "$(CFG)" == "handlocalization - Win32 Debug"
SOURCE=.\findhand.cpp

"$(INTDIR)\findhand.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\handlocalization.cpp

"$(INTDIR)\handlocalization.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hlbehavior.cpp

"$(INTDIR)\hlbehavior.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\zerocrossing.cpp

"$(INTDIR)\zerocrossing.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

