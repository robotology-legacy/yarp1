# Microsoft Developer Studio Generated NMAKE File, Based on alldrivers.dsp
!IF "$(CFG)" == ""
CFG=alldrivers - Win32 Debug
!MESSAGE No configuration specified. Defaulting to alldrivers - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "alldrivers - Win32 Release" && "$(CFG)" != "alldrivers - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "alldrivers.mak" CFG="alldrivers - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "alldrivers - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "alldrivers - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "alldrivers - Win32 Release"

OUTDIR=.\obj\Release
INTDIR=.\obj\Release

ALL : "..\lib\winnt\yarpdrivers.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPAndroidDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPControlBoardUtils.obj"
	-@erase "$(INTDIR)\YARPGalilDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPJR3DeviceDriver.obj"
	-@erase "$(INTDIR)\YARPMEIDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPNIDAQDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPPicoloDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPSoundDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPSoundResources.obj"
	-@erase "..\lib\winnt\yarpdrivers.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\common" /I "..\..\..\include" /I "..\..\..\include\sys" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\alldrivers.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\lib\winnt\yarpdrivers.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPAndroidDeviceDriver.obj" \
	"$(INTDIR)\YARPControlBoardUtils.obj" \
	"$(INTDIR)\YARPGalilDeviceDriver.obj" \
	"$(INTDIR)\YARPJR3DeviceDriver.obj" \
	"$(INTDIR)\YARPMEIDeviceDriver.obj" \
	"$(INTDIR)\YARPNIDAQDeviceDriver.obj" \
	"$(INTDIR)\YARPPicoloDeviceDriver.obj" \
	"$(INTDIR)\YARPSoundDeviceDriver.obj" \
	"$(INTDIR)\YARPSoundResources.obj"

"..\lib\winnt\yarpdrivers.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=installing library
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\lib\winnt\yarpdrivers.lib"
   copy .\mei\winnt\*.h ..\..\..\include
	copy .\galil\common\*.h ..\..\..\include
	copy .\nidaq\winnt\*.h ..\..\..\include
	copy .\picolo\winnt\*.h ..\..\..\include
	copy .\sound\winnt\*.h ..\..\..\include
	copy .\common\*.h ..\..\..\include
	copy .\jr3\winnt\*.h ..\..\..\include
	copy ..\lib\winnt\*.lib ..\..\..\lib\winnt
	copy .\androidworld\common\*.h ..\..\..\include
	make_lib.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "alldrivers - Win32 Debug"

OUTDIR=.\obj\Debug
INTDIR=.\obj\Debug

ALL : "..\lib\winnt\yarpdriversdb.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPAndroidDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPControlBoardUtils.obj"
	-@erase "$(INTDIR)\YARPGalilDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPJR3DeviceDriver.obj"
	-@erase "$(INTDIR)\YARPMEIDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPNIDAQDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPPicoloDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPSoundDeviceDriver.obj"
	-@erase "$(INTDIR)\YARPSoundResources.obj"
	-@erase "..\lib\winnt\yarpdriversdb.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\common" /I "..\..\..\include" /I "..\..\..\include\sys" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\alldrivers.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\alldrivers.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\lib\winnt\yarpdriversdb.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPAndroidDeviceDriver.obj" \
	"$(INTDIR)\YARPControlBoardUtils.obj" \
	"$(INTDIR)\YARPGalilDeviceDriver.obj" \
	"$(INTDIR)\YARPJR3DeviceDriver.obj" \
	"$(INTDIR)\YARPMEIDeviceDriver.obj" \
	"$(INTDIR)\YARPNIDAQDeviceDriver.obj" \
	"$(INTDIR)\YARPPicoloDeviceDriver.obj" \
	"$(INTDIR)\YARPSoundDeviceDriver.obj" \
	"$(INTDIR)\YARPSoundResources.obj"

"..\lib\winnt\yarpdriversdb.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=installing library
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\lib\winnt\yarpdriversdb.lib"
   copy .\mei\winnt\*.h ..\..\..\include
	copy .\galil\common\*.h ..\..\..\include
	copy .\nidaq\winnt\*.h ..\..\..\include
	copy .\picolo\winnt\*.h ..\..\..\include
	copy .\sound\winnt\*.h ..\..\..\include
	copy .\common\*.h ..\..\..\include
	copy .\jr3\winnt\*.h ..\..\..\include
	copy ..\lib\winnt\*.lib ..\..\..\lib\winnt
	copy .\androidworld\common\*.h ..\..\..\include
	make_lib_db.bat
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("alldrivers.dep")
!INCLUDE "alldrivers.dep"
!ELSE 
!MESSAGE Warning: cannot find "alldrivers.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "alldrivers - Win32 Release" || "$(CFG)" == "alldrivers - Win32 Debug"
SOURCE=.\androidworld\common\YARPAndroidDeviceDriver.cpp

"$(INTDIR)\YARPAndroidDeviceDriver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\common\YARPControlBoardUtils.cpp

"$(INTDIR)\YARPControlBoardUtils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\galil\winnt\YARPGalilDeviceDriver.cpp

"$(INTDIR)\YARPGalilDeviceDriver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\jr3\winnt\YARPJR3DeviceDriver.cpp

"$(INTDIR)\YARPJR3DeviceDriver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\mei\winnt\YARPMEIDeviceDriver.cpp

"$(INTDIR)\YARPMEIDeviceDriver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\nidaq\winnt\YARPNIDAQDeviceDriver.cpp

"$(INTDIR)\YARPNIDAQDeviceDriver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\picolo\winnt\YARPPicoloDeviceDriver.cpp

"$(INTDIR)\YARPPicoloDeviceDriver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\sound\winnt\YARPSoundDeviceDriver.cpp

"$(INTDIR)\YARPSoundDeviceDriver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\sound\winnt\YARPSoundResources.cpp

"$(INTDIR)\YARPSoundResources.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

