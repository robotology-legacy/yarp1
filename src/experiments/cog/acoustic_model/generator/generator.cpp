#include <string.h>
#include <stdlib.h>
#include <fstream.h>

#include "YARPPort.h"
#include "YARPTime.h"
#include "YARPViaVoiceTTS.h"
#include "YARPRandomNumber.h"

#include "acoustic_msg.h"

YARPOutputPortOf<GenAnnotation> out_annotation;

float GetRandom(float lo, float hi)
{
  return lo+YARPRandom::Uniform()*(hi-lo);
}

void main()
{
  YARPViaVoiceTTS master;
  ifstream script("script.txt");

  out_annotation.Register("/exp/acoustic/o:ann");
      YARPTime::DelayInSeconds(1);
  out_annotation.Connect("/exp/acoustic/i:ann");
      YARPTime::DelayInSeconds(1);

  while (1)
    {
      YARPTime::DelayInSeconds(1);
      master.SetClass(YARPRandom::Rand(VC_MALE,VC_OLD_FEMALE));
      master.SetQuality(VQ_PITCH,GetRandom(0.45,0.85));
      master.SetQuality(VQ_HEADSIZE,GetRandom(0.4,0.7));
      master.SetQuality(VQ_ROUGHNESS,GetRandom(0.0,0.7));
      master.SetQuality(VQ_BREATHINESS,GetRandom(0.0,0.7));
      master.SetQuality(VQ_RANGE,GetRandom(0.0,1.0));
      master.SetQuality(VQ_SPEED,GetRandom(0.4,0.6));
      master.SetQuality(VQ_VOLUME,GetRandom(0.8,1.0));

      script.getline(out_annotation.Content().txt, MAX_ANNOTATION);
      //strncpy(out_annotation.Content().txt, txt, MAX_ANNOTATION);
      char buf[1000];
      script.getline(buf,sizeof(buf));
      if (master.Sayable(buf))
	{
	  out_annotation.Write(1);
	  YARPTime::DelayInSeconds(0.5);
	  master.Say(buf);
	  //YARPTime::DelayInSeconds(2);
	}
    }
}
