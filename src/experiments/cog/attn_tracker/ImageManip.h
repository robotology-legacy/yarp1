#ifndef IMAGE_MANIP_H
#define IMAGE_MANIP_H

#include <math.h>

#include "Image.h"

#ifndef M_PI
#define M_PI 3.1415926535897932
#endif

extern const PixelRGB RGB_WHITE;
extern const PixelRGB RGB_BLACK;
extern const PixelRGB RGB_RED;
extern const PixelRGB RGB_GREEN;
extern const PixelRGB RGB_BLUE;
extern const PixelRGB RGB_YELLOW;

template <class T>
void AddCircle(Image<T>& dest, const T& pix, int i, int j, int r)
{
  float d, r2 = r*r;
  for (int ii=i-r; ii<=i+r; ii++)
    {
      for (int jj=j-r; jj<=j+r; jj++)
	{
	  d = (ii-i)*(ii-i)+(jj-j)*(jj-j);
	  if (d<=r2)
	    {
	      dest.SafePixel(ii,jj) = pix;
	    }
	}
    }
}

template <class T>
void AddCrossHair(Image<T>& dest, const T& pix, int i, int j, int r)
{
  for (int ii=i-r; ii<=i+r; ii++)
    {
      for (int jj=j-r; jj<=j+r; jj++)
	{
	  if (ii==i||jj==j)
	    {
	      dest.SafePixel(ii,jj) = pix;
	    }
	}
    }
}

template <class T>
void AddCircleOutline(Image<T>& dest, const T& pix, int i, int j, int r)
{
  float d, r2 = r*r, r2l = (r-1.1)*(r-1.1);
  for (int ii=i-r; ii<=i+r; ii++)
    {
      for (int jj=j-r; jj<=j+r; jj++)
	{
	  d = (ii-i)*(ii-i)+(jj-j)*(jj-j);
	  if (d<=r2 && d>=r2l)
	    {
	      dest.SafePixel(ii,jj) = pix;
	    }
	}
    }
}

template <class T>
void AddOvalOutline(Image<T>& dest, const T& pix, int i, int j, int h2, int w2)
{
  float x, y;
  for (float th=0; th<2*M_PI; th+=0.01)
    {
      x = j+w2*cos(th);
      y = i+h2*sin(th);
      dest.SafePixel((int)y,(int)x) = pix;
    }
}


template <class T>
void AddRectangle(Image<T>& dest, const T& pix, int i, int j, int h, int w)
{
  for (int ii=i-h; ii<=i+h; ii++)
    {
      dest.SafePixel(ii,j-w) = pix;
      dest.SafePixel(ii,j-w+1) = pix;
      dest.SafePixel(ii,j+w) = pix;
      dest.SafePixel(ii,j+w-1) = pix;
    }
  for (int jj=j-w; jj<=j+w; jj++)
    {
      dest.SafePixel(i-h,jj) = pix;
      dest.SafePixel(i-h+1,jj) = pix;
      dest.SafePixel(i+h,jj) = pix;
      dest.SafePixel(i+h-1,jj) = pix;
    }
}

int ApplyLabels(Image<int>& src, Image<int>& dest);

template <class T>
int ApplyThreshold(Image<T>& src, Image<T>& dest, 
		   const T& thetalo, const T& thetahi,
		   const T& pix0, const T& pix1)
{
  int h = src.GetHeight();
  int w = src.GetWidth();
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  if (src(i,j)>=thetalo && src(i,j)<=thetahi)
	    {
	      dest(i,j) = pix1;
	    }
	  else
	    {
	      dest(i,j) = pix0;
	    }
	}
    }
  return 0;
}

template <class T1, class T2>
int ApplyOverlay1(Image<T1>& src, Image<T2>& dest, 
		  T1& pix1, T2& pix2)
{
  int h = src.GetHeight();
  int w = src.GetWidth();
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  if (src(i,j)==pix1)
	    {
	      dest(i,j) = pix2;
	    }
	}
    }
  return 0;
}

template <class T>
void SetImage(Image<T>& src, const T& pix)
{
  int h = src.GetHeight();
  int w = src.GetWidth();
  for (int i=0; i<h; i++)
    {
      for (int j=0; j<w; j++)
	{
	  src(i,j) = pix;
	}
    }  
}

#define PFOR(img,i,j) for (int i=0; i<(img).GetHeight(); i++) \
for (int j=0; j<(img).GetWidth(); j++)

#endif

