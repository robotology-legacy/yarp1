# Microsoft Developer Studio Generated NMAKE File, Based on LogPolarSmallSDK.dsp
!IF "$(CFG)" == ""
CFG=LogPolarSmallSDK - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LogPolarSmallSDK - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LogPolarSmallSDK - Win32 Release" && "$(CFG)" != "LogPolarSmallSDK - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LogPolarSmallSDK.mak" CFG="LogPolarSmallSDK - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LogPolarSmallSDK - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LogPolarSmallSDK - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "LogPolarSmallSDK - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\LogPolarSmallSDK.lib"


CLEAN :
	-@erase "$(INTDIR)\Acquisition.obj"
	-@erase "$(INTDIR)\AuxFunctions.obj"
	-@erase "$(INTDIR)\LogPolarSDK.obj"
	-@erase "$(INTDIR)\TableGeneration.obj"
	-@erase "$(INTDIR)\TableLoading.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\LogPolarSmallSDK.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogPolarSmallSDK.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LogPolarSmallSDK.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Acquisition.obj" \
	"$(INTDIR)\AuxFunctions.obj" \
	"$(INTDIR)\LogPolarSDK.obj" \
	"$(INTDIR)\TableGeneration.obj" \
	"$(INTDIR)\TableLoading.obj"

"$(OUTDIR)\LogPolarSmallSDK.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\LogPolarSmallSDK.lib"
   copy .\Release\LogPolarSmallSDK.lib ..\..\..\..\lib\winnt
	copy ..\LogPolarSDKLib\LogPolarSDK.h ..\..\..\..\include
	copy ..\Tables\ReferenceImage.bmp ..\..\..\..\conf
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "LogPolarSmallSDK - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\LogPolarSmallSDKDB.lib"


CLEAN :
	-@erase "$(INTDIR)\Acquisition.obj"
	-@erase "$(INTDIR)\AuxFunctions.obj"
	-@erase "$(INTDIR)\LogPolarSDK.obj"
	-@erase "$(INTDIR)\TableGeneration.obj"
	-@erase "$(INTDIR)\TableLoading.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\LogPolarSmallSDKDB.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LogPolarSmallSDK.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\LogPolarSmallSDKDB.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Acquisition.obj" \
	"$(INTDIR)\AuxFunctions.obj" \
	"$(INTDIR)\LogPolarSDK.obj" \
	"$(INTDIR)\TableGeneration.obj" \
	"$(INTDIR)\TableLoading.obj"

"$(OUTDIR)\LogPolarSmallSDKDB.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=Installing library...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\LogPolarSmallSDKDB.lib"
   copy .\Debug\LogPolarSmallSDKDB.lib ..\..\..\..\lib\winnt
	copy ..\LogPolarSDKLib\LogPolarSDK.h ..\..\..\..\include
	copy ..\Tables\ReferenceImage.bmp ..\..\..\..\conf
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("LogPolarSmallSDK.dep")
!INCLUDE "LogPolarSmallSDK.dep"
!ELSE 
!MESSAGE Warning: cannot find "LogPolarSmallSDK.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LogPolarSmallSDK - Win32 Release" || "$(CFG)" == "LogPolarSmallSDK - Win32 Debug"
SOURCE=..\LogPolarSDKLib\Acquisition.cpp

"$(INTDIR)\Acquisition.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LogPolarSDKLib\AuxFunctions.cpp

"$(INTDIR)\AuxFunctions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LogPolarSDKLib\LogPolarSDK.cpp

"$(INTDIR)\LogPolarSDK.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LogPolarSDKLib\TableGeneration.cpp

"$(INTDIR)\TableGeneration.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\LogPolarSDKLib\TableLoading.cpp

"$(INTDIR)\TableLoading.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

