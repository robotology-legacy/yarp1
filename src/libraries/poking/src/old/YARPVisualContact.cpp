#include "YARPVisualContact.h"

#include "YARPImageDraw.h"
#include "YARPTime.h"
#include "IntegralImageTool.h"
#include "YARPImageLabel.h"
#include "YARPImageFile.h"
#include "graph.h"

#include "YARPVisMap.h"

//#ifdef USE_TIME
//#define FOR_PASA
#define TESTING

#ifdef __LINUX__
#define SHOW_OPT
#endif

#define PRETTY_OUTPUT
#define PRETTY_OUTPUT2

//#ifndef __LINUX__
//#endif

//#ifdef __LINUX__

// these defines turn on stuff that tries to deal with concavities
#define NEW_SEG
#define NEW_SEG2


//#endif

#ifdef __QNX__
#define for if (1) for 
#endif

/***************************************************************************
 *** Globals and simple functions
 ***************************************************************************/

int contact_declared = 0;
static double contact_time = -10000;
static double movement_time = -10000;
static int force_first = 0;
static int use_segmentation = 1;
static int since_contact = 10000;
static float global_dx_grip = 0;
static float global_dy_grip = 0;

#define IMAGE_GAP 3

template <class T>
T square(T x)
{
  return x*x;
}

template <class T>
T maxf(T x, T y)
{
  return (x>y)?x:y;
}

template <class T>
T minf(T x, T y)
{
  return (x<y)?x:y;
}


/***************************************************************************
 *** segment out good stuff
 ***************************************************************************/

#define DEL 1
#define DIAG 1.4142136
#define HALF_DIAG 2.236068
#define LINE_STRENGTH 10.0
#define PRE_MOTION 5
#define DIM 128

inline float Compare(YARPImageOf<YarpPixelFloat>& src_motion, 
		     YARPImageOf<YarpPixelFloat>& src_variance, 
		     int i0, int j0, int i1, int j1)
{
  float v = 0;
  float s0 = src_motion(i0,j0);
  float s1 = src_motion(i1,j1);
  //float d0 = src_variance(i0,j0);
  if ((s0>0.5)&&(s1>0.5))
    {
      v = 1;
    }
  else
    {
      v = 2; //+d0/50.0;
    }
  //if (d0<50)
  return v*0.001;
}

float reg_factor = 0.01;

inline float Compare2(YARPImageOf<YarpPixelFloat>& src_motion, 
		     YARPImageOf<YarpPixelFloat>& src_variance, 
		     int i0, int j0, int i1, int j1)
{
  float v = 0;
  float s0 = src_motion(i0,j0);
  float s1 = src_motion(i1,j1);
  //float d0 = src_variance(i0,j0);
  //v = 0.001*(top-minf(top,sqr(s0-s1)));
  //  v = 0.01*(top-minf(top,square(maxf(s0,s1))));
  /*
  int del = 4;
  if ((s0>del)&&(s1>del))
    {
      v = 1;
    }
  else
    {
      v = 2;
    }
  */

  //float top = 5;
  //v = 1 + 3*(top - minf(top,minf(s0,s1)))/top;

  //float top = 5;
  //v = 1 + 1.5 * (top - minf(top,maxf(s0,s1)))/top;

  //float top = 10;
  //v = 0.5 + 3 * (top - minf(top,maxf(s0,s1)))/top;

  float top = 10;
  float off = 1;

#ifndef NEW_SEG
  v = off + 2 * (top - minf(top,maxf(s0,s1)))/top;
#else
  off = 0.2;
#ifdef NEW_SEG2
  top = 40;
  off = reg_factor; //0.01;
#endif
  v = off + 2 * (top - minf(top,maxf(s0,s1)))/top;
#endif


  /*
  float ec = 0;
  if (i0==0 && i0==i1) v = ec;
  if (j0==0 && j0==j1) v = ec;
  if (i0==DIM-1 && i0==i1) v = ec;
  if (j0==DIM-1 && j0==j1) v = ec;
  */

  return v;
}


