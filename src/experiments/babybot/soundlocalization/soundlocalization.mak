# Microsoft Developer Studio Generated NMAKE File, Based on soundlocalization.dsp
!IF "$(CFG)" == ""
CFG=soundlocalization - Win32 Debug
!MESSAGE No configuration specified. Defaulting to soundlocalization - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "soundlocalization - Win32 Release" && "$(CFG)" != "soundlocalization - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "soundlocalization.mak" CFG="soundlocalization - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "soundlocalization - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "soundlocalization - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "soundlocalization - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\soundlocalization.exe"


CLEAN :
	-@erase "$(INTDIR)\ILDBuffer.obj"
	-@erase "$(INTDIR)\ITDBuffer.obj"
	-@erase "$(INTDIR)\soundlocalization.obj"
	-@erase "$(INTDIR)\soundprocessing.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\soundlocalization.exe"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundlocalization.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=math.lib utils.lib winmm.lib os_services.lib ace.lib images.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\soundlocalization.pdb" /machine:I386 /out:"$(OUTDIR)\soundlocalization.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\ILDBuffer.obj" \
	"$(INTDIR)\ITDBuffer.obj" \
	"$(INTDIR)\soundlocalization.obj" \
	"$(INTDIR)\soundprocessing.obj"

"$(OUTDIR)\soundlocalization.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\soundlocalization.exe"
   copy .\Release\soundlocalization.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "soundlocalization - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\soundlocalization.exe"


CLEAN :
	-@erase "$(INTDIR)\ILDBuffer.obj"
	-@erase "$(INTDIR)\ITDBuffer.obj"
	-@erase "$(INTDIR)\soundlocalization.obj"
	-@erase "$(INTDIR)\soundprocessing.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\soundlocalization.exe"
	-@erase "$(OUTDIR)\soundlocalization.ilk"
	-@erase "$(OUTDIR)\soundlocalization.pdb"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\soundlocalization.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=aced.lib motorcontroldb.lib mathdb.lib imagesdb.lib utilsdb.lib alldriversdb.lib winmm.lib os_servicesDB.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\soundlocalization.pdb" /debug /machine:I386 /out:"$(OUTDIR)\soundlocalization.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\ILDBuffer.obj" \
	"$(INTDIR)\ITDBuffer.obj" \
	"$(INTDIR)\soundlocalization.obj" \
	"$(INTDIR)\soundprocessing.obj"

"$(OUTDIR)\soundlocalization.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\soundlocalization.exe"
   copy .\Debug\soundlocalization.exe ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("soundlocalization.dep")
!INCLUDE "soundlocalization.dep"
!ELSE 
!MESSAGE Warning: cannot find "soundlocalization.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "soundlocalization - Win32 Release" || "$(CFG)" == "soundlocalization - Win32 Debug"
SOURCE=.\ILDBuffer.cpp

"$(INTDIR)\ILDBuffer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ITDBuffer.cpp

"$(INTDIR)\ITDBuffer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\soundlocalization.cpp

"$(INTDIR)\soundlocalization.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\soundprocessing.cpp

"$(INTDIR)\soundprocessing.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

