# Microsoft Developer Studio Project File - Name="hmm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=hmm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hmm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hmm.mak" CFG="hmm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hmm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "hmm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hmm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj/Release"
# PROP Intermediate_Dir "obj/Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\winnt\hmm.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing lib
PostBuild_Cmds=copy *.h ..\..\..\include
# End Special Build Tool

!ELSEIF  "$(CFG)" == "hmm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj/Debug"
# PROP Intermediate_Dir "obj/Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\winnt\hmmDB.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Installing lib
PostBuild_Cmds=copy *.h ..\..\..\include
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "hmm - Win32 Release"
# Name "hmm - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\YARPGaussianMixture.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPHmm.cpp
# End Source File
# Begin Source File

SOURCE=.\YARPHmmMath.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\YARPGaussianMixture.h
# End Source File
# Begin Source File

SOURCE=.\YARPHmm.h
# End Source File
# Begin Source File

SOURCE=.\YARPHmmMath.h
# End Source File
# Begin Source File

SOURCE=.\YARPRandomNumber.h
# End Source File
# End Group
# Begin Group "Matlab Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\matlab\approxeq.m
# End Source File
# Begin Source File

SOURCE=.\matlab\assert.m
# End Source File
# Begin Source File

SOURCE=.\matlab\consist.m
# End Source File
# Begin Source File

SOURCE=.\matlab\dist2.m
# End Source File
# Begin Source File

SOURCE=.\matlab\em_converged.m
# End Source File
# Begin Source File

SOURCE=.\matlab\enumerate_loglik.m
# End Source File
# Begin Source File

SOURCE=.\matlab\fhmm_infer.m
# End Source File
# Begin Source File

SOURCE=.\matlab\fixed_lag_demo.m
# End Source File
# Begin Source File

SOURCE=.\matlab\fixed_lag_smoother.m
# End Source File
# Begin Source File

SOURCE=.\matlab\forwards.m
# End Source File
# Begin Source File

SOURCE=.\matlab\forwards_backwards.m
# End Source File
# Begin Source File

SOURCE=.\matlab\forwards_backwards_mix.m
# End Source File
# Begin Source File

SOURCE=.\matlab\forwards_backwards_oneslice.m
# End Source File
# Begin Source File

SOURCE=.\matlab\forwards_backwards_pomdp.m
# End Source File
# Begin Source File

SOURCE=.\matlab\gaussian_prob.m
# End Source File
# Begin Source File

SOURCE=.\matlab\gmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\gmminit.m
# End Source File
# Begin Source File

SOURCE=.\matlab\gsamp.m
# End Source File
# Begin Source File

SOURCE=.\matlab\init_mhmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\isvector.m
# End Source File
# Begin Source File

SOURCE=.\matlab\kmeans.m
# End Source File
# Begin Source File

SOURCE=.\matlab\learn_dhmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\learn_dhmm_demo.m
# End Source File
# Begin Source File

SOURCE=.\matlab\learn_ghmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\learn_mhmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\learn_mhmm_demo.m
# End Source File
# Begin Source File

SOURCE=.\matlab\log_lik_dhmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\log_lik_ghmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\log_lik_mhmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\mk_dhmm_obs_lik.m
# End Source File
# Begin Source File

SOURCE=.\matlab\mk_fhmm_topology.m
# End Source File
# Begin Source File

SOURCE=.\matlab\mk_ghmm_obs_lik.m
# End Source File
# Begin Source File

SOURCE=.\matlab\mk_mhmm_obs_lik.m
# End Source File
# Begin Source File

SOURCE=.\matlab\mk_stochastic.m
# End Source File
# Begin Source File

SOURCE=.\matlab\normalise.m
# End Source File
# Begin Source File

SOURCE=.\matlab\online_em.m
# End Source File
# Begin Source File

SOURCE=.\matlab\online_em_demo.m
# End Source File
# Begin Source File

SOURCE=.\matlab\prob_path.m
# End Source File
# Begin Source File

SOURCE=.\matlab\README
# End Source File
# Begin Source File

SOURCE=.\matlab\sample_dhmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\sample_discrete.m
# End Source File
# Begin Source File

SOURCE=.\matlab\sample_mc.m
# End Source File
# Begin Source File

SOURCE=.\matlab\sample_mdp.m
# End Source File
# Begin Source File

SOURCE=.\matlab\sample_mhmm.m
# End Source File
# Begin Source File

SOURCE=.\matlab\sample_pomdp.m
# End Source File
# Begin Source File

SOURCE=.\matlab\viterbi_path.m
# End Source File
# End Group
# Begin Source File

SOURCE=.\TODO.txt
# End Source File
# End Target
# End Project
