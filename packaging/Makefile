
TARGETS =  $(patsubst %.dir, %, $(wildcard *.dir)) $(patsubst %.dir, %.tar.gz, $(wildcard *.dir))

#TARGETS = $(patsubst %.dir, %, $(wildcard *.dir))

default: all

all: $(TARGETS)

%: %.dir
	mkdir -p backup
	test -e backup/$@ && rm -f backup/$@ || echo -n
	test -e $@ && mv -f $@ backup || echo -n
	./package.pl < $<

%.tar: %
	tar cvf $@ $<

%.tar.gz: %.tar
	gzip $<

clean:
	echo should clean $(TARGETS)