float TweakSegment(YARPImageOf<YarpPixelBGR>& src,
		   YARPImageOf<YarpPixelMono>& src_mask,
		   YARPImageOf<YarpPixelMono>& dest_mask,
		   YARPImageOf<YarpPixelBGR>& dest, int reg_mode = 0)
{
  assert(src.GetHeight()==DIM && src.GetWidth()==DIM);

  SatisfySize(src,dest_mask);

  static YARPImageOf<YarpPixelFloat> src_mono, src_mono_blur, 
    src_mask0, src_mask_wide, src_r, src_g, src_b, src_r2, src_g2, src_b2;
  static IntegralImageTool ii_tool;

  src_mono.CastCopy(src);
  src_mask0.CastCopy(src_mask);
  int ww = 2;
  int ww2 = 1;
#ifdef NEW_SEG2
  ww = 3;
  ww2 = 3;
#endif
  if (reg_mode==0)
    {
      reg_factor = 0.01;
    }
  else
    {
      reg_factor = 0.1;
      ww = 2;
    }
  ii_tool.GetMean(src_mono,src_mono_blur,ww2);
  ii_tool.GetMean(src_mask0,src_mask_wide,ww);
  for (int i=0; i<ww; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  src_mask_wide(j,i) = src_mask0(j,i)/2;
	  src_mask_wide(j,DIM-i-1) = src_mask0(j,DIM-i-1)/2;
	  src_mask_wide(i,j) = src_mask0(i,j)/2;
	  src_mask_wide(DIM-1-i,j) = src_mask0(DIM-1-i,j)/2;
	}
    }
  IMGFOR(src_mask_wide,x,y)
    {
      YarpPixelFloat& v = src_mask_wide(x,y);
      if (v>1 && v<254)
	{
	  v = 128;
	}
      else
	{
	  if (v>128)
	    {
	      v = 255;
	    }
	  else
	    {
	      v = 0;
	    }
	}
    }
  SatisfySize(src,src_r);  SatisfySize(src,src_r2);
  SatisfySize(src,src_g);  SatisfySize(src,src_g2);
  SatisfySize(src,src_b);  SatisfySize(src,src_b2);
  IMGFOR(src,x,y)
    {
      YarpPixelBGR& pix = src(x,y);
      src_r(x,y) = pix.r;
      src_g(x,y) = pix.g;
      src_b(x,y) = pix.b;
    }
  ii_tool.GetMean(src_r,src_r2,ww2);
  ii_tool.GetMean(src_g,src_g2,ww2);
  ii_tool.GetMean(src_b,src_b2,ww2);

  static YARPImageOf<YarpPixelFloat> src_motion;
  
  SatisfySize(src,src_motion);
  IMGFOR(src_motion,x,y)
    {
      //  src_motion(x,y) = fabs(src_mono(x,y)-src_mono_blur(x,y));
      src_motion(x,y) = maxf(fabs(src_r(x,y)-src_r2(x,y)),
			     maxf(fabs(src_g(x,y)-src_g2(x,y)),
				  fabs(src_b(x,y)-src_b2(x,y))));
      
    }


  static Graph::node_id nodes[DIM][DIM];
  Graph *g = new Graph();
  
  for (int i=0; i<DIM; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  nodes[i][j] = g->add_node();
	}
    }
  for (int i=0; i<DIM; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  //float var = src_variance(i,j);
	  //float motion = src_motion(i,j);
	  //float pre_motion = src_pre_motion(i,j);
	  float val = src_mask_wide(i,j);
	  float big = 10000000.0;
	  //if (i<=0 || j<=0 || i>=DIM-1 || j>=DIM-1)
	  //{
	  //g->set_tweights(nodes[i][j], big, 0.0);
	  //}
	  //else
	    {
	      g->set_tweights(nodes[i][j], (val<1)?big:0, (val>254)?big:0);
	    }
	  //g->set_tweights(nodes[i][j], (var>0.5)?LINE_STRENGTH:0, 
	  //(motion>0.5)?LINE_STRENGTH:0);
	}
    }
  
  YARPImageOf<YarpPixelFloat>& src_variance = src_motion;
  for (int i=0; i<DIM; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  if (i>0)
	    {
	      float v = Compare2(src_motion,src_variance,i,j,i-1,j);
	      g->add_edge(nodes[i][j],nodes[i-1][j],v,v);
	    }
	  if (j>0)
	    {
	      float v = Compare2(src_motion,src_variance,i,j,i,j-1);
	      g->add_edge(nodes[i][j],nodes[i][j-1],v,v);
	    }
	  if (i<DIM-1)
	    {
	      float v = Compare2(src_motion,src_variance,i,j,i+1,j);
	      g->add_edge(nodes[i][j],nodes[i+1][j],v,v);
	    }
	  if (j<DIM-1)
	    {
	      float v = Compare2(src_motion,src_variance,i,j,i,j+1);
	      g->add_edge(nodes[i][j],nodes[i][j+1],v,v);
	    }
	  if (i>0)
	    {
	      if (j>0)
		{
		  float v = DIAG*Compare2(src_motion,src_variance,i,j,i-1,j-1);
		  g->add_edge(nodes[i][j],nodes[i-1][j-1],v,v);
		}
	      if (j<DIM-1)
		{
		  float v = DIAG*Compare2(src_motion,src_variance,i,j,i-1,j+1);
		  g->add_edge(nodes[i][j],nodes[i-1][j+1],v,v);
		}
	    }
	  if (i<DIM-1)
	    {
	      if (j>0)
		{
		  float v = DIAG*Compare2(src_motion,src_variance,i,j,i+1,j-1);
		  g->add_edge(nodes[i][j],nodes[i+1][j-1],v,v);
		}
	      if (j<DIM-1)
		{
		  float v = DIAG*Compare2(src_motion,src_variance,i,j,i+1,j+1);
		  g->add_edge(nodes[i][j],nodes[i+1][j+1],v,v);
		}
	    }
	  
	  // HALF_DIAG

	  if (i>1)
	    {
	      if (j>0)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i-2,j-1);
		  g->add_edge(nodes[i][j],nodes[i-2][j-1],v,v);
		}
	      if (j<DIM-1)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i-2,j+1);
		  g->add_edge(nodes[i][j],nodes[i-2][j+1],v,v);
		}
	    }
	  if (i<DIM-2)
	    {
	      if (j>0)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i+2,j-1);
		  g->add_edge(nodes[i][j],nodes[i+2][j-1],v,v);
		}
	      if (j<DIM-1)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i+2,j+1);
		  g->add_edge(nodes[i][j],nodes[i+2][j+1],v,v);
		}
	    }
	  if (j>1)
	    {
	      if (i>0)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i-1,j-2);
		  g->add_edge(nodes[i][j],nodes[i-1][j-2],v,v);
		}
	      if (i<DIM-1)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i+1,j-2);
		  g->add_edge(nodes[i][j],nodes[i+1][j-2],v,v);
		}
	    }
	  if (j<DIM-2)
	    {
	      if (i>0)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i-1,j+2);
		  g->add_edge(nodes[i][j],nodes[i-1][j+2],v,v);
		}
	      if (i<DIM-1)
		{
		  float v = HALF_DIAG*Compare2(src_motion,src_variance,
					       i,j,i+1,j+2);
		  g->add_edge(nodes[i][j],nodes[i+1][j+2],v,v);
		}
	    }
	}
    }

  Graph::flowtype flow = g -> maxflow();
  
  printf("Flow = %g\n", (double)flow);

  for (int i=0; i<DIM; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  int active = 1;
	  if (g->what_segment(nodes[i][j]) == Graph::SOURCE)
	    {
	      active = 0;
	    }
	  //printf("%c",active?'*':':');
	  dest.SafePixel(i,j).r = 0;
	  dest.SafePixel(i,j).g = 0;
	  dest.SafePixel(i,j).b = 0;
	  /*
	  if (active)
	    {
	      dest.SafePixel(i,j).g = 255;
	    }
	  if (src_mask(i,j)>10)
	    {
	      dest.SafePixel(i,j).r = 255;
	    }
	  */
	  dest_mask(i,j) = (active?255:0);
	  if (active) //&&src_mask(i,j)>10)
	    {
	      dest.SafePixel(i,j) = src.SafePixel(i,j);
	    }
	}
    }

  delete g;

  //dest.CastCopy(src_mask_wide);

  return flow;
}



