#include <stdio.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/time.h>

#include "conf/tx_data.h"
//YARP communication stuff
#include "YARPTime.h"
#include "YARPPort.h"


#include "YAPOCmsg.h"
#include "conf/K4_data.h"
#include "K4JointPos.h"

#define N_MEIAXES 8
#define N_AXIS 30

#define N_FACEMOTORS 21

#define DEBUG 0 

//there are currently two possible current_drive. 
//SOCIAL means that skin color is most salient
//PLAY means that bright color is most salient
#define SOCIAL 0
#define PLAY 1


//emotion, only calm, happy, sad, fear, and surprised are currently used
#define CALM 0
#define HAPPY 1
#define SAD 2
#define ANGRY 3
#define FEAR 4
#define SURPRISED 5
#define DISGUST 6


//number of iters per loop in counting sum of delta diff
#define N_DELTA_ITER 10


//input port, for receiving target position from vision
YARPInputPortOf<StereoPosData> in_pos;
//input port, for receiving boxes info from vision
YARPInputPortOf<FiveBoxesInARow> in_leftboxes;
YARPInputPortOf<FiveBoxesInARow> in_rightboxes;
YARPInputPortOf<FiveBoxesInARow> in_attnboxes;

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

//fixed positions for different facial expressions
float HAPPY_pos[N_FACEMOTORS] = { 1000.0f, 2000.0f, 18000.0f, -2000.0f, 1000.0f, 18000.0f, 2000.0f, 2000.0f, 0.0f, -160.0f, 160.0f, 0.0f, 500.0f, -200.0f, 200.0f, -500.0f, -370.0f, 370.0f, -370.0f, 370.0f, -10.0f};
float SAD_pos[N_FACEMOTORS]= { 0.0f, 100.0f, 4000.0f, 0.0f, 0.0f, 4000.0f, 100.0f, 0.0f, 0.0f, 160.0f, -200.0f, 0.0f, 1600.0f, 450.0f, -450.0f, -1600.0f, 370.0f, -370.0f, 370.0f, -370.0f, -10.0f};
float CALM_pos[N_FACEMOTORS]= { -1000.0f, 2000.0f, 8150.0f, -6000.0f, -1000.0f, 8150.0f, 2000.0f, -6000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 800.0f, 0.0f, 0.0f, -800.0f, -150.0f, 150.0f, 150.0f, -150.0f, 0.0f};
//float ANGRY_pos[N_FACEMOTORS]= { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float FEAR_pos[N_FACEMOTORS]= { -2000.0f, 2000.0f, 10000.0f, -10000.0f, -2000.0f, 10000.0f, 2000.0f, -10000.0f, -500.0f, 160.0f, -160.0f, 500.0f, 0.0f, -400.0f, 400.0f, 0.0f, 370.0f, -370.0f, 370.0f, -370.0f, -2000.0f};
float SURPRISED_pos[N_FACEMOTORS]= { 0.0f, 8000.0f, 22000.0f, 0.0f, 0.0f, 20000.0f, 8000.0f, 0.0f, 0.0f, -120.0f, 120.0f, 0.0f, 200.0f, -400.0f, 400.0f, -200.0f, 370.0f, -370.0f, -370.0f, 370.0f, -1500.0f};
//float DISGUST_pos[N_FACEMOTORS]= { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};


//utility function to send msg
void SendMessage(){
  outCommandPort.Content() = msg;
  /* pf -- added flag to make Write wait and block if receiver is
   * not ready for message yet */
  outCommandPort.Write(1);
}

/* this function is called by main to register all ports and initialize
   some parameters */
void Initialize(){
  //register ports
  outCommandPort.Register("/YAPOCClient/out/command");
  inPositionPort.Register("/YAPOCClient/in/position");
  in_pos.Register("/YAPOCClient/in/target");
  in_leftboxes.Register("/YAPOCClient/in/leftboxes");
  in_rightboxes.Register("/YAPOCClient/in/rightboxes");
  in_attnboxes.Register("/YAPOCClient/in/attnboxes");

  //initialize newpositions and statusbits
  for (int i = 0; i < N_AXIS; i++){
    newpositions[i] = 0;
    statusbits[i] = OFF;
  }
}

