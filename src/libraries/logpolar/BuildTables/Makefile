#$Id: Makefile,v 1.1 2003-06-25 15:40:07 beltran Exp $

include $(YARP_ROOT)/conf/Makefile.template

#SYSDIR = qnx6

OBJS =
CC = g++
CFLAGS = -I $(YARP_ROOT)/include 
LIBS = $(YARP_ROOT)/lib/$(OS)/LogPolarSmallSDK.lib

all: BuildTables

BuildTables: 
	$(CC) $(CFLAGS) BuildTables.cpp $(LIBS) -o BuildTables
	
clean:
	rm BuildTables
	
install:
	@echo nothing to do





