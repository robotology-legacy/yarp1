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
///                    #nat, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPDIBConverter.h,v 1.3 2003-06-17 20:20:36 babybot Exp $
///
///

// nat June 2001
// 
// May 2002 -- modified by nat
//

#ifndef __YARPDIBConverterh__
#define __YARPDIBConverterh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

//
// This class is mostly ok also for QNX.
//	- LATER: implement iplConvertToDIB within FakeIpl.
//

#ifdef __WIN32__	/// this is only because this part is not tested under QNX/Linux.

#include "YARPImage.h"
#include "sys/iplwind.h"


////
////
/// LATER: troubles in converting from a generic DIB size.

///
///
///
class YARPDIBConverter
{
public:
	YARPDIBConverter(void);
	YARPDIBConverter(const YARPGenericImage& img);
	~YARPDIBConverter();

	void Resize(const YARPGenericImage& img);

	// convert image to DIB.
	inline const unsigned char *ConvertToDIB (const YARPGenericImage& img)
	{
		ACE_ASSERT (img.GetIplPointer != NULL);

		_refresh_dib(img);
		return bufDIB;
	};

	inline void ConvertFromDIB (YARPGenericImage& img)
	{
		img.Resize(dimX, dimY);
		iplConvertFromDIB((BITMAPINFOHEADER *)bufDIB, img);
	};

	inline const unsigned char *GetBuffer(void)
	{
		return bufDIB;
	};

	inline bool LoadDIB(const char *filename)
	{
		BITMAPFILEHEADER fhdr;
		BITMAPINFOHEADER Hdr;

		int	addedBytesDIB = _pad_bytes(dimX,4);

		FILE  *fp = ACE_OS::fopen(filename, "rb");
		if (fp == NULL)
			return false;
		
		char *dummy = (char *) &fhdr;
		// read file header
		ACE_OS::fread(dummy, sizeof(char), sizeof(BITMAPFILEHEADER), fp);
		
		// read image header
		dummy = (char *) &Hdr;
		ACE_OS::fread(dummy, sizeof(char), sizeof(BITMAPINFOHEADER), fp);

		if ( (dimX != Hdr.biWidth) || (dimY != Hdr.biHeight) ||
			((pixelType == YARP_PIXEL_RGB) && (Hdr.biBitCount != 24)) ||
			((pixelType == YARP_PIXEL_MONO) && (Hdr.biBitCount != 8)))
			{
				if (Hdr.biBitCount == 8)
					pixelType = YARP_PIXEL_MONO;
				else if (Hdr.biBitCount == 24)
					pixelType = YARP_PIXEL_RGB;
				else
					return false;

				dimX = Hdr.biWidth;
				dimY = Hdr.biHeight;
				
				_alloc_dib();
			}
		
		
		// copy header ...
		memcpy(bufDIB, &Hdr, sizeof(BITMAPINFOHEADER));

		// read the palette, if any
		if (pixelType == YARP_PIXEL_MONO) 
		{
			COLORREF* rgb = (COLORREF *)(bufDIB + sizeof(BITMAPINFOHEADER));
			ACE_OS::fread(rgb, sizeof(RGBQUAD),256,fp);
		}

		// read bits ...
		ACE_OS::fread(dataAreaDIB, sizeof(char), imageSize, fp);
				
		// close file
		ACE_OS::fclose(fp);

		return true;
	}
	
	inline void const SaveDIB(const char *filename)
	{
		BITMAPFILEHEADER fhdr;
		LPBITMAPINFOHEADER lpbi;

		int	addedBytesDIB = _pad_bytes(dimX,4);
		int dibSize;
		int offset;

		lpbi = (LPBITMAPINFOHEADER) bufDIB;
		
		dibSize = imageSize + headerSize;
		offset = sizeof(BITMAPFILEHEADER) + headerSize;
				
		FILE  *fp = ACE_OS::fopen(filename, "wb");
		
		fhdr.bfType = 0x4d42;
		fhdr.bfSize = dibSize + sizeof(BITMAPFILEHEADER);
		fhdr.bfReserved1 = 0;
		fhdr.bfReserved2 = 0;
		fhdr.bfOffBits = (DWORD) offset;

		// write file header
		ACE_OS::fwrite((char*) &fhdr, sizeof(char), sizeof(BITMAPFILEHEADER), fp);
		
		// write DIB header and Bits
		ACE_OS::fwrite(bufDIB, sizeof(char), dibSize, fp);

		// close file
		ACE_OS::fclose(fp);
	}

private:
	// prepare internal buffer
	void _alloc_dib();
	// destroy internal buffer
	void _free_dib();

	inline void _refresh_dib (const YARPGenericImage& img) 
	{
		// prepare internal dib
		// TODO need the whole data area be prepared ?
		
		if (dimX != img.GetWidth() || dimY != img.GetHeight() || pixelType != img.GetID())
			Resize(img);
		if (bufDIB  == NULL)
			_alloc_dib ();

		if (pixelType == YARP_PIXEL_HSV)
		{
			// special conversion is required ...
			unsigned char *dest = dataAreaDIB;
			unsigned char *src = (unsigned char *) img.GetRawBuffer();

			int addedBytesDIB = _pad_bytes(dimX*3,4);
			int numBytes = (dimX*3+addedBytesDIB)*dimY;

			memcpy(src, dest, numBytes);
		}
		else
			iplConvertToDIB((IplImage *)img, (BITMAPINFOHEADER*)bufDIB, IPL_DITHER_NONE, IPL_PALCONV_NONE);
	}

	inline int _pad_bytes(int linesize, int align)
	{
		int rem = linesize % align;
		return (rem != 0) ? (align - rem) : rem;
	}

	unsigned char *dataAreaDIB;			//pointer to data
	unsigned char *bufDIB;				//pointer to header
	int dimX;
	int dimY;
	int pixelType;
	int headerSize;
	int imageSize;
};

#endif	// __WIN32__

#endif
