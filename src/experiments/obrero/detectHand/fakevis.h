// Yo Emacs!  Parse this file in -*- C++ -*- mode.
#ifndef FAKEVIS_INC
#define FAKEVIS_INC

#include <assert.h>
//cck 03-12-02
#include <typeinfo>
//#include "ipl.h"
#include "ipl.h"
#include "stdio.h"

////////////////////////////
/// helper classes from MFC

class CSize
{
 public:
  int cx;
  int cy;
	
  CSize(){}
  CSize(int x, int y)
    {cx = x; cy = y;}
	
  int operator == (const CSize& sz)
    { return (cx==sz.cx)&&(cy==sz.cy); }
	
  int operator != (const CSize& sz)
    { return (cx!=sz.cx)||(cy!=sz.cy); }
};

class CRect
{
 public:
  CSize r1, r2;
	
  CRect(){}
  CRect(int x0, int y0, int x1, int y1) :
    r1(x0,y0), r2(x1,y1)
    {}
};




/////////////////////////////

class CRowIndex
{
 private:
  int rows;
  //table is a pointer to an array of char *
  char **table;
 public:
  CRowIndex()
    { table = NULL;  rows = 0; }
	
  virtual ~CRowIndex()
    { ClearRowIndex(); }
	
  void ClearRowIndex()
    { 
      if (table!=NULL) delete[] table;
      rows = 0;  table = NULL;
    }
	
  void SetRowIndex(int rows_i, char *src, int row_step_in_bytes_i)
    {
      ClearRowIndex();
      table = new char * [rows_i];
      assert(table!=NULL);
      rows = rows_i;
      for (int i=0; i<rows; i++)
	{
	  table[i] = src + (i * row_step_in_bytes_i);
	}
    }
	
  inline char *RowIndexPointer(int index) const
    { return table[index]; }
};

enum BufferType {
  IPL_BUFFER,
  NON_IPL_BUFFER
};

enum OwnershipType {
  EXTERNAL_OWNERSHIP,
  INTERNAL_OWNERSHIP
};

//////////////////////////////

class MVisImageBase : public CRowIndex
{
 private:
  int h, w;
  int x0, y0;
  int x1, y1;
  int pixel_size_in_bytes;
  char *buffer;
  IplImage * ipl_image;
  BufferType buffer_type;
  OwnershipType ownership;
  int row_alignment_offset_in_bytes;
  int row_step_in_bytes;
  int actual_buffer_size_in_bytes;
	
  bool ipl_compatible;
	
 public:
  MVisImageBase()
    { 
      h=w=0; 
      pixel_size_in_bytes=0; 
		
      buffer = NULL; 
      ipl_image = NULL; 
		
      buffer_type = IPL_BUFFER;
      ownership = INTERNAL_OWNERSHIP;
      ipl_compatible = true;
    }
	
  virtual ~MVisImageBase()
    { Destroy(); }
	
  //////////////////////////////////////////
  //IPL allocation and deallocation
	
  IplImage * MyCreateImageHeader(CSize &size, int depth, int channels);
  void MyCreateAndSetImageData(IplImage *image);
  void MyDeleteIPLBuffer(IplImage *image);
  void MyReleaseImageHeader(IplImage *image);
	
  //////////////////////////////////////////
	
  void Create(int nh, int nw, int pixel_size_in_bytes_i, int channels);
  void Refer(char *nbuffer, int nh, int nw, int pixel_size_in_bytes_i, int channels);
	
  void SetBufferType(BufferType buffer_type_in)
    {
      //set the buffer to be of IPL type or not of IPL type
      buffer_type = buffer_type_in;
    }
	
  void SetBufferOwnership(OwnershipType ownership_in)
    {
      ownership = ownership_in;
    }
	
  OwnershipType GetBufferOwnership()
    {
      return(ownership);
    }
	
  BufferType GetBufferType()
    {
      return(buffer_type);
    }
	
  void Destroy();
	
  inline int Top() const
    { return y0; }
	
  inline int Bottom() const
    { return y1; }
	
  inline int Left() const
    { return x0; }
	
