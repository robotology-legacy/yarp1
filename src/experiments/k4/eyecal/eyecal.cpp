#include <stdio.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <iostream.h>

#include "conf/tx_data.h"

#include "YARPImage.h"
#include "YARPPort.h"
#include "YARPImagePort.h"
#include "YARPImageFile.h"
#include "YARPTime.h"
#include "YARPImageDraw.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"

#include "YAPOCmsg.h"
#include "conf/K4_data.h"
#include "K4JointPos.h"

#define N_MEIAXES 8
#define N_AXIS 30

#define DEBUG 1

#define for if(1) for

//flag to indicate whether or not to perform auto eyecal
int auto_eyecal;

// some globals
YARPSemaphore state_mutex(1);
float upper_index = -1;
float lower_index = -1;
float right_upper_index = -1;
float right_lower_index = -1;

YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;

YARPInputPortOf<YARPGenericImage> in_right;
YARPOutputPortOf<YARPGenericImage> out_right;

//output port, for sending YAPOCMessage to YAPOC
YARPOutputPortOf<YAPOCMessage> outCommandPort;
//input port, for receiving K4JointPos from YAPOC
YARPInputPortOf<K4JointPos> inPositionPort;

//YAPOCMessage is a class for encoding command message to be sent to YAPOC
//YAPOCMessage can be of different types 
//(see $YARP_ROOT/src/libraries/YAPOCmsg/)
//Relevant msg types: CALIBRATE_ALL, POSITIONCOMMAND, SET_VELOCITY
//Relevant elements: params (float[30]), statusbits(short[30])
//YAPOCMessage has a function ConvertFromRadian(float *vals) which convert each 
//element in vals to radians and return result as a float array
YAPOCMessage msg;

//K4JointPos is a class for encoding motor positions received from YAPOC
//(see $YARP_ROOT/src/libraries/K4JointPos/)
//K4JointPos contains positions(float[30])
//K4JointPos has a function ConvertToRadians() which cpnvert positions (sent by
//YAPOC in ticks) to radians
K4JointPos pos;

//global variables
float newpositions[N_AXIS];
float newvelocity[N_AXIS];
short statusbits[N_AXIS];


//temporary for debugging
FILE *output_fp = fopen("/home/lijin/output/client_out","wb");

//utility function to send msg
void SendMessage(){
  outCommandPort.Content() = msg;
  /* pf -- added flag to make Write wait and block if receiver is
   * not ready for message yet */
  outCommandPort.Write(1);
}

void Initialize(){
  //register ports
  
  //only register these ports if will perform auto eye calibration
  if (auto_eyecal == 1){
    in.Register("/eyecal/i:img");
    out.Register("/eyecal/o:img");
    in_right.Register("/eyecal/right/i:img");
    out_right.Register("/eyecal/right/o:img");
  }
  
  outCommandPort.Register("/eyecal/out/command");
  inPositionPort.Register("/eyecal/in/position");

  //initialize newpositions and statusbits
  for (int i = 0; i < N_AXIS; i++){
    newpositions[i] = 0;
    statusbits[i] = OFF;
  }
}


void MoveOneAxis(short axis, float pos){
  //function for moving only one axis at a time
  //send msg POSITIONCOMMAND to YAPOC with 2 inputs: float[30] containing 
  //requested positions and short[30] containing statusbits
  //YAPOC will only move axes whose statusbits is ON (1) and ignore the ones that
  //are OFF (0)
  
  
  statusbits[axis] = ON;
  newpositions[axis] = pos;
  short uselimit_flag = 0;
  //YAPOC expects positions to be in ticks, thus ConvertFromRadian is used since
  //position is set up in radians in this client code
  // msg.CreatePositionCommandMsg(msg.ConvertFromRadian(newpositions),statusbits);
  //command in ticks
  msg.CreatePositionCommandMsg(newpositions,statusbits,uselimit_flag);
  SendMessage();
  
  //set status bit back to zero, just in case
  //the other status bits should still be zero
  statusbits[axis] = OFF;
}


/* this function is called by main to send a message through command port to
 *    YAPOC to calibrate all motors */
