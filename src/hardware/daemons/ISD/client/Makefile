include $(YARP_ROOT)/conf/Makefile.template

CFLAGS = $(CFAST) -w9 -M -I$(YARP_ROOT)/src/hardware/include -I../include

%.o: %.cc 
	$(CC) $(CFLAGS) -c $^ -o $@

OFILES = wrappers.o
HFILES = 
# HFILES = ../include/isd.h ../include/isd-msg.h ../include/isd-err.h

.SUFFIXES:
.SUFFIXES:	.o .c .a .cc .cpp

isd.lib:  $(OFILES)
	-rm isd.lib
	wlib isd.lib + $(OFILES)

$(OFILES):  $(HFILES)


clean:
	-rm *.o *.err *.map isd