  inline int Right() const
    { return x1; }
	
  inline int LeftInBytes() const
    { return x0*pixel_size_in_bytes; }
	
  inline int RightInBytes() const
    { return x1*pixel_size_in_bytes; }
	
  inline int Width() const
    { return (x1-x0)+1; }
	
  inline int Height() const
    { return (y1-y0)+1; }
	
  inline int W() const
    { return(Width()); }
	
  inline int H() const
    { return(Height()); }
	
  inline int RowStepInBytes() const
    { return row_step_in_bytes; //this is in bytes
    }
	
  inline int ActualBufferSize()
    {
      return actual_buffer_size_in_bytes;
    }
	
  inline int Memory() const
    { return h*w; }
	
  inline int MemoryWidth() const
    { return w; }
	
  inline int MemoryHeight() const
    { return h; }
	
  inline CSize Size() const
    { return CSize(Width(),Height()); }
	
  inline CRect Rect() const
    { return CRect(x0,y0,x1,y1); }
	
  void SetRect(CRect r);
	
  inline void SetRect(int x0, int y0, int x1, int y1)
    { CRect r(x0,y0,x1,y1);  SetRect(r); }
	
  inline char *GetRawBuffer() const
    { return buffer; }
	
  inline IplImage * GetIPL()
    {
      return(ipl_image);
    }
};

int GetChannels(const std::type_info &t_info);
////////////////////////////////////////

template <class TPixel>
class MVisImage : public MVisImageBase
{
 public:
	
  void CheckPixelType()
    {
      /*
	if(GetChannels(typeid(TPixel)) < 1)
	{
	printf("\n\n");
	printf("*****************************************************************\n");
	printf("\nERROR: unknown pixel type!\n");
	printf("\n(specify the number of channels associated with this pixel\n");
	printf("within the function GetChannels() within the file fakevis.cpp)\n");
	printf("*****************************************************************\n\n\n");
	exit(1);
	}
      */
    }
	
  MVisImage() { CheckPixelType(); }
	
  MVisImage(int h, int w) 
    { Create(h,w); }
	
  MVisImage(CSize sizei) 
    {  Create(sizei.cx, sizei.cy); }
	
  virtual ~MVisImage()
    {};
	
  void Copy(MVisImage &inimage)
    {
      CheckPixelType();
		
      if( (inimage.Height() != Height()) ||
	  (inimage.Width() != Width()) )
	{
	  Create(inimage.Height(), inimage.Width());
	}
		
      int a_size;
      a_size = ActualBufferSize();
		
      if(a_size != inimage.ActualBufferSize())
	{
	  exit(0);
	}
		
      int i;
      for(i=0; i<a_size; i++) GetRawBuffer()[i] = inimage.GetRawBuffer()[i];
		
      //SetRect(inimage.Left(), inimage.Top(), inimage.Right(), inimage.Bottom());
    }
	
  //	MVisImage &SubImage(int x0, int y0, int x1, int y1)
  //	{
  //		MVisImage *out = new MVisImage();
  //		out->Refer(
  //			GetRawBuffer(),
  //			MemoryHeight(),
  //			MemoryWidth(),
  //			example_pixel.GetChannels()
  //			);
  //		out->SetRect(x0,y0,x1,y1);
  //		return(*out);
  //	}
	
  inline void Create(int h, int w) 
    { 
      CheckPixelType(); 
      MVisImageBase::Create(
			    h,w,
			    sizeof(TPixel), 
			    GetChannels(typeid(TPixel))); 
    }
	
  inline void Refer(char *buf, int h, int w) 
    { 
      CheckPixelType(); 
      MVisImageBase::Refer(
			   buf,
			   h,w,
			   sizeof(TPixel), 
			   GetChannels(typeid(TPixel))); 
    }
	
  inline TPixel *GetBuffer()  const
    { return (TPixel*)GetRawBuffer(); }
	
  inline TPixel *RowPointer(int index) const
    { return (TPixel *)(RowIndexPointer(index)); }
  //    { return GetBuffer()+(index*MemoryWidth()); }
	
