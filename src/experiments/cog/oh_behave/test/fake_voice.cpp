#include "YARPAll.h"

#include <iostream>
using namespace std;

#include "YARPTime.h"
#include "YARPPort.h"
#include "YARPBottle.h"


YARPOutputPortOf<YARPBottle> out_voc;

void main(int argc,char *argv[])
{
  int singleton = (argc>1);
  out_voc.Register("/oh_test/o:bot");
  out_voc.Connect("/behave/i:bot");
  do
    {
      char buf[256] = "stay";
      if (!singleton)
	{
	  cin.getline(buf,sizeof(buf));
	}
      //out_voc.Content().GetText() = "stay";
      YARPBottleIterator it(out_voc.Content());
      it.WriteVocab(YBC_VOICE);
      it.WriteVocab(YBC_VOICE_HEARD);
      it.WriteText((argc>1)?argv[1]:buf);
      it.WriteText("testing");
      out_voc.Write();
      YARPTime::DelayInSeconds(singleton?2:0);
    }
  while (!singleton);
}
