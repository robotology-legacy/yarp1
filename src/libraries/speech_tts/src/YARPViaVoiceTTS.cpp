#include <stdio.h>

#include "YARPViaVoiceTTS.h"

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <memory.h>
#include <eci.h>
#include <signal.h>

#include <fcntl.h>
#include <linux/kd.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPNetworkTypes.h"
#include "YARPSemaphore.h"

#include "conf/speech_posture.h"

#define PREFIX_PITCH 
//#define PREFIX_TEXT "`l1.0 `v1 `vb%d `vh80 `vr50 `vy33 `vf0 `vs50 `vv0 it `vv100 "
#define PREFIX_TEXT "`l1.0 `v1 `vf99 `vh70 `vb%d"
#define UHM_TEXT "`l1.0 `v1 `vf0 `vh70 `vb40 `vs60 uhm"

#include "begin_pack_for_net.h"
class NetMsg
{
public:
  char text[256];
} PACKED_FOR_NET;
#include "end_pack_for_net.h"

static float Random()
{
  return ((float)rand())/RAND_MAX;
}

#if 0
YARPInputPortOf<NetMsg> in;
YARPOutputPortOf<NetMsg> out;
YARPOutputPortOf<SpeechPosture> out_posture;
#endif

ECIHand hECI;
int eci_active = 0;
char phoneme_buffer[1000];
int phoneme_buffer_ok = 0;
double last_utterance = -10000;
int speaking = 0;
double last_speaking = -10000;
YARPSemaphore state_mutex(1);


#define FAILURE_STRING "`[.1bAk.2kwot]"

void eci_deinit(int x);

int ttyfd = -1;

static ECICallbackReturn callback(ECIHand eciHand, ECIMessage msg,
   long lparam, void *data)
{
  if (msg == eciPhonemeIndexReply)
    {
      const ECIMouthData *mouthData = (ECIMouthData *)lparam;
      if (1) cout << "*** saying phoneme " << mouthData->szPhoneme 
	   << " " 
	   << (int)(mouthData->mouthHeight)
	   << "/" << (int)(mouthData->mouthWidth)
	   << "/" << (int)(mouthData->jawOpen)
	   << " "
	   << endl;
    }
  else if (msg == eciPhonemeBuffer)
    {
      //      cout << phoneme_buffer << endl;
      int ok = 1;
      if (strstr(phoneme_buffer, FAILURE_STRING))
	{
	  fprintf(stderr, "*** Dodgy phoneme sequence detected\n");
#if 0
	  out_posture.Content().posture = SPEECH_GENERATION_FAILS;
	  out_posture.Write();
#endif
	  ok = 0;
	}
      phoneme_buffer_ok = ok;
    }
  return eciDataProcessed;
}

void eci_init()
{
  int ret;

  // Create the ECI Instance
  hECI = eciNew();

  eci_active = 1;

  signal(SIGTERM, eci_deinit);
  signal(SIGKILL, eci_deinit);

  eciRegisterCallback(hECI, callback, 0);
  eciSetParam(hECI, eciWantPhonemeIndices, 1);
  eciSetParam(hECI, eciSynthMode, 1);

  // You may define FILEOUTPUT in the Makefile to output sound to a file
#ifdef FILEOUTPUT
  // set Output to file - and set filename to "temp.au"
  ret=eciSetOutputFilename(hECI,"temp.au");
  cout << "ret=" << ret << " from eciSetOutputFilename" << endl;
#else
  // This section produces output to the default sound device
  ret=eciSetOutputDevice(hECI, 0);
  cout << "ret=" << ret <<" from eciSetOutputDevice" << endl;
#endif

  // Set engine to parse annotated text
  ret=eciSetParam(hECI,eciInputType,1);
}

void eci_deinit(int x)
{
  if (eci_active)
    {
      // Delete the instance
      eciDelete(hECI);
      eci_active = 0;
    }
  if (ttyfd!=-1)
    {
      close(ttyfd);
    }
}

