#include <iostream.h>

#include "YARPAll.h"
#include "YARPTime.h"
#include "YARPPort.h"
#include "conf/speech_posture.h"

YARPOutputPortOf<SpeechPosture> out_posture;

void main()
{
  out_posture.Register("/yarp/speech/o:pose");
  YARPTime::DelayInSeconds(0.5);
  out_posture.Connect("/yarp/speech/i:pose");
  YARPTime::DelayInSeconds(0.5);
  
  while (1)
    {
      SpeechPosture& p = out_posture.Content();
      p.posture = 5;
      cout << "Writing posture " << p.posture << endl;
      out_posture.Write();
      YARPTime::DelayInSeconds(0.5);
    }
}

