
SUBDIRS = libYARP_OS libYARP_sig libYARP_sig_logpolar tools

default:
	@for d in $(SUBDIRS) ; do \
		( test -d $$d && ( cd $$d; $(MAKE) ) ) ; done

clean:
	@for d in $(SUBDIRS) ; do \
		( test -d $$d && ( cd $$d; $(MAKE) clean ) ) ; done

