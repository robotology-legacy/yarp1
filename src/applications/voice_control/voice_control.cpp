#include <stdio.h>
#include <string.h>

#include <string>
#include <hash_map>
using namespace std;

//#define USE_SPHINX

#include "YARPPort.h"
#include "YARPNetworkTypes.h"
#include "YARPTime.h"

#ifndef USE_SPHINX
#include "YARPViaVoiceASR.h"
#else
#include "YARPSphinxASR.h"
#endif

//#include "YARPVoiceMessage.h"
#include "YARPBottle.h"
#include "YARPBottleCodes.h"

#define FORCE 0

//YARPOutputPortOf<YARPVoiceMessage> voice_cmd;
YARPOutputPortOf<YARPBottle> voice_cmd;

class ASR : public 
#ifndef USE_SPHINX
YARPViaVoiceASR 
#else
YARPSphinxASR 
#endif
{
public:
  ASR()
  { }

  virtual void OnHeardPhrase(Phrase& p)
  {
    printf(">>> I heard something\n");
    int set_target = 0;
    int revert_target = 0;
    char big_buf[256] = "";
    if (p.IsValid())
      {
	int first = 1;
	for (int i=0; i<p.GetWordCount(); i++)
	  {
	    char buf[256];
	    p.GetText(i,buf,sizeof(buf));
	    if (!first) strncat(big_buf," ",sizeof(big_buf));
	    strncat(big_buf,buf,sizeof(big_buf));
	    printf("    + (%s)\n", buf);
	    first = 0;
	  }
	printf("    > [%s]\n", big_buf);
      }
    //voice_cmd.Content().GetText() = big_buf;
    YARPBottleIterator it(voice_cmd.Content());
    it.Reset();
    it.WriteVocab(YBC_VOICE);
    it.WriteVocab(YBC_VOICE_HEARD);
    it.WriteText(big_buf);
    it.WriteText("heard");
    if (FORCE)
      {
	voice_cmd.Write(FORCE);
      }
  }

  virtual void OnHearingBegins() 
  { 
    printf("*** hearing begins\n"); 
    YARPBottleIterator it(voice_cmd.Content());
    it.Reset();
    it.WriteVocab(YBC_VOICE);
    it.WriteVocab(YBC_VOICE_HEARING);
    //it.WriteText("hearing");
    if (FORCE)
      {
	voice_cmd.Write(FORCE);
      }
  }
  
};

void main(int argc, char *argv[])
{
  ASR master;
  voice_cmd.Register("/voice_control/o:bot");
#ifndef USE_SPHINX
  master.CreateVocabulary("VISIBLES");
  master.Main("control.fsg");
#else
  master.Main(argc,argv);
  /*
  while(1)
    {
      printf("Sending...\n");
      voice_cmd.Content().GetText() = "hello";
      voice_cmd.Write();
      YARPTime::DelayInSeconds(5);
    }
  */
#endif
}