void Calibrate(){
    //send msg CALIBRATE_ALL to YAPOC
    //this simply means that all the dofs are set to their 0 position
    //and set their calibrated flags to 1.
     msg.CreateCalibrateAllMsg();
    SendMessage();
}

void ReadPosition(float *posbuffer){
  //function for reading position of all 30 dofs 
  //read a K4JointPos object sent by YAPOC into input port (inPositionPort)
  //this K4JointPos object contains an element: positions (float[30]) which 
  //encodes motor position in ticks

  inPositionPort.Read(1);
  pos = inPositionPort.Content();
  for (int i = 0; i < N_AXIS; i++)
    posbuffer[i] = pos.positions[i];  
}



//////////////////////////////////////////////////////////////////



void ProcessImage(YARPImageOf<YarpPixelBGR>& src,
		  YARPImageOf<YarpPixelBGR>& dest, int side = 0)
{
  int h = src.GetHeight();
  int w = dest.GetWidth();
  int top_grace = 3;
  int bot_grace = 4;
  int top_index = -1;
  int bot_index = h;
  int filter = 1;
  for (int x=0; x<w; x++)
    {
      YarpPixelBGR pix(255,0,0);
      dest(x,h/2) = pix;
    }
  static YARPImageOf<YarpPixelMono> mono;
  mono.CastCopy(src);
  for (int y=0; y<h-2; y++)
    {
      float total = 0;
      float mean = 0;
      float flat = 0;
      for (int x=0; x<w-1; x++)
	{
	  float diff = fabs(mono(x,y)-mono(x+2,y+2));
	  total += mono(x,y);
	  if (diff>=15)
	    {
	      flat++;
	    }
	}
      mean = total/w;
      if (flat<=10 && mean>=50)
	{
	  if (top_grace)
	    {
	      top_index = y;
	    }
	  if (!filter)
	    {
	      for (int x=0; x<w; x++)
		{
		  dest(x,y).g = 255;
		}
	    }
	}
      else
	{
	  if (top_grace>0) top_grace--;
	}
    }

  for (int y=h-1; y>=1; y--)
    {
      float total = 0;
      float mean = 0;
      float flat = 0;
      for (int x=0; x<w-1; x++)
	{
	  float diff = fabs(mono(x,y)-mono(x+2,y-1));
	  total += diff;
	  if (diff>=15)
	    {
	      flat++;
	    }
	}
      mean = total/w;
      if (flat<=10)
	{
	  if (bot_grace)
	    {
	      bot_index = y;
	    }
	  if (!filter)
	    {
	      for (int x=0; x<w; x++)
		{
		  dest(x,y).g = 255;
		}
	    }
	}
      else
	{
	  if (bot_grace>0) bot_grace--;
	}
    }
  
  
  for (int y=0; y<=top_index; y++)
    {
      for (int x=0; x<w; x++)
	{
	  dest(x,y).r = 255;
	}
    }

  for (int y=bot_index; y<h; y++)
    {
      for (int x=0; x<w; x++)
	{
	  dest(x,y).r = 255;
	}
    }
  
  int vis = 0;
  if (bot_index<h-2)
    {
      vis++;
    }
  else
    {
      bot_index = -1;
    }

  if (top_index>2)
    {
      vis++;
    }
  else
    {
      top_index = -1;
    }

  state_mutex.Wait();
  if (side==0)
    {
      lower_index = bot_index;
      upper_index = top_index;
    }
  else
    {
      right_lower_index = bot_index;
      right_upper_index = top_index;
    }
  state_mutex.Post();

}

class Viewer : public YARPThread
{
public:
  virtual void Body()
    {
      while(1)
	{
	  in.Read();
	  YARPGenericImage& inImg = in.Content();
	  YARPGenericImage& outImg = out.Content(); // Lasts until next Write()
	  outImg.PeerCopy(inImg);
	  YARPImageOf<YarpPixelBGR> src, dest;
	  src.Refer(inImg);
	  dest.Refer(outImg);
	  ProcessImage(src,dest);
	  out.Write();
	}
    }
} view_thread;

