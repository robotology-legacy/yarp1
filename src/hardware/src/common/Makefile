#$Id: Makefile,v 1.2 2003-09-16 13:57:34 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CFLAGS = -g -I../include -I../sys_include -I$(YARP_ROOT)/include/ -I$(YARP_ROOT)/src/hardware/include \
-I../common

OUTDIR = ../obj/$(SYSDIR)/

%.o: %.cpp
	@echo
	@echo ---------------Compiling common files --------------
	@echo
	$(CC) $(CFLAGS) -c $^ -o $@
	$(CP) -c $@ $(OUTDIR)

target: local install

local: $(OBJS)

w:
	@echo -n

clean:
	rm -f $(OUTDIR)*.o $(CODELIB)

install:
	@echo
	@echo ---------------Installing common files --------------
	@echo
	cp *.h $(YARP_ROOT)/include

