# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: compile

clean:
	+cd ./camview_photon/src; make clean; 
compile:
	+cd ./camview_photon/src; make proto; make app; 
install: 
	+cd ./camview_photon/src; make install; 