  inline TPixel& Pixel(int x, int y) const
    { return *(RowPointer(y)+x); }
  //    { return *(GetBuffer()+(y*MemoryWidth())+x); }
  // Check -- order
	
  inline TPixel *GetFirstPixel()
    { return &(Pixel(Left(),Top())); }
	
  inline int GetPixelSizeInBytes()
    {
      return( sizeof(TPixel) );
    }
	
  inline int GetLastToFirstOffsetInBytes()
    { return RowStepInBytes() - ( RightInBytes()-LeftInBytes()) - GetPixelSizeInBytes(); }
};



//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//	
//				Pixel Types
//

struct MVisRGBABytePixel
{
  public:
unsigned char r, g, b, a; //for Meteor 
  //for now we assume the a component is a don't care value
  //unsigned char r,g,b; //for PXC200
  // going to ignore "a"
	
  inline int R() const { return r; }
  inline int G() const { return g; }
  inline int B() const { return b; }
  inline int A() const { assert(1==0); return 0; }
	
  inline void SetR(unsigned char x) { r = x; }
  inline void SetG(unsigned char x) { g = x; }
  inline void SetB(unsigned char x) { b = x; }
  inline void SetA(unsigned char x) { assert(1==0); }
	
  inline void Set(unsigned char ri,unsigned char gi, unsigned char bi)
  {
    r = ri;
    g = gi;
    b = bi;
  }
};


struct MVisRGBBytePixel
{
  public:
// unsigned char r, g, b, a; //for Meteor 
unsigned char r,g,b; //for PXC200
  // going to ignore "a"
	
  inline int R() const { return r; }
  inline int G() const { return g; }
  inline int B() const { return b; }
  //  inline int A() const { assert(1==0); return 0; }
	
  inline void SetR(unsigned char x) { r = x; }
  inline void SetG(unsigned char x) { g = x; }
  inline void SetB(unsigned char x) { b = x; }
  // inline void SetA(unsigned char x) { assert(1==0); }
	
  inline void Set(unsigned char ri,unsigned char gi, unsigned char bi)
  {
    r = ri;
    g = gi;
    b = bi;
  }
	
  inline void Set(MVisRGBBytePixel & pix_in)
  {
    r = pix_in.R();
    g = pix_in.G();
    b = pix_in.B();
  }
};

typedef MVisRGBBytePixel MVisRGBPixel;
typedef unsigned char MVisBytePixel;
typedef signed char MVisSignedCharPixel;
typedef int MVisIntPixel;
typedef float MVisFloatPixel;
typedef unsigned short int MVisUSIntPixel;
typedef short int  MVisShortIntPixel;
typedef unsigned short MVisU16BitPixel;

////////////////////
// Image type names
//

typedef MVisImage<MVisU16BitPixel> MVisU16BitImage;
typedef MVisImage<MVisBytePixel> MVisByteImage;
typedef MVisImage<MVisFloatPixel> MVisFloatImage;
typedef MVisImage<MVisFloatPixel> MVisGrayFloatImage;
typedef MVisImage<MVisRGBABytePixel> MVisRGBAByteImage;
typedef MVisImage<MVisRGBBytePixel> MVisRGBByteImage;
typedef MVisImage<MVisRGBPixel> MVisRGBImage;
typedef MVisImage<MVisIntPixel> MVisIntImage;


////////////////////////////////////////////
//// Shorthand for Common Types

//Microsoft Specific Type sizes
//char, unsigned char, signed char 1 byte 
//short, unsigned short 2 bytes 
//int, unsigned int 4 bytes 
//long, unsigned long 4 bytes 
//float 4 bytes 
//double 8 bytes 
//long double 8 bytes 


typedef unsigned char U1Pix;
typedef signed char S1Pix;
typedef unsigned short U2Pix;
typedef short S2Pix;
typedef S2Pix S2Int;
typedef unsigned int U4Pix;
typedef int S4Pix;

typedef MVisImage<U1Pix> U1Im;
typedef MVisImage<S1Pix> S1Im;
typedef MVisImage<U2Pix> U2Im;
typedef MVisImage<S2Pix> S2Im;
typedef MVisImage<U4Pix> U4Im;
typedef MVisImage<S4Pix> S4Im;
typedef MVisImage<U4Pix> U8Im;
typedef MVisImage<S4Pix> S8Im;



