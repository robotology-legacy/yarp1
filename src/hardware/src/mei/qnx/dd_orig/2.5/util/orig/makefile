#
#	Motion Engineering, Inc.
#	QNX 2.43a Utility Makefile
#
#

CC		=	cc
CFLAGS		=	-3 -T 1

ALL: version config

version: version.c
	$(CC) $(CFLAGS) -o version version.c /mei/current/lib/medwc10f.lib

config: config.c
	$(CC) $(CFLAGS) -o config config.c /mei/current/lib/medwc10f.lib
