#include <stdio.h>
#include <assert.h>

#include <yarp/YARPAll.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageDraw.h>
#include <yarp/YARPRandom.h>
#include <yarp/YARPSpotter.h>

#include <fstream>
using namespace std;



// stupid windows
#define for if (1) for

#define USE_BLUR
//#define REALLY_USE_BLUR
//#define USE_GRAPH

YARPSpotter *p_spot = NULL;

#define spot (*p_spot)

void Process(YARPImageOf<YarpPixelBGR>& src, YARPImageOf<YarpPixelMono>& mask,
	     YARPImageOf<YarpPixelBGR>& dest, int training = 0,
		 int ignore = 0)
{
	for (int q=0; q<1; q++) {
#ifdef USE_BLUR
#ifdef REALLY_USE_BLUR
  YARPImageOf<YarpPixelFloat> tmp;
  YARPImageOf<YarpPixelRGBFloat> tmp2;
  tmp.CastCopy(src);
  tmp2.BiggerThan(tmp);
  IMGFOR(tmp,x,y) {
    tmp(x,y) = src(x,y).r;
  }
  IMGFOR(tmp,x,y) {
    int ct = 0;
    float sum = 0;
    for (int xx=-1; xx<=1; xx++) {
      for (int yy=-1; yy<=1; yy++) {
	sum += tmp.SafePixel(x+xx,y+yy);
	ct++;
      }
    }
    tmp2(x,y).r = sum/ct;
  }
  IMGFOR(tmp,x,y) {
    tmp(x,y) = src(x,y).g;
  }
  IMGFOR(tmp,x,y) {
    int ct = 0;
    float sum = 0;
    for (int xx=-1; xx<=1; xx++) {
      for (int yy=-1; yy<=1; yy++) {
	sum += tmp.SafePixel(x+xx,y+yy);
	ct++;
      }
    }
    tmp2(x,y).g = sum/ct;
  }
  IMGFOR(tmp,x,y) {
    tmp(x,y) = src(x,y).b;
  }
  IMGFOR(tmp,x,y) {
    int ct = 0;
    float sum = 0;
    for (int xx=-1; xx<=1; xx++) {
      for (int yy=-1; yy<=1; yy++) {
	sum += tmp.SafePixel(x+xx,y+yy);
	ct++;
      }
    }
    tmp2(x,y).b = sum/ct;
  }
  static YARPImageOf<YarpPixelRGBFloat> tmp3;
  //tmp3.ScaledCopy(tmp2,128,128);
  dest = src;
  //src.CastCopy(tmp3);
  src.CastCopy(tmp2);
#else
  dest = src;
#endif
#else
  static YARPImageOf<YarpPixelBGR> tmp3;
  tmp3.ScaledCopy(src,128,128);
  dest = src;
  src.CastCopy(tmp3);
#endif

  static int was_training = 0;
  if (training) {
	  if (!was_training) {
		  spot.Reset();
	  }
	  if (ignore) {
	    spot.TrainIgnore(src,mask,dest);
	  } else {
	    spot.Train(src,mask,dest);
	  }
  } else {
	SatisfySize(src,dest);
	spot.Test(src,dest);
	int x = spot.GetX();
	int y = spot.GetY();
	int obj = spot.GetID();
	YarpPixelBGR pix0(0,0,0);
	YarpPixelBGR pix1(255,255,255);
	AddCircle(dest,pix0,x,y,6);
	AddCircle(dest,pix1,x,y,3);
  }
  was_training = training;
	}
}

void Process(const char *fname)
{
  YARPImageOf<YarpPixelBGR> src, dest;
  YARPImageOf<YarpPixelMono> mask;
  YARPImageFile::Read(fname,src);
  Process(src,mask,dest);
  static int ct = 0;
  char buf[256];
  sprintf(buf,"out_%09d.ppm",ct);
  YARPImageFile::Write(buf,dest);
  ct++;
}

