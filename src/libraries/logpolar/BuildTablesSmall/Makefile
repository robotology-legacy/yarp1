#$Id: Makefile,v 1.7 2004-02-04 15:09:26 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS =
CFLAGS = $(CFAST) $(CDEFS) -I $(YARP_ROOT)/include 
LIBS = $(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib

all: BuildTablesSmall

BuildTablesSmall: 
	$(CC) $(CFLAGS) main.cpp $(LIBS) -o BuildTablesSmall 
	
clean:
	rm -f BuildTablesSmall
	
install:
	cp BuildTablesSmall $(YARP_ROOT)/bin/$(OS)/buildtables

