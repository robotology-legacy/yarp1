include $(YARP_ROOT)/conf/Makefile.template

ROOTDIR = $(YARP_ROOT)
LIBDIR  = ${ROOTDIR}/src/hardware/lib
SBINDIR = ${ROOTDIR}/bin/$(OS)
INCDIR  = ${ROOTDIR}/src/hardware/include


# -O flags:
# a- relaxed alias checking
# x- in-line library functions, loop opt., no stack check
# e- in-line expansion for user functions
# t- favor time over space


OPTFLAGS = -Oaeil+rt  $(CFASTSTRUCT)
DBGFLAGS = # -g -M
CFLAGS = -wx $(DBGFLAGS) $(OPTFLAGS) -I../include -I/home/cog/include

OFILES = wrappers.o util.o
HFILES = ../include/meid.h ../include/meid-msg.h

.SUFFIXES:
.SUFFIXES:	.o .c .a

TARGET = meid.lib

all:  $(TARGET)

$(TARGET):  $(OFILES)
	wlib -n -c -l $@ $(OFILES)


$(OFILES):  $(HFILES)



install:  all
	install -c -m ug=rw,o=r $(TARGET) $(LIBDIR)


clean:
	-rm *.o *.err *.map *.lib *.lst

squeaky:  clean
	-rm *.bak *~
