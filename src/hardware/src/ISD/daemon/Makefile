include $(YARP_ROOT)/conf/Makefile.template


CFLAGS = $(CFAST) -w9 -M -I$(YARP_ROOT)/src/hardware/include -I../include
LDFLAGS = -L$(YARP_ROOT)/src/hardware/lib 
#-lwinqnx32f

OFILES = main.o init.o sys_msg.o wraps.o
HFILES = extern.h ../include/isd-msg.h ../include/isd-int.h ../include/isd-err.h

.SUFFIXES:
.SUFFIXES:	.o .c .a

isd:	$(OFILES)
	$(CC) $(CFLAGS) -c
	$(CC) -o isd -T 1 $(LDFLAGS) $(OFILES)
	usemsg isd usemsg.txt

$(OFILES):  $(HFILES)



clean:
	-rm *.o *.err *.map isd

