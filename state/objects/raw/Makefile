include $(YARP_ROOT)/conf/Makefile.template

LIST = inventory.txt

all: $(LIST)

$(LIST):
	ls -1 src | sed "s/\.ppm//" > $(LIST)

# for f in `ls`; do mv $f `echo $f | sed "s/src_//"`; done

clean:
	rm -f $(LIST)
