#$Id: Makefile,v 1.1 2003-06-25 15:40:07 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS =
CC = g++
CFLAGS = -I $(YARP_ROOT)/include 
LIBS = $(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib

all: BuildTablesSmall

BuildTablesSmall: 
	$(CC) $(CFLAGS) main.cpp $(LIBS) -o BuildTablesSmall 
	
clean:
	rm BuildTablesSmall
	
install:
	@echo nothing to do





