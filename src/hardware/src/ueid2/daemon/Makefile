include $(YARP_ROOT)/conf/Makefile.template


CFLAGS = -w9 -g -M -I$(YARP_ROOT)/src/hardware/include -I../include
LDFLAGS = -L$(YARP_ROOT)/src/hardware/lib -lpdfw_lib

OFILES = main.o init.o sys_msg.o wraps.o
HFILES = extern.h ../include/ueid-msg.h ../include/ueid-int.h ../include/ueid-err.h

.SUFFIXES:
.SUFFIXES:	.o .c .a

ueid:	$(OFILES)
	cc -c $(CFLAGS)
	cc -o ueid -T 1 $(LDFLAGS) $(OFILES)
	usemsg ueid usemsg.txt

$(OFILES):  $(HFILES)



clean:
	-rm *.o *.err *.map ueid

