# Microsoft Developer Studio Generated NMAKE File, Based on os_services.dsp
!IF "$(CFG)" == ""
CFG=os_services - Win32 Debug
!MESSAGE No configuration specified. Defaulting to os_services - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "os_services - Win32 Release" && "$(CFG)" != "os_services - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "os_services.mak" CFG="os_services - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "os_services - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "os_services - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "os_services - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\os_services.lib"


CLEAN :
	-@erase "$(INTDIR)\BlockReceiver.obj"
	-@erase "$(INTDIR)\BlockSender.obj"
	-@erase "$(INTDIR)\mesh.obj"
	-@erase "$(INTDIR)\Port.obj"
	-@erase "$(INTDIR)\RefCounted.obj"
	-@erase "$(INTDIR)\Sendable.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wide_nameloc.obj"
	-@erase "$(INTDIR)\YARPAll.obj"
	-@erase "$(INTDIR)\YARPIterativeStats.obj"
	-@erase "$(INTDIR)\YARPMultipartMessage.obj"
	-@erase "$(INTDIR)\YARPNameClient.obj"
	-@erase "$(INTDIR)\YARPNameService.obj"
	-@erase "$(INTDIR)\YARPNativeNameService.obj"
	-@erase "$(INTDIR)\YARPPort.obj"
	-@erase "$(INTDIR)\YARPRefCount.obj"
	-@erase "$(INTDIR)\YARPScheduler.obj"
	-@erase "$(INTDIR)\YARPSemaphore.obj"
	-@erase "$(INTDIR)\YARPSocket.obj"
	-@erase "$(INTDIR)\YARPSocketDgram.obj"
	-@erase "$(INTDIR)\YARPSocketMcast.obj"
	-@erase "$(INTDIR)\YARPSocketMulti.obj"
	-@erase "$(INTDIR)\YARPSocketNameService.obj"
	-@erase "$(INTDIR)\YARPSocketSyncComm.obj"
	-@erase "$(INTDIR)\YARPSyncComm.obj"
	-@erase "$(INTDIR)\YARPThread.obj"
	-@erase "$(INTDIR)\YARPThreadAux.obj"
	-@erase "$(INTDIR)\YARPTime.obj"
	-@erase "$(OUTDIR)\os_services.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "../../include" /I "../../sys_include" /I "../../../../../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\os_services.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\os_services.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPMultipartMessage.obj" \
	"$(INTDIR)\YARPNativeNameService.obj" \
	"$(INTDIR)\BlockReceiver.obj" \
	"$(INTDIR)\BlockSender.obj" \
	"$(INTDIR)\mesh.obj" \
	"$(INTDIR)\Port.obj" \
	"$(INTDIR)\RefCounted.obj" \
	"$(INTDIR)\Sendable.obj" \
	"$(INTDIR)\wide_nameloc.obj" \
	"$(INTDIR)\YARPAll.obj" \
	"$(INTDIR)\YARPIterativeStats.obj" \
	"$(INTDIR)\YARPNameClient.obj" \
	"$(INTDIR)\YARPNameService.obj" \
	"$(INTDIR)\YARPPort.obj" \
	"$(INTDIR)\YARPRefCount.obj" \
	"$(INTDIR)\YARPScheduler.obj" \
	"$(INTDIR)\YARPSemaphore.obj" \
	"$(INTDIR)\YARPSocket.obj" \
	"$(INTDIR)\YARPSocketDgram.obj" \
	"$(INTDIR)\YARPSocketMcast.obj" \
	"$(INTDIR)\YARPSocketMulti.obj" \
	"$(INTDIR)\YARPSocketNameService.obj" \
	"$(INTDIR)\YARPSocketSyncComm.obj" \
	"$(INTDIR)\YARPSyncComm.obj" \
	"$(INTDIR)\YARPThread.obj" \
	"$(INTDIR)\YARPThreadAux.obj" \
	"$(INTDIR)\YARPTime.obj"

"$(OUTDIR)\os_services.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\os_services.lib"
   COPY Release\os_services.lib ..\..\lib\winnt\os_services.lib
	COPY Release\os_services.lib ..\..\..\..\..\lib\winnt\os_services.lib
	COPY ..\..\include\*.h ..\..\..\..\..\include
	COPY ..\..\sys_include\*.h ..\..\..\..\..\include
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "os_services - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\os_services.lib"


CLEAN :
	-@erase "$(INTDIR)\BlockReceiver.obj"
	-@erase "$(INTDIR)\BlockSender.obj"
	-@erase "$(INTDIR)\mesh.obj"
	-@erase "$(INTDIR)\Port.obj"
	-@erase "$(INTDIR)\RefCounted.obj"
	-@erase "$(INTDIR)\Sendable.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wide_nameloc.obj"
	-@erase "$(INTDIR)\YARPAll.obj"
	-@erase "$(INTDIR)\YARPIterativeStats.obj"
	-@erase "$(INTDIR)\YARPMultipartMessage.obj"
	-@erase "$(INTDIR)\YARPNameClient.obj"
	-@erase "$(INTDIR)\YARPNameService.obj"
	-@erase "$(INTDIR)\YARPNativeNameService.obj"
	-@erase "$(INTDIR)\YARPPort.obj"
	-@erase "$(INTDIR)\YARPRefCount.obj"
	-@erase "$(INTDIR)\YARPScheduler.obj"
	-@erase "$(INTDIR)\YARPSemaphore.obj"
	-@erase "$(INTDIR)\YARPSocket.obj"
	-@erase "$(INTDIR)\YARPSocketDgram.obj"
	-@erase "$(INTDIR)\YARPSocketMcast.obj"
	-@erase "$(INTDIR)\YARPSocketMulti.obj"
	-@erase "$(INTDIR)\YARPSocketNameService.obj"
	-@erase "$(INTDIR)\YARPSocketSyncComm.obj"
	-@erase "$(INTDIR)\YARPSyncComm.obj"
	-@erase "$(INTDIR)\YARPThread.obj"
	-@erase "$(INTDIR)\YARPThreadAux.obj"
	-@erase "$(INTDIR)\YARPTime.obj"
	-@erase "$(OUTDIR)\os_services.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

