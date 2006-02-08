# Microsoft Developer Studio Project File - Name="gCamView" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=gCamView - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gCamView.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gCamView.mak" CFG="gCamView - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gCamView - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "gCamView - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gCamView - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(GTK_BASEPATH)\include" /I "$(GTK_BASEPATH)\include\gtk-2.0" /I "$(GTK_BASEPATH)\include\glib-2.0" /I "$(GTK_BASEPATH)\include\pango-1.0" /I "$(GTK_BASEPATH)\include\cairo" /I "$(GTK_BASEPATH)\include\atk-1.0" /I "$(GTK_BASEPATH)\include\gtkglext-1.0" /I "$(GTK_BASEPATH)\include\libglade-2.0" /I "$(GTK_BASEPATH)\include\libxml2" /I "$(GTK_BASEPATH)\lib\gtk-2.0\include" /I "$(GTK_BASEPATH)\lib\glib-2.0\include" /I "$(GTK_BASEPATH)\lib\gtkglext-1.0\include" /I "$(YARP_ROOT)\include" /I "$(YARP_ROOT)\include\winnt" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libYARP_sig_logpolar.lib libYARP_sig.lib libYARP_OS.lib libYARP_dev.lib libYARP_robot.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gtk-win32-2.0.lib gdk-win32-2.0.lib gdk_pixbuf-2.0.lib gobject-2.0.lib glib-2.0.lib winmm.lib /nologo /subsystem:windows /machine:I386 /libpath:"$(GTK_BASEPATH)/lib" /libpath:"$(YARP_ROOT)\lib\winnt"

!ELSEIF  "$(CFG)" == "gCamView - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(GTK_BASEPATH)\include" /I "$(GTK_BASEPATH)\include\gtk-2.0" /I "$(GTK_BASEPATH)\include\glib-2.0" /I "$(GTK_BASEPATH)\include\pango-1.0" /I "$(GTK_BASEPATH)\include\cairo" /I "$(GTK_BASEPATH)\include\atk-1.0" /I "$(GTK_BASEPATH)\include\gtkglext-1.0" /I "$(GTK_BASEPATH)\include\libglade-2.0" /I "$(GTK_BASEPATH)\include\libxml2" /I "$(GTK_BASEPATH)\lib\gtk-2.0\include" /I "$(GTK_BASEPATH)\lib\glib-2.0\include" /I "$(GTK_BASEPATH)\lib\gtkglext-1.0\include" /I "$(YARP_ROOT)\include" /I "$(YARP_ROOT)\include\winnt" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libYARP_sig_logpolard.lib libYARP_sigd.lib libYARP_OSd.lib libYARP_devd.lib libYARP_robotd.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gtk-win32-2.0.lib gdk-win32-2.0.lib gdk_pixbuf-2.0.lib gobject-2.0.lib glib-2.0.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"$(GTK_BASEPATH)/lib" /libpath:"$(YARP_ROOT)\lib\winnt"

!ENDIF 

# Begin Target

# Name "gCamView - Win32 Release"
# Name "gCamView - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gCamView.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPImgRecv.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gCamView.h
# End Source File
# Begin Source File

SOURCE=.\YARPImgRecv.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
