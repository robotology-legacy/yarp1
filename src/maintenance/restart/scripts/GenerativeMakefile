ACT = /home/cog/active
RUN = run
INACTIVE = $(wildcard *.$(RUN))
ACTIVE = $(patsubst %.$(RUN), %.o, $(INACTIVE))

VPATH = .:$(ACT)

%.o: %.$(RUN)
	test -f $(ACT)/$*.stop && $(ACT)/$*.stop || echo No stop rules for $*
	rm -f $(ACT)/$*.stop $(ACT)/$*.scan $(ACT)/$*.o
	echo -n | tee $(ACT)/$*.stop | tee $(ACT)/$*.scan
	chmod u+x $(ACT)/$*.stop
	chmod u+x $(ACT)/$*.scan
	cp $*.$(RUN) $(ACT)/$*.start
	( export KEY=$*; $<; )
	echo -n > $(ACT)/$*.o
	echo "echo \$$missing" >> $(ACT)/$*.scan

all:
	echo Please specify a target

depend:
	# need to be on a unix machine for this
	makedepend *.$(RUN)
	sed "s/\.$(RUN)/\.o/g" < Makefile > Makefile.new
	mv Makefile.new Makefile

clean:
	{ cd $(ACT); rm -f *.o; } || echo -n

# DO NOT DELETE THIS LINE -- make depend depends on it.
