
TARGETS = $(patsubst %.dir, %, $(wildcard *.dir))

default: all

all: $(TARGETS)

%: %.dir
	./package.pl < $<


clean:
	echo should clean $(TARGETS)
