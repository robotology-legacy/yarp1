#include "YARPAll.h"

#include <iostream>

#ifndef __QNX__
using namespace std;
#endif

#include "YARPTime.h"
#include "YARPThread.h"
#include "YARPPort.h"
#include "YARPVoiceMessage.h"
#include "YARPBottle.h"
#include "YARPBottleCodes.h"
#include "YARPSemaphore.h"

double first_time = 0;

YARPOutputPortOf<YARPBottle> out_bot;

YARPSemaphore out_bot_sema(1);
YARPBottleIterator out_bot_it;

YARPBottleIterator& GetBottle()
{
  out_bot_sema.Wait();
  out_bot_it.Attach(out_bot.Content());
  out_bot_it.Reset();
  return out_bot_it;
}

void ReleaseBottle()
{
  out_bot.Write(1);
  out_bot_sema.Post();
}

class Numeric
{
public:
  double value, nominal, shift, factor;
  
  Numeric(double n_nominal = 0.0)
    { value = nominal = n_nominal;  shift = 1;  factor = 2; }
  
  void SetNominal(double n_nominal)
    { nominal = n_nominal; }
  
  void SetValue(double n_value)
    { value = n_value; }
  
  double GetNominal()
    { return nominal; }

  double GetValue()
    { return value; }

  void SetHard(double n_value)
    { nominal = value = n_value; }
  
  void Set(double n_value)
    { value = n_value; }
  
  double Get()
    { return value; }
  
  double Inc()
    { return value = value*factor+shift; }

  double Dec()
    { return value = (value-shift)/factor; }
  
  double CropBelow()
    { if (value<nominal) value = nominal; return value; }

  double CropAbove()
    { if (value>nominal) value = nominal; return value; }
  
  void SetBump(double n_shift, double n_factor)
    { shift = n_shift;  factor = n_factor; }
 
  void SetShift(double n_shift)
    { shift = n_shift; }
  
  void SetFactor(double n_factor)
    { factor = n_factor; }
};


class BState
{
public:
  static double now;
  static double& GetNow() { return now; }
};

double BState::now = 0;

class FixationManager
{
public:
  enum
    {
      FIX_NULL,
      FIX_DEFAULT,
      FIX_RELEASING,
      FIX_MAINTAINING,
    };
  
  int state;
  Numeric fix_duration;
  double msg_time, maintain_time;
  
  FixationManager()
    {
      Reset();
    }
  
  void Reset()
    {
      msg_time = -10000.0;
      state = FIX_DEFAULT;
      fix_duration.SetHard(5);
      fix_duration.SetShift(0);
      fix_duration.SetFactor(2);
    }
  
  void Update()
    {
      double now = BState::GetNow();
      switch (state)
	{
	case FIX_MAINTAINING:
	  if (now-msg_time>1)
	    {
	      printf("Should inhibit gaze\n");
	      msg_time = now;
	      YARPBottleIterator& bot = GetBottle();
	      bot.WriteVocab(YBC_GAZE);
	      bot.WriteVocab(YBC_GAZE_FIXATE);
	      bot.WriteInt(1);
	      bot.WriteText("fixating gaze");
	      ReleaseBottle();
	    }
	  if (now-maintain_time>10)
	    {
	      printf("No more inhibition\n");
	      state = FIX_DEFAULT;
	    }
	  break;
	case FIX_RELEASING:
	    {
	      YARPBottleIterator& bot = GetBottle();
	      bot.WriteVocab(YBC_GAZE);
	      bot.WriteVocab(YBC_GAZE_FIXATE);
	      bot.WriteInt(0);
	      bot.WriteText("fixating gaze");
	      ReleaseBottle();
	    }
	  state = FIX_DEFAULT;
	  break;
	}
    }
  
  void SetMaintain(int flag)
    {
      double now = BState::GetNow();
      maintain_time = now;
      state = FIX_MAINTAINING;
      if (flag == 0)
	{
	  state = FIX_RELEASING;
	}
      Update();
    }
} fixation_man;