void TrainProcess(const char *fname)
{
  YARPImageOf<YarpPixelBGR> src, dest;
  YARPImageOf<YarpPixelMono> mask;
  YARPImageFile::Read(fname,src);
  mask.BiggerThan(src);
  IMGFOR(mask,x,y) {
    mask(x,y) = 255;
  }
  spot.Train(src,mask,dest);
}

int flipper_main(int argc, char *argv[])
{
	p_spot = new YARPSpotter;
	assert(p_spot!=NULL);
  argv++;
  argc--;


  const char *list_name = NULL;
	
  if (argc>=1) {
	  argv[0];
	argv++;
	argc--;
  }

  //list_name = "y:\\zgarbage\\pf\\list1.txt";

#if 0
 {
  ifstream fin(list_name);
  while (!(fin.eof()||fin.bad()))
    {
      char buf[256];
      fin.getline(buf,sizeof(buf));
      if (!fin.eof())
	{
	  cout << "Processing image for training " << buf << endl;
	  TrainProcess(buf);
	}
    }
  ofstream fout("train.txt");
  spot.WriteTrain(fout,1);
 }
#endif

  char buffer[256];
  sprintf (buffer, "%s/conf/babybot/object_model/declare.txt", GetYarpRoot());
  ///spot.AddItems("y:/conf/babybot/object_model/declare.txt");
  spot.AddItems(buffer);

#ifdef USE_GRAPH
  spot.UseGraph(1);
#else
  spot.UseGraph(0);
#endif

  if (list_name!=NULL) {
  printf("list name is %s\n", list_name);
  ifstream fin(list_name);
  while (!(fin.eof()||fin.bad()))
    {
      char buf[256];
      fin.getline(buf,sizeof(buf));
      if (!fin.eof())
	{
	  cout << "Processing image " << buf << endl;
	  Process(buf);
	}
    }
  }
//	exit(0);
  return 0;
}

void flipper_end() {
	if (p_spot!=NULL) {
		delete p_spot;
		p_spot = NULL;
	}
}

void flipper_apply(YARPImageOf<YarpPixelBGR>& src,
				   YARPImageOf<YarpPixelMono>& mask,
				   YARPImageOf<YarpPixelBGR>& dest, int ignore) {
	Process(src,mask,dest,1,ignore);
}

void flipper_apply(YARPImageOf<YarpPixelBGR>& src,
				   YARPImageOf<YarpPixelBGR>& dest,
				   int& xx, int& yy, int& ff) {
	YARPImageOf<YarpPixelMono> mask;
	Process(src,mask,dest);
	YARPImageOf<YarpPixelFloat>& sal = spot.GetSalience();
#ifndef USE_GRAPH
#if 0
	IMGFOR(sal,x,y) {
		YarpPixelBGR& ref = src(x,y);
		YarpPixelBGR& def = dest(x,y);
		def.r = ref.r;
		def.g = ref.g;
		def.b = ref.b;
		int v = 255-sal(x,y);
		if (v>def.g) { def.g = v; }
	}
#endif
#endif
	YarpPixelBGR pix1(255,255,255);
	YarpPixelBGR pix2(0,0,0);
	int x = spot.GetX();
	int y = spot.GetY();
	int found = spot.Found();
	float val = spot.GetStrength();
	cout << "value of match " << val << endl;
	int offset = found?0:20;
	AddCircleOutline(dest,pix2,x,y,15+offset);
	AddCircleOutline(dest,pix1,x,y,13+offset);
	xx = x;
	yy = y;
	ff = found;
	//dest.CastCopy(sal);
	static int good = 0;
	int tgood = 4;
	if (val>525) {
		if (good<tgood) good++;
	} else {
		good = 0;
	}
	if (good>=tgood) {
		for (int w = 40; w>=35; w--) {
		for (int ox=0; ox<w; ox++) {
			dest.SafePixel(w-ox, ox).r = 255;
			dest.SafePixel(dest.GetWidth()-1-(w-ox), ox).r = 255;
			dest.SafePixel(w-ox, dest.GetHeight()-1-ox).r = 255;
			dest.SafePixel(dest.GetWidth()-1-(w-ox), dest.GetHeight()-1-ox).r = 255;
		}
		}
	}
}
