
ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))

INCS = -I../include -I$(YARP_ROOT)/include/$(SYSDIR)

CFLAGS = -D_REENTRANT $(CFAST) $(CDEFS) $(INCS)

#CFLAGS = $(CFAST) $(CDEFS) -I../include/yarp -I../include/yarp_private -I../include/$(SYSDIR) -I../include

SUBDIRS =

GENDIR = ../obj/$(SYSDIR)

# library suffix should actually be system dependent
CODELIB = libYARP_OS.a

LIBGENDIR = ../lib/$(SYSDIR)

VPATH += $(GENDIR)

all: target

target: $(TARGETS) $(LIBGENDIR)/$(CODELIB) subtarget

w:
	@echo -n

$(GENDIR)/%.o: %.cpp
	mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(GENDIR)/%.o: %.c
	mkdir -p $(GENDIR)
	$(CC1) $(CFLAGS) -c $< -o $@


$(LIBGENDIR)/$(CODELIB): $(TARGETS)
	mkdir -p $(LIBGENDIR)
	echo Making $(CODELIB) for $(TARGETS)
	$(AR) $(ARFLAGS) $(LIBGENDIR)/$(CODELIB) $(TARGETS)

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS) $(CODE_LIB)
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

../obj/linux/BlockReceiver.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/BlockReceiver.o: ../include/yarp_private/BlockReceiver.h
../obj/linux/BlockReceiver.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPNameService.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPAll.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/BlockReceiver.o: ../include/yarp/debug.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPNameID.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPString.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPPortContent.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPSyncComm.h
../obj/linux/BlockReceiver.o: ../include/yarp/YARPMultipartMessage.h
../obj/linux/BlockSender.o: ../include/yarp_private/BlockSender.h
../obj/linux/BlockSender.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/BlockSender.o: ../include/yarp_private/RefCounted.h
../obj/linux/BlockSender.o: ../include/yarp/YARPSemaphore.h
../obj/linux/BlockSender.o: ../include/yarp/YARPAll.h
../obj/linux/BlockSender.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/BlockSender.o: ../include/yarp/debug.h
../obj/linux/BlockSender.o: ../include/yarp/YARPNameID.h
../obj/linux/BlockSender.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/BlockSender.o: ../include/yarp/YARPString.h
../obj/linux/BlockSender.o: ../include/yarp/YARPPortContent.h
../obj/linux/BlockSender.o: ../include/yarp/YARPList.h
../obj/linux/BlockSender.o: ../include/yarp/YARPSyncComm.h
../obj/linux/BlockSender.o: ../include/yarp/YARPNameService.h
../obj/linux/BlockSender.o: ../include/yarp/YARPMultipartMessage.h
../obj/linux/Port.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/Port.o: ../include/yarp/YARPThread.h ../include/yarp/YARPAll.h
../obj/linux/Port.o: ../include/yarp/YARPErrorCodes.h ../include/yarp/debug.h
../obj/linux/Port.o: ../include/yarp/YARPSemaphore.h
../obj/linux/Port.o: ../include/yarp/YARPTime.h
../obj/linux/Port.o: ../include/yarp_private/mesh.h
../obj/linux/Port.o: ../include/yarp/YARPString.h
../obj/linux/Port.o: ../include/yarp_private/BlockSender.h
../obj/linux/Port.o: ../include/yarp_private/RefCounted.h
../obj/linux/Port.o: ../include/yarp/YARPNameID.h
../obj/linux/Port.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/Port.o: ../include/yarp/YARPPortContent.h
../obj/linux/Port.o: ../include/yarp/YARPList.h
../obj/linux/Port.o: ../include/yarp_private/BlockReceiver.h
../obj/linux/Port.o: ../include/yarp/YARPNameService.h
../obj/linux/Port.o: ../include/yarp_private/Port.h
../obj/linux/Port.o: ../include/yarp_private/Sendable.h
../obj/linux/Port.o: ../include/yarp_private/YARPFragments.h
../obj/linux/Port.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/Port.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/Port.o: ../include/yarp/end_pack_for_net.h
../obj/linux/Port.o: ../include/yarp/YARPNativeNameService.h
../obj/linux/Port.o: ../include/yarp/YARPSyncComm.h
../obj/linux/Port.o: ../include/yarp/YARPMultipartMessage.h
../obj/linux/Port.o: ../include/yarp/YARPScheduler.h
../obj/linux/Port.o: ../include/yarp/YARPSocket.h
../obj/linux/RefCounted.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/RefCounted.o: ../include/yarp_private/RefCounted.h
../obj/linux/RefCounted.o: ../include/yarp/YARPSemaphore.h
../obj/linux/RefCounted.o: ../include/yarp/YARPAll.h
../obj/linux/RefCounted.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/RefCounted.o: ../include/yarp/debug.h
../obj/linux/RefCounted.o: ../include/yarp/YARPString.h
../obj/linux/Sendable.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/Sendable.o: ../include/yarp_private/Sendable.h
../obj/linux/Sendable.o: ../include/yarp_private/BlockSender.h
../obj/linux/Sendable.o: ../include/yarp_private/RefCounted.h
../obj/linux/Sendable.o: ../include/yarp/YARPSemaphore.h
../obj/linux/Sendable.o: ../include/yarp/YARPAll.h
../obj/linux/Sendable.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/Sendable.o: ../include/yarp/debug.h ../include/yarp/YARPNameID.h
../obj/linux/Sendable.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/Sendable.o: ../include/yarp/YARPString.h
../obj/linux/Sendable.o: ../include/yarp/YARPPortContent.h
../obj/linux/Sendable.o: ../include/yarp/YARPList.h
../obj/linux/Sendable.o: ../include/yarp_private/BlockReceiver.h
../obj/linux/Sendable.o: ../include/yarp/YARPNameService.h
../obj/linux/Sendable.o: ../include/yarp_private/Sendables.h
../obj/linux/YARPAll.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPAll.o: ../include/yarp/YARPNameService.h
../obj/linux/YARPAll.o: ../include/yarp/YARPAll.h
../obj/linux/YARPAll.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPAll.o: ../include/yarp/debug.h ../include/yarp/YARPNameID.h
../obj/linux/YARPAll.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPAll.o: ../include/yarp/YARPString.h
../obj/linux/YARPAll.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPBottle.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPPort.h
../obj/linux/YARPBottle.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPAll.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPBottle.o: ../include/yarp/debug.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPPortContent.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPString.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPBottleCodes.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPList.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPBottle.o: ../include/yarp/YARPBottleContent.h
../obj/linux/YARPIterativeStats.o: ../include/yarp/YARPIterativeStats.h
../obj/linux/YARPIterativeStats.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPMultipartMessage.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPMultipartMessage.o: ../include/yarp/YARPMultipartMessage.h
../obj/linux/YARPMultipartMessage.o: ../include/yarp/YARPAll.h
../obj/linux/YARPMultipartMessage.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPMultipartMessage.o: ../include/yarp/debug.h
../obj/linux/YARPNameClient.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPNameClient.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPAll.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPNameClient.o: ../include/yarp/debug.h
../obj/linux/YARPNameClient.o: ../include/yarp/wide_nameloc.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPString.h
../obj/linux/YARPNameClient.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPNameClient.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPTime.h
../obj/linux/YARPNameClient.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPNameService.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPNameService.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPAll.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPNameService.o: ../include/yarp/debug.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPString.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPSocketNameService.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPNameClient.h
../obj/linux/YARPNameService.o: ../include/yarp/wide_nameloc.h
../obj/linux/YARPNameService.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPNameService.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPTime.h
../obj/linux/YARPNameService.o: ../include/yarp/YARPNativeNameService.h
../obj/linux/YARPNativeNameService.o: ../include/yarp/YARPNativeNameService.h
../obj/linux/YARPNativeNameService.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPNativeNameService.o: ../include/yarp/YARPAll.h
../obj/linux/YARPNativeNameService.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPNativeNameService.o: ../include/yarp/debug.h
../obj/linux/YARPNativeNameService.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPNativeNameService.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPNativeNameService.o: ../include/yarp/YARPString.h
../obj/linux/YARPPort.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPPort.o: ../include/yarp_private/Port.h
../obj/linux/YARPPort.o: ../include/yarp/YARPString.h
../obj/linux/YARPPort.o: ../include/yarp/YARPAll.h
../obj/linux/YARPPort.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPPort.o: ../include/yarp/debug.h ../include/yarp/YARPNameID.h
../obj/linux/YARPPort.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPPort.o: ../include/yarp/YARPTime.h
../obj/linux/YARPPort.o: ../include/yarp_private/mesh.h
../obj/linux/YARPPort.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPPort.o: ../include/yarp/YARPThread.h
../obj/linux/YARPPort.o: ../include/yarp_private/Sendable.h
../obj/linux/YARPPort.o: ../include/yarp_private/BlockSender.h
../obj/linux/YARPPort.o: ../include/yarp_private/RefCounted.h
../obj/linux/YARPPort.o: ../include/yarp/YARPPortContent.h
../obj/linux/YARPPort.o: ../include/yarp/YARPList.h
../obj/linux/YARPPort.o: ../include/yarp_private/BlockReceiver.h
../obj/linux/YARPPort.o: ../include/yarp/YARPNameService.h
../obj/linux/YARPPort.o: ../include/yarp_private/YARPFragments.h
../obj/linux/YARPPort.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPPort.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPPort.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPPort.o: ../include/yarp_private/Sendables.h
../obj/linux/YARPPort.o: ../include/yarp/YARPPort.h
../obj/linux/YARPRefCount.o: ../include/yarp/YARPRefCount.h
../obj/linux/YARPRefCount.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPRefCount.o: ../include/yarp/YARPAll.h
../obj/linux/YARPRefCount.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPRefCount.o: ../include/yarp/debug.h
../obj/linux/YARPRefCount.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPScheduler.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPScheduler.o: ../include/yarp/YARPScheduler.h
../obj/linux/YARPScheduler.o: ../include/yarp/YARPAll.h
../obj/linux/YARPScheduler.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPScheduler.o: ../include/yarp/debug.h
../obj/linux/YARPSemaphore.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSemaphore.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSemaphore.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSemaphore.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSemaphore.o: ../include/yarp/debug.h
../obj/linux/YARPSocket.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSocket.o: ../include/yarp/debug.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPString.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPThread.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPTime.h
../obj/linux/YARPSocket.o: ../include/yarp/YARPScheduler.h
../obj/linux/YARPSocket.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPSocket.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPSocketDgram.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/debug.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPString.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPSocketDgram.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPThread.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPTime.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/YARPScheduler.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPSocketDgram.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPSocketMcast.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/debug.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPString.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPSocketMcast.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPThread.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPTime.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/YARPScheduler.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPSocketMcast.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPSocketMulti.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPTime.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/debug.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPList.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPString.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPSocketMulti.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPSocketNameService.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPNameService.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPNameClient.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/wide_nameloc.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPThread.h
../obj/linux/YARPSocketMulti.o: ../include/yarp/YARPScheduler.h
../obj/linux/YARPSocketNameService.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPSocketNameService.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/debug.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPNameService.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPString.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPNameClient.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/wide_nameloc.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPTime.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPSocketDgram.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPSocketMcast.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPSocketMulti.h
../obj/linux/YARPSocketNameService.o: ../include/yarp/YARPNativeNameService.h
../obj/linux/YARPSocketSyncComm.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPSocketSyncComm.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/debug.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPSocketNameService.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPNameService.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPString.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPNameClient.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/wide_nameloc.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPTime.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPMultipartMessage.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPSocketDgram.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp/YARPScheduler.h
../obj/linux/YARPSocketSyncComm.o: ../include/yarp_private/BlockPrefix.h
../obj/linux/YARPSyncComm.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPSyncComm.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPAll.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPSyncComm.o: ../include/yarp/debug.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPNameService.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPNameID.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPString.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPMultipartMessage.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPNativeSyncComm.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPSocketSyncComm.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPSocketNameService.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPNameClient.h
../obj/linux/YARPSyncComm.o: ../include/yarp/wide_nameloc.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSyncComm.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/YARPSyncComm.o: ../include/yarp/end_pack_for_net.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPTime.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPSocket.h
../obj/linux/YARPSyncComm.o: ../include/yarp/YARPNativeNameService.h
../obj/linux/YARPThread.o: ../include/yarp/YARPThread.h
../obj/linux/YARPThread.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPThread.o: ../include/yarp/YARPAll.h
../obj/linux/YARPThread.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPThread.o: ../include/yarp/debug.h
../obj/linux/YARPThread.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPThread.o: ../include/yarp/YARPTime.h
../obj/linux/YARPThreadAux.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPThreadAux.o: ../include/yarp/YARPThread.h
../obj/linux/YARPThreadAux.o: ../include/yarp/YARPAll.h
../obj/linux/YARPThreadAux.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPThreadAux.o: ../include/yarp/debug.h
../obj/linux/YARPThreadAux.o: ../include/yarp/YARPSemaphore.h
../obj/linux/YARPThreadAux.o: ../include/yarp/YARPTime.h
../obj/linux/YARPTime.o: ../include/yarp/YARPTime.h
../obj/linux/YARPTime.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPTime.o: ../include/yarp/YARPAll.h
../obj/linux/YARPTime.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/YARPTime.o: ../include/yarp/debug.h
../obj/linux/mesh.o: ../include/yarp/YARPString.h
../obj/linux/mesh.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/mesh.o: ../include/yarp/YARPAll.h
../obj/linux/mesh.o: ../include/yarp/YARPErrorCodes.h ../include/yarp/debug.h
../obj/linux/mesh.o: ../include/yarp_private/mesh.h
../obj/linux/wide_nameloc.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/wide_nameloc.o: ../include/yarp/wide_nameloc.h
../obj/linux/wide_nameloc.o: ../include/yarp/YARPNetworkTypes.h
../obj/linux/wide_nameloc.o: ../include/yarp/YARPAll.h
../obj/linux/wide_nameloc.o: ../include/yarp/YARPErrorCodes.h
../obj/linux/wide_nameloc.o: ../include/yarp/debug.h
../obj/linux/wide_nameloc.o: ../include/yarp/YARPNameID_defs.h
../obj/linux/wide_nameloc.o: ../include/yarp/YARPString.h
../obj/linux/wide_nameloc.o: ../include/yarp/begin_pack_for_net.h
../obj/linux/wide_nameloc.o: ../include/yarp/end_pack_for_net.h