/* this function is called by main to send a message through command port to
   YAPOC to calibrate all motors */
void Calibrate(){
  //send msg CALIBRATE_ALL to YAPOC
  //right now YAPOC does kismet style calibration, simply set encoder to zero
  //only for axis 0,1,2, 4,5,6,7 (no 3!)
  //all the dofs must be set to their 0 position
  //when this is called
  //this just sets the origins of the dofs
  //axis 3 will have to be calibrated by moving to limits??
  msg.CreateCalibrateAllMsg();
  SendMessage();
}

/* this function is used to send position command (in ticks) to one motor */
void MoveOneAxis(short axis, float pos){
  //function for moving only one axis at a time
  //send msg POSITIONCOMMAND to YAPOC with 2 inputs: float[30] containing 
  //requested positions and short[30] containing statusbits
  //YAPOC will only move axes whose statusbits is ON (1) and ignore the ones 
  //that are OFF (0)
  
  statusbits[axis] = ON;
  newpositions[axis] = pos;
  
  //uselimit_flag is set to 1 b/c we're not calibrating
  short uselimit_flag = 1;
  //command in ticks
  msg.CreatePositionCommandMsg(newpositions,statusbits,uselimit_flag);
  SendMessage();
  
  //set status bit back to zero, just in case
  //the other status bits should still be zero
  statusbits[axis] = OFF;
}

/* this function is used to send position command (in ticks) to multiple motors */
void MoveMultipleAxes(int naxes, short *axes, float *pos){
  //function for moving multiple axes at a time
  //naxes = number of axes, axes contains # of axes to be moved, pos contains
  //corresponding requested positions
  //send msg POSITIONCOMMAND to YAPOC with 2 inputs: float[30] containing 
  //requested positions and short[30] containing statusbits
  //YAPOC will only move axes whose statusbits is ON (1) and ignore the ones 
  //that are OFF (0)
  
  //clear all bits first
  for (int i = 0; i < N_AXIS; i++)
    statusbits[i] = OFF;
  
  for (i = 0; i < naxes; i++){
    newpositions[axes[i]] = pos[i];
    statusbits[axes[i]] = ON;
  }

  //make sure to set uselimit_flag to 1 b/c motorcontrol will enforce limits only when uselimit_flag = 1
  short uselimit_flag = 1;
  //command in ticks
  msg.CreatePositionCommandMsg(newpositions,statusbits,uselimit_flag);
  SendMessage();
  
  //set status bit back to zero, just in case
  //the rest of status bits should still be zero
  for ( i = 0; i < naxes; i++){
        statusbits[axes[i]] = OFF;
  }
}
  
/* this function is called by main to generate facial expression. 
it receives an array of 21 face motor positions and an array of 30 motors'
current positions, to be used to give offset to the upper lids depending on
current position of eye tilt */

void FaceExpression(float *face_pos, float *posbuffer){
  short face_axes[21] = {8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,29};
   
   //move upper lids
   float lid_diff = posbuffer[EYE_TILT]/0.28 * 400;

   //Note that position specified for the upper lids are relative to current position
   face_pos[LEFT_UPPER_LID - 8] -= lid_diff;
   face_pos[RIGHT_UPPER_LID - 8] += lid_diff;
      MoveMultipleAxes(21,face_axes,face_pos);
   face_pos[LEFT_UPPER_LID - 8] += lid_diff;
   face_pos[RIGHT_UPPER_LID - 8] -= lid_diff;
}
 


/*a convenient class for sending position commands to multiple motors 
  in radians*/
class PositionCommand
{
public:
  
  PositionCommand(){ClearAll();};
  
  void Set(int axis, float pos_rad){
  statusbits[axis] = ON;
  newpositions[axis]=pos_rad;  
  };
  
  float GetPos(int axis){
  return(newpositions[axis]);
  };

  void ClearAll(){
    int i;
    for(i = 0; i<N_AXIS; i++)
      {  
    statusbits[i] = OFF;
	newpositions[i]=0.0f;
      }
   }

