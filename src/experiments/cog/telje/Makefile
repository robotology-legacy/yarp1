include $(YARP_ROOT)/conf/Makefile.template

TARGETS = $(patsubst %.cpp, $(GENDIR)/%.o, $(wildcard *.cpp))
EXEC_TARGETS = telje
# $(patsubst %.cpp, $(GENDIR)/%, $(wildcard *.cpp))
CFLAGS = $(CDEFS) $(CFAST) -I$(YARP_ROOT)/include -I/home/cog/include/cpp -I/usr/include
LIBS = $(YARP_ROOT)/lib/$(SYSDIR)/os_services.lib $(YARP_ROOT)/lib/$(SYSDIR)/math.lib $(SYSLIB)

SUBDIRS =

GENDIR = .

VPATH += $(GENDIR)

all: target

test: target
	@for f in $(EXEC_TARGETS); do \
	echo "# Executing $$f ################################"; \
	$$f; \
	done

target: $(TARGETS) $(EXEC_TARGETS) subtarget

$(GENDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

$(GENDIR)/%: %.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

pts.txt: log.txt $(EXEC_TARGETS)
	./$(EXEC_TARGETS) | tee $@

pts1.txt: pts.txt
	cat $< | grep "TELJE_POINT" | sed "s/TELJE_POINT //" | tee $@

trace.txt: log.txt
	cat $< | perl -pe 's/([^ ]* [^ ]*) .*/$$1/' | tee $@

pts: 
	rm -f pts.txt pts1.txt trace.txt
	make pts1.txt trace.txt

clean: subclean
	rm -f $(TARGETS) $(EXEC_TARGETS)

install: subinstall
	$(CP) $(EXEC_TARGETS) yarplog $(YARP_ROOT)/bin/$(OS)

subtarget :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) target ) ; done

subclean :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) clean ) ; done

subinstall :
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) $(MAKEFLAGS) install ) ; done


