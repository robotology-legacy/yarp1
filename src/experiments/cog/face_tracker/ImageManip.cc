
#include "ImageManip.h"

#include "legacy_stdimg.h"

const PixelRGB RGB_WHITE(255,255,255);
const PixelRGB RGB_BLACK(0,0,0);
const PixelRGB RGB_RED(255,0,0);
const PixelRGB RGB_GREEN(0,255,0);
const PixelRGB RGB_BLUE(0,0,255);
const PixelRGB RGB_YELLOW(255,255,0);


// this is deliberately inefficient for various reasons,
// there are much faster ways of doing this.

// Need to patch up if plan to call from multiple threads

int xstack[IMG_H*IMG_W*4];
int ystack[IMG_H*IMG_W*4];
int bestId;

static int ApplyRG(ImgBigInt src, ImgBigInt dest)
{
  int i, j;
  int ii, jj;
  int id = 1;
  int top = 0;
  int maxCount = 0;
  int maxCountId = 0;
  int count;
  for (i=0; i<IMG_H; i++)
    {
      for (j=0; j<IMG_W; j++)
	{
	  dest[i][j] = (src[i][j]==0);
	}
    }
  for (i=0; i<IMG_H; i++)
    {
      for (j=0; j<IMG_W; j++)
	{
	  if (dest[i][j] == 0)
	    {
	      id++;
	      /*
	      if (id>255)
		{
		  id = 2;
		}
	       */
	      count = 0;
	      top = 1;
	      ystack[0] = i;
	      xstack[0] = j;
	      while (top>0)
		{
		  top--;
		  ii = ystack[top];
		  jj = xstack[top];
		  if (dest[ii][jj] == 0)
		    {
		      dest[ii][jj] = id;
		      count++;
		      if (ii>0)
			{
			  if (dest[ii-1][jj] == 0)
			    {
			      ystack[top] = ii-1;
			      xstack[top] = jj;
			      top++;
			    }
			}
		      if (ii+1<IMG_H)
			{
			  if (dest[ii+1][jj] == 0)
			    {
			      ystack[top] = ii+1;
			      xstack[top] = jj;
			      top++;
			    }
			}
		      if (jj>0)
			{
			  if (dest[ii][jj-1] == 0)
			    {
			      ystack[top] = ii;
			      xstack[top] = jj-1;
			      top++;
			    }
			}
		      if (jj+1<IMG_W)
			{
			  if (dest[ii][jj+1] == 0)
			    {
			      ystack[top] = ii;
			      xstack[top] = jj+1;
			      top++;
			    }
			}
		    }
		}
	      if (count>maxCount)
		{
		  maxCount = count;
		  maxCountId = id;
		}
	    }
	}
    }
  bestId = maxCountId;
  return maxCountId;
}


// Slower version that applies to images with arbitrary size
static int ApplyRG(Image<int>& src, Image<int>& dest)
{
  static Image<int> xstack;
  static Image<int> ystack;
  int i, j;
  int ii, jj;
  int id = 1;
  int top = 0;
  int maxCount = 0;
  int maxCountId = 0;
  int count;
  int h = src.GetHeight();
  int w = src.GetWidth();
  xstack.Create(h*w*4,1);
  ystack.Create(h*w*4,1);

  for (i=0; i<h; i++)
    {
      for (j=0; j<w; j++)
	{
	  dest(i,j) = (src(i,j)==0);
	}
    }
  for (i=0; i<h; i++)
    {
      for (j=0; j<w; j++)
	{
	  if (dest(i,j) == 0)
	    {
	      id++;
	      /*
	      if (id>255)
		{
		  id = 2;
		}
	       */
	      count = 0;
	      top = 1;
	      ystack(0,0) = i;
	      xstack(0,0) = j;
	      while (top>0)
		{
		  top--;
		  ii = ystack(top,0);
		  jj = xstack(top,0);
		  if (dest(ii,jj) == 0)
		    {
		      dest(ii,jj) = id;
		      count++;
		      if (ii>0)
			{
			  if (dest(ii-1,jj) == 0)
			    {
			      ystack(top,0) = ii-1;
			      xstack(top,0) = jj;
			      top++;
			    }
			}
		      if (ii+1<h)
			{
			  if (dest(ii+1,jj) == 0)
			    {
			      ystack(top,0) = ii+1;
			      xstack(top,0) = jj;
			      top++;
			    }
			}
		      if (jj>0)
			{
			  if (dest(ii,jj-1) == 0)
			    {
			      ystack(top,0) = ii;
			      xstack(top,0) = jj-1;
			      top++;
			    }
			}
		      if (jj+1<w)
			{
			  if (dest(ii,jj+1) == 0)
			    {
			      ystack(top,0) = ii;
			      xstack(top,0) = jj+1;
			      top++;
			    }
			}
		    }
		}
	      if (count>maxCount)
		{
		  maxCount = count;
		  maxCountId = id;
		}
	    }
	}
    }
  bestId = maxCountId;
  return maxCountId;
}



int ApplyLabels(Image<int>& src, Image<int>& dest)
{
  if (src.GetWidth()!=IMG_W||src.GetHeight()!=IMG_H||
      dest.GetWidth()!=IMG_W||dest.GetHeight()!=IMG_H)
    {
      return ApplyRG(src,dest);
    }
  else
    {
      ImgBigInt& ii1 = *((ImgBigInt *)src.GetRawBuffer());
      ImgBigInt& ii2 = *((ImgBigInt *)dest.GetRawBuffer());
      return ApplyRG(ii1,ii2);
    }
  return 0;
}
