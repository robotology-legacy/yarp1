# Microsoft Developer Studio Generated NMAKE File, Based on utils.dsp
!IF "$(CFG)" == ""
CFG=utils - Win32 Debug
!MESSAGE No configuration specified. Defaulting to utils - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "utils - Win32 Release" && "$(CFG)" != "utils - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak" CFG="utils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "utils - Win32 Release"

OUTDIR=.\obj\Release
INTDIR=.\obj\Release

ALL : ".\lib\winnt\utils.lib"


CLEAN :
	-@erase "$(INTDIR)\mbp.obj"
	-@erase "$(INTDIR)\Mm.obj"
	-@erase "$(INTDIR)\Random.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPBottle.obj"
	-@erase "$(INTDIR)\YARPBPNNet.obj"
	-@erase "$(INTDIR)\YARPConfigFile.obj"
	-@erase "$(INTDIR)\YARPFft.obj"
	-@erase "$(INTDIR)\YARPLogFile.obj"
	-@erase "$(INTDIR)\YARPLowPassFilter.obj"
	-@erase "$(INTDIR)\YARPParseParameters.obj"
	-@erase "$(INTDIR)\YARPPidFilter.obj"
	-@erase "$(INTDIR)\YARPRecursiveLS.obj"
	-@erase "$(INTDIR)\YARPTwoDKalmanFilter.obj"
	-@erase ".\lib\winnt\utils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/o"$(OUTDIR)\utils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/out:".\lib\winnt\utils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPBottle.obj" \
	"$(INTDIR)\YARPBPNNet.obj" \
	"$(INTDIR)\YARPConfigFile.obj" \
	"$(INTDIR)\YARPFft.obj" \
	"$(INTDIR)\YARPLogFile.obj" \
	"$(INTDIR)\YARPLowPassFilter.obj" \
	"$(INTDIR)\YARPParseParameters.obj" \
	"$(INTDIR)\YARPPidFilter.obj" \
	"$(INTDIR)\YARPRecursiveLS.obj" \
	"$(INTDIR)\YARPTwoDKalmanFilter.obj" \
	"$(INTDIR)\mbp.obj" \
	"$(INTDIR)\Mm.obj" \
	"$(INTDIR)\Random.obj"

".\lib\winnt\utils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=installing lib
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\lib\winnt\utils.lib"
   copy .\include\*.h ..\..\..\include
	copy .\lib\winnt\utils.lib ..\..\..\lib\winnt\utils.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

OUTDIR=.\obj\Debug
INTDIR=.\obj\Debug

ALL : ".\lib\winnt\utilsdb.lib"


CLEAN :
	-@erase "$(INTDIR)\mbp.obj"
	-@erase "$(INTDIR)\Mm.obj"
	-@erase "$(INTDIR)\Random.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPBottle.obj"
	-@erase "$(INTDIR)\YARPBPNNet.obj"
	-@erase "$(INTDIR)\YARPConfigFile.obj"
	-@erase "$(INTDIR)\YARPFft.obj"
	-@erase "$(INTDIR)\YARPLogFile.obj"
	-@erase "$(INTDIR)\YARPLowPassFilter.obj"
	-@erase "$(INTDIR)\YARPParseParameters.obj"
	-@erase "$(INTDIR)\YARPPidFilter.obj"
	-@erase "$(INTDIR)\YARPRecursiveLS.obj"
	-@erase "$(INTDIR)\YARPTwoDKalmanFilter.obj"
	-@erase ".\lib\winnt\utilsdb.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\utils.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\lib\winnt\utilsdb.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPBottle.obj" \
	"$(INTDIR)\YARPBPNNet.obj" \
	"$(INTDIR)\YARPConfigFile.obj" \
	"$(INTDIR)\YARPFft.obj" \
	"$(INTDIR)\YARPLogFile.obj" \
	"$(INTDIR)\YARPLowPassFilter.obj" \
	"$(INTDIR)\YARPParseParameters.obj" \
	"$(INTDIR)\YARPPidFilter.obj" \
	"$(INTDIR)\YARPRecursiveLS.obj" \
	"$(INTDIR)\YARPTwoDKalmanFilter.obj" \
	"$(INTDIR)\mbp.obj" \
	"$(INTDIR)\Mm.obj" \
	"$(INTDIR)\Random.obj"

".\lib\winnt\utilsdb.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\lib\winnt\utilsdb.lib"
   copy .\include\*.h ..\..\..\include
	copy .\lib\winnt\utilsdb.lib ..\..\..\lib\winnt\utilsdb.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("utils.dep")
!INCLUDE "utils.dep"
!ELSE 
!MESSAGE Warning: cannot find "utils.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "utils - Win32 Release" || "$(CFG)" == "utils - Win32 Debug"
SOURCE=.\src\YARPBottle.cpp

"$(INTDIR)\YARPBottle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPBPNNet.cpp

"$(INTDIR)\YARPBPNNet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPConfigFile.cpp

"$(INTDIR)\YARPConfigFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPFft.cpp

"$(INTDIR)\YARPFft.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPLogFile.cpp

"$(INTDIR)\YARPLogFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPLowPassFilter.cpp

"$(INTDIR)\YARPLowPassFilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPParseParameters.cpp

"$(INTDIR)\YARPParseParameters.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPPidFilter.cpp

"$(INTDIR)\YARPPidFilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPRecursiveLS.cpp

"$(INTDIR)\YARPRecursiveLS.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\YARPTwoDKalmanFilter.cpp

"$(INTDIR)\YARPTwoDKalmanFilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\mbp\mbp.cpp

"$(INTDIR)\mbp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\mbp\Mm.c

"$(INTDIR)\Mm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\mbp\Random.c

"$(INTDIR)\Random.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

