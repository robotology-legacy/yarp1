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

#include <list>
#include <fstream>

#include <assert.h>

#define TWEAK_TEST

#define OBJECTS_PATH "/YARP/state/objects/raw"

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
  YARPImageOf<YarpPixelBGR> masked_appearance;
  YARPVisualSearch seeker;
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

void load()
{
  char buf[256];
  sprintf(buf,"%s/inventory.txt",OBJECTS_PATH);
  printf("Looking for object traces in %s\n",buf);
  ifstream fin(buf);
  assert(!fin.fail());
  while (!fin.eof())
    {
      char fname[256];
      fin.getline(fname,sizeof(fname));
      if (!fin.eof())
	{
	  objects.push_back(ObjectBoss());
	  ObjectBoss& ob = objects.back();
	  sprintf(buf,"%s/mask/%s.ppm",OBJECTS_PATH,fname);
	  YARPImageFile::Read(buf,ob.appearance);
	  ob.mask.CastCopy(ob.appearance);
	  sprintf(buf,"%s/src/%s.ppm",OBJECTS_PATH,fname);
	  YARPImageFile::Read(buf,ob.appearance);
	  sprintf(buf,"%s/label/%s.txt",OBJECTS_PATH,fname);
	  ifstream fprop(buf);
	  if (!fprop.fail())
	    {
	      int label;
	      fprop >> label;
	      ob.label = label;
	    }
	  ob.name = fname;
	  ob.Update();
	  sprintf(buf,"%s/overlay/%s.ppm",OBJECTS_PATH,fname);
	  YARPImageFile::Write(buf,ob.masked_appearance);
	  printf("File %s (%d)\n", fname, ob.label);
	}
    }
}

#define TEST_THETA 0.5

void test()
{
  float total_pos = 0, total_neg = 0;
  int count_pos = 0, count_neg = 0;
  int class_good = 0, class_total = 0;
  for (ObjectList::iterator ob1=objects.begin(); ob1!=objects.end(); ob1++)
    {
      for (ObjectList::iterator ob2=objects.begin(); ob2!=objects.end(); ob2++)
	{
	  float v = (*ob1).seeker.Compare((*ob2).seeker);
	  printf("Comparing %s (%d) with %s (%d) gives %g\n",
		 (*ob1).GetName().c_str(), (*ob1).GetLabel(),
		 (*ob2).GetName().c_str(), (*ob2).GetLabel(),
		 v);
	  class_total++;
	  float theta = TEST_THETA;
	  if ((*ob1).GetLabel()==(*ob2).GetLabel())
	    {
	      if (v>theta) class_good++;
	    }
	  else
	    {
	      if (v<theta) class_good++;
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
  if (count_pos<1) count_pos = 1;
  if (count_neg<1) count_neg = 1;
  total_pos /= count_pos;
  total_neg /= count_neg;
  float total = total_pos-total_neg;
  printf("TWEAK_ITEM %g\n", total);
  printf("Classification %d total, %d good (%g\%)\n", class_total, class_good,
	 (100.0*class_good)/class_total);
}

void main()
{
  load();

#ifdef TWEAK_TEST
  test();
#endif

#ifndef TWEAK_TEST
  in_img.Register("/foveal_egomap/i:img");
  out_img.Register("/foveal_egomap/o:img");
  out_bot.Register("/foveal_egomap/o:bot");

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
    }
#endif
}