int eci_say(const char *txt, const char *prefix, int rehearsal = 0)
{
  printf("SAYING %s // %s \n", prefix, txt);
  int ret;
  int sayable = 0;
  if (prefix!=NULL)
    {
      ret=eciAddText(hECI,prefix);
    }
  ret=eciAddText(hECI,txt);
  cout << "ret=" << ret << " from eciAddText" << endl;

  //cout << "================================" << endl;
  ret=eciGeneratePhonemes(hECI,sizeof(phoneme_buffer),phoneme_buffer);
  //ret=eciSynchronize(hECI);
  //cout << "(" << ret << ") ================================" << endl;

  sayable = phoneme_buffer_ok;
  if (!phoneme_buffer_ok && !rehearsal)
    {
      prefix = UHM_TEXT;
      txt = NULL;
      phoneme_buffer_ok = 1;
    }

  if (phoneme_buffer_ok)
    {
#if 0
      out_posture.Content().posture = SPEECH_GENERATION_BEGINS;
      out_posture.Write();
#endif

      if (prefix!=NULL)
	{
	  ret=eciAddText(hECI,prefix);
	}
      if (txt!=NULL)
	{
	  ret=eciAddText(hECI,txt);
	}
      
      // Synthesize the text
      state_mutex.Wait();
      speaking = 1;
      last_speaking = YARPTime::GetTimeAsSeconds();
      state_mutex.Post();
      ret=eciSynthesize(hECI);
      cout << "ret=" << ret << " from eciSynthesize" << endl;
      
      // Synchronize. This will wait until synthesis completes
      ret=eciSynchronize(hECI);

#if 0      
      out_posture.Content().posture = SPEECH_GENERATION_ENDS;
      out_posture.Write();
#endif

    }

  if (!rehearsal)
    {
      state_mutex.Wait();
      speaking = 0;
      last_speaking = YARPTime::GetTimeAsSeconds();
      state_mutex.Post();
    }

  return sayable;
}


static int inited = 0;

YARPViaVoiceTTS::YARPViaVoiceTTS()
{
  if (!inited)
    {
      eci_init();
      inited = 1;
    }
  for (int i = 0; i<VQ_COUNT; i++)
    {
      vq_set[i] = 0;
    }
  vc = VC_MALE;
}

void YARPViaVoiceTTS::SetClass(int n_voice_class)
{
  vc = n_voice_class;
  for (int i = 0; i<VQ_COUNT; i++)
    {
      vq_set[i] = 0;
    }
}

void YARPViaVoiceTTS::SetQuality(int n_voice_quality, float value)
{
  vq[n_voice_quality] = value;
  vq_set[n_voice_quality] = 1;
}



int YARPViaVoiceTTS::Say(const char *text, int rehearsal = 0)
{
  int sayable = 0;
  int volume = 100;
  printf("%s\n",text);

  char buf[1000] = "";
  char buf2[1000] = "";

  int voice = 1;
  switch (vc)
    {
    case VC_MALE:       voice = 1;   break;
    case VC_FEMALE:     voice = 2;   break;
    case VC_CHILD:      voice = 3;   break;
    case VC_OLD_MALE:   voice = 7;   break;
    case VC_OLD_FEMALE: voice = 8;   break;
    }

  sprintf(buf2, "`l1.0 `v%d ", voice); 
  strcat(buf,buf2);

  for (int i=0; i<VQ_COUNT; i++)
    {
      if (vq_set[i])
	{
	  int v = (int)(vq[i]*100+0.5);
	  if (v<0) v = 0;
	  if (v>99) v = 99;
	  char ch = 'f';
	  switch (i)
	    {
	    case VQ_PITCH:       ch = 'b';   break;
	    case VQ_HEADSIZE:    ch = 'h';   break;
	    case VQ_ROUGHNESS:   ch = 'r';   break;
	    case VQ_BREATHINESS: ch = 'y';   break;
	    case VQ_RANGE:       ch = 'f';   break;
	    case VQ_SPEED:       ch = 's';   break;
	    case VQ_VOLUME:      ch = 'v';   volume = v;  break;
	    }
	  sprintf(buf2, "`v%c%d ", ch, v);
	  strcat(buf,buf2);
	}
    }

  sprintf(buf2, "`vv0 it `vv%d ", volume); 
  strcat(buf,buf2);


  if (!rehearsal)
    {
      double now = YARPTime::GetTimeAsSeconds();
      
      state_mutex.Wait();
      if (!speaking && now-last_speaking>1)
	{
	  speaking = 1;
	  last_utterance = now;
	  state_mutex.Post();
	  sayable = eci_say(text,buf);
	}
      else
	{
	  state_mutex.Post();
	  sayable = 0;
	}
    }
  else
    {
      sayable = eci_say(text,buf,1);
    }
  return sayable;
}

