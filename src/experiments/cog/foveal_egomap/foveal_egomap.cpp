#include "YARPAll.h"

#include <iostream>
using namespace std;

#include "YARPImageDraw.h"
#include "YARPImageFile.h"
#include "YARPVisualSearch.h"
#include "YARPBottle.h"
#include "YARPString.h"

#include "YARPImagePortContent.h"
#include "YARPMapCameras.h"
#include "YARPObjectAnalysis.h"
#include "YARPTime.h"

#include <list>
#include <fstream>

#include <assert.h>

#define dbg_printf if(0)printf


#define LOCATE_TEST

//#define TWEAK_TEST

#ifdef __QNX__
#ifdef LOCATE_TEST
#undef LOCATE_TEST
#endif
#define SLOW_TEST
#endif

#define OBJECT_ANALYSIS

#define OBJECTS_PATH "/YARP/state/objects/raw"

#ifdef __QNX__
#undef Yield
#endif
#include "YARPScheduler.h"

YARPInputPortOf<YARPGenericImage> in_img;
YARPOutputPortOf<YARPGenericImage> out_img;
YARPOutputPortOf<YARPBottle> out_bot;

/*
class InBot : public YARPInputPortOf<YARPBottle> in_bot
{
public:
  InBot() : YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS)
    {}

  virtual void OnRead()
    {
      assert(Read(0));
    }
};
*/

class ObjectBoss
{
public:
  ObjectBoss()
  {}

  ObjectBoss(const ObjectBoss& ob)
  {
    //appearance = ob.appearance;
    //mask = ob.mask;
    label = -1;
    internal_q = 0;
    is_internal = 0;
  }
  
  int operator==(const ObjectBoss& ob)
    {
      return 1;
    }

  int operator<(const ObjectBoss& ob)
    {
      return 0;
    }

  YARPImageOf<YarpPixelBGR> appearance;
  YARPImageOf<YarpPixelMono> mask;
  YARPImageOf<YarpPixelMono> internal_mask;
  YARPImageOf<YarpPixelMono> external_mask;
  YARPImageOf<YarpPixelBGR> masked_appearance;
  YARPVisualSearch seeker;
  YARPVisualSearch internal_seeker;
  YARPVisualSearch external_seeker;
  int is_internal;
  float internal_q;
  int label;
  string name;

  void Update()
  {
    YarpPixelBGR black(0,0,0);
    YarpPixelBGR off_black(1,1,1);
    SatisfySize(appearance,masked_appearance);
    IMGFOR(appearance,x,y)
      {
	if (mask(x,y))
	  {
	    masked_appearance(x,y) = appearance(x,y);
	    if (appearance(x,y).r == 0)
	      {
		if (appearance(x,y).g == 0)
		  {
		    if (appearance(x,y).b == 0)
		      {
			masked_appearance(x,y) = off_black;
		      }
		  }
	      }
	  }
	else
	  {
	    masked_appearance(x,y) = black;
	  }
      }
    seeker.Add(appearance,mask);
  }
  string GetName() { return name; }
  int GetLabel() { return label; }
};

typedef list<ObjectBoss> ObjectList;
ObjectList objects;


void setup_boss(ObjectBoss& ob, YARPImageOf<YarpPixelBGR>& src,
		YARPImageOf<YarpPixelMono>& mask, float shrink)
{
  dbg_printf("line %ld\n", __LINE__);
  ob.appearance.PeerCopy(src);
  dbg_printf("line %ld\n", __LINE__);
  ob.mask.PeerCopy(mask);
  ob.label = -1;
  ob.name = "unknown";
  dbg_printf("line %ld\n", __LINE__);
  ob.Update();
  dbg_printf("line %ld\n", __LINE__);
#ifdef OBJECT_ANALYSIS
  YARPObjectAnalysis analysis;
  YARPImageOf<YarpPixelBGR> dest, seg;
  dbg_printf("line %ld\n", __LINE__);
  SatisfySize(ob.appearance,seg);
  SatisfySize(ob.appearance,dest);
  dbg_printf("line %ld\n", __LINE__);
  SatisfySize(ob.appearance,ob.internal_mask);
  SatisfySize(ob.appearance,ob.external_mask);
  dbg_printf("line %ld\n", __LINE__);
  analysis.ColorSegment(ob.appearance,seg);
  //sprintf(buf,"%s/seg/%s.ppm",OBJECTS_PATH,fname);
  //YARPImageFile::Write(buf,seg);
  dest.PeerCopy(ob.appearance);
  dbg_printf("line %ld\n", __LINE__);
  analysis.SetShrinkFactor(shrink);
  ob.internal_q = analysis.SpotInternals(seg,ob.mask,
					 ob.internal_mask,dest);
  ob.is_internal = 0;
  dbg_printf("line %ld\n", __LINE__);
  if (ob.internal_q>0.01)
    {
      ob.is_internal = 1;
      ob.internal_seeker.Add(ob.appearance,ob.internal_mask);
      IMGFOR(ob.mask,x,y)
	{
	  ob.external_mask(x,y) = 
	    255*(ob.mask(x,y) && (!ob.internal_mask(x,y)));
	}
      ob.external_seeker.Add(ob.appearance,ob.external_mask);
    }
  dbg_printf("line %ld\n", __LINE__);
  #endif
}


