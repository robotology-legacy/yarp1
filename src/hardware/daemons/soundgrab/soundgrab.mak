# Microsoft Developer Studio Generated NMAKE File, Based on soundgrab.dsp
!IF "$(CFG)" == ""
CFG=soundgrab - Win32 Debug
!MESSAGE No configuration specified. Defaulting to soundgrab - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "soundgrab - Win32 Release" && "$(CFG)" != "soundgrab - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "soundgrab.mak" CFG="soundgrab - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "soundgrab - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "soundgrab - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "soundgrab - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\soundgrab.exe"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\soundgrab.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32Babybot__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundgrab.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=math.lib utils.lib images.lib os_services.lib alldrivers.lib motorcontrol.lib ace.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\soundgrab.pdb" /machine:I386 /out:"$(OUTDIR)\soundgrab.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\soundgrab.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\soundgrab.exe"
   copy .\Release\soundgrab.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "soundgrab - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\soundgrab.exe"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\soundgrab.exe"
	-@erase "$(OUTDIR)\soundgrab.ilk"
	-@erase "$(OUTDIR)\soundgrab.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__WIN32Babybot__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundgrab.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=mathDB.lib utilsDB.lib imagesDB.lib os_servicesDB.lib alldriversdb.lib motorcontroldb.lib aced.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\soundgrab.pdb" /debug /machine:I386 /out:"$(OUTDIR)\soundgrab.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj"

"$(OUTDIR)\soundgrab.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\soundgrab.exe"
   copy .\Debug\soundgrab.exe ..\..\..\..\bin\winnt
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
!IF EXISTS("soundgrab.dep")
!INCLUDE "soundgrab.dep"
!ELSE 
!MESSAGE Warning: cannot find "soundgrab.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "soundgrab - Win32 Release" || "$(CFG)" == "soundgrab - Win32 Debug"
SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

