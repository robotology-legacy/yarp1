#include "ImgTrack.h"

#include <math.h>

#define ImgTrackerTool ImgTrackTool

//#define BLOCK_SIZE 9
//#define SEARCH_SIZE 5

//#define BLOCK_SIZE 10
//#define SEARCH_SIZE 5

#include <stdlib.h>

struct Pixel3
{
  unsigned char b, g, r;
};

static float ComparePixel(ImgInt s1, int r1, int c1, 
		   ImgInt s2, int r2, int c2, int BLOCK_SIZEX, 
			  int BLOCK_SIZEY, int NORMALIZE)
{
  int delx = BLOCK_SIZEX;
  int dely = BLOCK_SIZEY;
  int i, j;
  float d, diff=0;
  if (!NORMALIZE)
    {
      for (i=-dely; i<=dely; i++)
	{
	  for (j=-delx; j<=delx; j++)
	    {
	      d = s1[r1+i][c1+j] - s2[r2+i][c2+j];
	      diff += (d>0)?d:-d;
	    }
	}
    }
  else
    {
      float t1=0, t2=0;
      float mu1, mu2;
      float s12 = 1;
      for (i=-dely; i<=dely; i++)
	{
	  for (j=-delx; j<=delx; j++)
	    {
	      t1 += s1[r1+i][c1+j];
	      t2 += s2[r2+i][c2+j];
	    }
	}
      float area = (2*delx+1)*(2*dely+1);
      mu1 = t1/area;
      mu2 = t2/area;
      float theta = 0.1; //128*del*del;
      float w;
      if (t1>theta && t2>theta)
	{
	  s12 = t2/t1;
	}
      for (i=-dely; i<=dely; i++)
	{
	  for (j=-delx; j<=delx; j++)
	    {
//	      w = 2*del+1-abs(i)-abs(j);
//	      d = (s1[r1+i][c1+j]-mu1)-(s2[r2+i][c2+j]-mu2);
	      d = (s1[r1+i][c1+j]*s12)-(s2[r2+i][c2+j]);
//	      d *= w;
	      diff += (d>0)?d:-d;
	    }
	}
    }
  return diff;
}

template <class T>
T max(T x, T y)
{ return (x>y)?x:y; }

static float ComparePixel(ImgInt3 s1, int r1, int c1, 
		   ImgInt3 s2, int r2, int c2,int BLOCK_SIZEX,
			  int BLOCK_SIZEY, int NORMALIZE)
{
  int delx = BLOCK_SIZEX;
  int dely = BLOCK_SIZEY;
  int i, j;
  float d, diff=0;

  if (NORMALIZE)
    {
      for (i=-dely; i<=dely; i++)
	{
	  for (j=-delx; j<=delx; j++)
	    {
	      d = s1[r1+i][c1+j][0] - s2[r2+i][c2+j][0];
	      diff += (d>0)?d:-d;
	      d = s1[r1+i][c1+j][1] - s2[r2+i][c2+j][1];
	      diff += (d>0)?d:-d;
	      d = s1[r1+i][c1+j][2] - s2[r2+i][c2+j][2];
	      diff += (d>0)?d:-d;
	    }
	}
    }
  else
    {
      Pixel3 *p1, *p2;
      int drg, drb, dgb;
      for (i=-dely; i<=dely; i++)
	{
	  for (j=-delx; j<=delx; j++)
	    {
	      p1 = (Pixel3*) s1[r1+i][c1+j];
	      p2 = (Pixel3*) s2[r2+i][c2+j];
	      //diff += max(0,-(p1->r - p1->g) * (p2->r - p2->g));
	      //diff += max(0,-(p1->r - p1->b) * (p2->r - p2->b));
	      d = max(abs(p1->r-p1->g),abs(p2->r-p2->g))
		*abs((p1->r-p1->g) - (p2->r-p2->g));
	      d += max(abs(p1->r-p1->b),abs(p2->r-p2->b))
		*abs((p1->r-p1->b) - (p2->r-p2->b));
	      d *= abs(p1->r+p1->g+p1->b-(p2->r+p2->g+p2->b));
//	      d /= 1000;
//	      if (d>255) d=255;
	      diff += d;
	    }
	}
    }
  return diff;
}

