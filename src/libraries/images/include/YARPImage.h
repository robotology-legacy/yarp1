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
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPImage.h,v 1.5 2003-06-22 11:35:24 gmetta Exp $
///
///

#ifndef YARPImage_INC
#define YARPImage_INC

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#ifdef __QNX4__
#include "YARPSafeNew.h"
#endif

///
/// this is required to ship images through ports.
#include "YARPNetworkTypes.h"

#ifdef __LINUX__
#include <sys/ipl.h>
#endif

// the image types partially reflect the IPL image types.
// IPL allows 16/32bpp images. Should we constrain our lib to 8bpp.
// There must be a pixel type for every ImageType entry.
enum
{	 
	YARP_PIXEL_INVALID = 0,
	YARP_PIXEL_MONO,
	YARP_PIXEL_RGB,
	YARP_PIXEL_HSV,
	YARP_PIXEL_BGR,  // external libraries sometimes want flipped order
	YARP_PIXEL_MONO_SIGNED,
	YARP_PIXEL_RGB_SIGNED,
	YARP_PIXEL_MONO_FLOAT,
	YARP_PIXEL_RGB_FLOAT,
	YARP_PIXEL_HSV_FLOAT,
	YARP_PIXEL_INT,
	// negative ids reserved for pixels of undeclared type but known size
	// in bytes
};

#include "begin_pack_for_net.h"

// Pasa: had to change the pixel type names because they would clash
//	with the old lib names.

typedef unsigned char YarpPixelMono;

typedef NetInt32 YarpPixelInt;

struct YarpPixelRGB
{ 
  unsigned char r,g,b; 
  
  YarpPixelRGB() { r = g = b = 0; }
  YarpPixelRGB(unsigned char n_r, unsigned char n_g, unsigned char n_b)
    { r = n_r;  g = n_g;  b = n_b; }
} PACKED_FOR_NET;

struct YarpPixelBGR
{ 
  unsigned char b,g,r; 
  YarpPixelBGR() { b = g = r = 0; }
  YarpPixelBGR(unsigned char n_r, unsigned char n_g, unsigned char n_b)
    { r = n_r;  g = n_g;  b = n_b; }
} PACKED_FOR_NET;

typedef struct { unsigned char h,s,v; } YarpPixelHSV;
typedef char YarpPixelMonoSigned;
typedef struct { char r,g,b; } YarpPixelRGBSigned;
typedef float YarpPixelFloat;
typedef struct { float r,g,b; } YarpPixelRGBFloat;
typedef struct { float h,s,v; } YarpPixelHSVFloat;

#include "end_pack_for_net.h"

class YARPRefCount;

///
/// alignment required for IP, MMX, optimized stuff.
#ifdef __QNX__
const int YarpImageAlign = 8;
#else
const int YarpImageAlign = 8;
#endif

// Note on IPL alignment stuff:
//	- initial address is aligned on a QUADWORD boundary (allocation).
//	- lines are extended to QUADWORD boundary.
//	- the initial alignment is mostly transparent.
//	- NT: memory allocated by IPL must be released within the same thread.
//
// Note on IPL memory allocation.
//	- Cleanup function is used to clean IPL allocated mem in a MT environment.
//	- A better memory allocator for IPL should be provided in the future.
//	- Use Cleanup before leaving the thread which allocated the image.

// Note on IPL alignment
//	- it's not required that all functions being compatible with IPL align.
//	- those which are not should assert if called for an image with
//		padding not equal to zero.
//	- most of the time image size is anyway a multiple of a QUADWORD.
//	- ex: 128 sq. images or typical 32x64 logpolar format.
//
#ifndef __WIN32__
#	include <FakeIpl.h>
#endif

// TODO: linux implementation.
//#ifdef __LINUX__
//#	include <ipl.h>
//#endif

#ifdef __WIN32__

#	ifdef __FAKEIPL__
#	include <FakeIpl.h>		// use FakeIpl under WIN32
#	else
#	include <sys/ipl.h>
#	endif

#endif

// Note on Data double array. It's nice to have it because allows 
// transparently ignoring padding stuff. Simply access the image
// through row pointers... this should ease porting stuff from older 
// formats.
//
class YARPGenericImage
{
protected:
	int type_id;

	IplImage* pImage;
	char **Data;       // this is not IPL. it's char to maintain IPL compatibility

	YARPRefCount *buffer_references; // counts users of pImage->imageData
	int is_owner;

	// ipl allocation is done in two steps.
	// _alloc allocates the actual ipl pointer.
	// _alloc_data allocates the image array and data.
	// memory is allocated in a single chunck. Row ptrs are then
	// made to point appropriately. This is compatible with IPL and
	// SOMEONE says it's more efficient on NT.
	void _alloc (void);
	void _alloc_extern (void *buf);
	void _alloc_data (void);
	void _free (void);
	void _free_data (void);

	void _make_independent(); 
	void _set_ipl_header(int x, int y, int pixel_type);
	void _free_ipl_header();
	void _alloc_complete(int x, int y, int pixel_type);
	void _free_complete();

	void _alloc_complete_extern(void *buf, int x, int y, int pixel_type);