//cck 03-12-02
//typedef unsigned __int64 U8Pix;
//typedef signed __int64	S8Pix;

/////////////////////////////////////////////////
/////////////////////////////////////////////////

typedef MVisRGBBytePixel RGBPix;
typedef char CPix;
typedef unsigned char UCPix;
typedef signed char SCPix;
typedef signed short int SSIPix;
typedef unsigned short int USIPix;
typedef int IPix;
typedef unsigned int UIPix;
typedef signed int SIPix;
typedef long int LIPix;
typedef unsigned long int ULIPix;
typedef signed long int SLIPix;
typedef float FPix;
typedef double DPix;

typedef MVisImage<RGBPix> RGBIm;
typedef MVisImage<CPix> CIm;
typedef MVisImage<UCPix>  UCIm;
typedef MVisImage<SCPix>  SCIm;
typedef MVisImage<SSIPix>  SSIIm;
typedef MVisImage<USIPix>  USIIm;
typedef MVisImage<IPix>  IIm;
typedef MVisImage<UIPix>  UIIm;
typedef MVisImage<SIPix>  SIIm;
typedef MVisImage<LIPix>  LIIm;
typedef MVisImage<ULIPix>  ULIIm;
typedef MVisImage<SLIPix> SLIIm;
typedef MVisImage<FPix> FIm;
typedef MVisImage<DPix> DIm;

/////////////////////////////////////////////////
/////////////////////////////////////////////////


//ANSI and some more Microsoft
//	__int8 nSmall;      // Declares 8-bit integer
//	__int16 nMedium;    // Declares 16-bit integer
//	__int32 nLarge;     // Declares 32-bit integer
//	__int64 nHuge;      // Declares 64-bit integer
//	The types __int8, __int16, and __int32 are synonyms for the 
//	ANSI types that have the same size, and are useful for writing 
//	portable code that behaves identically across multiple platforms. 
//	Note that the __int8 data type is synonymous with type char,
//	__int16 is synonymous with type short, and __int32 is synonymous 
//	 with type int. The __int64 type has no ANSI equivalent.

//REASON FOR NOT USING __int8 etc.:
//	Because __int8, __int16, and __int32 are considered synonyms
//  by the compiler, care should be taken when using these types
//	as arguments to overloaded function calls. 




//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


void CopyToImage(MVisRGBImage &image_out, unsigned char * buf, int buf_w, int buf_h);
void CopyToBuffer(MVisRGBImage &image_in, unsigned char * & buf, int & buf_w, int &buf_h);



//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		
//				VisMap
//
//

// For now, VisMap bodies will be kept in this header file --
// portability issues.

// will assume regions of same width selected within the images


template <class Operator, class TPixel1>
  void VisMap1(Operator& op, 
	       MVisImage<TPixel1>& img1)
{
  char *it1 = (char *) img1.GetFirstPixel();
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int h = img1.Height();
  int w = img1.Width();
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1));
	  it1=it1+pixel_size_in_bytes1;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
    }
}

template <class Operator, class TPixel1, class TPixel2>
  void VisMap2(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2)
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
	
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
	
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
    }
}


template <class Operator, class TPixel1, class TPixel2, class TPixel3>
  void VisMap3(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2,
	       MVisImage<TPixel3>& img3)
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
  char * it3 = (char *) img3.GetFirstPixel();
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
  int pixel_size_in_bytes3 = img3.GetPixelSizeInBytes();
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  assert(h==img3.Height() && w==img3.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2),* ((TPixel3 *) it3));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	  it3=it3+pixel_size_in_bytes3;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
      it3 += img3.GetLastToFirstOffsetInBytes();
    }
}

