
MAIN = YARP

ZIPS = $(patsubst %.dir, %.tar.gz, $(wildcard *.dir))
TARGETS =  $(patsubst %.dir, %, $(wildcard *.dir)) $(ZIPS)

#TARGETS = $(patsubst %.dir, %, $(wildcard *.dir))

default: all

all: $(TARGETS) $(MAIN).tar.gz

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

unpacked.tag: $(ZIPS)
	rm -rf tmp
	mkdir -p tmp
	@cd tmp && for d in $(ZIPS); do \
		echo $$d && tar xvzf - < ../$$d ; done
	echo -n | tee unpacked.tag

$(MAIN).tar: unpacked.tag
	cd tmp && tar cvf ../$@ YARP

test: unpacked.tag
	@cd tmp/YARP && make

clean:
	rm -rf $(TARGETS) unpacked.tag $(MAIN).tar $(MAIN).tar.gz
