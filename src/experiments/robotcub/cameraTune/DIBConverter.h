/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
/// $Id: DIBConverter.h,v 1.1 2005-02-18 16:59:26 emmebi75 Exp $
///
///

// nat June 2001
// 
// May 2002 -- modified by nat
//

#ifndef __YARPDIBConverterh__
#define __YARPDIBConverterh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPImage.h>

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
	}

	// flip and convert image to DIB.
	// check the convertion, it may not work for all data type
	// (tested on YarpPixelBGR)
	inline const unsigned char *ConvertAndFlipToDIB (const YARPGenericImage& img)
	{
		ACE_ASSERT (img.GetIplPointer != NULL);

		_refresh_flip_dib(img);
		return bufDIB;
	}

	inline const unsigned char *GetBuffer(void)
	{
		return bufDIB;
	}

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
		if (dimX != img.GetWidth() || dimY != img.GetHeight() || pixelType != img.GetID())
			Resize(img);
		if (bufDIB  == NULL)
			_alloc_dib ();

		switch (pixelType)
		{
			case YARP_PIXEL_HSV:
			case YARP_PIXEL_BGR:
			case YARP_PIXEL_RGB:
			{
				const int addedBytesDIB = _pad_bytes(dimX*3,4);

				int i = 0;
				const int h = img.GetHeight();
				char *out = (char *) dataAreaDIB;

				for (i = 0 ; i < h; i++)
				{
					const char *array = img.GetArray()[i];
						
					memcpy(out, array, dimX*3);
					out += (addedBytesDIB+dimX*3);
				}
			}
			break;

			case YARP_PIXEL_MONO:
			{
				const int addedBytesDIB = _pad_bytes(dimX,4);

				int i = 0;
				const int h = img.GetHeight();
				char *out = (char *) dataAreaDIB;

				for (i = 0 ; i < h; i++)
				{
					const char *array = img.GetArray()[i];
						
					memcpy(out, array, dimX);
					out += (addedBytesDIB+dimX);
				}
			}
			break;

			default:
				{
					/// silently ignores the conversion.
					memset (dataAreaDIB, 0, ((BITMAPINFOHEADER*)bufDIB)->biSizeImage);
				}
				break;
		}
	}

	inline void _refresh_flip_dib (const YARPGenericImage& img) 
	{
		// prepare internal dib
		// TODO need the whole data area be prepared ?
		
		if (dimX != img.GetWidth() || dimY != img.GetHeight() || pixelType != img.GetID())
			Resize(img);
		if (bufDIB  == NULL)
			_alloc_dib ();

		switch (pixelType)
		{
			case YARP_PIXEL_HSV:
			case YARP_PIXEL_BGR:
			case YARP_PIXEL_RGB:
			{
				const int addedBytesDIB = _pad_bytes(dimX*3,4);

				int i = 0;
				const int h = img.GetHeight()-1;
				char *out = (char *) dataAreaDIB;

				for (i = h ; i >= 0; i--)
				{
					const char *array = img.GetArray()[i];
						
					memcpy(out, array, dimX*3);
					out += (addedBytesDIB+dimX*3);
				}
			}
			break;

			case YARP_PIXEL_MONO:
			{
				const int addedBytesDIB = _pad_bytes(dimX,4);

				int i = 0;
				const int h = img.GetHeight()-1;
				char *out = (char *) dataAreaDIB;

				for (i = h ; i >= 0; i--)
				{
					const char *array = img.GetArray()[i];
						
					memcpy(out, array, dimX);
					out += (addedBytesDIB+dimX);
				}
			}
			break;

			default:
				{
					/// silently ignores the conversion.
					memset (dataAreaDIB, 0, ((BITMAPINFOHEADER*)bufDIB)->biSizeImage);
				}
				break;
		}
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

#endif
