#include <stdio.h>

#include "YARPViaVoiceASR.h"

class ASR : public YARPViaVoiceASR 
{
public:
  virtual void OnHeardPhrase(Phrase& p)
  {
    printf(">>> I heard something\n");
    if (p.IsValid())
      {
	for (int i=0; i<p.GetWordCount(); i++)
	  {
	    char buf[256];
	    p.GetText(i,buf,sizeof(buf));
	    printf("    + %s\n", buf);
	  }
	p.SaveWave("foo.wav");
      }
    else
      {
	p.SaveWave("discard.wav");
      }
  }
};

void main()
{
  ASR master;
  master.Main("kismetgram1.fsg");
}