template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4>
  void VisMap4(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2,
	       MVisImage<TPixel3>& img3,
	       MVisImage<TPixel4>& img4)
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
  char * it3 = (char *) img3.GetFirstPixel();
  char * it4 = (char *) img4.GetFirstPixel();
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
  int pixel_size_in_bytes3 = img3.GetPixelSizeInBytes();
  int pixel_size_in_bytes4 = img4.GetPixelSizeInBytes();
	
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  assert(h==img3.Height() && w==img3.Width());
  assert(h==img4.Height() && w==img4.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2),* ((TPixel3 *) it3),
	     * ((TPixel4 *) it4));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	  it3=it3+pixel_size_in_bytes3;
	  it4=it4+pixel_size_in_bytes4;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
      it3 += img3.GetLastToFirstOffsetInBytes();
      it4 += img4.GetLastToFirstOffsetInBytes();
    }
}

template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5>
  void VisMap5(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2,
	       MVisImage<TPixel3>& img3,
	       MVisImage<TPixel4>& img4,
	       MVisImage<TPixel5>& img5)
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
  char * it3 = (char *) img3.GetFirstPixel();
  char * it4 = (char *) img4.GetFirstPixel();
  char * it5 = (char *) img5.GetFirstPixel(); 
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
  int pixel_size_in_bytes3 = img3.GetPixelSizeInBytes();
  int pixel_size_in_bytes4 = img4.GetPixelSizeInBytes();
  int pixel_size_in_bytes5 = img5.GetPixelSizeInBytes();
	
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  assert(h==img3.Height() && w==img3.Width());
  assert(h==img4.Height() && w==img4.Width());
  assert(h==img5.Height() && w==img5.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2),* ((TPixel3 *) it3),
	     * ((TPixel4 *) it4),* ((TPixel5 *) it5));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	  it3=it3+pixel_size_in_bytes3;
	  it4=it4+pixel_size_in_bytes4;
	  it5=it5+pixel_size_in_bytes5;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
      it3 += img3.GetLastToFirstOffsetInBytes();
      it4 += img4.GetLastToFirstOffsetInBytes();
      it5 += img5.GetLastToFirstOffsetInBytes();
    }
}


template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5, class TPixel6>
  void VisMap6(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2,
	       MVisImage<TPixel3>& img3,
	       MVisImage<TPixel4>& img4,
	       MVisImage<TPixel5>& img5,
	       MVisImage<TPixel6>& img6
	       )
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
  char * it3 = (char *) img3.GetFirstPixel();
  char * it4 = (char *) img4.GetFirstPixel();
  char * it5 = (char *) img5.GetFirstPixel(); 
  char * it6 = (char *) img6.GetFirstPixel();
	
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
  int pixel_size_in_bytes3 = img3.GetPixelSizeInBytes();
  int pixel_size_in_bytes4 = img4.GetPixelSizeInBytes();
  int pixel_size_in_bytes5 = img5.GetPixelSizeInBytes();
  int pixel_size_in_bytes6 = img6.GetPixelSizeInBytes();
	
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  assert(h==img3.Height() && w==img3.Width());
  assert(h==img4.Height() && w==img4.Width());
  assert(h==img5.Height() && w==img5.Width());
  assert(h==img6.Height() && w==img6.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2),* ((TPixel3 *) it3),
	     * ((TPixel4 *) it4),* ((TPixel5 *) it5),* ((TPixel6 *) it6));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	  it3=it3+pixel_size_in_bytes3;
	  it4=it4+pixel_size_in_bytes4;
	  it5=it5+pixel_size_in_bytes5;
	  it6=it6+pixel_size_in_bytes6;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
      it3 += img3.GetLastToFirstOffsetInBytes();
      it4 += img4.GetLastToFirstOffsetInBytes();
      it5 += img5.GetLastToFirstOffsetInBytes();
      it6 += img6.GetLastToFirstOffsetInBytes();
    }
}

