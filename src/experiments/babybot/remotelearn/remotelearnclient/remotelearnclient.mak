# Microsoft Developer Studio Generated NMAKE File, Based on remotelearnclient.dsp
!IF "$(CFG)" == ""
CFG=remotelearnclient - Win32 Debug
!MESSAGE No configuration specified. Defaulting to remotelearnclient - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "remotelearnclient - Win32 Release" && "$(CFG)" != "remotelearnclient - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "remotelearnclient.mak" CFG="remotelearnclient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "remotelearnclient - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "remotelearnclient - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "remotelearnclient - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\remotelearnclient.exe"


CLEAN :
	-@erase "$(INTDIR)\readwrite.obj"
	-@erase "$(INTDIR)\remotelearnclient.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\remotelearnclient.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\remotelearnclient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib os_services.lib images.lib motorcontrol.lib math.lib utils.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\remotelearnclient.pdb" /machine:I386 /out:"$(OUTDIR)\remotelearnclient.exe" /libpath:"..\..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\readwrite.obj" \
	"$(INTDIR)\remotelearnclient.obj"

"$(OUTDIR)\remotelearnclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\remotelearnclient.exe"
   copy .\release\remotelearnclient.exe ..\..\..\..\..\bin\winnt\remotelearnclient.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "remotelearnclient - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\remotelearnclient.exe"


CLEAN :
	-@erase "$(INTDIR)\readwrite.obj"
	-@erase "$(INTDIR)\remotelearnclient.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\remotelearnclient.exe"
	-@erase "$(OUTDIR)\remotelearnclient.ilk"
	-@erase "$(OUTDIR)\remotelearnclient.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\remotelearnclient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib imagesdb.lib os_servicesdb.lib mathdb.lib motorcontroldb.lib aced.lib utilsdb.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\remotelearnclient.pdb" /debug /machine:I386 /out:"$(OUTDIR)\remotelearnclient.exe" /pdbtype:sept /libpath:"..\..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\readwrite.obj" \
	"$(INTDIR)\remotelearnclient.obj"

"$(OUTDIR)\remotelearnclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=installing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\remotelearnclient.exe"
   copy .\debug\remotelearnclient.exe ..\..\..\..\..\bin\winnt\remotelearnclient.exe
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
!IF EXISTS("remotelearnclient.dep")
!INCLUDE "remotelearnclient.dep"
!ELSE 
!MESSAGE Warning: cannot find "remotelearnclient.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "remotelearnclient - Win32 Release" || "$(CFG)" == "remotelearnclient - Win32 Debug"
SOURCE=.\readwrite.cpp

"$(INTDIR)\readwrite.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\remotelearnclient.cpp

"$(INTDIR)\remotelearnclient.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