float OptimizeSegment(YARPImageOf<YarpPixelBGR>& src,
		      YARPImageOf<YarpPixelMono>& src_mask,
		      YARPImageOf<YarpPixelMono>& dest_mask,
		      YARPImageOf<YarpPixelBGR>& dest)
{
  float prev_flow = 100000000000.0;
  float flow = 0;
  static YARPImageOf<YarpPixelMono> inter_mask;
#ifdef SHOW_OPT
  static int show_ct = 0;
  char show_buf[256];
  YARPImageOf<YarpPixelBGR> show_view;
  YarpPixelBGR show_black(0,0,0);
  SatisfySize(src,show_view);
#endif

  inter_mask.CastCopy(src_mask);
  for (int i=0; i<20; i++)
    {
#ifdef SHOW_OPT
      {
	IMGFOR(show_view,x,y)
	  show_view(x,y) = inter_mask(x,y)?src(x,y):show_black;
	sprintf(show_buf,"opt%04d.ppm", show_ct);  show_ct++;
	YARPImageFile::Write(show_buf, show_view);
      }
#endif
      flow = TweakSegment(src,inter_mask,dest_mask,dest);
      if (fabs(flow-prev_flow)<1)
	{
	  break;
	}
      prev_flow = flow;
      inter_mask.CastCopy(dest_mask);
    }
#ifdef SHOW_OPT
  {
    IMGFOR(show_view,x,y)
      show_view(x,y) = inter_mask(x,y)?src(x,y):show_black;
    sprintf(show_buf,"opt%04d.ppm", show_ct);  show_ct++;
    YARPImageFile::Write(show_buf, show_view);
  }
#endif

#ifdef NEW_SEG2
  inter_mask.CastCopy(dest_mask);
  flow = TweakSegment(src,inter_mask,dest_mask,dest,1);
#endif

  float total = 0;
  IMGFOR(dest_mask,x,y)
  {
    total += (dest_mask(x,y)>0);
  }
  
  //dest.CastCopy(dest_aux);

  return total/(dest_mask.GetHeight()*dest_mask.GetWidth());
}





float FindMovingSegments(YARPImageOf<YarpPixelBGR>& src,
			 YARPImageOf<YarpPixelFloat>& src_motion, 
			 YARPImageOf<YarpPixelFloat>& src_pre_motion, 
			 YARPImageOf<YarpPixelBGR>& dest, 
			 YARPImageOf<YarpPixelMono>& dest_aux,
			 YARPImageOf<YarpPixelMono>& dest_aux_grip,
			 int contact)
{
  static YARPImageOf<YarpPixelFloat> src_variance;
  static YARPImageOf<YarpPixelMono> src_mono;
  static YARPImageLabel label_tool;
  src_mono.CastCopy(src);
  int h = src.GetHeight();
  int w = src.GetWidth();
  SatisfySize(src,src_variance);
  SatisfySize(src,dest_aux);
  IMGFOR(dest_aux,x,y)
    {
      dest_aux(x,y) = 0;
    }
  IMGFOR(src_variance,x,y)
    {
      int v = src_mono(x,y);
      src_mono.NullPixel() = v;
      int vd0 = src_mono.SafePixel(x+DEL,y);
      int v0d = src_mono.SafePixel(x,y+DEL);
      int vD0 = src_mono.SafePixel(x-DEL,y);
      int v0D = src_mono.SafePixel(x,y-DEL);
      src_variance(x,y) = maxf(fabs(vd0-vD0),fabs(v0d-v0D));
    }
  if (0)
    {
      IMGFOR(src_motion,x,y)
	{
	  float rnd = 0; //YARPRandom::Uniform();
	  int v = 0;
	  int v2 = 0;
	  if ((y>=30&&y<=32) || (y>=70&&y<=72))
	    {
	      if ((x>=10&&x<=30)||(x>=30&&x<=50))
		{
		  if ((x+y)%1==0)
		    {
		      v = 255;
		    }
		}
	    }
	  if (x<=2 || y<=2 || y>=125 || x>= 125)// || y==55)
	    {
	      v2 = 255;
	    }
	  // if (x==64 && y==64)
	  //{
	  //  v2 = 255;
	  //}
	  float rate = 0.005;
	  if (rnd<rate) v=255;
	  if (rnd>(1-rate)) v2=255;
	  src_motion(x,y) = v;
	  src_variance(x,y) = v2;
	}
    }
  IMGFOR(src,x,y)
    {
      float v = src_variance(x,y);
      float v2 = src_motion(x,y);
      v2 /= (v+0.5);
      v2 *= src_mono(x,y);
      v2 /= 20;
      v *= 5;  if (v>255) v=255;
      v2 *= 20;  if (v2>255) v2=255;
      dest(x,y).r = (unsigned char) v2;
      dest(x,y).g = 0; //(unsigned char) v;
      dest(x,y).b = 0;
    }

  assert(src.GetHeight()==DIM && src.GetWidth()==DIM);

  static Graph::node_id nodes[DIM][DIM];
  Graph *g = new Graph();
  
  for (int i=0; i<DIM; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  nodes[i][j] = g->add_node();
	}
    }
  for (int i=0; i<DIM; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  //float var = src_variance(i,j);
	  float motion = src_motion(i,j);
	  float pre_motion = src_pre_motion(i,j);
	  if (i<=0 || j<=0 || i>=DIM-1 || j>=DIM-1)
	    {
	      g->set_tweights(nodes[i][j], LINE_STRENGTH*1000000.0,
			      0.0);
	    }
	  else
	    {
	      if (contact&&(pre_motion>=PRE_MOTION))
		{
		  g->set_tweights(nodes[i][j], LINE_STRENGTH*1000.0,
				  0.0);
		}
	      else
		{
		  g->set_tweights(nodes[i][j], LINE_STRENGTH*0.0001,
				  (motion/256.0)*LINE_STRENGTH);
		}
	    }
	  //g->set_tweights(nodes[i][j], (var>0.5)?LINE_STRENGTH:0, 
	  //(motion>0.5)?LINE_STRENGTH:0);
	}
    }
  for (int i=0; i<DIM; i++)
    {
      for (int j=0; j<DIM; j++)
	{
	  if (i>0)
	    {
	      float v = Compare(src_motion,src_variance,i,j,i-1,j);
	      g->add_edge(nodes[i][j],nodes[i-1][j],v,v);
	    }
	  if (j>0)
	    {
	      float v = Compare(src_motion,src_variance,i,j,i,j-1);
	      g->add_edge(nodes[i][j],nodes[i][j-1],v,v);
	    }
	  if (i<DIM-1)
	    {
	      float v = Compare(src_motion,src_variance,i,j,i+1,j);
	      g->add_edge(nodes[i][j],nodes[i+1][j],v,v);
	    }
	  if (j<DIM-1)
	    {
	      float v = Compare(src_motion,src_variance,i,j,i,j+1);
	      g->add_edge(nodes[i][j],nodes[i][j+1],v,v);
	    }
	  if (i>0)
	    {
	      if (j>0)
		{
		  float v = DIAG*Compare(src_motion,src_variance,i,j,i-1,j-1);
		  g->add_edge(nodes[i][j],nodes[i-1][j-1],v,v);
		}
	      if (j<DIM-1)
		{
		  float v = DIAG*Compare(src_motion,src_variance,i,j,i-1,j+1);
		  g->add_edge(nodes[i][j],nodes[i-1][j+1],v,v);
		}
	    }
	  if (i<DIM-1)
	    {
	      if (j>0)
		{
		  float v = DIAG*Compare(src_motion,src_variance,i,j,i+1,j-1);
		  g->add_edge(nodes[i][j],nodes[i+1][j-1],v,v);
		}
	      if (j<DIM-1)
		{
		  float v = DIAG*Compare(src_motion,src_variance,i,j,i+1,j+1);
		  g->add_edge(nodes[i][j],nodes[i+1][j+1],v,v);
		}
	    }
	}
    }

  Graph::flowtype flow = g -> maxflow();
  
  printf("Flow = %g\n", (double)flow);

  static YARPImageOf<YarpPixelBGR> prev_frame_seg;
  if (contact)
    {
      
    }

  if (1)
    {
      for (int i=0; i<DIM; i++)
	{
	  for (int j=0; j<DIM; j++)
	    {
	      int active = 1;
	      if (g->what_segment(nodes[i][j]) == Graph::SOURCE)
		{
		  active = 0;
		}
	      //printf("%c",active?'*':':');
	      if (active)
		{
		  dest.SafePixel(i,j).g = 255;
		  dest_aux.SafePixel(i,j) = 255;
		}
	      if (contact)
		{
		  if (src_pre_motion(i,j)>=PRE_MOTION)
		    {
		      dest.SafePixel(i,j).b = 255;
		    }
		}
	    }
	  //printf("\n");
	}
    }
  static YARPImageOf<YarpPixelMono> dest_aux2;
  SatisfySize(src,dest_aux2);
  int best_id = label_tool.Apply(dest_aux,dest_aux2);
  float total = 0;
  IMGFOR(dest_aux,x,y)
    {
      dest_aux(x,y) = 255*(dest_aux2(x,y)==best_id);
      total += (dest_aux2(x,y)==best_id);
    }

  if (contact)
    {
      IntegralImageTool ii_tool;
      SatisfySize(dest_aux,dest_aux_grip);
      YARPImageOf<YarpPixelFloat> work, work2;
      IMGFOR(src_pre_motion,x,y)
	{
	  if (src_pre_motion(x,y)>=PRE_MOTION)
	    {
	      dest_aux_grip(x,y) = 255;
	    }
	  else
	    {
	      dest_aux_grip(x,y) = 0;
	    }
	}
      int best_id = label_tool.Apply(dest_aux_grip,dest_aux2);
      IMGFOR(dest_aux_grip,x,y)
	{
	  dest_aux_grip(x,y) = 255*(dest_aux2(x,y)==best_id);
	}
      work.CastCopy(dest_aux_grip);
      SatisfySize(work,work2);
      ii_tool.GetMean(work,work2,4);
      IMGFOR(src_pre_motion,x,y)
	{
	  if (work2(x,y)>4)
	    {
	      dest_aux_grip(x,y) = 255;
	    }
	}
    }
  
  //dest.CastCopy(dest_aux);

  delete g;

  return total/(dest_aux.GetHeight()*dest_aux.GetWidth());
}

