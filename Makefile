
SUBDIRS = src

default:
	@for d in $(SUBDIRS) ; do \
		( test -d $$d && ( cd $$d; $(MAKE) $(MAKEFLAGS) ) ) ; done

clean:
	@for d in $(SUBDIRS) ; do \
		( test -d $$d && ( cd $$d; $(MAKE) $(MAKEFLAGS) clean ) ) ; done

