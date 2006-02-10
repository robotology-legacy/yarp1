
ifndef  YARP_ROOT
YARP_ROOT = ../..
endif


include $(YARP_ROOT)/conf/Makefile.template

all: build fake

# make a .deb package for libYARP_OS and associated tools
build:
	rm -rf tmp
	cp -R libyarp-os tmp
	find tmp -iname "CVS" | xargs rm -rf
	mkdir -p tmp/usr/bin
	cp $(YARP_ROOT)/bin/$(OS)/yarp-* tmp/usr/bin
	mkdir -p tmp/usr/lib
	cp $(YARP_ROOT)/lib/linux/libYARP_OS.* tmp/usr/lib
	dpkg --build tmp libyarp-os.deb

buildalt:
	rm -rf tmp
	cp -R libyarp-os2 tmp
	find tmp -iname "CVS" | xargs rm -rf
	mkdir -p tmp/usr/bin
	cp $(YARP_ROOT)/src/libYARP_OS2/bin/$(OS)/yarp tmp/usr/bin
	mkdir -p tmp/usr/lib
	cp $(YARP_ROOT)/src/libYARP_OS2/lib/$(OS)/libYARP_OS2.a tmp/usr/lib/libYARP_OS.a
	mkdir -p tmp/usr/include/yarp
	cp `find $(YARP_ROOT)/src/libYARP_OS2/include/yarp -iname "*.h" -exec grep -l "namespace os" {} \;` tmp/usr/include/yarp
	mkdir -p tmp/usr/share/doc/libyarp-os
	cp $(YARP_ROOT)/src/libYARP_OS2/README.TXT tmp/usr/share/doc/libyarp-os
	dpkg --build tmp libyarp-os.deb


# make a fake .deb package for libace-dev, in case it is installed in
# a non-managed way
fake:
	rm -rf tmp
	cp -R libace-dev-fake tmp
	find tmp -iname "CVS" | xargs rm -rf
	dpkg --build tmp libace-dev-fake.deb

install:
	sudo dpkg -i libyarp-os.deb
