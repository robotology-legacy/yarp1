#
#	QNX 4.23a Makefile
#
#	MEI DSP motion controller C function library:
#	rebuild instructions for Watcom C v10.6
#

LIBNAME		=	medwc10f

CFLAGS		=	-zq -3 -mf -i=/usr/include
CC			=	wcc386
LIB			=	wlib
LIBFLAGS	=	-+
OBJ			=	o
ERASE		=	rm
BACKUP		=	$(LIBNAME).bak

OBJS		=	idsp.$(OBJ)			\
				imemory.$(OBJ)		\
				iframe.$(OBJ)		\
				ifixed.$(OBJ)		\
									\
				llframe.$(OBJ)		\
				llstatus.$(OBJ)		\
				llpid.$(OBJ)		\
				llpos.$(OBJ)		\
				llef.$(OBJ)			\
				llio.$(OBJ)			\
				llsetvel.$(OBJ)		\
				llmove.$(OBJ)		\
				llconfig.$(OBJ)		\
				llink.$(OBJ)		\
				llbm.$(OBJ)			\
				llfirm.$(OBJ)		\
									\
				bconfig.$(OBJ)		\
				blimits.$(OBJ)		\
									\
				mldsp.$(OBJ)		\
				mluom.$(OBJ)		\
				mlpos.$(OBJ)		\
				mlef.$(OBJ)			\
				mlsetvel.$(OBJ)		\
				mlmove.$(OBJ)		\
				mlsmove.$(OBJ)		\
				mlconfig.$(OBJ)		\
				mlio.$(OBJ)			\
				mlstatus.$(OBJ)		\
				mllink.$(OBJ)		\
				mlerror.$(OBJ)		\
				mlframe.$(OBJ)		\
				mlpmove.$(OBJ)		\
				mldspf.$(OBJ)		\
				mlrmove.$(OBJ)		\
                mlrsmove.$(OBJ)		\
                mlspline.$(OBJ)		\
									\
				hldsp.$(OBJ)		\
				envutil.$(OBJ)		\
				backward.$(OBJ)		\
				extras.$(OBJ)		\
				mboard.$(OBJ)		\
									\
				system.$(OBJ)		\
				mlserc.$(OBJ)		\
				llserc.$(OBJ)		\
				sercrset.$(OBJ)		\
				llsrset.$(OBJ)		\
				sercmisc.$(OBJ)		\
				sercdiag.$(OBJ)		\
				sydsp.$(OBJ)		\
									\
                mllsr.$(OBJ)		\
                mltan.$(OBJ)		\
				sincomm.$(OBJ)

marker:		$(OBJS) $(OPTIONALOBJS)
	$(ERASE) $(BACKUP)


.c.$(OBJ):
	$(CC) $(CFLAGS) $*.c
	$(LIB) $(LIBNAME) $(LIBFLAGS) $*.o $(LIBDEBUG)

