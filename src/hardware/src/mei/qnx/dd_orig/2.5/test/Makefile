


CFLAGS = -w9 -g -M -I/home/cog/include
LDFLAGS = -L/home/cog/lib -lmedwc10f -lmeid
LDFLAGS2 = -L/home/cog/lib -lmeid

OFILES = analog.o da.o
HFILES = 

.SUFFIXES:
.SUFFIXES:	.o .c .a

all: da

analog:	$(OFILES)
	cc -c $(CFLAGS)
	cc -o analog -T 1 $(LDFLAGS) $(OFILES)

da:	da.o
	cc -o da -T 1 $(LDFLAGS) da.o

$(OFILES):  $(HFILES)



clean:
	-rm *.o *.err *.map da analog

