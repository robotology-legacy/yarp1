# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: compile

clean:
	+cd ./src; make clean; 
compile:
	+cd ./src; make proto; make app; 
install: 
	+cd ./src; make install; 