  void Clear(int axis){
  statusbits[axis] = OFF;
  };

void Execute()
{
  if(DEBUG) 
    printf("About to send move to:\n ");
  for (int ii = 0; ii < N_AXIS; ii++)
     if(DEBUG) 
    printf("axis %d pos = %g, status = %d\n",ii,newpositions[ii],statusbits[ii]);
  short uselimit_flag = 1;
  //YAPOC expects requested positions to be in ticks, thus ConvertFromRadian 
  //is used since position is set up in radians in this client code
  msg.CreatePositionCommandMsg(msg.ConvertFromRadian(newpositions,newpositions_rad),statusbits,uselimit_flag);
  if(DEBUG) 
    printf("Sending move to:\n ");
  for ( ii = 0; ii < N_AXIS; ii++){
    if(DEBUG) 
    printf("axis %d pos = %g, status = %d\n",ii,newpositions[ii],statusbits[ii]);
  }
  SendMessage();
  };

private:

float newpositions[N_AXIS];
float newpositions_rad[N_AXIS];
float newvelocity[N_AXIS];
short statusbits[N_AXIS];

};

/* this function is called by main to send a request to YAPOC through command 
   port to change velocity (in radians) of multiple axes */
void ChangeVelocity(int naxes, short *axes, float *newvel){
  //function for setting velocity for multiple axes at a time
  //send msg SET_VELOCITY to YAPOC with 2 inputs: float[30] containing requested
  //velocities and short[30] containing statusbits
  //YAPOC will only set velocity for axes whose statusbit is ON (1) and ignore
  //the ones that are OFF (0)
  float newvelocity_rad[N_AXIS];
  
  for (int i = 0; i < naxes; i++){
    statusbits[axes[i]] = ON;
    newvelocity[axes[i]] = newvel[i];
    if(DEBUG) printf("axis %d statusbits = %d newvel = %f\n",axes[i],statusbits[axes[i]], newvel[i]);
  }
  //YAPOC expects requested velocities to be in ticks, thus ConvertFromRadian is
  //used since velocity is set up in radians in this client code
  msg.CreateSetVelocityMsg(msg.ConvertFromRadian(newvelocity, newvelocity_rad),statusbits);
  SendMessage();
  
  //set status bit back to zero, just in case
  //the rest of status bits should still be zero
  for ( i = 0; i < naxes; i++){
    statusbits[axes[i]] = OFF;
  }
  
  
}

/* this function is called by main to read from input port, translate content 
from ticks to radians, and copy to posbuffer array */
void ReadPosition(float *posbuffer){
  //function for reading position of all 30 dofs 
  //read a K4JointPos object sent by YAPOC into input port (inPositionPort)
  //this K4JointPos object contains an element: positions (float[30]) which 
  //encodes motor position in ticks
  //ConvertToRadians is used to convert each value in positions into radians
  //these radian values are then copied into posbuffer
  inPositionPort.Read(1);
  pos = inPositionPort.Content();
  pos.ConvertToRadians();
  for (int i = 0; i < N_AXIS; i++)
    posbuffer[i] = pos.positions[i];  
}

//useful constants
float pi = 3.14159;
float radians_per_degree = (2.0f*pi)/360.0f;
 

