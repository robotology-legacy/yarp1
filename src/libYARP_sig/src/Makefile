
ifndef  YARP_ROOT
YARP_ROOT = ../../..
endif

include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp)) $(patsubst %.c, $(GENDIR)/%.o, $(wildcard *.c))

INCS = -I../include -I../../libYARP_OS/include -I$(YARP_ROOT)/include/$(SYSDIR)

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

../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/YARPTime.h
../obj/linux/YARPImage.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/YARPAll.h
../obj/linux/YARPImage.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/debug.h
../obj/linux/YARPImage.o: ../include/yarp/YARPImage.h
../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/begin_pack_for_net.h
../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/YARPNetworkTypes.h
../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/end_pack_for_net.h
../obj/linux/YARPImage.o: ../../libYARP_OS/include/yarp/YARPRefCount.h
../obj/linux/YARPImageFile.o: ../include/yarp/YARPImageFile.h
../obj/linux/YARPImageFile.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPImageFile.o: ../include/yarp/YARPImage.h
../obj/linux/YARPImageFile.o: ../../libYARP_OS/include/yarp/begin_pack_for_net.h
../obj/linux/YARPImageFile.o: ../../libYARP_OS/include/yarp/YARPNetworkTypes.h
../obj/linux/YARPImageFile.o: ../../libYARP_OS/include/yarp/YARPAll.h
../obj/linux/YARPImageFile.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
../obj/linux/YARPImageFile.o: ../../libYARP_OS/include/yarp/debug.h
../obj/linux/YARPImageFile.o: ../../libYARP_OS/include/yarp/end_pack_for_net.h
../obj/linux/YARPImageFile.o: ../include/yarp/YARPSimpleOperations.h
../obj/linux/YARPImageFile.o: ../include/yarp/YARPFilters.h
../obj/linux/YARPImageUtils.o: ../include/yarp/YARPImageUtils.h
../obj/linux/YARPImageUtils.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPImageUtils.o: ../include/yarp/YARPImage.h
../obj/linux/YARPImageUtils.o: ../../libYARP_OS/include/yarp/begin_pack_for_net.h
../obj/linux/YARPImageUtils.o: ../../libYARP_OS/include/yarp/YARPNetworkTypes.h
../obj/linux/YARPImageUtils.o: ../../libYARP_OS/include/yarp/YARPAll.h
../obj/linux/YARPImageUtils.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
../obj/linux/YARPImageUtils.o: ../../libYARP_OS/include/yarp/debug.h
../obj/linux/YARPImageUtils.o: ../../libYARP_OS/include/yarp/end_pack_for_net.h
../obj/linux/YARPSimpleOperations.o: ../include/yarp/YARPSimpleOperations.h
../obj/linux/YARPSimpleOperations.o: ../../../include/linux/yarp/YARPConfig.h
../obj/linux/YARPSimpleOperations.o: ../include/yarp/YARPImage.h
../obj/linux/YARPSimpleOperations.o: ../../libYARP_OS/include/yarp/begin_pack_for_net.h
../obj/linux/YARPSimpleOperations.o: ../../libYARP_OS/include/yarp/YARPNetworkTypes.h
../obj/linux/YARPSimpleOperations.o: ../../libYARP_OS/include/yarp/YARPAll.h
../obj/linux/YARPSimpleOperations.o: ../../libYARP_OS/include/yarp/YARPErrorCodes.h
../obj/linux/YARPSimpleOperations.o: ../../libYARP_OS/include/yarp/debug.h
../obj/linux/YARPSimpleOperations.o: ../../libYARP_OS/include/yarp/end_pack_for_net.h
../obj/linux/YARPSimpleOperations.o: ../include/yarp/YARPFilters.h
../obj/linux/cvbase.o: ../include/yarp/cvbase.h
../obj/linux/cvbase.o: ../../../include/linux/yarp/YARPConfig.h