	// computes the # of padding bytes. These are always at the end of the row.
	int _pad_bytes (int linesize, int align) const;

public:
	YARPGenericImage(void);
	YARPGenericImage(const YARPGenericImage& i);
	virtual ~YARPGenericImage();
	virtual void Cleanup () { _free_complete(); }

	virtual void operator=(const YARPGenericImage& i);

	virtual int GetID (void) const { return type_id; }
	void SetID (int n_id) { type_id = n_id; }
	void CastID (int n_id) { type_id = n_id; }

	int GetPixelSize (void) const;

	inline int GetHeight() const 
	{ if (pImage!=NULL) return pImage->height; else return 0; }
	inline int GetWidth() const  
	{ if (pImage!=NULL) return pImage->width;  else return 0; }

	void Resize (int x, int y, int pixel_type);
	void Resize (int x, int y) { Resize(x,y,GetID()); }
	void Clear (void);
	void Zero (void);

	void UncountedRefer(void *buf, int x, int y, int pixel_type) { _alloc_complete_extern(buf,x,y,pixel_type); }
	void UncountedRefer(void *buf, int x, int y) { _alloc_complete_extern(buf,x,y,GetID()); }
	void UncountedRefer(const YARPGenericImage& src)
	{
		_alloc_complete_extern((void*)src.GetRawBuffer(), src.GetWidth(), src.GetHeight(), src.GetID());
	}

	void Refer(YARPGenericImage& src);
	void ReferOrCopy(YARPGenericImage& src);

	void CastCopy(const YARPGenericImage& src);
	void PeerCopy(const YARPGenericImage& src);
	void ScaledCopy(const YARPGenericImage& src);
	void ScaledCopy(const YARPGenericImage& src, int nx, int ny);

	void Crop (YARPGenericImage& id, int startX, int startY);
	void Paste (YARPGenericImage& is, int startX, int startY);

	char *GetRawBuffer() const { return (pImage!=NULL) ? pImage->imageData : NULL; }

	// cast operator allows calling ipl functions directly.
	operator IplImage*() const { return pImage; }
	IplImage* GetIplPointer(void) const { return pImage; }

	// rows array.
	inline char **GetArray(void) const { return (char **)Data; }

	// actual array.
	inline char *GetAllocatedArray(void) const { return (char*) pImage->imageData; }

	// actual array size.
	inline int GetAllocatedDataSize(void) const { return pImage->imageSize; }

	// actual row size in bytes.
	inline int GetAllocatedLineSize(void) const { return pImage->widthStep; }

	// LATER: take into account pixel size also!
	inline int GetPadding() const { return _pad_bytes (pImage->width * pImage->nChannels, YarpImageAlign); }
	inline char *RawPixel(int x, int y) const { return (Data[y] + x*GetPixelSize()); }
};


template <class T>
class YARPImageOf : public YARPGenericImage
{
public:
	YARPImageOf(const YARPImageOf<T>& i) : YARPGenericImage (i) {}
	YARPImageOf () : YARPGenericImage () {}

	virtual void operator=(const YARPImageOf<T>& i) { YARPGenericImage::operator= (i); }

	// if type is not specialized, arbitrary id is specified that carries size
	// of pixel
	virtual inline int GetID() const;

	void Resize(int x, int y) { YARPGenericImage::Resize(x,y,GetID()); }

	T null_pixel;

	inline T& Pixel(int x, int y) { return *((T*)(Data[y] + x*sizeof(T))); }

	T& operator()(int x, int y)	{ return Pixel(x,y); }

	T& AssertedPixel(int x, int y)
	{
		ACE_ASSERT(Data!=NULL && pImage != NULL);
		ACE_ASSERT(x >= 0 && 
			 x < pImage->width &&
			 y >= 0 &&
			 y < pImage->height);
		return Pixel(x,y);
	}

	T& SafePixel(int x, int y)
	{
		ACE_ASSERT(Data != NULL && pImage != NULL);
		if (x >= 0 &&
			x < pImage->width &&
			y >= 0 &&
			y < pImage->height)  
			return Pixel(x,y);
		return null_pixel;
	}

	T& NullPixel() { return null_pixel; }
	T *GetTypedBuffer() { return (T*)GetRawBuffer(); }
};

template<class T>
inline int YARPImageOf<T>::GetID() const
{ return -((int)sizeof(T)); }

#define __YARPIMAGE_ASSOCIATE_TAG(tag,T) inline int YARPImageOf<T>::GetID() const { return tag; }

__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO,YarpPixelMono)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB,YarpPixelRGB)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_HSV,YarpPixelHSV)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_BGR,YarpPixelBGR)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO_SIGNED,YarpPixelMonoSigned)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB_SIGNED,YarpPixelRGBSigned)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_MONO_FLOAT,YarpPixelFloat)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_RGB_FLOAT,YarpPixelRGBFloat)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_HSV_FLOAT,YarpPixelHSVFloat)
__YARPIMAGE_ASSOCIATE_TAG(YARP_PIXEL_INT,YarpPixelInt)

#undef __YARPIMAGE_ASSOCIATE_TAG

void SatisfySize(YARPGenericImage& src, YARPGenericImage& dest);

#endif



