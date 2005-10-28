MEX=mex

SRC=port.c global.cpp
MEX_OPTIONS=./gccopts.sh

all: $(SRC)
	$(MEX) $(SRC) -f $(MEX_OPTIONS) -v

clean:
	rm -f *.o

install:
	make all
	cp port.mexglx $(YARP_ROOT)/bin/linux



