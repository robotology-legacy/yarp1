
SUBDIRS = src

default:
	@for d in $(SUBDIRS) ; do \
		( test -d $$d && ( cd $$d; $(MAKE) ) ) ; done

clean:
	@for d in $(SUBDIRS) ; do \
		( test -d $$d && ( cd $$d; $(MAKE) clean ) ) ; done

