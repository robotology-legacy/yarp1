include $(YARP_ROOT)/conf/Makefile.template

LIBS = Acquisition.o AuxFunctions.o LogPolarSDK.o TableGeneration.o TableLoading.o
# $(patsubst %.cpp, %.o, $(wildcard *.cpp))

CODELIB = LogPolarSmallSDK
OS = $(SYSDIR)

CFLAGS = $(CFAST) -I. -I$(YARP_ROOT)/include $(CDEFS) -D__QNX__ -D__QNX6__ -llibc
SRC = ../LogPolarSDKLib

all: $(CODELIB).a

%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(CODELIB).a: $(LIBS)
	$(AR) $(ARFLAGS) $(CODELIB).a $(LIBS)

clean:
	rm -f *.o
	rm -f $(CODELIB).a

install: $(CODELIB).a
	$(CP) $(CODELIB).a $(YARP_ROOT)/lib/$(OS)/$(CODELIB).lib
	$(CP) $(SRC)/*.h $(YARP_ROOT)/include
	$(CP) ../Tables/ReferenceImage.bmp $(YARP_ROOT)/conf
