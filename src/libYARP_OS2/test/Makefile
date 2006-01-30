
ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.template


TARGETS = harness.bin

run:
	make clean
	make $(TARGETS)
	./$(TARGETS)


INCS = -I../include -I$(YARP_ROOT)/include/$(SYSDIR)
CFLAGS = -g -D_REENTRANT $(CFAST) $(CDEFS) $(INCS)

SUBDIRS =

GENDIR = .

VPATH += $(GENDIR)

CFLAGS += -I../src-test

all: target

target: $(TARGETS) subtarget

w:
	@echo -n

$(GENDIR)/%.bin: %.cpp
	mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -L../lib/$(SYSDIR) -lACE -lYARP_OS2_test -lYARP_OS2 

$(GENDIR)/%.o: %.cpp
	mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/%.o: %.c
	mkdir -p $(GENDIR)
	$(CC1) $(CFLAGS) -c $^ -o $@

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)
	rm -f *.o

install: target subinstall
	@echo Doing nothing as yet

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


depend:
	makedepend -Y -I. $(INCS) -p$(GENDIR)/ *.cpp 

# DO NOT DELETE

obj/linux/demo00.o: ../include/yarp/YARPPort.h
obj/linux/demo00.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/demo00.o: ../../../include/linux/yarp/YARPConfigWin32.h
obj/linux/demo00.o: ../include/yarp/YARPAll.h
obj/linux/demo00.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/demo00.o: ../include/yarp/YARPErrorCodes.h ../include/yarp/debug.h
obj/linux/demo00.o: ../include/yarp/YARPPortContent.h
obj/linux/demo00.o: ../include/yarp/YARPNameID.h
obj/linux/demo00.o: ../include/yarp/YARPNameID_defs.h
obj/linux/demo00.o: ../include/yarp/YARPString.h ../include/yarp/YARPBottle.h
obj/linux/demo00.o: ../include/yarp/YARPBottleCodes.h
obj/linux/demo00.o: ../include/yarp/YARPVector.h ../include/yarp/YARPList.h
obj/linux/demo00.o: ../include/yarp/YARPNetworkTypes.h
obj/linux/demo00.o: ../include/yarp/YARPBottleContent.h
obj/linux/demo01.o: ../include/yarp/YARPPort.h
obj/linux/demo01.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/demo01.o: ../../../include/linux/yarp/YARPConfigWin32.h
obj/linux/demo01.o: ../include/yarp/YARPAll.h
obj/linux/demo01.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/demo01.o: ../include/yarp/YARPErrorCodes.h ../include/yarp/debug.h
obj/linux/demo01.o: ../include/yarp/YARPPortContent.h
obj/linux/demo01.o: ../include/yarp/YARPNameID.h
obj/linux/demo01.o: ../include/yarp/YARPNameID_defs.h
obj/linux/demo01.o: ../include/yarp/YARPString.h ../include/yarp/YARPThread.h
obj/linux/demo01.o: ../include/yarp/YARPSemaphore.h
obj/linux/demo01.o: ../include/yarp/YARPTime.h
