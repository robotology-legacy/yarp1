/*
paulfitz Mon May 21 16:51:13 EDT 2001

Pending: adding alignment, ipl compatibility. -paulfitz

Trying to simplify reference counting structures by removing cloning
functionality. -paulfitz

Should probably commit to XY coordinates -paulfitz
*/

#ifndef YARPImage_INC
#define YARPImage_INC

#include "YARPAll.h"

#include <assert.h>

// class for maintaining index of row pointers -- feel free to discard
class YARPRowIndex
{
private:
  int rows;
  char **table;
public:
  YARPRowIndex()
    { table = NULL;  rows = 0; }

  virtual ~YARPRowIndex()
    { ClearRowIndex(); }
  
  void ClearRowIndex()
    { 
      if (table!=NULL) delete[] table;
      rows = 0;  table = NULL;
    }
  
  void SetRowIndex(int size, char *src, int step)
    {
      ClearRowIndex();
      table = new char * [size];
      assert(table!=NULL);
      rows = size;
      for (int i=0; i<rows; i++)
	{
	  table[i] = src+i*step;
	}
    }
      
  char *RowIndexPointer(int index) const
    { return table[index]; }
};


class YARPBaseImage
{
protected:
  
  // probably redundant with ipl data
  int type_id;
  int height, width;

  // need actual buffer
  char *buffer;
  YARPReferenceCount buffer_references;

  // may be useful for maintaining index of row pointers in absence of ipl
  YARPRowIndex row_index;

public:

  YARPBaseImage();

  virtual ~YARPBaseImage();

  virtual int GetTypeID();
  virtual int GetBytesPerPixel();
  int GetHeight();
  int GetWidth();

  void CreateRC(int height, int width, int bytes);
  void CreateXY(int x, int y, int bytes);
  
  void Copy(const YARPBaseImage& src);
  void CastCopy(const YARPBaseImage& src);
  void ScaledCopy(const YARPBaseImage& src, int height, int width);
  void Refer(const YARPBaseImage& src);
  
  char *GetRawBuffer();
  int GetImageLength();
};


template <class T>
class YARPImageOf : public YARPBaseImage
{
public:
  virtual int GetTypeID();

  virtual int GetBytesPerPixel()
    { return sizeof(T); }

  void CreateRC(int height, int width);  // bytes now implicit
  void CreateXY(int x, int y);
  
  // pixel access
  T null_pixel;

  T& PixelXY(int x, int y);     // give inline definition
  T& SafePixelXY(int x, int y); // returns reference to null_pixel if 
                                // out-of-bounds
  T& operator()(int x, int y) 
    { return PixelXY(x,y); }

  T& PixelRC(int row, int column)       
    { return PixelXY(column,row);     }

  T& SafePixelRC(int row, int column);  
    { return SafePixelXY(column,row); }
};



#endif
