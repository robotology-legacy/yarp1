#$Id: Makefile,v 1.6 2004-01-31 00:55:52 babybot Exp $

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
	cp BuildTablesSmall $(YARP_ROOT)/bin/$(OS)/buildtables

