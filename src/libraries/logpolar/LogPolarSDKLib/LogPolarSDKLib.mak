# Microsoft Developer Studio Generated NMAKE File, Based on LogPolarSDKLib.dsp
!IF "$(CFG)" == ""
CFG=LogPolarSDKLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LogPolarSDKLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LogPolarSDKLib - Win32 Release" && "$(CFG)" != "LogPolarSDKLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LogPolarSDKLib.mak" CFG="LogPolarSDKLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LogPolarSDKLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LogPolarSDKLib - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "LogPolarSDKLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\LogPolarSDK.lib"


CLEAN :
	-@erase "$(INTDIR)\Acquisition.obj"
	-@erase "$(INTDIR)\AuxFunctions.obj"
	-@erase "$(INTDIR)\LogPolarSDK.obj"
	-@erase "$(INTDIR)\Mosaic.obj"
	-@erase "$(INTDIR)\TableGeneration.obj"
	-@erase "$(INTDIR)\TableLoading.obj"
	-@erase "$(INTDIR)\TestFunctions.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\LogPolarSDK.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /G6 /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\LogPolarSDKLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogPolarSDKLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LogPolarSDK.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Acquisition.obj" \
	"$(INTDIR)\AuxFunctions.obj" \
	"$(INTDIR)\LogPolarSDK.obj" \
	"$(INTDIR)\Mosaic.obj" \
	"$(INTDIR)\TableGeneration.obj" \
	"$(INTDIR)\TableLoading.obj" \
	"$(INTDIR)\TestFunctions.obj"

"$(OUTDIR)\LogPolarSDK.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LogPolarSDKLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\LogPolarSDKD.lib"


CLEAN :
	-@erase "$(INTDIR)\Acquisition.obj"
	-@erase "$(INTDIR)\AuxFunctions.obj"
	-@erase "$(INTDIR)\LogPolarSDK.obj"
	-@erase "$(INTDIR)\Mosaic.obj"
	-@erase "$(INTDIR)\TableGeneration.obj"
	-@erase "$(INTDIR)\TableLoading.obj"
	-@erase "$(INTDIR)\TestFunctions.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\LogPolarSDKD.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\LogPolarSDKLib" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\LogPolarSDKLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogPolarSDKLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LogPolarSDKD.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Acquisition.obj" \
	"$(INTDIR)\AuxFunctions.obj" \
	"$(INTDIR)\LogPolarSDK.obj" \
	"$(INTDIR)\Mosaic.obj" \
	"$(INTDIR)\TableGeneration.obj" \
	"$(INTDIR)\TableLoading.obj" \
	"$(INTDIR)\TestFunctions.obj"

"$(OUTDIR)\LogPolarSDKD.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

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
!IF EXISTS("LogPolarSDKLib.dep")
!INCLUDE "LogPolarSDKLib.dep"
!ELSE 
!MESSAGE Warning: cannot find "LogPolarSDKLib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LogPolarSDKLib - Win32 Release" || "$(CFG)" == "LogPolarSDKLib - Win32 Debug"
SOURCE=.\Acquisition.cpp

"$(INTDIR)\Acquisition.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AuxFunctions.cpp

"$(INTDIR)\AuxFunctions.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\LogPolarSDK.cpp

"$(INTDIR)\LogPolarSDK.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Mosaic.cpp

"$(INTDIR)\Mosaic.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TableGeneration.cpp

"$(INTDIR)\TableGeneration.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TableLoading.cpp

"$(INTDIR)\TableLoading.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\TestFunctions.cpp

"$(INTDIR)\TestFunctions.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