/***************************************************************************
 **** spot point of contact
 ***************************************************************************/

static void AlignMotion(YARPImageOf<YarpPixelFloat>& src,
			YARPImageOf<YarpPixelFloat>& prev_src,
			YARPImageOf<YarpPixelFloat>& mask,
			YARPImageOf<YarpPixelFloat>& dest_mask,
			YARPImageOf<YarpPixelBGR>& dest,
			int *ext_dx = NULL,
			int *ext_dy = NULL,
			int active = 1)
{
  static IntegralImageTool ii_tool;
  static YARPImageOf<YarpPixelFloat> work;
  SatisfySize(src,work);
  SatisfySize(src,dest_mask);
  float best = 999999999999999999.0;
  float bx = 0, by = 0;
  if (active)
    {
      for (int dx=-10; dx<=10; dx++)
	{
	  for (int dy=-10; dy<=10; dy++)
	    {
	      work.Zero();
	      ii_tool.Offset(src,work,dx,dy);
	      float total = 0;
	      IMGFOR(work,x,y)
		{
		  float q = fabs(work(x,y)-prev_src(x,y));
		  total += q*mask(x,y);
		}
	      if (total<best)
		{
		  best = total;
		  bx = -dx;
		  by = -dy;
		}
	    }
	}
      printf("*** %g %g\n", bx,by);
    }
  else
    {
      if (ext_dx!=NULL) bx = (*ext_dx);
      if (ext_dy!=NULL) by = (*ext_dy);
    }
  dest_mask.Zero();
  ii_tool.Offset(mask,dest_mask,bx,by);
  if (ext_dx!=NULL) (*ext_dx) = (int)(bx+0.5);
  if (ext_dy!=NULL) (*ext_dy) = (int)(by+0.5);
  /*
  IMGFOR(mask,x,y)
    {
      if (dest_mask(x,y)>10)
	{
	  dest(x,y).r = 0;
	}
    }
  */
}