class ViewerRight : public YARPThread
{
public:
  virtual void Body()
    {
      while(1)
	{
	  in_right.Read();
	  YARPGenericImage& inImg = in_right.Content();
	  YARPGenericImage& outImg = out_right.Content(); // Lasts until next Write()
	  outImg.PeerCopy(inImg);
	  YARPImageOf<YarpPixelBGR> src, dest;
	  src.Refer(inImg);
	  dest.Refer(outImg);
	  ProcessImage(src,dest,1);
	  out_right.Write();
	}
    }
} view_right_thread;



 /* this function parses arguments passed into main function.
  *   * if argument is '-auto_eyecal no', then auto_eyecal will be set to 0,
  *   * meaning, auto_eyecal will not be performed.
  *   * otherwise, if argument is '-auto_eyecal yes' or anything else, i.e.
  *   * empty or simply junk, auto_eyecal will be set default value = 1. */

 void ParseCommandLine(int argc, char *argv[])
{
     if (argc <= 1){
       //assume default setting: use auto_eyecal
       auto_eyecal = 1;
     }
  else {
    if (strcmp(argv[1],"-auto_eyecal") == 0){
      if (strcmp(argv[2],"no") == 0){
	auto_eyecal = 0;
      }
      else
	auto_eyecal = 1;
      
    }
    else
      auto_eyecal = 1;
  }
  if (auto_eyecal == 1)
    printf("Performing automatic eye calibration\n");
  if (auto_eyecal == 0)
    printf("Not performing automatic eye calibration\n");
}

   ////////////////////////////////////////////////////////////////////////
 
