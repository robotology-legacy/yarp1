
ZIPS = $(patsubst %.dir, %.tar.gz, $(wildcard *.dir))
TARGETS =  $(patsubst %.dir, %, $(wildcard *.dir)) $(ZIPS)

#TARGETS = $(patsubst %.dir, %, $(wildcard *.dir))

default: all

all: $(TARGETS)

%: %.dir
	mkdir -p backup
	test -e backup/$@ && rm -f backup/$@ || echo -n
	test -e $@ && mv -f $@ backup || echo -n
	./package.pl < $<

%.tar: %
	rm -rf YARP
	cp -r $< YARP
	tar cvf $@ YARP

%.tar.gz: %.tar
	gzip $<

test: $(ZIPS)
	rm -rf tmp
	mkdir -p tmp
	@cd tmp && for d in $(ZIPS); do \
		echo $$d && tar xvzf - < ../$$d ; done
	@cd tmp/YARP && make

clean:
	rm -rf $(TARGETS)