static void FindMovingPatches(YARPImageOf<YarpPixelBGR>& src,
			      YARPImageOf<YarpPixelFloat>& src_motion, 
			      YARPImageOf<YarpPixelBGR>& dest, int boring,
			      int allowed)
{
  double now = YARPTime::GetTimeAsSeconds();
  int res = 8;
  int high_age = 12;
  float recent_age = 11.5;
  int sw = src.GetWidth()/res;
  int sh = src.GetHeight()/res;

#ifdef USE_TIME
  if (now-contact_time<1.0)
    {
      dest.PeerCopy(src);
    }
#else
  if (since_contact<10)
    {
      dest.PeerCopy(src);
    }
#endif

  static YARPImageOf<YarpPixelBGR> contact_image_post;
  static YARPImageOf<YarpPixelFloat> accum, narrow_accum, wide_accum;
  //static YARPImageOf<YarpPixelFloat> rollback_accum;
  static int first = 1;
  static int frames_since_reset = 0;
  static int frames_since_reset_msg = 0;

  if (force_first)
    {
      first = 1;
      frames_since_reset = 0;
      frames_since_reset_msg = 0;
    }

  if (first)
    {
      accum.Resize(sw,sh);
      narrow_accum.Resize(sw,sh);
      wide_accum.Resize(sw,sh);
      IMGFOR(wide_accum,x,y)
	{
	  wide_accum(x,y) = 0;
	  //rollback_accum(x,y) = 0;
	}
    }

  IMGFOR(accum,x,y)
    {
      accum(x,y) = 0;
      narrow_accum(x,y) = 0;
    }

  {
    IMGFOR(src_motion,x,y)
      {
	int xd = x/res;
	int yd = y/res;
#ifndef PRETTY_OUTPUT
	if (x%res == 0 || y%res == 0)
	  {
	    dest(x,y).g = 255;
	  }
#endif
	accum(xd,yd) += fabs(src_motion(x,y));
      }
  }

  float total_count = 0;
  IMGFOR(accum,x,y)
    {
      float q = accum(x,y)/(res*res);
      if (q>5.0)
	{
	  if (wide_accum(x,y)<recent_age)
	    {
	      narrow_accum(x,y) = high_age;
	    }
	  total_count++;
	  if (q>10)
	    {
	      total_count+=2;
	    }
#ifndef PRETTY_OUTPUT
	  for (int dx=x*res; dx<(x+1)*res; dx++)
	    {
	      for (int dy=y*res; dy<(y+1)*res; dy++)
		{
		  if ((dx+dy)%7 == 0)
		    {
		      dest(dx,dy).g = 255;
		      dest(dx,dy).b = 255;
		      dest(dx,dy).g = 255;
		      dest(dx,dy).b = 255;
		    }
		}
	    }
#endif
	}
    }
  int total_area = accum.GetHeight()*accum.GetWidth();
  total_count /= total_area;
  int order_reset = 0;
  if (total_count>5)
    {
      if (frames_since_reset>10)
	{
	  printf("*** Reset ordered, too much darned motion\n");
	  frames_since_reset_msg = 0;
	}
      frames_since_reset = 0;
      order_reset = 1;
    }
  if (0)
    {
      if ((total_count<0.05 && (now-movement_time)>1.0)||
	  now-movement_time>5.0)
	{
	  if (frames_since_reset_msg>10)
	    {
	      printf("*** Reset ordered, too little motion\n");
	      frames_since_reset_msg = 0;
	    }
	  order_reset = 1;
	  frames_since_reset = 0;
	}
    }
  if (total_count>0.1)
    {
      movement_time = now;
    }

  if (order_reset)
    {
      IMGFOR(dest,x,y)
	{
	  /*
	  if ((1000000000+x-y)%3 == 0)
	    {
	      dest(x,y).r = 255;
	      dest(x,y).g = 255;
	      dest(x,y).b = 255;
	    }
	  */
	  int xd = x/res;
	  int yd = y/res;
	  if (x%res == 0 || y%res == 0)
	    {
	      dest(x,y).r = 255;
	      dest(x,y).g = 0;
	    }
	}
      IMGFOR(wide_accum,x,y)
	{
	  wide_accum(x,y) = narrow_accum(x,y);
	  //accum(x,y) = 0;
	}
    }

  wide_accum.NullPixel() = 1;
  float novel_count = 0;
  IMGFOR(narrow_accum,x,y)
    {
      if (narrow_accum(x,y)>0.5)
	{
	  int v = 0;
	  float v0 = wide_accum.SafePixel(x,y); 
	  float v1 = wide_accum.SafePixel(x-1,y); 
	  float v2 = wide_accum.SafePixel(x+1,y);
	  float v3 = wide_accum.SafePixel(x,y-1);
	  float v4 = wide_accum.SafePixel(x,y+1);

	  if (v0>0.5 && v0<recent_age) v = 1;
	  else if (v1>0.5 && v1<recent_age) v = 1;
	  else if (v2>0.5 && v2<recent_age) v = 1;
	  else if (v3>0.5 && v3<recent_age) v = 1;
	  else if (v4>0.5 && v4<recent_age) v = 1;
	  if (!v)
	    {
	      novel_count++;
#ifndef PRETTY_OUTPUT
	      for (int dx=x*res; dx<(x+1)*res; dx++)
		{
		  for (int dy=y*res; dy<(y+1)*res; dy++)
		    {
		      if ((dx+dy)%3 == 0)
			{
			  dest(dx,dy).r = 255;
			  dest(dx,dy).g = 255;
			  dest(dx,dy).b = 255;
			}
		    }
		}
#endif
	    }
	}
    }
  novel_count /= total_area;

  contact_declared = 0;
  if (novel_count>0.05) // && frames_since_reset>=3)
    {
      if (allowed)
	{
	  printf("*** CONTACT!\n");
	  contact_declared = 1;
	  contact_time = now;
	  contact_image_post.PeerCopy(src);
	  since_contact = 0;
	}
      else
	{
	  printf("*** ignoring contact, models aren't trustworthy yet\n");
	}
    }
  else
    {
      since_contact++;
    }
#ifdef USE_TIME
  if (now-contact_time<1.0)
    {
      IMGFOR(dest,x,y)
	{
	  if (((x>y-5) && (x<y+5))||
	      ((128-x>y-5) && (128-x<y+5)))
	    {
	      dest(x,y).r = 255;
	      dest(x,y).g = 255;
	      dest(x,y).b = 128;
	    }
	}
      //dest.PeerCopy(contact_image_post);
    }
#endif

  if (!boring)
    {
      IMGFOR(narrow_accum,x,y)
	{
	  wide_accum(x,y) = maxf(wide_accum(x,y)-1,narrow_accum(x,y));
#ifdef PRETTY_OUTPUT
	  if (wide_accum(x,y))
	    {
	      for (int dx=x*res; dx<(x+1)*res; dx++)
		{
		  for (int dy=y*res; dy<(y+1)*res; dy++)
		    {
		      if (dest(dx,dy).g != 0 && dest(dx,dy).b != 0)
			{
			  dest(dx,dy) = src(dx,dy);
			}
		    }
		}
	    }
#endif
	}
    }
//  printf(">>> %g  %g\n", total_count, novel_count);

  //rollback_accum.PeerCopy(wide_accum);

  first = 0;
  frames_since_reset++;
  frames_since_reset_msg++;
}