template <class ImgType>
static float FindPixel(ImgType s1, int r1, int c1, ImgType s2, 
		       int *pr2, int *pc2,int BLOCK_SIZEX, int BLOCK_SIZEY, 
		       int SEARCH_SIZEX, int SEARCH_SIZEY,
		       int OFFSET_X, int OFFSET_Y,
		       int NORMALIZE)
{
  int r2=(*pr2)+OFFSET_Y, c2=(*pc2)+OFFSET_X;
  float best = 999999999999.0, current, central=0;
  float worst = 0;
  int i, j;
  int delx = SEARCH_SIZEX;
  int dely = SEARCH_SIZEY;
  for (i=-dely+OFFSET_Y; i<=dely+OFFSET_Y; i++)
    {
      for (j=-delx+OFFSET_X; j<=delx+OFFSET_X; j++)
	{
	  int r1d = (*pr2)+i;
	  int c1d = (*pc2)+j;
	  if (c1+BLOCK_SIZEX<128 && c1-BLOCK_SIZEX>=0 &&
	      r1+BLOCK_SIZEY<128 && r1-BLOCK_SIZEY>=0 &&
	      c1d+BLOCK_SIZEX<128 && c1d-BLOCK_SIZEX>=0 &&
	      r1d+BLOCK_SIZEY<128 && r1d-BLOCK_SIZEY>=0)
	    {
	      current = ComparePixel(s1,r1,c1,s2,r1d,c1d,BLOCK_SIZEX,BLOCK_SIZEY,NORMALIZE);
	      if (i==0 && j==0)
		{
		  central = current;
		}
	      if (current<best || (current==best && i==0 && j==0))
		{
		  r2 = (*pr2)+i;
		  c2 = (*pc2)+j;
		  best = current;
		}
	      if (current>worst)
		{
		  worst = current;
		}
	    }
	}
    }
  /*
  if (central<best*1.1)
    {
      r2 = (*pr2);
      c2 = (*pc2);
    }
  */
  *pr2 = r2;
  *pc2 = c2;
  
  float safe = 10;
  return (fabs(worst-best)+safe)/(best+safe);
//  return best;
}

void ImgTrackerTool::Apply(ImgInt frame1, ImgInt frame2, int x, int y)
{
  quality = FindPixel(frame1, y, x, frame2, &ty, &tx,blockXSize,blockYSize,windowXSize,windowYSize,windowXOffset,windowYOffset,normalize);
}

void ImgTrackerTool::Apply(ImgInt3 frame1, ImgInt3 frame2, int x, int y)
{
  quality = FindPixel(frame1, y, x, frame2, &ty, &tx,blockXSize,blockYSize,windowXSize,windowYSize,windowXOffset,windowYOffset,normalize);
}




void ImgTrackerTool::Apply(YARPImageOf<YarpPixelMono>& src)
{
  YARPImageOf<YarpPixelMono> nextImg,blah;
  int ox, oy;
  int q;

  if (src.GetWidth()!=IMG_W||src.GetHeight()!=IMG_H)
    {
      printf("Image tracking code is old, and specific to 128x128 images\n");
      exit(1);
    }

  nextImg.Refer(src);
  if (first)
    {
      first = 0;
      ResetXY();
      prevImg.PeerCopy(nextImg);
      delta = 0;
    }
  else
    {
      ox = tx;
      oy = ty;
      ImgInt& ii1 = *((ImgInt *)prevImg.GetRawBuffer());
      ImgInt& ii2 = *((ImgInt *)src.GetRawBuffer());
      Apply(ii1, ii2, tx, ty);
      if (ox != tx || oy != ty || delta)
	{
	  //printf("Copy %d %d %d!\n",ox-tx,oy-ty,delta);
	  prevImg.PeerCopy(nextImg);
	  delta = 0;
	}
    }
}




void ImgTrackerTool::Apply(YARPImageOf<YarpPixelMono>& src, 
			   YARPImageOf<YarpPixelMono>& dest, 
			   int x, int y)
{
  YARPImageOf<YarpPixelMono> prevImg, nextImg;
  int ox, oy;
  int q;

  if ((src.GetWidth()!=IMG_W||src.GetHeight()!=IMG_H) || 
      (dest.GetWidth()!=IMG_W||dest.GetHeight()!=IMG_H))
    {
      printf("Image tracking code is old, and specific to 128x128 images\n");
      exit(1);
    }

  prevImg.Refer(src);
  nextImg.Refer(dest);
  tx = x; ty = y;
  ImgInt& ii1 = *((ImgInt *)prevImg.GetRawBuffer());
  ImgInt& ii2 = *((ImgInt *)nextImg.GetRawBuffer());
  Apply(ii1, ii2, tx, ty);
}





void ImgTrackerTool::Apply(YARPImageOf<YarpPixelBGR>& src)
{
  YARPImageOf<YarpPixelBGR> nextImg, blah;
  int ox, oy;
  int q;
  
  if (src.GetWidth()!=IMG_W||src.GetHeight()!=IMG_H)
    {
      printf("Image tracking code is old, and specific to 128x128 images\n");
      exit(1);
    }

  nextImg.Refer(src);
  if (first)
    {
      first = 0;
      ResetXY();
      prevImg3.PeerCopy(nextImg);
      delta = 0;
    }
  else
    {
      ox = tx;
      oy = ty;
      ImgInt3& ii1 = *((ImgInt3 *)prevImg3.GetRawBuffer());
      ImgInt3& ii2 = *((ImgInt3 *)src.GetRawBuffer());
      Apply(ii1, ii2, tx, ty);
      if (ox != tx || oy != ty || delta)
	{
	  //printf("Copy %d %d %d!\n",ox-tx,oy-ty,delta);
	  prevImg3.PeerCopy(nextImg);
	  delta = 0;
	}
    }
}
