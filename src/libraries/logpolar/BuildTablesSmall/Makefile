#$Id: Makefile,v 1.5 2003-10-31 10:32:50 babybot Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS =
CFLAGS = -I $(YARP_ROOT)/include $(CFAST)
LIBS = $(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib

all: BuildTablesSmall

BuildTablesSmall: 
	$(CC) $(CFLAGS) main.cpp $(LIBS) -o BuildTablesSmall 
	
clean:
	rm -f BuildTablesSmall
	
install:
	cp BuildTablesSmall $(YARP_ROOT)/bin/$(OS)