/***************************************************************************
 **** simple background model for image differencing
 ***************************************************************************/

#define YarpPixelRGBFoo YarpPixelRGBFloat

class PNDiff2Op
{
public:
  void operator()(YarpPixelRGBFoo& pix_x,
		  YarpPixelRGBFoo& pix_x2,
		  YarpPixelRGBFoo& pix_total_x,
		  YarpPixelRGBFoo& pix_total_x2)
  {
    pix_total_x.r -= pix_x.r;
    pix_total_x.g -= pix_x.g;
    pix_total_x.b -= pix_x.b;
    pix_total_x2.r -= pix_x2.r;
    pix_total_x2.g -= pix_x2.g;
    pix_total_x2.b -= pix_x2.b;
  }
};


class PNDiffOp
{
public:
  void operator()(YarpPixelRGBFoo& pix_x,
		  YarpPixelRGBFoo& pix_total_x)
  {
    pix_total_x.r -= pix_x.r;
    pix_total_x.g -= pix_x.g;
    pix_total_x.b -= pix_x.b;
  }
};


class PNSquareOp
{
public:
  void operator()(YarpPixelRGBFoo& p_x, YarpPixelRGBFoo& p_x2)
  {
    p_x2.r = square(p_x.r);
    p_x2.g = square(p_x.g);
    p_x2.b = square(p_x.b);
  }
};

class PNSum2Op
{
public:
  void operator()(YarpPixelRGBFoo& pix_x,
		  YarpPixelRGBFoo& pix_x2,
		  YarpPixelRGBFoo& pix_total_x,
		  YarpPixelRGBFoo& pix_total_x2)
  {
    pix_total_x.r += pix_x.r;
    pix_total_x.g += pix_x.g;
    pix_total_x.b += pix_x.b;
    pix_total_x2.r += pix_x2.r;
    pix_total_x2.g += pix_x2.g;
    pix_total_x2.b += pix_x2.b;
  }
};

class PNSumOp
{
public:
  void operator()(YarpPixelRGBFoo& pix_x,
		  YarpPixelRGBFoo& pix_total_x)
  {
    pix_total_x.r += pix_x.r;
    pix_total_x.g += pix_x.g;
    pix_total_x.b += pix_x.b;
  }
};

class PNVarOp
{
public:
  float index_count;

  void operator()(YarpPixelRGBFoo& mean,
		  YarpPixelRGBFoo& var,
		  YarpPixelRGBFoo& pix_total_x,
		  YarpPixelRGBFoo& pix_total_x2)
  {
    //mean.r = pix_total_x.r/index_count;
    //var.r = pix_total_x2.r/index_count;
    //var.r=sqrt(var.r - square(mean.r));
    //mean.g = pix_total_x.g/index_count;
    //var.g = pix_total_x2.g/index_count;
    //var.g=sqrt(var.g - square(mean.g));
    //mean.b = pix_total_x.b/index_count;
    //var.b = pix_total_x2.b/index_count;
    //var.b=sqrt(var.b - square(mean.b));
    mean.r = pix_total_x.r/index_count;
    var.r= sqrt(pix_total_x2.r/index_count - square(mean.r));
    mean.g = pix_total_x.g/index_count;
    var.g= sqrt(pix_total_x2.g/index_count - square(mean.g));
    mean.b = pix_total_x.b/index_count;
    var.b = sqrt(pix_total_x2.b/index_count - square(mean.b));
  }
};


#define NOISE_GAP 8