MTL=midl.exe
LINK32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../sys_include" /I "../../../../../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\os_services.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\os_services.lib" 
LIB32_OBJS= \
	"$(INTDIR)\YARPMultipartMessage.obj" \
	"$(INTDIR)\YARPNativeNameService.obj" \
	"$(INTDIR)\BlockReceiver.obj" \
	"$(INTDIR)\BlockSender.obj" \
	"$(INTDIR)\mesh.obj" \
	"$(INTDIR)\Port.obj" \
	"$(INTDIR)\RefCounted.obj" \
	"$(INTDIR)\Sendable.obj" \
	"$(INTDIR)\wide_nameloc.obj" \
	"$(INTDIR)\YARPAll.obj" \
	"$(INTDIR)\YARPIterativeStats.obj" \
	"$(INTDIR)\YARPNameClient.obj" \
	"$(INTDIR)\YARPNameService.obj" \
	"$(INTDIR)\YARPPort.obj" \
	"$(INTDIR)\YARPRefCount.obj" \
	"$(INTDIR)\YARPScheduler.obj" \
	"$(INTDIR)\YARPSemaphore.obj" \
	"$(INTDIR)\YARPSocket.obj" \
	"$(INTDIR)\YARPSocketDgram.obj" \
	"$(INTDIR)\YARPSocketMcast.obj" \
	"$(INTDIR)\YARPSocketMulti.obj" \
	"$(INTDIR)\YARPSocketNameService.obj" \
	"$(INTDIR)\YARPSocketSyncComm.obj" \
	"$(INTDIR)\YARPSyncComm.obj" \
	"$(INTDIR)\YARPThread.obj" \
	"$(INTDIR)\YARPThreadAux.obj" \
	"$(INTDIR)\YARPTime.obj"

"$(OUTDIR)\os_services.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\os_services.lib"
   COPY Debug\os_services.lib ..\..\lib\winnt\os_servicesDB.lib
	COPY Debug\os_services.lib ..\..\..\..\..\lib\winnt\os_servicesDB.lib
	COPY ..\..\include\*.h ..\..\..\..\..\include
	COPY ..\..\sys_include\*.h ..\..\..\..\..\include
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
!IF EXISTS("os_services.dep")
!INCLUDE "os_services.dep"
!ELSE 
!MESSAGE Warning: cannot find "os_services.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "os_services - Win32 Release" || "$(CFG)" == "os_services - Win32 Debug"
SOURCE=..\..\winnt\YARPMultipartMessage.cpp

"$(INTDIR)\YARPMultipartMessage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\winnt\YARPNativeNameService.cpp

"$(INTDIR)\YARPNativeNameService.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\BlockReceiver.cpp

"$(INTDIR)\BlockReceiver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\BlockSender.cpp

"$(INTDIR)\BlockSender.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\mesh.cpp

"$(INTDIR)\mesh.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\Port.cpp

"$(INTDIR)\Port.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\RefCounted.cpp

"$(INTDIR)\RefCounted.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\Sendable.cpp

"$(INTDIR)\Sendable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\wide_nameloc.cpp

"$(INTDIR)\wide_nameloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPAll.cpp

"$(INTDIR)\YARPAll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPIterativeStats.cpp

"$(INTDIR)\YARPIterativeStats.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPNameClient.cpp

"$(INTDIR)\YARPNameClient.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPNameService.cpp

"$(INTDIR)\YARPNameService.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPPort.cpp

"$(INTDIR)\YARPPort.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPRefCount.cpp

"$(INTDIR)\YARPRefCount.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPScheduler.cpp

"$(INTDIR)\YARPScheduler.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSemaphore.cpp

"$(INTDIR)\YARPSemaphore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSocket.cpp

"$(INTDIR)\YARPSocket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSocketDgram.cpp

"$(INTDIR)\YARPSocketDgram.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSocketMcast.cpp

"$(INTDIR)\YARPSocketMcast.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSocketMulti.cpp

"$(INTDIR)\YARPSocketMulti.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSocketNameService.cpp

"$(INTDIR)\YARPSocketNameService.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSocketSyncComm.cpp

"$(INTDIR)\YARPSocketSyncComm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPSyncComm.cpp

"$(INTDIR)\YARPSyncComm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPThread.cpp

"$(INTDIR)\YARPThread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPThreadAux.cpp

"$(INTDIR)\YARPThreadAux.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\common\YARPTime.cpp

"$(INTDIR)\YARPTime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

