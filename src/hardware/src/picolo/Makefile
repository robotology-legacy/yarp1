# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: compile 

clean:
	+cd ./$(OS); make clean 
	#+cd ./common; make $(@F)
	
compile:
	+cd ./$(OS); make
	#+cd ./common; make

install: 
	+cd ./$(OS); make install
	#+cd ./common; make install
