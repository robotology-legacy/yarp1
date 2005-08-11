
#include "ObreroVision.h"

#include <yarp/YARPImageDraw.h>
#include <yarp/YARPVisMap.h>
#include <yarp/YARPTime.h>
#include "Roller.h"
#include "IntegralImageTool.h"



#define VisMap YARPVisMap



/***************************************************************************
 *** Globals and simple functions
 ***************************************************************************/

int contact_declared = 0;
int some_contact = 0;
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


float Compare(const YarpPixelRGB& pix1, const YarpPixelRGB& pix2)
{
  return sqrt((double)(square(pix1.r-pix2.r)+square(pix1.g-pix2.g)+
		       square(pix1.b-pix2.b)));
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

  void operator()(YarpPixelFloat& pix_x,
		  YarpPixelFloat& pix_x2,
		  YarpPixelFloat& pix_total_x,
		  YarpPixelFloat& pix_total_x2)
  {
    pix_total_x -= pix_x;
    pix_total_x2 -= pix_x2;
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

  void operator()(YarpPixelFloat& pix_x,
		  YarpPixelFloat& pix_total_x)
  {
    pix_total_x -= pix_x;
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

  void operator()(YarpPixelFloat& p_x, YarpPixelFloat& p_x2)
  {
    p_x2 = square(p_x);
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

  void operator()(YarpPixelFloat& pix_x,
		  YarpPixelFloat& pix_x2,
		  YarpPixelFloat& pix_total_x,
		  YarpPixelFloat& pix_total_x2)
  {
    pix_total_x += pix_x;
    pix_total_x2 += pix_x2;
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

  void operator()(YarpPixelFloat& pix_x,
		  YarpPixelFloat& pix_total_x)
  {
    pix_total_x += pix_x;
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
    mean.r = pix_total_x.r/index_count;
    var.r= sqrt(fabs(pix_total_x2.r/index_count - square(mean.r)));
    mean.g = pix_total_x.g/index_count;
    var.g= sqrt(fabs(pix_total_x2.g/index_count - square(mean.g)));
    mean.b = pix_total_x.b/index_count;
    var.b = sqrt(fabs(pix_total_x2.b/index_count - square(mean.b)));
  }

  void operator()(YarpPixelFloat& mean,
		  YarpPixelFloat& var,
		  YarpPixelFloat& pix_total_x,
		  YarpPixelFloat& pix_total_x2)
  {
    mean = pix_total_x/index_count;
    var= sqrt(fabs(pix_total_x2/index_count - square(mean)));
  }
};


//#ifdef LONG_NOISE_GAP
//#define NOISE_GAP 16
//#else
//#define NOISE_GAP 8
//#endif

#define NOISE_GAP 8

static float GetMotion(YARPImageOf<YarpPixelRGB>& src, 
		       YARPImageOf<YarpPixelRGB>& dest,
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
      YarpPixelRGB& p_src = src(x,y);
      YarpPixelRGB& p_dest = dest(x,y);
      YarpPixelRGBFoo& mean = wide_mean(x,y);
      YarpPixelRGBFoo& var = wide_var(x,y);

      float out = ((fabs(p_src.r-mean.r)/(var.r+0.1f))+
		   (fabs(p_src.g-mean.g)/(var.g+0.1f))+
		   (fabs(p_src.b-mean.b)/(var.b+0.1f)))/3.0f;
      out -= 2.2;
      if (out<0) out = 0;
      out *= 5;
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










#undef NOISE_GAP
#define NOISE_GAP 30

static float GetMotion(YARPImageOf<YarpPixelRGB>& src, 
		       YARPImageOf<YarpPixelRGB>& dest)
{
  static int first = 1;
  static YARPImageOf<YarpPixelFloat> record_x[NOISE_GAP];
  static YARPImageOf<YarpPixelFloat> record_x2[NOISE_GAP];
  static YARPImageOf<YarpPixelFloat> total_x;
  static YARPImageOf<YarpPixelFloat> total_x2;  
  static YARPImageOf<YarpPixelFloat> wide_mean;  
  static YARPImageOf<YarpPixelFloat> wide_var;  
  static YARPImageOf<YarpPixelFloat> src_gray;
  static YARPImageOf<YarpPixelFloat> response;
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
	  SatisfySize(src,record_x2[k]);
	}
      IMGFOR(src,x,y)
	{
	  YarpPixelFloat zero = 0;
	  total_x(x,y) = zero;
	  total_x2(x,y) = zero;
	}
    }

  {
    index_first = (index_first+1)%NOISE_GAP;
    if (index_first == index_last)
      {
	YARPImageOf<YarpPixelFloat>& x = record_x[index_last];
	YARPImageOf<YarpPixelFloat>& x2 = record_x2[index_last];
	PNDiffOp diff_op;
	VisMap(diff_op,x,total_x);
	VisMap(diff_op,x2,total_x2);
	index_last = (index_last+1)%NOISE_GAP;
      }
    if (index_last<0) index_last = 0;
    int idx = 0;
    YARPImageOf<YarpPixelFloat>& x = record_x[index_first];
    x.CastCopy(src);

    YARPImageOf<YarpPixelFloat>& x2 = record_x2[index_first];
    PNSquareOp square_op;
    VisMap(square_op,x,x2);

    idx = index_first;
      {
	YARPImageOf<YarpPixelFloat>& x = record_x[idx];
	YARPImageOf<YarpPixelFloat>& x2 = record_x2[idx];
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

  YARPImageOf<YarpPixelFloat>& src_mono = record_x[index_first];

  float total = 0;
  int ct = 0;
  IMGFOR(src,x,y)
    {
      YarpPixelFloat& p_src = src_mono(x,y);
      YarpPixelRGB& p_dest = dest(x,y);
      YarpPixelFloat& mean = wide_mean(x,y);
      YarpPixelFloat& var = wide_var(x,y);

      float out = (fabs(p_src-mean)/(var+2));
      out -= 1.2;
      if (out<0) out = 0;
      out *= 75;
      if (out>255) out = 255;
      response(x,y) = out;
      total += out;
      ct++;
      p_dest.r = (unsigned char)out;
      p_dest.g = (unsigned char)out;
      p_dest.b = (unsigned char)0;
    }
  total /= ct;

  first = 0;
  return total;
}




#define WCT (30)
static void Process(YARPImageOf<YarpPixelRGB>& src,
		    YARPImageOf<YarpPixelRGB>& dest) {
  SatisfySize(src,dest);  

  static YARPImageOf<YarpPixelFloat> mono, pmono, pmonos[WCT], diff, mark;
  static double last_vote = -10000;
  static Roller<double> levels(100);

  //GetMotion(src,dest);
  //  return;

  static int index = 0;
  mono.CastCopy(src);
  static int first = 1;
  if (first) {
    first = 0;
    SatisfySize(src,diff);
    diff.Zero();
    pmono.CastCopy(mono);
    mark.CastCopy(mono);
    for (int i=0; i<WCT; i++) {
      pmonos[i].CastCopy(mono);
    }
  }

  dest.Zero();

  float r = 0.8;
  YARPImageOf<YarpPixelFloat>& prev = pmonos[index];
  YARPImageOf<YarpPixelFloat>& prev2 = pmonos[(index+WCT-WCT/3)%WCT];
  float votes = 0;
  IMGFOR(src,x,y) {
    int dd = (int)(mono(x,y)-mark(x,y));
    int pp = (int)(mono(x,y)-prev2(x,y));
    int d = dd;
    if (d<0) d=-d;
    d *= 50;
    diff(x,y) *= r;
    diff(x,y) += (1-r)*d;
    d = (int)diff(x,y);
    int r = d/64;
    int b = dd;
    if (pp<0) pp = -pp;
    pp *= 50;
    if (pp>255) pp = 255;
    if (b>255) b = 255;
    if (r>255) r = 255;
    if (d>255) d = 255;
    YarpPixelRGB& pix = dest(x,y);
    if (d>100) {
      pix.r = pp;
      pix.g = b; //d;
      pix.b = b*0;
      if (d>200) {
	votes++;
      }
    } else {
      pix = src(x,y);
    }
  }
  votes /= src.GetWidth();
  votes /= src.GetHeight();
  levels.add(votes);
  int roll = 0;
  double now = YARPTime::GetTimeAsSeconds();
  static double show_bar = -10000;
  if (votes<0.05 || (levels.std()<0.01)) {
    if (now-last_vote>1) {
      roll = 1;
      if (show_bar<now-2) {
	show_bar = now;
      } else {
	show_bar = now-1;
      }
    }
    //pmono.CastCopy(mono);
  } else {
    last_vote = now;
  }
 

  if (now-show_bar<1.5) {
    roll = 1;
    for (int i=0; i<src.GetWidth(); i++) {
      for (int j=0; j<src.GetHeight()*0.1; j++) {
	dest(i,j).b = 255;
	dest(i,src.GetHeight()-j-1).b = 255;
      }
    }
  }

  //pmono.CastCopy(mono);
  
  static int need_update = 1;

  //if (need_update&&!roll) {
  //need_update = 0;
  //mark.CastCopy(pmonos[(index+1)%WCT]);
  //}
  prev.CastCopy(mono);
  index = (index+1)%WCT;
  if (roll) {
    need_update = 1;
    mark.CastCopy(pmonos[(index+1)%WCT]);
  }
}





void ObreroVision::Apply(YARPImageOf<YarpPixelRGB>& src,
			 YARPImageOf<YarpPixelRGB>& dest) {
  static IntegralImageTool ii;
  static YARPImageOf<YarpPixelFloat> work1, work2;
  GetMotion(src,dest);
  work1.CastCopy(dest);
  ii.GetMean(work1,work2,10);
  int bx = work2.GetWidth()/2, by = work2.GetHeight()/2;
  float bv = -1;
  float tv = 0;
  int ct = 0;
  IMGFOR(work2,x,y) {
    float& v = work2(x,y);
    if (v>bv) {
      bv = v;
      bx = x;
      by = y;
    }
    tv += v;
    v *= 10;
    if (v>255) v = 255;
    ct++;
  }
  if (ct<1) ct=1;
  tv /= ct;
  dest.CastCopy(work2);
  ox = bx;
  oy = by;
  ov = 0;
  if (tv>1 && bv>10) {
    printf(">>>> %g %g\n", bv, tv);
    AddCircle(dest,YarpPixelRGB(255,0,0),bx,by,5);
    ov = bv;
  }
  
}


