
EXECDIR = ../bin/$(SYSDIR)
EXEC_TARGET = $(EXECDIR)/$(notdir $(PWD))


ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))

INCS = -I../../libYARP_OS/include -I$(YARP_ROOT)/include/$(SYSDIR) -I.

CFLAGS = $(CFAST) $(INCS) $(CDEFS) 

SUBDIRS =

GENDIR = obj/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) $(EXEC_TARGET)

w:
	@echo -n

$(EXEC_TARGET): $(TARGETS)
	mkdir -p $(EXECDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -lACE -L../../libYARP_OS/lib/$(SYSDIR) -lYARP_OS

$(GENDIR)/%.o: %.cpp
	mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(GENDIR)/%.o: %.c
	mkdir -p $(GENDIR)
	$(CC1) $(CFLAGS) -c $< -o $@

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
	makedepend -Y $(INCS) -p$(GENDIR)/ *.cpp

# DO NOT DELETE

obj/linux/LocalNameServer.o: LocalNameServer.h
obj/linux/LocalNameServer.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/LocalNameServer.o: ../../../include/linux/yarp/YARPConfigWin32.h
obj/linux/LocalNameServer.o: ../../libYARP_OS/include/yarp/YARPString.h
obj/linux/LocalNameServer.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/LocalNameServer.o: ../../libYARP_OS/include/yarp/YARPAll.h
obj/linux/LocalNameServer.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
obj/linux/LocalNameServer.o: ../../libYARP_OS/include/yarp/debug.h
obj/linux/LocalNameServer.o: ../../libYARP_OS/include/yarp/YARPList.h
obj/linux/NetworkMap.o: NetworkMap.h
obj/linux/NetworkMap.o: ../../libYARP_OS/include/yarp/YARPString.h
obj/linux/NetworkMap.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/NetworkMap.o: ../../libYARP_OS/include/yarp/YARPAll.h
obj/linux/NetworkMap.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
obj/linux/NetworkMap.o: ../../libYARP_OS/include/yarp/debug.h
obj/linux/NetworkMap.o: ../../libYARP_OS/include/yarp/YARPList.h
obj/linux/YARPNameServer.o: YARPNameServer.h
obj/linux/YARPNameServer.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/YARPNameServer.o: CThreadImpl.h
obj/linux/YARPNameServer.o: ../../libYARP_OS/include/yarp/YARPString.h
obj/linux/YARPNameServer.o: ../../libYARP_OS/include/yarp/YARPAll.h
obj/linux/YARPNameServer.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
obj/linux/YARPNameServer.o: ../../libYARP_OS/include/yarp/debug.h
obj/linux/YARPNameServer.o: LocalNameServer.h
obj/linux/YARPNameServer.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/YARPNameServer.o: ../../../include/linux/yarp/YARPConfigWin32.h
obj/linux/YARPNameServer.o: ../../libYARP_OS/include/yarp/YARPList.h
obj/linux/YARPNameServer.o: NetworkMap.h
obj/linux/YARPNameService.o: YARPNameServer.h
obj/linux/YARPNameService.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/YARPNameService.o: CThreadImpl.h
obj/linux/YARPNameService.o: ../../libYARP_OS/include/yarp/YARPString.h
obj/linux/YARPNameService.o: ../../libYARP_OS/include/yarp/YARPAll.h
obj/linux/YARPNameService.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
obj/linux/YARPNameService.o: ../../libYARP_OS/include/yarp/debug.h
obj/linux/YARPNameService.o: LocalNameServer.h
obj/linux/YARPNameService.o: ../../../include/linux/yarp/YARPConfig.h
obj/linux/YARPNameService.o: ../../../include/linux/yarp/YARPConfigWin32.h
obj/linux/YARPNameService.o: ../../libYARP_OS/include/yarp/YARPList.h
obj/linux/YARPNameService.o: NetworkMap.h