static float PixelNoise(YARPImageOf<YarpPixelBGR>& src, 
			YARPImageOf<YarpPixelBGR>& dest,
			YARPImageOf<YarpPixelFloat>& response)
{
  static int first = 1;
  static YARPImageOf<YarpPixelRGBFoo> record_x[NOISE_GAP];
  static YARPImageOf<YarpPixelFloat> record_x_gray[NOISE_GAP];
  static YARPImageOf<YarpPixelRGBFoo> record_x2[NOISE_GAP];
  static YARPImageOf<YarpPixelRGBFoo> total_x;
  static YARPImageOf<YarpPixelRGBFoo> total_x2;  
  static YARPImageOf<YarpPixelRGBFoo> wide_mean;  
  static YARPImageOf<YarpPixelRGBFoo> wide_var;  
  static YARPImageOf<YarpPixelFloat> src_gray;
  static int index_first = -1;
  static int index_last = -1;
  static int index_count = 0;


  if (force_first)
    {
      first = 1;
      index_first = -1;
      index_last = -1;
      index_count = 0;
    }

  SatisfySize(src,dest);
  SatisfySize(src,response);
  if (first)
    {
      SatisfySize(src,total_x);
      SatisfySize(src,total_x2);
      SatisfySize(src,wide_mean);
      SatisfySize(src,wide_var);      
      for (int k=0; k<NOISE_GAP; k++)
	{
	  SatisfySize(src,record_x[k]);
	  SatisfySize(src,record_x_gray[k]);
	  SatisfySize(src,record_x2[k]);
	}
      IMGFOR(src,x,y)
	{
	  YarpPixelRGBFoo zero = {0,0.0};
	  total_x(x,y) = zero;
	  total_x2(x,y) = zero;
	}
    }

  {
    index_first = (index_first+1)%NOISE_GAP;
    if (index_first == index_last)
      {
	YARPImageOf<YarpPixelRGBFoo>& x = record_x[index_last];
	YARPImageOf<YarpPixelRGBFoo>& x2 = record_x2[index_last];
	//PNDiff2Op diff_op;
	//VisMap(diff_op,x,x2,total_x,total_x2);
	PNDiffOp diff_op;
	VisMap(diff_op,x,total_x);
	VisMap(diff_op,x2,total_x2);
	index_last = (index_last+1)%NOISE_GAP;
      }
    if (index_last<0) index_last = 0;
    int idx = 0;
    YARPImageOf<YarpPixelRGBFoo>& x = record_x[index_first];
    x.CastCopy(src);
    YARPImageOf<YarpPixelRGBFoo>& x2 = record_x2[index_first];
    PNSquareOp square_op;
    VisMap(square_op,x,x2);

    idx = index_first;
      {
	YARPImageOf<YarpPixelRGBFoo>& x = record_x[idx];
	YARPImageOf<YarpPixelRGBFoo>& x2 = record_x2[idx];
	//PNSum2Op sum_op;
	//VisMap(sum_op,x,x2,total_x,total_x2);
	PNSumOp sum_op;
	VisMap(sum_op,x,total_x);
	VisMap(sum_op,x2,total_x2);
	idx = (idx+1)%NOISE_GAP;
      }

    if (index_count<NOISE_GAP)
      {
	index_count++;
      }

    PNVarOp var_op;
    var_op.index_count = index_count;
    VisMap(var_op,wide_mean,wide_var,total_x,total_x2);
  }

  float total = 0;
  int ct = 0;
  IMGFOR(src,x,y)
    {
      YarpPixelBGR& p_src = src(x,y);
      YarpPixelBGR& p_dest = dest(x,y);
      YarpPixelRGBFoo& mean = wide_mean(x,y);
      YarpPixelRGBFoo& var = wide_var(x,y);

      float out = ((fabs(p_src.r-mean.r)/(var.r+0.1f))+
		   (fabs(p_src.g-mean.g)/(var.g+0.1f))+
		   (fabs(p_src.b-mean.b)/(var.b+0.1f)))/3.0f;
      out -= 2.2;
      if (out<0) out = 0;
      out *= 50*20;
      if (out>255) out = 255;
      response(x,y) = out;
      total += out;
      ct++;
#ifdef PRETTY_OUTPUT
      if (out>0)
	{
	  out *= 100;
	  if (out>255) out = 255;
	  p_dest.r = (unsigned char)out;
	  p_dest.g = (unsigned char)0;
	  p_dest.b = (unsigned char)0;
	}
      else
	{
	  unsigned char factor = 2;
	  p_dest.r = p_src.r/factor;
	  p_dest.g = p_src.g/factor;
	  p_dest.b = p_src.b/factor;
	}
#else
      p_dest.r = (unsigned char)out;
      p_dest.g = (unsigned char)0;
      p_dest.b = (unsigned char)0;
#endif
    }
  total /= ct;

  first = 0;
  return total;
}

/*
#ifdef TESTING
  static YARPImageOf<YarpPixelFloat> pr;
  static YARPImageOf<YarpPixelFloat> mono, pmono;
  static YARPImageOf<YarpPixelFloat> mask;
  mono.CastCopy(src);
  if (first)
    {
      pr.CastCopy(response);
      pmono.PeerCopy(mono);
    }
  SatisfySize(src,mask);
  IMGFOR(response,x,y)
    {
      mask(x,y) = 0;
      if (response(x,y)>10)
	{
	  dest(x,y).r = 255;
	}
      //if (pr(x,y)>10)
	{
	  mask(x,y) = pr(x,y);
	}
    }
  AlignMotion(mono,pmono,mask,dest);
  pr.CastCopy(response);
  pmono.PeerCopy(mono);
#endif


 */

static int ProcessImage1(YARPImageOf<YarpPixelBGR>& src, 
			 YARPImageOf<YarpPixelBGR>& dest,
			 YARPImageOf<YarpPixelMono>& dest_aux,
			 YARPImageOf<YarpPixelMono>& dest_aux_grip)
{
  static IntegralImageTool ii_tool;
  static YARPImageOf<YarpPixelFloat> response, prev_response, prev_seg,
    before, before2, before3, prev_mono, mono, prev_mono2, prev_mask2,
    prev_mono_b, before_b, work;
  //  static YARPImageOf<YarpPixelMono> dest_aux2;

  static int first = 1;
  static int slowdown = 0;
  static int post_contact = 0;
  static int awaiting_contact = 1;
  static long int frame_idx = 0;
  int should_declare = 0;
  
  if (force_first) 
    {
      first = 1;
      slowdown = 0;
      frame_idx = 0;
    }
  
  float v = PixelNoise(src,dest,response);

  FindMovingPatches(src,response,dest,0,frame_idx>=10);
  
  int contact = contact_declared && awaiting_contact;

  frame_idx++;

  if (contact)
    {
      awaiting_contact = 0;
    }

  mono.CastCopy(src);
  if (first)
    {
      before.PeerCopy(response);
      before_b.PeerCopy(response);
      before2.PeerCopy(response);
      before3.PeerCopy(response);
      prev_mono.PeerCopy(mono);
      prev_mono_b.PeerCopy(mono);
    }
  
  if (contact&&use_segmentation)
    {
      AlignMotion(mono,prev_mono_b,before_b,before2,dest);
      ii_tool.GetMean(before2,before3,1);
      prev_mask2.PeerCopy(before3);
#ifdef PRETTY_OUTPUT
#ifndef PRETTY_OUTPUT2
      YarpPixelBGR pixg(0,255,0);
      YarpPixelBGR pixb(0,0,255);
      for (int i=58; i<62; i++)
	{
	  AddCircleOutline(dest,(1)?pixg:pixb,64,64,i);
	}
      YARPImageOf<YarpPixelBGR> dest;
      dest = src;
#endif
#endif
      float q = FindMovingSegments(src,response,before3,dest,dest_aux,dest_aux_grip,contact);
      slowdown = 3;
      //before3.PeerCopy(before2);
      prev_response.PeerCopy(response);
      prev_seg.CastCopy(dest_aux);
    }
  else if (post_contact&&use_segmentation)
    {
      SatisfySize(mono,work);
      int bx, by;
      AlignMotion(mono,prev_mono,before3,before2,dest,&bx,&by);
      global_dx_grip = bx;
      global_dy_grip = by;
      AlignMotion(mono,prev_mono,prev_response,work,dest,&bx,&by,0);
      prev_response.PeerCopy(work);
      AlignMotion(mono,prev_mono,prev_seg,work,dest,&bx,&by,0);
      prev_seg.PeerCopy(work);
      IMGFOR(prev_response,x,y)
	{
	  //prev_response(x,y) = maxf(prev_response(x,y),response(x,y));
	  prev_response(x,y) = maxf(prev_response(x,y),
				    maxf(prev_seg(x,y),response(x,y)));
	  //before2(x,y) = maxf(before2(x,y),prev_mask2(x,y));
	}
      printf("*** POST CONTACT!\n");
      float q = FindMovingSegments(src,prev_response,before2,dest,dest_aux,dest_aux_grip,1);

      // getting the flipper for real...
      //float q = FindMovingSegments(src,prev_response,before2,dest,dest_aux,dest_aux_grip,1);

#ifdef PRETTY_OUTPUT2
      YARPImageOf<YarpPixelBGR> dest;
      dest = src;
#endif
      slowdown = 3;
      should_declare = (q>0.01);
      if (should_declare)
	{
	  q = OptimizeSegment(src,dest_aux,dest_aux,dest);
	  should_declare = (q>0.01);
	}
      //dest_aux.CastCopy(prev_seg);
    }

  prev_mono_b.PeerCopy(prev_mono);
  before_b.PeerCopy(before);
  prev_mono.PeerCopy(mono);
  before.PeerCopy(response);

  first = 0;
  if (post_contact == 1) 
    {
      post_contact = 0;
      awaiting_contact = 1;
    }
  if (contact)  post_contact = 1;


  return should_declare;
}


