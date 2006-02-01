#
# $Id: Makefile,v 1.1 2006-02-01 21:54:19 gmetta Exp $
#

include $(YARP_ROOT)/conf/Makefile.template

CFLAGS = -I ../ -I ../../include -I $(YARP_ROOT)/include -I $(YARP_ROOT)/include/$(OS)
CFLAGS += $(CFAST) $(CDEFS)

TARGETS = $(patsubst %.cpp, %.o, $(wildcard *.cpp)) $(patsubst %.c, %.o, $(wildcard *.c))

GENDIR = ../../obj/$(OS)

all: target

target: $(TARGETS)

w:
	@echo -n

clean:
	rm -f *.o

.cpp.o:
	mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) -c $< -o $(GENDIR)/$@
	
.c.o:
	mkdir -p $(GENDIR)
	$(CC1) $(CFLAGS) -c $< -o $(GENDIR)/$@


