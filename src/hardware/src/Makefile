# Device driver global makefile
include $(YARP_ROOT)/conf/Makefile.template

all: drivers

clean:
	+cd ./ISD; make $(@F)
	+cd ./meid2; make $(@F)
	+cd ./ueid2; make $(@F)
	+cd ./grabber; make $(@F)
	+cd ./grabber2; make $(@F)
	+cd ./porter; make $(@F)

install: drivers

drivers: 
	+cd ./ISD; make install
	+cd ./meid2; make install
	+cd ./ueid2; make install
	+cd ./grabber; make install
	+cd ./grabber2; make install
	+cd ./porter; make install