void load()
{
  char buf[256];
  sprintf(buf,"/YARP/src/applications/foveal_egomap/inv3.txt",OBJECTS_PATH);
  //sprintf(buf,"%s/inventory.txt",OBJECTS_PATH);
  printf("Looking for object traces in %s\n",buf);
  ifstream fin(buf);
  assert(!fin.fail());
  while (!fin.eof())
    {
      YARPScheduler::Yield();
      char fname[256];
      fin.getline(fname,sizeof(fname));
      if (!fin.eof())
	{
	  objects.push_back(ObjectBoss());
	  ObjectBoss& ob = objects.back();
	  YARPImageOf<YarpPixelBGR> src;
	  YARPImageOf<YarpPixelMono> mask;
	  sprintf(buf,"%s/mask/%s.ppm",OBJECTS_PATH,fname);
	  YARPImageFile::Read(buf,src);
	  mask.CastCopy(src);
	  sprintf(buf,"%s/src/%s.ppm",OBJECTS_PATH,fname);
	  YARPImageFile::Read(buf,src);
	  sprintf(buf,"%s/label/%s.txt",OBJECTS_PATH,fname);
	  setup_boss(ob,src,mask,0.75);
	  ifstream fprop(buf);
	  if (!fprop.fail())
	    {
	      int label;
	      fprop >> label;
	      ob.label = label;
	    }
	  ob.name = fname;
	  sprintf(buf,"%s/overlay/%s.ppm",OBJECTS_PATH,fname);
	  YARPImageFile::Write(buf,ob.masked_appearance);
	  printf("File %s (%d)\n", fname, ob.label);
	}
    }
}


#define TEST_THETA 0.5 /* TWEAKED */

float Compare(ObjectBoss& ob1, ObjectBoss& ob2)
{
  float v = (ob1).seeker.Compare((ob2).seeker);
  return v;
  if ((ob1).is_internal != (ob2).is_internal)
    {
      v = v-0.2;
    }
  else
    {
      if ((ob1).is_internal && (ob2).is_internal)
	{
	  float v2 = 
	    (ob1).internal_seeker.Compare((ob2).internal_seeker);
	  float v3 = 
	    (ob1).external_seeker.Compare((ob2).external_seeker);
	  float v1 = v;
	  float v_new = (v2+v3)/2.0;
	  if (v>v_new)
	    {
	      v = v_new;
	    }
	  if (v2<v) v = v2;
	  if (v3<v) v = v3;
	  //v = (v+v2+v3)/3.0;
	  v = (v+v1)/2.0;
	}
    }
  return v - TEST_THETA;
}

void test()
{
  float total_pos = 0, total_neg = 0;
  int count_pos = 0, count_neg = 0;
  int class_good = 0, class_total = 0;
  float lowest_positive = 1000.0;
  float highest_negative = -1000.0;
  for (ObjectList::iterator ob1=objects.begin(); ob1!=objects.end(); ob1++)
    {
      for (ObjectList::iterator ob2=ob1; ob2!=objects.end(); ob2++)
	{
	  float v = Compare((*ob1),(*ob2));
	  //	  if ((*ob1).GetLabel()==(*ob2).GetLabel())
	  int lab1 = (*ob1).GetLabel();
	  int lab2 = (*ob2).GetLabel();
	  YARPString name1 = (*ob1).GetName();
	  YARPString name2 = (*ob2).GetName();
	  if (0)
	    {
	      printf("Comparing %s with %s gives %g\t\t%d %d\n",
		     (*ob1).GetName().c_str(), 
		     (*ob2).GetName().c_str(),
		     v,
		     (*ob1).GetLabel(), (*ob2).GetLabel());
	    }
	  class_total++;
	  float theta = 0;
	  if ((*ob1).GetLabel()==(*ob2).GetLabel())
	    {
	      lowest_positive = min(lowest_positive,v);
	      if (v>theta) class_good++;
	      else
		{
		  printf("Failed to match a %d by %g    %s %s\n", lab1, v, name1.c_str(), name2.c_str());
		}
	    }
	  else
	    {
	      highest_negative = max(highest_negative,v);
	      if (v<theta) class_good++;
	      else
		{
		  printf("Confused a %d with a %d by %g   %s %s\n", lab1, lab2,
			 v,
			 name1.c_str(), name2.c_str());
		}
	    }
	  if (ob1 != ob2)
	    {
	      if ((*ob1).GetLabel()==(*ob2).GetLabel())
		{
		  total_pos += v;
		  count_pos++;
		}
	      else
		{
		  total_neg += v;
		  count_neg++;
		}
	    }
	}
    }
  printf("*** lowest positive is %g, highest negative is %g\n",
	 lowest_positive, highest_negative);
  if (count_pos<1) count_pos = 1;
  if (count_neg<1) count_neg = 1;
  total_pos /= count_pos;
  total_neg /= count_neg;
  float total = total_pos-total_neg;
  float percent = (100.0*class_good)/class_total;
  printf("Classification %d total, %d good (%g\%)\n", class_total, class_good,
	 percent);
  printf("TWEAK_ITEM %g\n", total);
  //printf("TWEAK_ITEM %g\n", percent);
}

