#$Id: Makefile,v 1.1 2004-07-12 23:40:29 babybot Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CFLAGS += -I../include -I../sys_include -I$(YARP_ROOT)/include/ -I$(YARP_ROOT)/src/hardware/include \
-I../common

OUTDIR = ../obj/$(SYSDIR)/

%.o: %.cpp
	@echo
	@echo ---------------Compiling common files --------------
	@echo
	$(CC) $(EXTRA_OPTS) $(CFLAGS) -c $^ -o $@
	$(CP) -c $@ $(OUTDIR)

target: local install

local: $(OBJS)

w:
	@echo -n

clean:
	rm -f $(OUTDIR)*.o $(CODELIB)
	rm -f *.o

install:
	@echo
	@echo ---------------Installing common files --------------
	@echo
	cp *.h $(YARP_ROOT)/include

