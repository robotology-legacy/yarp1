cc=gcc -O2 -I.

all : zut mandel boing cplxanimate

install : /usr/local/bin/zut

zut : zut.c
	$(cc) -Wall -L/usr/X11R6/lib zut.c -o zut -lm -lX11 -lXext

mandel : mandel.c
	$(cc) mandel.c -o mandel -lm

boing : boing.c
	$(cc) boing.c -o boing -lm

cplxanimate : cplxanimate.c
	$(cc) cplxanimate.c -o cplxanimate -lm

/usr/local/bin/zut : zut
	install -m a+rx zut /usr/local/bin/zut