void locate_test(int argc, char*argv[])
{
  YARPImageOf<YarpPixelBGR> src, dest, org, dest2;
  YARPImageFile::Read((argc>1)?argv[1]:"locate.ppm",src);
  dest.PeerCopy(src);
  org.PeerCopy(src);
  dest2.PeerCopy(src);

  int idx = 0;
  ObjectList::iterator best = objects.begin();
  float vbest = -1;
  float xbest = 64, ybest = 64;
  printf("Scanning.");
  fflush(stdout);
  for (ObjectList::iterator ob2=objects.begin(); ob2!=objects.end();
       ob2++)
    {
      float x = 64, y = 64;
      src.PeerCopy(org);
      dest.PeerCopy(org);
      float v = (*ob2).seeker.Localize(src,dest,x,y);
      printf("<%g>",v);
      if (v>vbest)
	{
	  best = ob2;
	  vbest = v;
	  xbest = x;
	  ybest = y;
	  dest2.PeerCopy(dest);
	}
      idx++;
      printf(".");
      fflush(stdout);
    }
  printf("\n");
  fflush(stdout);
  float v = vbest;
  ObjectList::iterator object = best;
  printf("Best match is with %s with value %g at (%g,%g)\n",
	 (*object).GetName().c_str(), v, xbest, ybest);
  YARPImageFile::Write("result.ppm",dest2);
  float xx = xbest, yy = ybest, theta = 0;
  (*best).seeker.Orient(src,dest2,xbest,ybest,theta);
  YARPImageFile::Write("result2.ppm",dest2);
  exit(0);
}

void slow_test(int argc, char *argv[])
{
  YARPImageOf<YarpPixelBGR> back;
  YARPImageFile::Read("back.ppm",back);
  while(1)
    {
      printf("Waiting for an image...\n");
      in_img.Read();
      printf("Got an image\n");

      out_img.Content().PeerCopy(in_img.Content());
      out_img.Write(1);
      out_img.FinishSend();
      YARPTime::DelayInSeconds(2);
      
      ObjectBoss current;
      YARPImageOf<YarpPixelBGR> src, dest, seg;
      YARPImageOf<YarpPixelMono> mask, mask_dest;
      src.Refer(in_img.Content());
      YARPImageFile::Write("src.ppm",src);

      char buf[256] = "";
      sprintf(buf,"poke/%ld.ppm", (long int)(YARPTime::GetTimeAsSeconds()));
      YARPImageFile::Write(buf,src);

      SatisfySize(src,mask);
      IMGFOR(src,x,y)
	{
	  if (src(x,y).r||src(x,y).g||src(x,y).b)
	    {
	      mask(x,y)=255;
	    }
	  else
	    {
	      mask(x,y)=0;
	      src(x,y) = back(x,y);
	    }
	}
      YARPImageFile::Write("mask.pgm",mask);

      out_img.Content().PeerCopy(src);
      out_img.Write(1);
      out_img.FinishSend();
      YARPTime::DelayInSeconds(2);

      
      setup_boss(current,src,mask,0.75);
      YARPObjectAnalysis analysis;
      mask_dest.PeerCopy(mask);
      dest.PeerCopy(src);
      SatisfySize(src,seg);
      analysis.ColorSegment(src,seg);
      YARPImageFile::Write("seg.ppm",seg);
      analysis.SpotInternals(seg,mask,mask_dest,dest);
      YARPImageFile::Write("mask_dest.pgm",mask_dest);
      YARPImageFile::Write("dest.ppm",dest);
      
      ObjectBoss * ob1 = &current;
      float best_v = -10000.0;
      ObjectList::iterator best = objects.begin();
      for (ObjectList::iterator ob2=objects.begin(); ob2!=objects.end(); ob2++)
	{
	  float v = Compare((*ob1),(*ob2));
	  //	  if ((*ob1).GetLabel()==(*ob2).GetLabel())
	  int lab1 = (*ob1).GetLabel();
	  int lab2 = (*ob2).GetLabel();
	  YARPString name1 = (*ob1).GetName();
	  YARPString name2 = (*ob2).GetName();
	  if (1)
	    {
	      printf("Comparing %s with %s gives %g\t\t%d %d\n",
		     (*ob1).GetName().c_str(), 
		     (*ob2).GetName().c_str(),
		     v,
		     (*ob1).GetLabel(), (*ob2).GetLabel());
	    }
	  if (v>best_v)
	    {
	      best = ob2;
	      best_v = v;
	    }
	}
      printf("Best is %s, score %g\n", (*best).GetName().c_str(),
	     best_v);
      YARPImageFile::Write("best.ppm",(*best).appearance);
      out_img.Content().PeerCopy((*best).appearance);
      out_img.Write(1);
      out_img.FinishSend();
    }
}

