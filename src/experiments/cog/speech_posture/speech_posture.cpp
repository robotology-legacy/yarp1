
//
// test msg
//	- Sept 2001.
//
#include <sys/kernel.h>
#include <iostream.h>

#include <VisDMatrix.h>

#include "YARPAll.h"
//#include "YARPImageServices.h"
#include "YARPPort.h"
//#include "YARPImagePort.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPTime.h"
#include "YARPRandomNumber.h"

#include <conf/speech_posture.h>
#include <conf/tx_data.h>
#include <conf/lp_layout.h>
#include <stdlib.h>

YARPOutputPortOf<HeadMessage> out;
YARPSemaphore out_sema(1);

YARPInputPortOf<SpeechPosture> in_posture;

char *my_name = "/yarp/speech";

enum
{
  COMMAND_NULL,
  COMMAND_QUIRK = 'q',
  COMMAND_BOB = 'b',
};

class PostureGenerator : public YARPThread
{
public:
  enum
    {
      POSTURE_NULL,
      POSTURE_QUIRK,
      POSTURE_BOB,
    };
  
  int state;
  int state2;
  YARPSemaphore update_sema;
  long int quirk_setpoint, bob_setpoint;
  double quirk_start;
  double bob_start;
  
  PostureGenerator() : 
     update_sema(1), state(POSTURE_NULL), state2(POSTURE_NULL)
    {
      bob_start = quirk_start = -1;
      bob_setpoint = quirk_setpoint = 0;
    }
  
  void Update(int cmd = -1)
    {
      update_sema.Wait();

      double now = YARPTime::GetTimeAsSeconds();
      
      switch(state)
	{
	case POSTURE_QUIRK:
	  if (now-quirk_start>1.1)
	    {
	      state = POSTURE_NULL;
	    }
	  break;
	}
      switch(state2)
	{
	case POSTURE_BOB:
	  if (now-bob_start>1.1)
	    {
	      state2 = POSTURE_NULL;
	    }
	  break;
	};
      
      switch(cmd)
	{
	case COMMAND_QUIRK:
	  quirk_start = now;
	  state = POSTURE_QUIRK;
	  if (fabs(quirk_setpoint)<0.01)
	    {
	      quirk_setpoint = YARPRandom::Rand(10,15);
	      if (YARPRandom::Rand(0,1))
		{
		  quirk_setpoint = -quirk_setpoint;
		}
	    }
	  break;
	case COMMAND_BOB:
	  bob_start = now;
	  state2 = POSTURE_BOB;
	  if (fabs(bob_setpoint)<0.01)
	    {
	      bob_setpoint = -YARPRandom::Rand(3000,6000);
	      /*
	      if (YARPRandom::Rand(0,1))
		{
		  bob_setpoint = -bob_setpoint;
		}
	       */
	    }
	  break;
	case COMMAND_NULL:
	  state = state2 = POSTURE_NULL;
	  break;
	};
      
      HeadMessage msg;
      
      double j5 = 0, j7 = 0;
      //	  j7 --> tilt 1000s
      //	  j5 --> roll 100
      
      if (state == POSTURE_QUIRK)
	{
	  j5 = quirk_setpoint;
	}
      else
	{
	  quirk_setpoint = 0;
	}
      if (state2 == POSTURE_BOB)
	{
	  j7 = bob_setpoint;
	}
      else
	{
	  bob_setpoint = 0;
	}
      
      memset (&msg, 0, sizeof(HeadMessage));
      msg.type = HeadMsgOffset;
      
      if (j5>20) j5=20;
      if (j5<-20) j5 = -20;
      
      msg.j5 = j5;
      msg.j7 = j7;
      
      out_sema.Wait();
      out.Content() = msg;
      out.Write ();
      if (fabs(j5)>0.01 || fabs(j7)>0.01)
	{
	  printf("Sending postural command %g %g\n", j5, j7);
	}
      out_sema.Post();

      update_sema.Post();
    }
  
  virtual void Body()
    {

      while (1)
	{
	  YARPTime::DelayInSeconds(0.25);
	  Update();
	}
    }
} posture_generator;

class PostureReceiver : public YARPThread
{
public:

  virtual void Body()
    {
      in_posture.Register("/yarp/speech/i:pose");
      YARPTime::DelayInSeconds(0.5);
      
      while (1)
	{
	  in_posture.Read();
	  SpeechPosture& p = in_posture.Content();
	  cout << "Got posture " << p.posture << endl;
	  
	  switch (p.posture)
	    {
	    case SPEECH_HEARING_BEGINS:
	      posture_generator.Update(COMMAND_BOB);
	      break;
	    case SPEECH_HEARING_ENDS:
	      break;
	    case SPEECH_HEARING_HUMANLIKE:
	      break;
	    case SPEECH_GENERATION_BEGINS:
	      posture_generator.Update(COMMAND_QUIRK);
	      break;
	    case SPEECH_GENERATION_ENDS:
	      posture_generator.Update(COMMAND_NULL);
	      break;
	    case SPEECH_GENERATION_FAILS:
	      break;
	    }

	}
    }
} posture_receiver;


char out_name[256];

// parses command line.
void ParseCommandLine(int argc, char *argv[])
{
  int i;
  
  for(i=1;i<argc;i++)
    {
      switch(argv[i][0])
	{
	case '-':
	  switch(argv[i][1])
	    {
	    case 'h':
	    case '-':
	      cout << endl;
	      cout << my_name << " argument:" << endl;
	      cout << endl;
	      exit(0);
	      break;
	      
	    default:
	      break;
	    }
	  break;
	case '+':
	  my_name = &argv[i][1];
	  break;
	default:
	  break;
	}
    }
  
  sprintf (out_name, "%s/out", my_name); 
}

void main(int argc, char *argv[])
{
  ParseCommandLine (argc, argv);
  
  HeadMessage msg;
  
  out.Register (out_name);
  posture_generator.Begin();
  posture_receiver.Begin();
  
  char c;
  printf ("ready to send message\n");
  printf ("msg->> ");
  
  for (;;)
    {
      scanf ("%c", &c);
      switch (c)
	{
	case 'm':
	    {
	      double j5, j7;
	      scanf ("%lf%lf", &j5, &j7);
	      memset (&msg, 0, sizeof(HeadMessage));
	      msg.type = HeadMsgOffset;
	      msg.j5 = j5;
	      msg.j7 = j7;
	      out_sema.Wait();
	      out.Content() = msg;
	      out.Write ();
	      out_sema.Post();
	      printf ("msg->> ");
	    }
	  break;
	  
	case 'c':
	  msg.type = 0;
	  out_sema.Wait();
	  out.Content() = msg;
	  out.Write ();
	  out_sema.Post();
	  printf ("msg->> ");
	  break;
	  
	case 'f':
	    {
	      char ch;
	      scanf ("%c", &ch);
	      posture_generator.Update(ch);
	    }
	  break;
	  
	case 'q':
	    {
	      return;
	    }
	  break;
	  
	case 'h':
	case '?':
	  printf ("h,?: this message\n");
	  printf ("m j5 j7: send offset message with arg j5 j7\n");
	  printf ("c: clean last message\n");
	  printf ("q: quit\n");
	  break;
	}
    }
}

