include $(YARP_ROOT)/conf/Makefile.template

ROOTDIR = $(YARP_ROOT)
LIBDIR  = ${ROOTDIR}/src/hardware/lib
SBINDIR = ${ROOTDIR}/bin/$(OS)
INCDIR  = ${ROOTDIR}/src/hardware/include


OPTFLAGS = -Oaeil+rt $(CFASTSTRUCT)
DBGFLAGS =  #-g
CFLAGS = -wx $(DBGFLAGS) $(OPTFLAGS) -I../include -I$(YARP_ROOT)/src/hardware/include
LDFLAGS = -L$(YARP_ROOT)/lib/$(OS) -L$(YARP_ROOT)/src/hardware/lib -lmedwc10f

OFILES = main.o init.o sys_msg.o wraps.o io.o
HFILES = ../include/meid-msg.h meid-int.h

.SUFFIXES:
.SUFFIXES:	.o .c .a


TARGETS = meid

all:  $(TARGETS)

meid:	$(OFILES)
	cc -c $(CFLAGS)
	cc -o $@ -T 1 $(LDFLAGS) $(OFILES)
	usemsg $@ usemsg.txt

$(OFILES):  $(HFILES)



install:  all
	install -c -m ug=rwx,o=rx $(TARGETS) $(SBINDIR)


clean:
	-rm *.o *.err *.map $(TARGETS)

squeaky:  clean
	-rm *~



.PHONY:  install clean squeaky all