void main(int argc, char*argv[])
{

#ifdef TWEAK_TEST
  load();
  test();
#endif

#ifdef LOCATE_TEST
  load();
  locate_test(argc,argv);
#endif

#ifdef SLOW_TEST
  load();

  in_img.Register("/foveal_egomap/i:img");
  out_img.Register("/foveal_egomap/o:img");
  slow_test(argc,argv);
#endif

#ifndef TWEAK_TEST
  in_img.Register("/foveal_egomap/i:img");
  out_img.Register("/foveal_egomap/o:img");
  out_bot.Register("/foveal_egomap/o:bot");

  load();

  int ct = 0;
  ObjectList::iterator object = objects.begin();
  int tracking = 0;
  
      float thet = 0.1;
  float x_pos = 64, y_pos = 64;
  while (1)
    {
      in_img.Read();

      YARPImageOf<YarpPixelBGR> src, dest;
      src.Refer(in_img.Content());
      dest.PeerCopy(src);
      float v = -1;
      if (!tracking)
	{
	  out_img.Content().PeerCopy(src);
	  out_img.Write();
	  int idx = 0;
	  ObjectList::iterator best = object;
	  float vbest = -1;
	  printf("Scanning.");
	  fflush(stdout);
	  for (ObjectList::iterator ob2=objects.begin(); ob2!=objects.end();
	       ob2++)
	    {
	      float v = (*ob2).seeker.Localize(src,dest,x_pos,y_pos);
	      if (v>vbest)
		{
		  best = ob2;
		  vbest = v;
		}
	      idx++;
	      printf(".");
	      fflush(stdout);
	    }
	  printf("\n");
	  fflush(stdout);
	  v = vbest;
	  object = best;
	  
	}
      else
	{
	  v = (*object).seeker.Localize(src,dest,x_pos,y_pos,1000);
	  if (v<thet/2)
	    {
	      ct+=2;
	    }
	  if (v<thet/5)
	    {
	      ct += 100;
	    }
	  else if (v>thet)
	    {
	      if (ct>0)
		{
		  ct--;
		}
	    }
	  //					  tracking?16:1000);
	}
      printf("%svalue is %g\n", tracking?"tracking, ":"", v);

      if (v>thet)
	{
	  tracking = 1;
	}

      if (tracking)
	{
	  out_img.Content().PeerCopy(dest);
	  //ct++;
	  YARPBottleIterator bottle(out_bot.Content());
	  bottle.WriteVocab(YBC_OBJECT);
	  bottle.WriteVocab(YBC_FOUND);
	  YARPCogMapCameras mapper;
	  double x2, y2;
	  mapper.Foveal2WideangleSquashed(x_pos,y_pos,x2,y2);
	  bottle.WriteInt((int)(x2+0.5));
	  bottle.WriteInt((int)(y2+0.5));
	  out_bot.Write(1);
	}
      else
	{
	  out_img.Content().PeerCopy(src);
	}
      out_img.Write();
  
      if (ct>30)
	{
	  ct = 0;
	  tracking = 0;
	  object++;  if (object==objects.end()) object = objects.begin();
	}
      tracking = 0;
    }
#endif
}