class VoiceCommand : public YARPInputPortOf<YARPBottle>
{
public:
  VoiceCommand() : YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS)
    {
    }
  
  int IsFront(const char *ref, const char *target)
    {
      int ct = 0;
      int success = 0;
      for (int i=0; i<strlen(ref) && i<strlen(target); i++)
	{
	  if (ref[i] == target[i])
	    {
	      ct++;
	    }
	}
      if (ct == strlen(target))
	{
	  if (strlen(ref)>ct)
	    {
	      if (ref[ct]==' ')
		{
		  success = 1;
		}
	    }
	  else
	    {
	      success = 1;
	    }
	}
      return success;
    }
  
  virtual void OnRead()
    {
      Read();
      YARPBottleIterator it(Content());
      it.Rewind();
      int v = it.ReadVocab();
      if (v!=YBC_TICK)
	{
	  printf("]]] "); it.Display();
	}
      it.Rewind();
      v = it.ReadVocab();
      switch (v)
	{
	case YBC_VOICE:
	    {
	      int v2 = it.ReadVocab();
	      if (v2 == YBC_VOICE_HEARD)
		{
		  YARPBottleIterator& bot = GetBottle();
		  bot.WriteVocab(YBC_GAZE);
		  bot.WriteVocab(YBC_GAZE_UNFREEZE);
		  bot.WriteText("speech recog complete");
		  ReleaseBottle();
		  
		  const char *msg = it.ReadText();
		  printf("Got voice message [%s]\n", msg);
		  if (IsFront(msg,"stay"))
		    {
		      printf("got STAY!\n");
		      fixation_man.SetMaintain(1);
		    }
		  if (IsFront(msg,"next"))
		    {
		      printf("got NEXT!\n");
		      fixation_man.SetMaintain(0);
		    }
		  int assess = 0;
		  int val = 0;
		  if (IsFront(msg,"good")||IsFront(msg,"yes"))
		    {
		      printf("got GOOD!\n");
		      assess = 1;
		      val = 1;
		    }
		  if (IsFront(msg,"bad")||IsFront(msg,"no"))
		    {
		      printf("got BAD!\n");
		      assess = 1;
		      val = 0;
		    }
		  if (assess)
		    {
		      printf("Assessment %d\n", val);
		      YARPBottleIterator& bot = GetBottle();
		      bot.WriteVocab(YBC_ASSESSMENT);
		      bot.WriteVocab(val?YBC_POSITIVE:YBC_NEGATIVE);
		      bot.WriteText("externally supplied assessment");
		      ReleaseBottle();
		    }
		}
	      else if (v2==YBC_VOICE_HEARING)
		{
		  YARPBottleIterator& bot = GetBottle();
		  bot.WriteVocab(YBC_GAZE);
		  bot.WriteVocab(YBC_GAZE_FREEZE);
		  bot.WriteText("freezing, heard something");
		  ReleaseBottle();
		}
	    }
	  break;
	}
    }
} voice_cmd;


void logo()
{
  printf("------------------------------------------------------\n");
  printf("Yet Another Behavior System (YABS) tick %ld\n",
	 (long int)(YARPTime::GetTimeAsSeconds()-first_time+0.5));
  printf("\n\n");
}

void main()
{
  voice_cmd.Register("/behave/i:bot");
//  in_bot.Register("/behave/i:bot");
  out_bot.Register("/behave/o:bot");
  YARPTime::DelayInSeconds(0.5);
  first_time = YARPTime::GetTimeAsSeconds();
  double tick_time = 0;
  double target = 0;
  while (1)
    {
      double now = YARPTime::GetTimeAsSeconds() - first_time;
      if (now>=target)
	{
	  target += 10;      
	  //logo();
	}
      if (now>=tick_time)
	{
	  YARPBottleIterator& it = GetBottle();
	  it.WriteVocab(YBC_TICK);
	  it.WriteVocab(YBC_TICK_SECOND);
	  it.WriteText("time signal");
	  ReleaseBottle();
	  tick_time += 1;
	}
      BState::GetNow() = now;
      fixation_man.Update();
      YARPTime::DelayInSeconds(0.1);
    }
}
