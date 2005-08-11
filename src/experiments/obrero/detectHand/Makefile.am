AM_CXXFLAGS = -D_REENTRANT -D_FILE_OFFSET_BITS=64
AM_CFLAGS   = -D_REENTRANT -D_FILE_OFFSET_BITS=64

AUTOMAKE_OPTIONS = foreign
EXTRA_DIST       = ChangeLog TODO dvgrab.dox dvgrab.spec dvgrab.1 debian NEWS
man_MANS         = dvgrab.1
bin_PROGRAMS     = dvgrab-shell
noinst_PROGRAMS  = riffdump rawdump

dvgrab_shell_SOURCES   = error.cc error.h avi.cc avi.h riff.h riff.cc ieee1394io.cc ieee1394io.h \
			frame.cc frame.h main.cc filehandler.cc filehandler.h affine.h \
			raw1394util.c raw1394util.h dvgrab.cc dvgrab.h io.c io.h \
			stringutils.h stringutils.cc smiltime.h smiltime.cc sidetap.h sidetap.cc \
  PotHand.cc Observe.cc Grapher.cc IntegralImageTool.cc YARPImageLabel.cc YARPBinaryAnalysis.cc \
  EdHeadRemote.h EdHeadRemote.cc

riffdump_SOURCES = error.cc error.h riffdump.cc avi.h riff.h avi.cc riff.cc frame.h frame.cc

rawdump_SOURCES  = rawdump.c

# a C++ formatter

ASTYLEFLAGS = -c -t -b -P

pretty:
	astyle $(ASTYLEFLAGS) $(dvgrab_SOURCES)

# a documentation generator

dox:
	doxygen dvgrab.dox