template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5, class TPixel6, class TPixel7>
  void VisMap6(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2,
	       MVisImage<TPixel3>& img3,
	       MVisImage<TPixel4>& img4,
	       MVisImage<TPixel5>& img5,
	       MVisImage<TPixel6>& img6,
	       MVisImage<TPixel7>& img7
	       )
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
  char * it3 = (char *) img3.GetFirstPixel();
  char * it4 = (char *) img4.GetFirstPixel();
  char * it5 = (char *) img5.GetFirstPixel(); 
  char * it6 = (char *) img6.GetFirstPixel(); 
  char * it7 = (char *) img7.GetFirstPixel(); 
	
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
  int pixel_size_in_bytes3 = img3.GetPixelSizeInBytes();
  int pixel_size_in_bytes4 = img4.GetPixelSizeInBytes();
  int pixel_size_in_bytes5 = img5.GetPixelSizeInBytes();
  int pixel_size_in_bytes6 = img6.GetPixelSizeInBytes();
  int pixel_size_in_bytes7 = img7.GetPixelSizeInBytes();
	
	
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  assert(h==img3.Height() && w==img3.Width());
  assert(h==img4.Height() && w==img4.Width());
  assert(h==img5.Height() && w==img5.Width());
  assert(h==img6.Height() && w==img6.Width());
  assert(h==img7.Height() && w==img7.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2),* ((TPixel3 *) it3),
	     * ((TPixel4 *) it4),* ((TPixel5 *) it5),* ((TPixel6 *) it6),
	     * ((TPixel7 *) it7));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	  it3=it3+pixel_size_in_bytes3;
	  it4=it4+pixel_size_in_bytes4;
	  it5=it5+pixel_size_in_bytes5;
	  it6=it6+pixel_size_in_bytes6;
	  it7=it7+pixel_size_in_bytes7;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
      it3 += img3.GetLastToFirstOffsetInBytes();
      it4 += img4.GetLastToFirstOffsetInBytes();
      it5 += img5.GetLastToFirstOffsetInBytes();
      it6 += img6.GetLastToFirstOffsetInBytes();
      it7 += img7.GetLastToFirstOffsetInBytes();
    }
}


template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5, class TPixel6, class TPixel7, class TPixel8>
  void VisMap8(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2,
	       MVisImage<TPixel3>& img3,
	       MVisImage<TPixel4>& img4,
	       MVisImage<TPixel5>& img5,
	       MVisImage<TPixel6>& img6,
	       MVisImage<TPixel7>& img7,
	       MVisImage<TPixel8>& img8
	       )
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
  char * it3 = (char *) img3.GetFirstPixel();
  char * it4 = (char *) img4.GetFirstPixel();
  char * it5 = (char *) img5.GetFirstPixel(); 
  char * it6 = (char *) img6.GetFirstPixel(); 
  char * it7 = (char *) img7.GetFirstPixel(); 
  char * it8 = (char *) img8.GetFirstPixel(); 
	
	
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
  int pixel_size_in_bytes3 = img3.GetPixelSizeInBytes();
  int pixel_size_in_bytes4 = img4.GetPixelSizeInBytes();
  int pixel_size_in_bytes5 = img5.GetPixelSizeInBytes();
  int pixel_size_in_bytes6 = img6.GetPixelSizeInBytes();
  int pixel_size_in_bytes7 = img7.GetPixelSizeInBytes();
  int pixel_size_in_bytes8 = img8.GetPixelSizeInBytes();
	
	
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  assert(h==img3.Height() && w==img3.Width());
  assert(h==img4.Height() && w==img4.Width());
  assert(h==img5.Height() && w==img5.Width());
  assert(h==img6.Height() && w==img6.Width());
  assert(h==img7.Height() && w==img7.Width());
  assert(h==img8.Height() && w==img8.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2),* ((TPixel3 *) it3),
	     * ((TPixel4 *) it4),* ((TPixel5 *) it5),* ((TPixel6 *) it6),
	     * ((TPixel7 *) it7),* ((TPixel8 *) it8));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	  it3=it3+pixel_size_in_bytes3;
	  it4=it4+pixel_size_in_bytes4;
	  it5=it5+pixel_size_in_bytes5;
	  it6=it6+pixel_size_in_bytes6;
	  it7=it7+pixel_size_in_bytes7;
	  it8=it8+pixel_size_in_bytes8;
	}
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
      it3 += img3.GetLastToFirstOffsetInBytes();
      it4 += img4.GetLastToFirstOffsetInBytes();
      it5 += img5.GetLastToFirstOffsetInBytes();
      it6 += img6.GetLastToFirstOffsetInBytes();
      it7 += img7.GetLastToFirstOffsetInBytes();
      it8 += img8.GetLastToFirstOffsetInBytes();
    }
}