/***************************************************************************
 ***************************************************************************
 ***************************************************************************/

static int ProcessImage(YARPImageOf<YarpPixelBGR>& src, 
			YARPImageOf<YarpPixelBGR>& dest,
			YARPImageOf<YarpPixelMono>& dest_aux,
			YARPImageOf<YarpPixelMono>& dest_aux_grip)
			
{
  return ProcessImage1(src,dest,dest_aux,dest_aux_grip);
}

static int helper_count = 0;

class ContactHelper
{
public:
  YARPImageOf<YarpPixelMono> seg_mask, seg_flipper_mask;
  float dx_grip, dy_grip;
  int seg_mask_valid;
    
  ContactHelper()
  {
    seg_mask_valid = 0;
    helper_count++;
    if (helper_count>1)
      {
	fprintf(stderr,"WARNING WARNING WARNING from YARPVisualContact.\n");
	fprintf(stderr,"Multiple instances not supported.  Blame Paul.\n");
      }
    dx_grip = dy_grip = 0;
  }

  void Reset() { force_first = 1; }

  int Apply(YARPImageOf<YarpPixelBGR>& src, 
	    YARPImageOf<YarpPixelBGR>& dest)
  {
    //dest.PeerCopy(src);
    contact_declared = 0;
    int result = ProcessImage(src,dest,seg_mask,seg_flipper_mask);
    seg_mask_valid = result;
    if (result)
      {
	dx_grip = global_dx_grip;
	dy_grip = global_dy_grip;
      }
    force_first = 0;
    return result;
  }
  
  int GetSegmentedImage(YARPImageOf<YarpPixelMono>& dest)
    {
      if (seg_mask_valid)
	{
	  dest.PeerCopy(seg_mask);
	}
      return seg_mask_valid;
    }

  int GetFlipper(YARPImageOf<YarpPixelMono>& dest)
    {
      if (seg_mask_valid)
	{
	  dest.PeerCopy(seg_flipper_mask);
	}
      return seg_mask_valid;
    }

  int GetPokeDirection(float& x, float &y)
  {
    x = 0;
    y = 0;
    if (seg_mask_valid)
      {
	x = dx_grip;
	y = dy_grip;
      }
    return seg_mask_valid;
  }

  float GetMotion(YARPImageOf<YarpPixelBGR>& src, 
		  YARPImageOf<YarpPixelBGR>& dest,
		  YARPImageOf<YarpPixelFloat>& response)
    {
      return PixelNoise(src,dest,response);
    }
};



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


#define RES (*((ContactHelper *)(system_resource)))

YARPVisualContact::YARPVisualContact()
{
  system_resource = NULL;
  system_resource = new ContactHelper;
  assert(system_resource!=NULL);
}


YARPVisualContact::~YARPVisualContact()
{
  delete (&RES);
}


void YARPVisualContact::Reset()
{
  RES.Reset();
}


int YARPVisualContact::Apply(YARPImageOf<YarpPixelBGR>& src, 
			     YARPImageOf<YarpPixelBGR>& dest)
{
  /*
  YARPImageFile::Read("off_src_000042.ppm",src);
  YARPImageFile::Read("off_foo_000042.ppm",dest);
  YARPImageOf<YarpPixelMono> work1, work2;
  work1.CastCopy(dest);
  work2.CastCopy(dest);
#if 0
  for (int i=0; i<9; i++)
    {
      printf("Phase %d\n", i);
      TweakSegment(src,work1,work2,dest);
      char buf[256];
      sprintf(buf,"out%d.ppm",i);
      YARPImageFile::Write(buf,dest);
      work1.CastCopy(work2);
    }
#else
  OptimizeSegment(src,work1,work2,dest);
  YARPImageFile::Write("out.ppm",dest);
#endif
  exit(0);
  */

  return RES.Apply(src,dest);
}

int YARPVisualContact::GetSegmentedImage(YARPImageOf<YarpPixelMono>& dest)
{
  return RES.GetSegmentedImage(dest);
}

void YARPVisualContact::UseSegmentation(int flag)
{
  use_segmentation = 1;
}


int YARPVisualContact::GetPokeDirection(float& x, float &y)
{
  return RES.GetPokeDirection(x,y);
}


int YARPVisualContact::GetFlipper(YARPImageOf<YarpPixelMono>& dest)
{
  return RES.GetFlipper(dest);
}


float YARPVisualContact::GetMotion(YARPImageOf<YarpPixelBGR>& src, 
				   YARPImageOf<YarpPixelBGR>& dest,
				   YARPImageOf<YarpPixelFloat>& response)
{
  return RES.GetMotion(src,dest,response);
}


