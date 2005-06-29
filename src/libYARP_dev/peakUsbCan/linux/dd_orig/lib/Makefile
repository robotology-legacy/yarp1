#****************************************************************************
# Copyright (C) 2001,2002,2003,2004  PEAK System-Technik GmbH
#
# linux@peak-system.com
# www.peak-system.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# Maintainer(s): Klaus Hitschler (klaus.hitschler@gmx.de)
#****************************************************************************

#****************************************************************************
#
# Makefile - Makefile for the shared library libpcan.so.x.x
#
# $Log: not supported by cvs2svn $
# Revision 1.15  2004/08/15 09:46:04  klaus
# rpm building process improved
#
# Revision 1.14  2004/07/24 09:47:18  klaus
# LINUX_CAN_Read_Timeout() added, libpcan.so.0.2 as experimental released
#
# Revision 1.13  2004/04/13 20:36:33  klaus
# added LINUX_CAN_Read() to get the timestamp. Made libpcan.so.0.1.
#
# Revision 1.11  2004/04/11 22:03:29  klaus
# cosmetic changes
#
# Revision 1.10  2004/04/11 22:03:28  klaus
# cosmetic changes
#
# Revision 1.9  2003/03/02 10:58:07  klaus
# merged USB thread into main path
#
# Revision 1.8  2003/03/02 10:58:07  klaus
# merged USB thread into main path
#
# Revision 1.7.2.2  2003/02/24 18:38:15  klaus
# adapted to gcc 3.2
#
# Revision 1.7.2.1  2003/02/05 23:12:19  klaus
# adapted to RedHat 7.2
#
# Revision 1.7  2002/02/20 20:55:09  klaus
# added rpminstall for to manage rpm generation
#
# Revision 1.6  2002/02/19 19:21:01  klaus
# Corrections to circumvent using lib before installing it
#
# Revision 1.5  2002/02/11 18:09:37  klaus
# moved libpcan.h from include/libpcan.h
#
# Revision 1.4  2002/01/30 20:54:27  klaus
# simple source file header change
#
#****************************************************************************

SRC     = src
INC     = -I. -I../driver
FILES   = $(SRC)/libpcan.c
DBG     = -g

LIBPATH = /usr/lib
LDNAME  = libpcan.so
SONAME  = $(LDNAME).0
TARGET  = $(SONAME).3

CFLAGS = -fPIC -shared -O2 -Wall -Wl,-soname,$(SONAME) -lc $(INC)

all:    $(TARGET)

$(TARGET) : $(FILES)
	# ln -sf ../driver/pcan.h pcan.h
	$(CC) $(FILES) $(CFLAGS) -o $(TARGET)
	ln -sf $(TARGET) $(LDNAME)

clean:
	rm -f $(SRC)/*~ $(SRC)/*.o *~ *.so.*

#********** this entry is reserved for root access only *********************
install:
	cp $(TARGET) $(LIBPATH)/$(TARGET)
	ln -sf $(LIBPATH)/$(TARGET) $(LIBPATH)/$(SONAME)
	ln -sf $(LIBPATH)/$(SONAME) $(LIBPATH)/$(LDNAME)
	cp libpcan.h /usr/include/libpcan.h
	chmod 644 /usr/include/libpcan.h
	/sbin/ldconfig
  
  

