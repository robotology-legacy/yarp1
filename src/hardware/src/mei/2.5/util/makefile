#
#	Motion Engineering, Inc.
#	QNX 2.43a Utility Makefile
#
#

CC		=	cc
CFLAGS		=	-3 -T 1
LIB = ../lib/medwc10f.lib

ALL: version config

version: version.c
	$(CC) $(CFLAGS) -o version version.c $(LIB)

config: config.c
	$(CC) $(CFLAGS) -o config config.c $(LIB)
