# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: compile 

clean:
	+cd ./$(OS)/yarp; make $(@F)
	
compile:
	+cd ./$(OS)/yarp; make

install: 
	+cd ./$(OS)/yarp; make install
