
SUBDIRS = src $(wildcard yarp-*)


default:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) ) ; done


clean:
	@for d in $(SUBDIRS) ; do \
		( cd $$d ; $(MAKE) clean ) ; done


