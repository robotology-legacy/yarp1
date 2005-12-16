#
# Makefile for Linux
#
C =		gcc -c -DUNIX -DLINUX
L =		gcc
LIBS =		-ldl

all:  		ismain

run:
		export LD_LIBRARY_PATH=. && ./ismain

ismain:		main.o isense.o
		$(L) -o $@ main.o isense.o $(LIBS)

main.o:		main.c *.h
		$(C) main.c

isense.o:	isense.c *.h
		$(C) isense.c
