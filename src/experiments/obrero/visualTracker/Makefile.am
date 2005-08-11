AM_CXXFLAGS = -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D__LINUX__ -DLINUX_BUILD -DYARP_OS_CONFIG=LINUX
AM_CFLAGS   = -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D__LINUX__ -DLINUX_BUILD -DYARP_OS_CONFIG=LINUX

AUTOMAKE_OPTIONS = foreign
EXTRA_DIST       = ChangeLog TODO dvgrab.dox dvgrab.spec dvgrab.1 debian NEWS
man_MANS         = dvgrab.1
bin_PROGRAMS     = dvgrab-shell
noinst_PROGRAMS  = riffdump rawdump

dvgrab_shell_SOURCES   = error.cc error.h avi.cc avi.h riff.h riff.cc ieee1394io.cc ieee1394io.h \
			frame.cc frame.h main.cc filehandler.cc filehandler.h affine.h \
			raw1394util.c raw1394util.h dvgrab.cc dvgrab.h io.c io.h \
			stringutils.h stringutils.cc smiltime.h smiltime.cc sidetap.h sidetap.cc robot_gui.h robot_gui.cc EdHead.h EdHead.cc EdHeadRemote.h EdHeadRemote.cc ParallelPort.h ParallelPort.cc SPICommBoard.h SPICommBoard.cc HeadChannel.h HeadChannel.cc ObreroVision.h ObreroVision.cc YARPVisMap.h Roller.h IntegralImageTool.h IntegralImageTool.cc ImgTrack.h ImgTrack.cc MotionGroup.cc

riffdump_SOURCES = error.cc error.h riffdump.cc avi.h riff.h avi.cc riff.cc frame.h frame.cc

rawdump_SOURCES  = rawdump.c

# a C++ formatter

ASTYLEFLAGS = -c -t -b -P

pretty:
	astyle $(ASTYLEFLAGS) $(dvgrab_SOURCES)

# a documentation generator

dox:
	doxygen dvgrab.dox
