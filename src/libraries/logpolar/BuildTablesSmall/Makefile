#$Id: Makefile,v 1.4 2003-10-06 10:07:04 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS =
#CC = g++
CFLAGS = -I $(YARP_ROOT)/include $(CFAST) -g
LIBS = $(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib

all: BuildTablesSmall

BuildTablesSmall: 
	$(CC) $(CFLAGS) main.cpp $(LIBS) -o BuildTablesSmall 
	
clean:
	rm -f BuildTablesSmall
	
install:
	@echo nothing to do





