#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <fstream.h>
#include <string.h>

#include "YARPViaVoiceASR.h"
#include "YARPThread.h"
#include "YARPTime.h"
#include "YARPPort.h"
#include "YARPSemaphore.h"

#include "acoustic_msg.h"

YARPInputPortOf<GenAnnotation> in_annotation;
YARPSemaphore annotation_mutex(1);
char annotation_text[1000];
double annotation_time = -10000;

time_t rawtime;

void AdvanceTime()
{
  time ( &rawtime );
}

void CreateName(char *filename, int buffer_length, const char *prefix,
		const char *postfix)
{
  assert(buffer_length>=200);

  struct tm * timeinfo;
  
  timeinfo = localtime ( &rawtime );
  //printf ( "Current date and time is: %s\n", asctime (timeinfo) );
  timeinfo->tm_year = 2000 + (timeinfo->tm_year % 100);

  sprintf ( filename, "%s%04d%02d%02d%02d%02d%02d%s", 
	    prefix,
	    timeinfo->tm_year,
	    timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour,
	    timeinfo->tm_min, timeinfo->tm_sec,
	    postfix);
}

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
	int annotated = 0;
	char annotation[1000];

	annotation_mutex.Wait();
	if (YARPTime::GetTimeAsSeconds()-annotation_time<5)
	  {
	    strncpy(annotation,annotation_text,sizeof(annotation));
	    annotation_time = -10000;
	    annotated = 1;
	  }
	annotation_mutex.Post();

	if (annotated)
	  {
	    char buf[1000];
	    AdvanceTime();
	    CreateName(buf,sizeof(buf),"/usr/yarp/training/via/cog",".wav");
	    printf("    Saving to %s\n", buf);
	    p.SaveWave(buf);
	    CreateName(buf,sizeof(buf),"/usr/yarp/training/via/cog",".txt");
	    ofstream fout(buf);
	    fout << annotation << endl;
	    fout.close();
	  }
	else
	  {
	    p.SaveWave("/tmp/discard.wav");
	  }
      }
    else
      {
	p.SaveWave("/tmp/discard.wav");
      }
  }
};

class AnnotationThread : public YARPThread
{
public:
  virtual void Body()
  {
    while(1)
      {
	in_annotation.Read();
	printf("Received annotation %s\n", in_annotation.Content().txt);
	annotation_mutex.Wait();
	strncpy(annotation_text,in_annotation.Content().txt,
		sizeof(annotation_text));
	annotation_time = YARPTime::GetTimeAsSeconds();
	annotation_mutex.Post();
      }
  }
} annotation_thread;

void main()
{
  ASR master;
  in_annotation.Register("/exp/acoustic/i:ann");
  YARPTime::DelayInSeconds(0.25);
  annotation_thread.Begin();
  master.Main("kismetgram1.fsg");
}

