#include <stdio.h>
#include <string.h>

#include <string>
#include <hash_map>
using namespace std;

#define USE_SPHINX

#include "YARPPort.h"
#include "YARPNetworkTypes.h"

#ifndef USE_SPHINX
#include "YARPViaVoiceASR.h"
#else
#include "YARPSphinxASR.h"
#endif

#define ON_VOICE(x,y) if (0==strcmp(x,y))

YARPOutputPortOf<NetInt32> voice_cmd;

class eq_str
{
public:
  int operator() (const string& s1, const string& s2) const
  {
    return s1 == s2;
  }
};

class hash_str
{
public:
  size_t operator() (const string& s1) const
  {
    hash<const char *> h;
    return h(s1.c_str());
  }
};

typedef hash_map<int, string, hash<int>, equal_to<int> > hash_is;
typedef hash_map<string, int, hash_str, eq_str > hash_si;


class VoiceController
{
public:

  void Say(int x)
  {
    printf("command %d\n", x);
    voice_cmd.Content() = x;
    voice_cmd.Write();
  }

  void Stay() { Say(0); }

  void Next() { Say(1); }

  void SetTarget(int id) { Say(200+id); }

  void RevertTarget(int id) { Say(300+id); }

  void Look() { Say(4); }
};


class ASR : public 
#ifndef USE_SPHINX
YARPViaVoiceASR 
#else
YARPSphinxASR 
#endif
{
public:
  hash_si phrase_register;
  int free_id;

  VoiceController controller;

  ASR()
  { free_id = 0; }

  virtual void OnHeardPhrase(Phrase& p)
  {
    printf(">>> I heard something\n");
    int set_target = 0;
    int revert_target = 0;
    if (p.IsValid())
      {
	for (int i=0; i<p.GetWordCount(); i++)
	  {
	    char buf[256];
	    p.GetText(i,buf,sizeof(buf));
	    printf("    + (%s)\n", buf);
	    ON_VOICE(buf,"next") controller.Next();
	    ON_VOICE(buf,"stay") controller.Stay();
	    ON_VOICE(buf,"look") controller.Look();
	    ON_VOICE(buf,"mark") set_target = 1;
	    ON_VOICE(buf,"go") revert_target = 1;
	    if (buf[0]=='p' && buf[1]=='h')
	      {
		string s(buf);
		hash_si::iterator it = phrase_register.find(s);
		if (it == phrase_register.end())
		  {
		    AddPhrase(buf,"VISIBLES");
		    phrase_register[s] = free_id;
		    free_id++;
		  }
		if (1)
		  {
		    int id = phrase_register[s];
		    if (set_target)
		      {
			controller.SetTarget(id);
		      }
		    if (revert_target)
		      {
			controller.RevertTarget(id);
		      }
		  }
	      }
	  }
      }
  }
};

void main(int argc, char *argv[])
{
  ASR master;
  voice_cmd.Register("/voice/command/out");
#ifndef USE_SPHINX
  master.CreateVocabulary("VISIBLES");
  master.Main("control.fsg");
#else
  master.Main(argc,argv);
#endif
}

