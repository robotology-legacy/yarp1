include $(YARP_ROOT)/conf/Makefile.template


CFLAGS = -w9 -g -M -I../include

OFILES = wrappers.o
HFILES = ../include/ueid.h ../include/ueid-msg.h ../include/ueid-err.h

.SUFFIXES:
.SUFFIXES:	.o .c .a

ueid.lib:  $(OFILES)
	-rm ueid.lib
	wlib ueid.lib + $(OFILES)

$(OFILES):  $(HFILES)


clean:
	-rm *.o *.err *.map ueid

