/* 
    bttv - Bt848 frame grabber driver

    Copyright (C) 1996,97 Ralph Metzler (rjkm@thp.uni-koeln.de)
	Copyright (C) 2002. QNX version Carlos Beltran (cbeltran@dist.unige.it)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <string.h>
#ifdef __cplusplus
	extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <hw/pci.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <devctl.h>
#include <inttypes.h>
#include <pthread.h>


#include "../include/bttv.h"
#define BTTV_MAX 6

static resmgr_connect_funcs_t    connect_funcs;
static resmgr_io_funcs_t         io_funcs;
static iofunc_attr_t             attr;


unsigned char * image_buffer = NULL;

extern int width;
extern int height;

/********************************************************************************
	The read function.
*********************************************************************************/
int
io_read (resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    int         nleft;
    int         nbytes;
    int         nparts;
    int         status;
	int i;
	
	

    if ((status = iofunc_read_verify (ctp, msg, ocb, NULL)) != EOK)
        return (status);
        
    if (msg->i.xtype & _IO_XTYPE_MASK != _IO_XTYPE_NONE)
        return (ENOSYS);
        
    //Not offset, therefore is the fist time we enter the read for this
    //chuck of data.
    if ( ocb->offset == 0 )
    {
    	BttvxWaitEvent();
		BttvxAcquireBuffer(image_buffer);
	}
    /*
     *  on all reads (first and subsequent) calculate
     *  how many bytes we can return to the client,
     *  based upon the number of bytes available (nleft)
     *  and the client's buffer size
     */
     
    nleft = ocb->attr->nbytes - ocb->offset;
    nbytes = min (msg->i.nbytes, nleft);

    if (nbytes > 0) {

		SETIOV(ctp->iov,image_buffer+ ocb->offset,nbytes);

        /* set up the number of bytes (returned by client's read()) */
        _IO_SET_READ_NBYTES (ctp, nbytes);

        /*
         * advance the offset by the number of bytes
         * returned to the client.
         */

        ocb->offset += nbytes;
        
        if (ocb->offset == ocb->attr->nbytes)
        {
		 	BttvxReleaseBuffer();
		 	ocb->offset = 0; //TETS
        }
        
        nparts = 1;
    } else {
        /*
         * they've asked for zero bytes or they've already previously
         * read everything
         */
         
        _IO_SET_READ_NBYTES (ctp, 0);
        nparts = 0;
    }

    /* mark the access time as invalid (we just accessed it) */

    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_ATIME;

    return (_RESMGR_NPARTS (nparts));
}


/********************************************************************************
  Open
*********************************************************************************/
int
io_open(resmgr_context_t *ctp, io_open_t *msg,RESMGR_HANDLE_T *handle, void *extra)
//open_bttvx()
{
	
	BttvxSetImageBuffer(0, image_buffer);
	open_bttvx();
	
	return (iofunc_open_default (ctp, msg, handle, extra));
}


/*************************************************************************
 *	Main function. Activate the resorce manager	
 *
 *************************************************************************/

//int init_bttvx(int video_mode, int device_number, int w, int h) //Video format, device id, width and height
int 
main (int argc, char * argv[])
{
	int i; 
	/* declare variables we'll be using */
	resmgr_attr_t		resmgr_attr;
	dispatch_t			*dpp;
	dispatch_context_t  *ctp;
	int                 id;
	int video_format = 0;
	int device_id = 0;
	char device_name[100];
	char shell_command[100];
	char buffer[20];
	char buffer2[20];
	int bus, dev_func;


	//Check the arguments
	
	if ( argc == 1 || argc > 3)
	{
		printf("Use: ./bttvx video_format [device_id]\n");
		printf("0 ----> PAL\n");
		printf("1 ----> NTSC\n"); 
		printf("2 ----> S-Video\n");
		exit(0);
	}
	

	//Get video format
	video_format = atoi(argv[1]);

	//Check driver index
	if (argv[2] != NULL)
	{
		device_id = atoi(argv[2]);
		if (device_id < 0 || device_id > BTTV_MAX)
		{
			printf("bttvx: Sorry!, the device id overcomes the maximum number of devices allowed\n");
			exit(0);
		}
	}
	

	//Filling passed width and height
	
	width = W;
	height= H;
	
	image_buffer = (unsigned char *) malloc (width*height*deep);
		
	strcpy(device_name,"/dev/bttvx");
	strcat(device_name,itoa(device_id,buffer,10)); //Complete the name

	init_bttvx(video_format,device_id, width,height, 0, 0);
	
	/* initialize dispatch interface */
	
	if((dpp = dispatch_create()) == NULL) 
	{
		fprintf(stderr, 
			    "%s: Unable to allocate dispatch handle.\n",
				argv[0]);
		return EXIT_FAILURE;
	}
	

	/* initialize resource manager attributes */
	memset(&resmgr_attr, 0, sizeof resmgr_attr);
	
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size=VBIBUF_SIZE;

	/* initialize functions for handling messages */
	
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, 
					 &connect_funcs,
					 _RESMGR_IO_NFUNCS, 
					 &io_funcs);

	io_funcs.read = io_read;
	//io_funcs.devctl = io_devctl;
	connect_funcs.open = io_open;
	
	/* initialize attribute structure used by the device */
	iofunc_attr_init(&attr, 
					 S_IFNAM | 0666, 
					 0, 0);
	//attr.nbytes = VBIBUF_SIZE + 1;
	attr.nbytes = VBIBUF_SIZE;

    /* attach our device name */
    id = resmgr_attach(dpp,            /* dispatch handle        */
                       &resmgr_attr,   /* resource manager attrs */
                       device_name,  /* device name            */
                       _FTYPE_ANY,     /* open type              */
                       0,              /* flags                  */
                       &connect_funcs, /* connect routines       */
                       &io_funcs,      /* I/O routines           */
                       &attr);         /* handle                 */
    if(id == -1) {
        fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
        return EXIT_FAILURE;
    }

	/* allocate a context structure */
	ctp = dispatch_context_alloc(dpp);

	/* start the resource manager message loop */
	
	while(1) 
	{
		if((ctp = dispatch_block(ctp)) == NULL) 
		{
			fprintf(stderr, "block error\n");
			return EXIT_FAILURE;
		}
		dispatch_handler(ctp);
	}
}

#ifdef  __cplusplus
}
#endif