/* YAPOCClient main function */
void main(){

  Initialize();

  
  //wait until at least one client is connected to output port 
  while (outCommandPort.IsReceiving() <= 0){
    YARPTime::DelayInSeconds(1);
  }
  

  //change velocity
  //note that this can also be done by changing default velocities in the 
  //configuration file
  short axes[] = {NECK_TILT,LEFTEYE_PAN,RIGHTEYE_PAN,EYE_TILT,NECK_PAN, HEAD_ROLL, HEAD_TILT, NECK_CRANE};
  float newvel[8] = {0.08, 0.5, 0.5, 0.3, 0.3, 0.2, 0.2, 0.5};
  ChangeVelocity(8,axes,newvel);

  //time variables for regulating movement frequency
  time_t curtime;
  time(&curtime);
  time_t lastmove_neckpan = curtime;
  time_t lastmove_headtilt = curtime;
  time_t lastmove_headroll = curtime;
  time_t lastmove_expression = curtime;
  time_t lastmove_drive = curtime;
  time_t lastmove_neckcrane = curtime;
  time_t lastmove_basepan = curtime;
  time_t lastmove_lowerlid = curtime;
  

  //convert pixel distance to pan/tilt radians
  float pan_radians_per_pixel = -0.5f * radians_per_degree;
  float tilt_radians_per_pixel = -0.5f * radians_per_degree;
 
  float posbuffer[N_AXIS];

  PositionCommand eye_com, neck_pan_com, base_pan_com, head_tilt_com, head_roll_com, neck_crane_com;
  
  int first = 1;
  int current_emotion = CALM; 
  int modify_emotion = 0;
  int current_drive;

  //initially randomly choose current drive, 50% SOCIAL 50% PLAY
  if (rand() > 16383)
    current_drive = SOCIAL;
  else
    current_drive = PLAY;
  
  printf("current drive is %d\n",current_drive);
  
  
  FiveBoxesInARow target_leftboxes, target_rightboxes;
  int total_pixels, total_sal, last_total_sal = 0, sum_delta_sal = 0, delta_iter = 0;
  
  //continuous loop
  while (1){

    //read position from encoder
    ReadPosition(posbuffer);
    if(DEBUG) printf("Read position:");
    for (int j = 0; j < N_MEIAXES; j++)
      if(DEBUG) printf(" %f ",posbuffer[j]);
    if(DEBUG) printf("\n");
    
    //read target pos, produced by vision modules
    in_pos.Read();
    StereoPosData& target_pos = in_pos.Content();
    if(DEBUG) printf("The position as of this time is %4g,%4g  %4g,%4g, %d :",
	     target_pos.xl, target_pos.yl,
	     target_pos.xr, target_pos.yr, target_pos.valid);
    
    
    modify_emotion = 0;

    //if current drive is SOCIAL, read boxes from skin detectors
    if (current_drive == SOCIAL){
      in_leftboxes.Read();
      in_rightboxes.Read();
    }
    else {
      //if current drive is PLAY, read boxes from color detectors
      in_attnboxes.Read();
    }
    
    if (current_drive == SOCIAL){
      target_leftboxes = in_leftboxes.Content();
      target_rightboxes = in_rightboxes.Content();
      
      //calculate total pixels from all detected boxes
      total_pixels = target_leftboxes.box1.total_pixels+target_leftboxes.box2.total_pixels+
	target_leftboxes.box3.total_pixels+target_leftboxes.box4.total_pixels+target_leftboxes.box5.total_pixels
	+ target_rightboxes.box1.total_pixels+target_rightboxes.box2.total_pixels+
	target_rightboxes.box3.total_pixels+target_rightboxes.box4.total_pixels+target_rightboxes.box5.total_pixels;
      total_sal = target_leftboxes.box1.total_sal+target_leftboxes.box2.total_sal+
	target_leftboxes.box3.total_sal+target_leftboxes.box4.total_sal+target_leftboxes.box5.total_sal
	+ target_rightboxes.box1.total_sal+target_rightboxes.box2.total_sal+
	target_rightboxes.box3.total_sal+target_rightboxes.box4.total_sal+target_rightboxes.box5.total_sal;
      
      printf("Total pixels = %d  saliency = %d \n",total_pixels, total_sal);
      
    }
    else {
      target_leftboxes = in_attnboxes.Content();
      
      //calculate total pixels from all detected boxes
       total_pixels = target_leftboxes.box1.total_pixels+target_leftboxes.box2.total_pixels+
	      target_leftboxes.box3.total_pixels+target_leftboxes.box4.total_pixels+target_leftboxes.box5.total_pixels;

       total_sal = target_leftboxes.box1.total_sal+target_leftboxes.box2.total_sal+
	        target_leftboxes.box3.total_sal+target_leftboxes.box4.total_sal+target_leftboxes.box5.total_sal;
      
       printf("Total pixels = %d  saliency = %d \n",total_pixels, total_sal);
      
    }
    
    double delta_pan_pixels;
    double delta_tilt_pixels;
    
    //translate target pos to pan and tilt delta pixels
    delta_pan_pixels = target_pos.xl - 63;
    delta_tilt_pixels = target_pos.yl - 63;

    if(DEBUG)
      printf("%4g,%4g\n", delta_pan_pixels, delta_tilt_pixels);
    
    //at startup, generate CALM expression
    if (first == 1){
      FaceExpression(CALM_pos,posbuffer);
      first = 0;
    }

    //move eye pan and tilt motors according to target pos
    eye_com.ClearAll();
    float new_eye_pos_pan;
    new_eye_pos_pan = posbuffer[LEFTEYE_PAN] + 
      (pan_radians_per_pixel * delta_pan_pixels); 
    float new_eye_pos_tilt;
    new_eye_pos_tilt = posbuffer[EYE_TILT] +
      (tilt_radians_per_pixel * delta_tilt_pixels);
    eye_com.Set(LEFTEYE_PAN, new_eye_pos_pan);
    eye_com.Set(RIGHTEYE_PAN, -new_eye_pos_pan);
    eye_com.Set(EYE_TILT, new_eye_pos_tilt);
    eye_com.Execute();
    
    //get current time
    time(&curtime);   
 
    /*if total saliency (total pixels of all boxes) haven't changed much in 
    the last 20 iterations and total saliency jumps by more than 10000, 
    generate surprised expression */
    if (delta_iter > 20 && abs (last_total_sal - total_sal) > 10000){
      printf("--> Surprised\n");
      FaceExpression(SURPRISED_pos, posbuffer);
      modify_emotion = 1;
      current_emotion = SURPRISED;
      lastmove_expression = curtime;
      
      short axis[1];
      axis[0] = NECK_TILT;
      float newvelo[1];
      newvelo[0] = 0.16f;
      
      ChangeVelocity(1,axis,newvelo);
      
      head_tilt_com.ClearAll();
      head_tilt_com.Set(NECK_TILT, posbuffer[NECK_TILT] + 0.15f);
      
      head_tilt_com.Execute();
      
      
      newvelo[0]= 0.08f;
      ChangeVelocity(1,axis,newvelo);

    }
    else {
      //if total saliency > threshold and current emotion is not FEAR, generate
      //fear expression
      if (total_sal > 550000 && current_emotion != FEAR){
	printf("--> Fear\n");
	FaceExpression(FEAR_pos, posbuffer);

	modify_emotion = 1;
	current_emotion = FEAR;
	lastmove_expression = curtime;

	
	short axis[1];
	axis[0] = NECK_TILT;
	float newvelo[1];
	newvelo[0] = 0.25f;
	
	ChangeVelocity(1,axis,newvelo);
	
	head_tilt_com.ClearAll();
	head_tilt_com.Set(NECK_TILT, posbuffer[NECK_TILT] + 0.3f);
	
	head_tilt_com.Execute();
	
	newvelo[0]= 0.08f;
	ChangeVelocity(1,axis,newvelo);
      }
      
      
    }
    
    //if change in total saliency is less than threshold, increment delta_iter
    if (abs(total_sal - last_total_sal) < 5000){
      delta_iter++;
    }
    //otherwise, reset to 0
    else
      delta_iter = 0;
    

    //if the last expression change occurs more than 3 secs ago
    if (curtime - lastmove_expression > 3){
      //if current emotion is surprised, become happy
      if (current_emotion == SURPRISED){
	printf("--> Happy\n");
	FaceExpression(HAPPY_pos, posbuffer);
	modify_emotion = 1;
	current_emotion = HAPPY;
	
	//tilt neck forward
	head_tilt_com.ClearAll();
	head_tilt_com.Set(NECK_TILT, posbuffer[NECK_TILT] - 0.08f);	
	head_tilt_com.Execute();
      }
      //if current emotion is fear, become sad
      else if (current_emotion == FEAR){ 
	printf("--> Sad\n");
	FaceExpression(SAD_pos,posbuffer);
	modify_emotion = 1;
	current_emotion = SAD;
	
	//tilt neck forward
	head_tilt_com.ClearAll();
	head_tilt_com.Set(NECK_TILT, posbuffer[NECK_TILT] - 0.08f);
	head_tilt_com.Execute();
	
      }	
      else
	{
	//if total saliency is low and not currently sad, about 50% chance of
	  //becoming sad
	if (total_sal < 25000 && current_emotion != SAD){
	  if (rand() > 16000){
	    printf("--> Sad\n");
	    FaceExpression(SAD_pos,posbuffer);
	    modify_emotion = 1;
	    current_emotion = SAD;
	    
	    head_tilt_com.ClearAll();
	    head_tilt_com.Set(NECK_TILT, posbuffer[NECK_TILT] - 0.08f);
	    
	    head_tilt_com.Execute();
	    
	  }
	  // else do nothing
	}
	
	//if total saliency is high, either become calm or happy
	if (total_sal > 60000){
	  if (rand() > 16000){
	    printf("--> Calm");
	    FaceExpression(CALM_pos,posbuffer);
	    modify_emotion = 1;
	    current_emotion = CALM;
	    
	  }
	  else {
	    FaceExpression(HAPPY_pos,posbuffer);
	    modify_emotion = 1; current_emotion = HAPPY;
	    
	    head_tilt_com.ClearAll();
	    head_tilt_com.Set(NECK_TILT, posbuffer[NECK_TILT] + 0.08f);
	    
	    head_tilt_com.Execute();
	  }
	}
      }
      lastmove_expression = curtime;

    }
    
    last_total_sal = total_sal;
    

    //Upper eyelids are adjusted during face expression, so if there's no
    //change in face expression, we need to adjust eyelids here.
    if (modify_emotion == 0){
      //move eyelids
      float uleftlid_diff, urightlid_diff;
      
      if (current_emotion == HAPPY){
	uleftlid_diff = posbuffer[EYE_TILT]/0.28 * 400 + HAPPY_pos[LEFT_UPPER_LID - 8];
	urightlid_diff = - (posbuffer[EYE_TILT]/0.28 * 400) + HAPPY_pos[RIGHT_UPPER_LID - 8];
      }
      if (current_emotion == SAD){
	uleftlid_diff = posbuffer[EYE_TILT]/0.28 * 400 + SAD_pos[LEFT_UPPER_LID - 8];
	urightlid_diff = - (posbuffer[EYE_TILT]/0.28 * 400) + SAD_pos[RIGHT_UPPER_LID - 8];
      }
	if (current_emotion == SURPRISED){
	  uleftlid_diff = posbuffer[EYE_TILT]/0.28 * 400 + SURPRISED_pos[LEFT_UPPER_LID - 8];
	  urightlid_diff = - (posbuffer[EYE_TILT]/0.28 * 400) + SURPRISED_pos[RIGHT_UPPER_LID - 8];
	}
      if (current_emotion == FEAR){
	uleftlid_diff = posbuffer[EYE_TILT]/0.28 * 400 + FEAR_pos[LEFT_UPPER_LID - 8];
	urightlid_diff = - (posbuffer[EYE_TILT]/0.28 * 400) + FEAR_pos[RIGHT_UPPER_LID - 8];
      }
      if (current_emotion == CALM){
	uleftlid_diff = posbuffer[EYE_TILT]/0.28 * 400 + CALM_pos[LEFT_UPPER_LID - 8];
	urightlid_diff = - (posbuffer[EYE_TILT]/0.28 * 400) + CALM_pos[RIGHT_UPPER_LID - 8];
      }
      
      MoveOneAxis(LEFT_UPPER_LID, -uleftlid_diff);
        MoveOneAxis(RIGHT_UPPER_LID, -urightlid_diff);
      
    }
     
     //if left eye pan is beyond threshold & the last neckpan move is more than
     //1 sec ago, move neckpan accordingly
     float neck_eye_pan_diff;
     neck_eye_pan_diff = posbuffer[LEFTEYE_PAN];
     if (curtime - lastmove_neckpan > 1 && (neck_eye_pan_diff > 0.16f || neck_eye_pan_diff < -0.16f))
      {
	neck_pan_com.ClearAll();
	neck_pan_com.Set(NECK_PAN, 
			 posbuffer[NECK_PAN] + (-1.25f *neck_eye_pan_diff));
	neck_pan_com.Execute();
	lastmove_neckpan = curtime;
    
      }
     
  
     //if neck pan is beyond threshold & the last base pan move is more than 
     //2 secs ago, move base_pan accordingly
     float neck_base_pan_diff = posbuffer[NECK_PAN];
     if (curtime - lastmove_basepan > 2 && (neck_base_pan_diff > 0.6f || neck_base_pan_diff < -0.6f)){ 
       //printf("neck_base_pan_diff = %g,  ",neck_base_pan_diff);
       base_pan_com.ClearAll();
       base_pan_com.Set(BASE_PAN, posbuffer[BASE_PAN] + (-0.4f * neck_base_pan_diff));
       //printf("currently at %g , send base pan to %g \n",posbuffer[BASE_PAN], posbuffer[BASE_PAN] + (-1.0f * neck_base_pan_diff));
       base_pan_com.Execute();
       lastmove_basepan = curtime;
  }

     
     //if eye tilt is beyond threshold and the last head tilt movement happens //in more than 2 sec ago, move head and neck tilt accordingly
     float head_eye_tilt_diff, head_roll_tilt_diff;
     head_eye_tilt_diff = posbuffer[EYE_TILT];
     if (curtime - lastmove_headtilt > 1 && (head_eye_tilt_diff > 0.16f || head_eye_tilt_diff < -0.16f))
    {

      head_tilt_com.ClearAll();
      head_tilt_com.Set(HEAD_TILT,
			posbuffer[HEAD_TILT] +
			 head_eye_tilt_diff);
      
      //we know posbuffer[HEAD_TILT] is always >= 0
      if (head_eye_tilt_diff < 0 && posbuffer[HEAD_TILT] < -head_eye_tilt_diff){
	head_roll_tilt_diff = -posbuffer[HEAD_TILT];
      }
      else
	head_roll_tilt_diff = head_eye_tilt_diff;
      
      
      head_tilt_com.Set(HEAD_ROLL, posbuffer[HEAD_ROLL] + (-0.7f * head_roll_tilt_diff));

      head_tilt_com.Set(NECK_TILT, posbuffer[NECK_TILT] + (0.4f * head_eye_tilt_diff));      
      
      head_tilt_com.Execute();

      lastmove_headtilt = curtime;

    }

  //if head tilt is beyond threshold and the last neck crane move is more than
     //2 secs ago, move neck crane accordingly.     
  float neck_tilt_crane_diff = posbuffer[NECK_TILT];
  float head_tilt_crane_diff = posbuffer[HEAD_TILT];
  if (curtime - lastmove_neckcrane > 2 && (head_tilt_crane_diff > 0.2f || head_tilt_crane_diff < 0.05f))//neck_tilt_crane_diff < -0.08f))
    {
      neck_crane_com.ClearAll();
      if (head_tilt_crane_diff > 0.2f)
	neck_crane_com.Set(NECK_CRANE, posbuffer[NECK_CRANE] + (-2.0f * head_tilt_crane_diff));
      if (neck_tilt_crane_diff < -0.08f)
	neck_crane_com.Set(NECK_CRANE, posbuffer[NECK_CRANE] + (-3.0f * neck_tilt_crane_diff));
      neck_crane_com.Execute();
      lastmove_neckcrane = curtime;
      
    }
 
  //randomly switch drive
  if ( lastmove_drive - curtime > 10 && rand() > 5000){
    if (current_drive == SOCIAL){
      current_drive = PLAY;
      printf("current drive is PLAY\n");
    }
    else{
      current_drive = SOCIAL;
      printf("current drive is SOCIAL\n");
    }
    lastmove_drive = curtime;
  }
     
     
}   
   
}
