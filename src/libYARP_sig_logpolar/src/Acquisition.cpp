/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #fberton#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: Acquisition.cpp,v 1.1 2004-07-20 15:13:41 eshuy Exp $
///
///

#if !defined(__QNX__) && !defined(__LINUX__)


#include <stdio.h>
#include <stdlib.h> 
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>

#include "LogPolarSDK.h"

#define EPP_D   4 /* EPP:  Data Register          */
#define EPP_A   3 /* EPP:  Addr Register          */
#define SPP_C   2 /* SPP:  Control Register       */

static VCHANNEL vchannel;

/************************************************************************
*																		*
* Giotto_Init															*
* 																		*
* Camera initialization.												*
*																		*
* Input: Address of the parallel port									*
* 																		*
************************************************************************/	

int Giotto_Init(int address)
{
	int retval;

	unsigned short epp_addr, epp_data, spp_ctrl;

	epp_addr = (unsigned short)(address + EPP_A);
	epp_data = (unsigned short)(address + EPP_D);
	spp_ctrl = (unsigned short)(address + SPP_C);

	retval = _outp(spp_ctrl,0x04);
	if (retval != 0x04)
		retval = 0;
	else 
	{
		retval = _outp(epp_addr,0x83);
		if (retval != 0x83)
			retval = 0;
		else
			retval = epp_addr;
	}

	return retval;
}
/************************************************************************
*																		*
* Giotto_Init_Wrapper													*
* 																		*
* Camera initialization.												*
* 																		*
* Input: width															*
* 			Width of the image to be acquired							*
* 		 height															*
* 			Height of the image to be acquired							*
*		 dev_name														*
*			//./LPTSIMPLE1 	for camera acquisition						*
*			filename 		for file acquisition						*
*		 channel														*
*			Channel number (not used)									*
*		 format															*
*			It can be MONO or COLOR (not used)							*
*		 address														*
*			Physical address of the parallel port						*
*			Typically it is 0x378										*
*		 vdesc															*
*			Channel descriptor (not used)								*
* 																		*
************************************************************************/	

int Giotto_Init_Wrapper(int width,
						int height,
						char * dev_name,
						int channel,
						int format,
						int address,
						int *vdesc)

{
	vchannel.format = format;
	vchannel.width  = width;
	vchannel.height = height;
	
	int size = width * height;
		

#ifdef WINNT

	int i;
	unsigned char buf[2];

	if( ((int)dev_name[0]=='\\')&&((int)dev_name[1]=='\\')&&((int)dev_name[2]=='.')) {
	//Input -> Telecamera
		vchannel.source = 1;
		vchannel.grab_fd = open(dev_name, _O_BINARY|_O_RDONLY);
		i = read(vchannel.grab_fd, buf, 2);
	}
	else {
	//Input -> File
		vchannel.source = 0;
		vchannel.grab_fd = _open(dev_name, _O_BINARY|_O_RDONLY);
	}
	return vchannel.grab_fd;

#else

	if( ((int)dev_name[0]=='\\')&&((int)dev_name[1]=='\\')&&((int)dev_name[2]=='.')) 
	{
		vchannel.source = 1;
		vchannel.grab_fd = Giotto_Init(address);
	}
	else
	{
		vchannel.source = 0;
		vchannel.grab_fd = _open(dev_name, _O_BINARY|_O_RDONLY);
	}

	return vchannel.grab_fd;

#endif
}

/************************************************************************
*																		*
* Giotto_Acq															*
* 																		*
* Acquires one frame from the camera									*
*																		*
* Input: image															*
* 			Pointer to the image array									*
* 		 width															*
* 			Width of the destination image								*
* 		 height															*
* 			Height of the destination image								*
* 																		*
************************************************************************/	

int Giotto_Acq(unsigned char * image,
			   int width,
			   int height,			   
			   int address)
{
	int size;
	int i,j,l;
	int retval;
	
	size = width * height;
			
	for (j = 0; j < height; j++)
	{
		for (i = 0; i < width; i++)
			*(image+(j*width+i))=~_inp(address);

		for (i = width; i < 256; i++)
			l=_inp(address);		
	}
	retval = _outp(890,0x04);

	retval = _outp(address,0x83);
	if (retval != 0x83)
		retval = 0;
	else
		retval = size;

	return retval;
}

/************************************************************************
*																		*
* Giotto_Acq_Wrapper													*
* 																		*
* Acquires one frame from the camera									*
*																		*
* Input: Path															*
* 			Working folder												*
* 																		*
************************************************************************/	

int Giotto_Acq_Wrapper(unsigned char * image,
					   struct timeval *acq_time,
					   int *time_stamp,
					   int *vdesc,
					   int address)
{
	int size;
	int H_Size,V_Size;
	int retval;

	H_Size = vchannel.width;
	V_Size = vchannel.height;
	size = vchannel.width * vchannel.height;


#ifdef WINNT

	//Camera Acquisition 
	if (vchannel.source == 1) 
		retval = read(vchannel.grab_fd, image, size);

	//File Acquisition
	else 
	{
		retval = read(vchannel.grab_fd, image, size);
		if (retval != size)
		{
			lseek(vchannel.grab_fd, size, SEEK_SET);
			retval = read(vchannel.grab_fd, image, size);
		}
	}

	return retval;
#else

	//Camera Acquisition 
	if (vchannel.source == 1) 
		retval = Giotto_Acq(image,H_Size,V_Size,address);

	//File Acquisition
	else 
	{
		retval = read(vchannel.grab_fd, image, size);
		if (retval != size)
		{
			lseek(vchannel.grab_fd, 0, SEEK_SET);
			retval = read(vchannel.grab_fd, image, size);
		}
	}

	return retval;

#endif

}

/************************************************************************
*																		*
* Giotto_Close															*
* 																		*
* Closes the acquisition												*
*																		*
************************************************************************/	

void Giotto_Close()
{
}
/************************************************************************
*																		*
* Giotto_Close_Wrapper													*
* 																		*
* Closes the acquisition												*
*																		*
************************************************************************/	

void Giotto_Close_Wrapper(int *vdesc)
{
	if (vchannel.source == 1)
		close(vchannel.grab_fd);
	else
		_close(vchannel.grab_fd);
}


#else

/// defined(__QNX__)

int Giotto_Init(int) { return -1; }
int Giotto_Init_Wrapper(int, int, char *, int, int, int, int *) { return -1; }
int Giotto_Acq(unsigned char *, int, int, int) { return -1; }
int Giotto_Acq_Wrapper(unsigned char *, struct timeval *, int *, int *, int) { return -1; }
void Giotto_Close() { }
void Giotto_Close_Wrapper(int *) { }


#endif
