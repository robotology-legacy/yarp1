include $(YARP_ROOT)/conf/Makefile.template

ROOTDIR = $(YARP_ROOT)
LIBDIR  = ${ROOTDIR}/src/hardware/lib
SBINDIR = ${ROOTDIR}/bin/$(OS)
INCDIR  = ${ROOTDIR}/src/hardware/include

TARGETS = meid.h


all:


install: all
	install -c -m ug=rw,o=r $(TARGETS) $(INCDIR)

clean:
	-rm *.o *.err *.map *.lib

squeaky:  clean
	-rm *~
