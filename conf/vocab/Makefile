include $(YARP_ROOT)/conf/Makefile.template

HFILE=YARPBottleCodes.h

all: $(HFILE)

$(HFILE): vocab.txt
	./convert.pl < $< > $@

clean:
	rm -f $(HFILE)

install: all
	$(CP) $(HFILE) $(YARP_ROOT)/include



