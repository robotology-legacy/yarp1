HELPROOT = /home/ai/mb/cdp/public_html/help
HELPDIR = $(HELPROOT)/Drivers/meid2

TXT2HLP = $(HELPROOT)/txt2hlp.sh


all:  include lib driver

include:
	@echo "Making include..."
	cd include && $(MAKE) all

lib:
	@echo "Making libs..."
	cd lib && $(MAKE) all

test:
	@echo "Making tests..."
	cd test && $(MAKE) all

driver:
	@echo "Making driver..."
	cd driver && $(MAKE) all


install: 
	cd include && $(MAKE) install
	cd lib && $(MAKE) install
	cd driver && $(MAKE) install

install-help:
	\rm -r $(HELPDIR)
	mkdir -p $(HELPDIR)
	$(TXT2HLP) README > $(HELPDIR)/README.html
	$(TXT2HLP) README.new_functions > $(HELPDIR)/README.new_functions.html
	chmod -R g+rwX,o+rX $(HELPDIR)

clean:
	cd include && $(MAKE) $@
	cd lib && $(MAKE) $@
	cd driver && $(MAKE) $@
	cd test && $(MAKE) $@

squeaky:
	cd include && $(MAKE) $@
	cd lib && $(MAKE) $@
	cd driver && $(MAKE) $@
	cd test && $(MAKE) $@
	-rm *~


.PHONY: include lib driver test install install-help clean squeaky

