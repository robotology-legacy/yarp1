# Microsoft Developer Studio Generated NMAKE File, Based on visualattention.dsp
!IF "$(CFG)" == ""
CFG=visualattention - Win32 Debug
!MESSAGE No configuration specified. Defaulting to visualattention - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "visualattention - Win32 Release" && "$(CFG)" != "visualattention - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "visualattention.mak" CFG="visualattention - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "visualattention - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "visualattention - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "visualattention - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\visualattention.exe"


CLEAN :
	-@erase "$(INTDIR)\attention.obj"
	-@erase "$(INTDIR)\ImgAtt.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPColorVQ.obj"
	-@erase "$(INTDIR)\YARPConvKernel.obj"
	-@erase "$(INTDIR)\YARPConvKernelFile.obj"
	-@erase "$(INTDIR)\YARPWatershed.obj"
	-@erase "$(OUTDIR)\visualattention.exe"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\visualattention.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontrol.lib utils.lib images.lib os_services.lib math.lib ace.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\visualattention.pdb" /machine:I386 /out:"$(OUTDIR)\visualattention.exe" /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\attention.obj" \
	"$(INTDIR)\ImgAtt.obj" \
	"$(INTDIR)\YARPColorVQ.obj" \
	"$(INTDIR)\YARPConvKernel.obj" \
	"$(INTDIR)\YARPConvKernelFile.obj" \
	"$(INTDIR)\YARPWatershed.obj"

"$(OUTDIR)\visualattention.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\visualattention.exe"
   copy .\Release\visualattention.exe  ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "visualattention - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\visualattention.exe"


CLEAN :
	-@erase "$(INTDIR)\attention.obj"
	-@erase "$(INTDIR)\ImgAtt.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPColorVQ.obj"
	-@erase "$(INTDIR)\YARPConvKernel.obj"
	-@erase "$(INTDIR)\YARPConvKernelFile.obj"
	-@erase "$(INTDIR)\YARPWatershed.obj"
	-@erase "$(OUTDIR)\visualattention.exe"
	-@erase "$(OUTDIR)\visualattention.ilk"
	-@erase "$(OUTDIR)\visualattention.pdb"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\visualattention.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=motorcontrolDB.lib utilsDB.lib imagesDB.lib os_servicesDB.lib mathDB.lib aced.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\visualattention.pdb" /debug /machine:I386 /out:"$(OUTDIR)\visualattention.exe" /pdbtype:sept /libpath:"..\..\..\..\lib\winnt" 
LINK32_OBJS= \
	"$(INTDIR)\attention.obj" \
	"$(INTDIR)\ImgAtt.obj" \
	"$(INTDIR)\YARPColorVQ.obj" \
	"$(INTDIR)\YARPConvKernel.obj" \
	"$(INTDIR)\YARPConvKernelFile.obj" \
	"$(INTDIR)\YARPWatershed.obj"

"$(OUTDIR)\visualattention.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\visualattention.exe"
   copy  .\Debug\visualattention.exe  ..\..\..\..\bin\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("visualattention.dep")
!INCLUDE "visualattention.dep"
!ELSE 
!MESSAGE Warning: cannot find "visualattention.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "visualattention - Win32 Release" || "$(CFG)" == "visualattention - Win32 Debug"
SOURCE=.\attention.cpp

"$(INTDIR)\attention.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ImgAtt.cpp

"$(INTDIR)\ImgAtt.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPColorVQ.cpp

"$(INTDIR)\YARPColorVQ.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPConvKernel.cpp

"$(INTDIR)\YARPConvKernel.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPConvKernelFile.cpp

"$(INTDIR)\YARPConvKernelFile.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\YARPWatershed.cpp

"$(INTDIR)\YARPWatershed.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