//Main
void main(int argc, char *argv[]){

  auto_eyecal = 0;
  ParseCommandLine(argc,argv);
  
  Initialize();

//wait until at least one client is connect to output port
  while (outCommandPort.IsReceiving() <= 0){
    YARPTime::DelayInSeconds(1);
  }
  YARPTime::DelayInSeconds(4);
  
  //if auto_eyecal is 1, perform automatic eye calibration
  if (auto_eyecal == 1){
  view_thread.Begin();
  view_right_thread.Begin();
  delay(2000);
  float posbuffer[N_AXIS];
  int direction = 0;
  float start = 0;
  int lower_is_visible = 0;
  int lower_right_is_visible = 0;
  int upper_is_visible = 0;
  int phase = 0;

  int not_done = 1;
  float last_left_lowerlid_pos = 0.0f, last_right_lowerlid_pos = 0.0f;
  while (not_done == 1){
    //read position from encoder
    ReadPosition(posbuffer);
    state_mutex.Wait();
    float lower = lower_index;
    float upper = upper_index;
    float right_upper = right_upper_index;
    float right_lower = right_lower_index;
    state_mutex.Post();
    lower_is_visible = (lower>-0.5);
    lower_right_is_visible = (right_lower>-0.5);
    upper_is_visible = (upper>-0.5);
    if (lower_is_visible)
      {
	lower = 128-lower;
      }
    if (right_lower>-0.5)
      {
	right_lower = 128-right_lower;
      }
    if(DEBUG) printf("%3d(%d) --", phase, lower_is_visible);
    float lu_pos = posbuffer[LEFT_UPPER_LID];
    float ll_pos = posbuffer[LEFT_LOWER_LID];
    float ru_pos = posbuffer[RIGHT_UPPER_LID];
    float rl_pos = posbuffer[RIGHT_LOWER_LID];
    float tilt_pos = posbuffer[EYE_TILT];
    float tilt_ticks = tilt_pos;
    if(DEBUG) printf(" LU:%+5.2f  LL:%+5.2f  RU:%+5.2f  RL:%+5.2f  TT:%+2.3g",
		     lu_pos, ll_pos, ru_pos, rl_pos,
		     tilt_pos);
    if(DEBUG) printf("\n");
    if (direction == 0)
      {
      }

    static int tilted = 0;

    static float vis = lower;
    static int changes = 0;
    static float min_vis = lower;
    static float pvis = lower;
    static int oscillations = 0;
    static int last_dir = 1;
    static tilt_target = 0;
    float es = 0.05;
    
    switch (phase)
      {
      case 0:
	  {
	    float tilt = tilt_ticks-2000;
	    if (!lower_is_visible)
	      {
		MoveOneAxis(EYE_TILT, tilt);
	      }
	    /*
	    if (ll_pos>0)
	      {
		direction = -1;
	      }
	    else
	      {
		direction = +1;
	      }
	    */ 
	    direction = -1;
	    start = ll_pos;

	    if (lower_is_visible)
	      {
		phase = 10;
	      }
	    else
	      {
		phase = 1;
	      }
	  }
	break;
	
      case 1:  // Move lower lid until it becomes visible, if possible
	  {
	    float curr = ll_pos;
	    if (fabs(curr-start)>60000*es)
	      {
		phase = 2;
	      }
	    else
	      {
		if (!lower_is_visible)
		  {
		    float lid = posbuffer[LEFT_LOWER_LID]-10000*direction*es;
		    MoveOneAxis(LEFT_LOWER_LID, lid);
		  }
		else
		  {
		    vis = pvis = min_vis = lower;
		    changes = 0;
		    phase = 10;
		  }
	      }
	  }
	break;
	
      case 2:
	  {
	    if (tilted<3)
	      {
		float tilt = tilt_ticks-2000;
		MoveOneAxis(EYE_TILT, tilt);
		phase = 0;
		tilted++;
	      }
	    printf("FAILED\n");
	  }
	break;
	
      case 10:  // Lower lid is visible; now move it for max visibility
	  {
	    if (lower_is_visible)
	      {
		if (lower<pvis)
		  {
		    direction = -direction;
		    changes++;
		    if (changes>=2)
		      {
			phase = 20;
		      }
		  }
		float lid = posbuffer[LEFT_LOWER_LID]-1000*direction*es;
		MoveOneAxis(LEFT_LOWER_LID, lid);
		if (lower>vis)
		  {
		    vis = lower;
		  }
		pvis = lower;
	      }
	    else
	      {
		phase = 20;
		oscillations = 0;
	      }
	  }
	break;
	
      case 20:  // now move tilt to consistent location
	  {
	    int next = 21;
	    int setpoint = 20;
	    if (oscillations>3)
	      {
		phase = next;
	      }
	    else
	      {
		if (lower<setpoint)
		  {
		    if (last_dir != 1) oscillations++;
		    last_dir = 1;
		    float tilt = tilt_ticks-100;
		    MoveOneAxis(EYE_TILT, tilt);
		    printf("*** DOWN ***\n");
		  }
		else if (lower>setpoint+1)
		  {
		    if (last_dir != -1) oscillations++;
		    last_dir = -1;
		    float tilt = tilt_ticks+100;
		    MoveOneAxis(EYE_TILT, tilt);
		    printf("*** UP ***\n");
		  }
		else
		  {
		    phase = next;
		    printf("*** -- OVER -- ***\n");
		  }
	      }
	  }
	break;
	
      case 21: // now move the lower lid in a given direction until invisible
	  {
	    tilt_target = tilt_ticks;
	    if (lower_is_visible)
	      {
		float lid = posbuffer[LEFT_LOWER_LID]-1000*es;
		MoveOneAxis(LEFT_LOWER_LID, lid);
	      }
	    else
	      {
		phase = 30;
	      }
	  }
	break;

      case 30: // now look up!
	  {
	    float tilt = tilt_ticks+2000;
	    MoveOneAxis(EYE_TILT, tilt);
	    oscillations = 0;
	    phase = 31;

	    last_dir = 0;
	    oscillations = 0;
	  }
	break;
	
      case 31:  // deal with upper eyelid
	  {
	    int setpoint = 20;
	    if (oscillations>3)
	      {
		phase = 40;
	      }
	    else
	      {
		if (upper<setpoint)
		  {
		    if (last_dir != 1) oscillations++;
		    last_dir = 1;
		    float lid = lu_pos+50;
		    MoveOneAxis(LEFT_UPPER_LID, lid);
		    printf("[[[ DOWN ]]]\n");
		  }
		else if (upper>setpoint+1)
		  {
		    if (last_dir != -1) oscillations++;
		    last_dir = -1;
		    float lid = lu_pos-50;
		    MoveOneAxis(LEFT_UPPER_LID, lid);
		    printf("[[[ UP ]]]\n");
		  }
		else
		  {
		    phase = 40;
		    printf("[[[ -- OVER -- ]]]\n");
		  }
	      }
	  }
	break;

      case 40:
	  {
	    printf("SUCCEEDED ON LEFT\n");
	    phase = 50;
	    last_dir = 0;
	    oscillations = 0;
	  }
	break;

      case 50: // try for right upper lid
	  {
	    int setpoint = 20;
	    if (oscillations>3)
	      {
		phase = 60;
	      }
	    else
	      {
		if (right_upper<setpoint)
		  {
		    if (last_dir != 1) oscillations++;
		    last_dir = 1;
		    float lid = ru_pos-50;
		    MoveOneAxis(RIGHT_UPPER_LID, lid);
		    printf("[[[ DOWN ]]]\n");
		  }
		else if (right_upper>setpoint+1)
		  {
		    if (last_dir != -1) oscillations++;
		    last_dir = -1;
		    float lid = ru_pos+50;
		    MoveOneAxis(RIGHT_UPPER_LID, lid);
		    printf("[[[ UP ]]]\n");
		  }
		else
		  {
		    phase = 60;
		    printf("[[[ -- OVER -- ]]]\n");
		  }
	      }
	  }

	break;
	
      case 60: // now look down!
	  {
	    float tilt = tilt_target; //tilt_ticks-3000;
	    MoveOneAxis(EYE_TILT, tilt);
	    oscillations = 0;
	    phase = 61;
	    last_dir = 0;
	    oscillations = 0;
	    vis = pvis = min_vis = right_lower;
	    changes = 0;
	    direction = -1;
	    start = rl_pos;
	  }
	break;

      case 61:  // twiddle right lower lid
	  {
	    float lid = posbuffer[RIGHT_LOWER_LID]-5000*direction*es;
	    if (!lower_right_is_visible)
	      {
		MoveOneAxis(RIGHT_LOWER_LID, lid);
	      }
	    else
	      {
		phase = 62;
	      }
	  }
	break;

      case 62: // now move the lower lid in a given direction until invisible
	  {
	    
	    if (lower_right_is_visible)
	      {
		float lid = posbuffer[RIGHT_LOWER_LID]+1000*es;
		MoveOneAxis(RIGHT_LOWER_LID, lid);
	      }
	    else
	      {
		phase = 63;
	      }
	    
	  }
	break;


      case 63: // now look up!
	  {
	    float tilt = tilt_ticks+3000;
	    MoveOneAxis(EYE_TILT, tilt);
	    phase = 100;
	  }
	break;
	

      case 100:
	  {
	    MoveOneAxis(RIGHT_UPPER_LID, posbuffer[RIGHT_UPPER_LID]+1150);
	    MoveOneAxis(LEFT_UPPER_LID,  posbuffer[LEFT_UPPER_LID]-1150);
	    MoveOneAxis(LEFT_LOWER_LID,  posbuffer[LEFT_LOWER_LID]-2200);
	    MoveOneAxis(RIGHT_LOWER_LID,  posbuffer[RIGHT_LOWER_LID]+2200);
	    phase = 101;
	  }
	break;
	
      case 101:
	  {
	    if (last_left_lowerlid_pos == posbuffer[LEFT_LOWER_LID] &&
		last_right_lowerlid_pos == posbuffer[RIGHT_LOWER_LID]){
	      printf("Succeeded\n");
	      not_done = 0;
	    }
	    last_left_lowerlid_pos = posbuffer[LEFT_LOWER_LID];
	    last_right_lowerlid_pos = posbuffer[RIGHT_LOWER_LID];
	  }
	break;
      
      }
    
  }
  }
  
  //now zero all encoders
  Calibrate();
}














