# Microsoft Developer Studio Generated NMAKE File, Based on YARPNameService.dsp
!IF "$(CFG)" == ""
CFG=YARPNameService - Win32 Debug
!MESSAGE No configuration specified. Defaulting to YARPNameService - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "YARPNameService - Win32 Release" && "$(CFG)" != "YARPNameService - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "YARPNameService.mak" CFG="YARPNameService - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "YARPNameService - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "YARPNameService - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "YARPNameService - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\YARPNameService.exe"


CLEAN :
	-@erase "$(INTDIR)\LocalNameServer.obj"
	-@erase "$(INTDIR)\NetworkMap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPNameServer.obj"
	-@erase "$(INTDIR)\YARPNameService.obj"
	-@erase "$(OUTDIR)\YARPNameService.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "." /I "..\..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\YARPNameService.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=os_services.lib winmm.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\YARPNameService.pdb" /machine:I386 /out:"$(OUTDIR)\YARPNameService.exe" /libpath:"..\..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\LocalNameServer.obj" \
	"$(INTDIR)\NetworkMap.obj" \
	"$(INTDIR)\YARPNameServer.obj" \
	"$(INTDIR)\YARPNameService.obj"

"$(OUTDIR)\YARPNameService.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing name server...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\YARPNameService.exe"
   copy .\Release\YARPNameService.exe ..\..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "YARPNameService - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\YARPNameService.exe"


CLEAN :
	-@erase "$(INTDIR)\LocalNameServer.obj"
	-@erase "$(INTDIR)\NetworkMap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPNameServer.obj"
	-@erase "$(INTDIR)\YARPNameService.obj"
	-@erase "$(OUTDIR)\YARPNameService.exe"
	-@erase "$(OUTDIR)\YARPNameService.ilk"
	-@erase "$(OUTDIR)\YARPNameService.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\YARPNameService.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=os_servicesdb.lib winmm.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\YARPNameService.pdb" /debug /machine:I386 /out:"$(OUTDIR)\YARPNameService.exe" /pdbtype:sept /libpath:"..\..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\LocalNameServer.obj" \
	"$(INTDIR)\NetworkMap.obj" \
	"$(INTDIR)\YARPNameServer.obj" \
	"$(INTDIR)\YARPNameService.obj"

"$(OUTDIR)\YARPNameService.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing name server...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\YARPNameService.exe"
   copy .\Debug\YARPNameService.exe ..\..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("YARPNameService.dep")
!INCLUDE "YARPNameService.dep"
!ELSE 
!MESSAGE Warning: cannot find "YARPNameService.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "YARPNameService - Win32 Release" || "$(CFG)" == "YARPNameService - Win32 Debug"
SOURCE=.\LocalNameServer.cpp

"$(INTDIR)\LocalNameServer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\NetworkMap.cpp

"$(INTDIR)\NetworkMap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPNameServer.cpp

"$(INTDIR)\YARPNameServer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPNameService.cpp

"$(INTDIR)\YARPNameService.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