template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5, class TPixel6, class TPixel7, class TPixel8, class TPixel9>
  void VisMap9(Operator& op, 
	       MVisImage<TPixel1>& img1,
	       MVisImage<TPixel2>& img2,
	       MVisImage<TPixel3>& img3,
	       MVisImage<TPixel4>& img4,
	       MVisImage<TPixel5>& img5,
	       MVisImage<TPixel6>& img6,
	       MVisImage<TPixel7>& img7,
	       MVisImage<TPixel8>& img8,
	       MVisImage<TPixel9>& img9
	       )
{
  char * it1 = (char *) img1.GetFirstPixel();
  char * it2 = (char *) img2.GetFirstPixel();
  char * it3 = (char *) img3.GetFirstPixel();
  char * it4 = (char *) img4.GetFirstPixel();
  char * it5 = (char *) img5.GetFirstPixel(); 
  char * it6 = (char *) img6.GetFirstPixel(); 
  char * it7 = (char *) img7.GetFirstPixel(); 
  char * it8 = (char *) img8.GetFirstPixel(); 
  char * it9 = (char *) img9.GetFirstPixel(); 
	
  int pixel_size_in_bytes1 = img1.GetPixelSizeInBytes();
  int pixel_size_in_bytes2 = img2.GetPixelSizeInBytes();
  int pixel_size_in_bytes3 = img3.GetPixelSizeInBytes();
  int pixel_size_in_bytes4 = img4.GetPixelSizeInBytes();
  int pixel_size_in_bytes5 = img5.GetPixelSizeInBytes();
  int pixel_size_in_bytes6 = img6.GetPixelSizeInBytes();
  int pixel_size_in_bytes7 = img7.GetPixelSizeInBytes();
  int pixel_size_in_bytes8 = img8.GetPixelSizeInBytes();
  int pixel_size_in_bytes9 = img9.GetPixelSizeInBytes();
	
  int h = img1.Height();
  int w = img1.Width();
  assert(h==img2.Height() && w==img2.Width());
  assert(h==img3.Height() && w==img3.Width());
  assert(h==img4.Height() && w==img4.Width());
  assert(h==img5.Height() && w==img5.Width());
  assert(h==img6.Height() && w==img6.Width());
  assert(h==img7.Height() && w==img7.Width());
  assert(h==img8.Height() && w==img8.Width());
  assert(h==img9.Height() && w==img9.Width());
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  op(* ((TPixel1 *) it1),* ((TPixel2 *) it2),* ((TPixel3 *) it3),
	     * ((TPixel4 *) it4),* ((TPixel5 *) it5),* ((TPixel6 *) it6),
	     * ((TPixel7 *) it7),* ((TPixel8 *) it8),* ((TPixel9 *) it9));
	  it1=it1+pixel_size_in_bytes1;
	  it2=it2+pixel_size_in_bytes2;
	  it3=it3+pixel_size_in_bytes3;
	  it4=it4+pixel_size_in_bytes4;
	  it5=it5+pixel_size_in_bytes5;
	  it6=it6+pixel_size_in_bytes6;
	  it7=it7+pixel_size_in_bytes7;
	  it8=it8+pixel_size_in_bytes8;
	  it9=it9+pixel_size_in_bytes9;
	}
		
      it1 += img1.GetLastToFirstOffsetInBytes();
      it2 += img2.GetLastToFirstOffsetInBytes();
      it3 += img3.GetLastToFirstOffsetInBytes();
      it4 += img4.GetLastToFirstOffsetInBytes();
      it5 += img5.GetLastToFirstOffsetInBytes();
      it6 += img6.GetLastToFirstOffsetInBytes();
      it7 += img7.GetLastToFirstOffsetInBytes();
      it8 += img8.GetLastToFirstOffsetInBytes();
      it9 += img9.GetLastToFirstOffsetInBytes();
    }
}




#endif

