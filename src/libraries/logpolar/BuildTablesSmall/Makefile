#$Id: Makefile,v 1.2 2003-07-02 21:36:29 babybot Exp $

include $(YARP_ROOT)/conf/Makefile.template

OBJS =
CC = g++
CFLAGS = -I $(YARP_ROOT)/include $(CFAST) -g
LIBS = $(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib

all: BuildTablesSmall

BuildTablesSmall: 
	$(CC) $(CFLAGS) main.cpp $(LIBS) -o BuildTablesSmall 
	
clean:
	rm BuildTablesSmall
	
install:
	@echo nothing to do





