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
/// $Id: YARPDIBConverter.cpp,v 1.2 2003-06-06 22:50:22 gmetta Exp $
///
///

//
// YARPConverter.cpp
//

#include "YARPDIBConverter.h"

#ifdef __WIN32__

YARPDIBConverter::YARPDIBConverter()
{
	bufDIB = NULL;
	dataAreaDIB = NULL;
	dimX = 0;
	dimY = 0;
	pixelType = 0;
	headerSize = 0;
	imageSize = 0;
}

YARPDIBConverter::YARPDIBConverter(const YARPGenericImage &img)
{
	ACE_ASSERT (img.GetIplPointer != NULL);

	dimX = img.GetWidth();
	dimY = img.GetHeight();
	pixelType = img.GetID();
	_alloc_dib();
}

YARPDIBConverter::~YARPDIBConverter()
{
	_free_dib();
};

void YARPDIBConverter::Resize(const YARPGenericImage &img)
{
	ACE_ASSERT (img.GetIplPointer != NULL);

	dimX = img.GetWidth();
	dimY = img.GetHeight();
	pixelType = img.GetID();

	_alloc_dib();
}

void YARPDIBConverter::_alloc_dib()
{
	// alloc internal dib
	if (bufDIB != NULL)
		_free_dib();

	switch (pixelType)
	{
		case YARP_PIXEL_MONO_SIGNED:
		case YARP_PIXEL_MONO:
			{
				// MONO
				if ( (dimX > 0) && (dimY > 0) )
				{
					int	addedBytesDIB = _pad_bytes(dimX,4);
					bufDIB = new unsigned char[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256 + (dimX+addedBytesDIB)*dimY];
					ACE_ASSERT(bufDIB != NULL);

					BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)bufDIB;

					COLORREF* rgb = (COLORREF *)(bufDIB + sizeof(BITMAPINFOHEADER));

					headerSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256;
					imageSize = (dimX + addedBytesDIB) * dimY;
					dataAreaDIB = bufDIB + headerSize;

					for (int i = 0; i < 256; i++)
						rgb[i] = RGB(i,i,i); // prepare the palette
		
					dibhdr->biSize = sizeof(BITMAPINFOHEADER);
					dibhdr->biWidth = dimX;
					dibhdr->biHeight = dimY;
					dibhdr->biPlanes = 1;
					dibhdr->biBitCount = 8; // TODO ...
					dibhdr->biCompression = BI_RGB;
					dibhdr->biSizeImage = imageSize;
		
					dibhdr->biXPelsPerMeter = 0;
					dibhdr->biYPelsPerMeter = 0;
					dibhdr->biClrUsed = 256;
					dibhdr->biClrImportant = 256;
				}
			}
			break;
		case YARP_PIXEL_HSV:
		case YARP_PIXEL_BGR:
		case YARP_PIXEL_RGB:
			{
				// RGB
				if ( (dimX > 0) && (dimY > 0) )
				{
					int	addedBytesDIB = _pad_bytes(dimX*3,4);
					bufDIB = new unsigned char[sizeof(BITMAPINFOHEADER) + (dimX*3+addedBytesDIB)*dimY];
					ACE_ASSERT(bufDIB != NULL);

					headerSize = sizeof(BITMAPINFOHEADER);
					imageSize = (dimX * 3 + addedBytesDIB) * dimY;

					BITMAPINFOHEADER* dibhdr = (BITMAPINFOHEADER*)bufDIB;

					dataAreaDIB = bufDIB + headerSize;
		
					dibhdr->biSize = sizeof(BITMAPINFOHEADER);
					dibhdr->biWidth = dimX;
					dibhdr->biHeight = dimY;
					dibhdr->biPlanes = 1;
					dibhdr->biBitCount = 24; 
					dibhdr->biCompression = BI_RGB;
					dibhdr->biSizeImage = imageSize;
		
					dibhdr->biXPelsPerMeter = 0;
					dibhdr->biYPelsPerMeter = 0;
					dibhdr->biClrUsed = 0;
					dibhdr->biClrImportant = 0;
				}
			}
			break;
		case YARP_PIXEL_RGB_SIGNED:
		case YARP_PIXEL_MONO_FLOAT:
		case YARP_PIXEL_RGB_FLOAT:
		case YARP_PIXEL_HSV_FLOAT:
		case YARP_PIXEL_INVALID:
		default:
			// not supported !
			printf ("*** Sorry, convertion to DIB not supported\n");
			ACE_OS::exit(1);
			///ACE_ASSERT (0);
			break;
	}

}

void YARPDIBConverter::_free_dib()
{
	if (bufDIB != NULL)
	{
		delete [] bufDIB;
		bufDIB = NULL;
	};
	dataAreaDIB = NULL;
}


#endif	// __WIN32__