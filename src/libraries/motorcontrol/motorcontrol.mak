# Microsoft Developer Studio Generated NMAKE File, Based on motorcontrol.dsp
!IF "$(CFG)" == ""
CFG=motorcontrol - Win32 Debug
!MESSAGE No configuration specified. Defaulting to motorcontrol - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "motorcontrol - Win32 Release" && "$(CFG)" != "motorcontrol - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "motorcontrol.mak" CFG="motorcontrol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "motorcontrol - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "motorcontrol - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "motorcontrol - Win32 Release"

OUTDIR=.\obj\Release
INTDIR=.\obj\Release

ALL : ".\lib\winnt\motorcontrol.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\YARPBabybotArm.obj"
	-@erase "$(INTDIR)\YARPBabybotInertialSensor.obj"
	-@erase "$(INTDIR)\YARPTrajectoryGen.obj"
	-@erase ".\lib\winnt\motorcontrol.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\common" /I ".\babybot" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\motorcontrol.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\lib\winnt\motorcontrol.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPTrajectoryGen.obj" \
	"$(INTDIR)\YARPBabybotArm.obj" \
	"$(INTDIR)\YARPBabybotInertialSensor.obj"

".\lib\winnt\motorcontrol.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\lib\winnt\motorcontrol.lib"
   copy .\babybot\*.h ..\..\..\include
	copy .\common\*.h ..\..\..\include
	copy .\lib\winnt\motorcontrol.lib ..\..\..\lib\winnt
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "motorcontrol - Win32 Debug"

OUTDIR=.\obj\Debug
INTDIR=.\obj\Debug

ALL : ".\lib\winnt\motorcontroldb.lib"


CLEAN :
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\YARPBabybotArm.obj"
	-@erase "$(INTDIR)\YARPBabybotInertialSensor.obj"
	-@erase "$(INTDIR)\YARPTrajectoryGen.obj"
	-@erase ".\lib\winnt\motorcontroldb.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

LINK32=link.exe -lib
MTL=midl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\common" /I ".\babybot" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\motorcontrol.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:".\lib\winnt\motorcontroldb.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPTrajectoryGen.obj" \
	"$(INTDIR)\YARPBabybotArm.obj" \
	"$(INTDIR)\YARPBabybotInertialSensor.obj"

".\lib\winnt\motorcontroldb.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"
PostBuild_Desc=installing lib
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : ".\lib\winnt\motorcontroldb.lib"
   copy .\common\*.h ..\..\..\include
	copy .\babybot\*.h ..\..\..\include
	copy .\lib\winnt\motorcontroldb.lib ..\..\..\lib\winnt
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
!IF EXISTS("motorcontrol.dep")
!INCLUDE "motorcontrol.dep"
!ELSE 
!MESSAGE Warning: cannot find "motorcontrol.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "motorcontrol - Win32 Release" || "$(CFG)" == "motorcontrol - Win32 Debug"
SOURCE=.\common\YARPTrajectoryGen.cpp

"$(INTDIR)\YARPTrajectoryGen.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\eurobot\YARPEurobotArm.cpp
SOURCE=.\babybot\YARPBabybotArm.cpp

"$(INTDIR)\YARPBabybotArm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\babybot\YARPBabybotInertialSensor.cpp

"$(INTDIR)\YARPBabybotInertialSensor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

