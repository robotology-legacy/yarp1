
RUN = run
INACTIVE = $(wildcard *.$(RUN))
ACTIVE = $(patsubst %.$(RUN), %.o, $(INACTIVE))

%.o: %.$(RUN)
	rm -f $*.stop $*.scan
	echo -n | tee $*.stop | tee $*.scan
	chmod u+x $*.stop
	chmod u+x $*.scan
	{ export KEY=$*; $<; } | tee $@
	echo "echo \$$missing" >> $*.scan

all:
	echo Please specify a target

depend:
	# need to be on a unix machine for this
	makedepend *.$(RUN)
	sed "s/\.$(RUN)/\.o/g" < Makefile > Makefile.new
	mv Makefile.new Makefile

clean:
	{ mv $(ACTIVE) backup 2> /dev/null; } || echo -n

# DO NOT DELETE

attention.o: dpramd.o meid.o
