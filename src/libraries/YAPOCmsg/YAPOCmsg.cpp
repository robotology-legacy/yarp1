/* YAPOC-msg functions */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "YAPOCmsg.h"

#define DEBUG 0

//temporary for debugging
FILE *msg_outfp = fopen("/home/lijin/output/msg_out","wb");

YAPOCMessage::YAPOCMessage(){
  ReadConvertParams(CONVERT_FILENAME);
}

YAPOCMessage::~YAPOCMessage(){
  
}

void YAPOCMessage::SetParamsArg(float *pos, short* bits){
  for (int i = 0; i < MAXVIRTUALAXES; i++){
    params[i] = pos[i];
    statusbits[i] = bits[i];
  }
}
void YAPOCMessage::SetGainsArg(short *g){
  
  for (int i = 0; i < MAXVIRTUALAXES; i++){ 
       gains[i] = g[i];

  }
}

void YAPOCMessage::CreateCalibrateEachMsg(short axis){
  type = CALIBRATE_EACH;
  axisvnumber = axis;
}
void YAPOCMessage::CreateCalibrateAllMsg(){
  type = CALIBRATE_ALL;
}

void YAPOCMessage::CreatePositionCommandMsg(float *pos, short *bits, short ulim){
  type = POSITIONCOMMAND;
  uselimit_flag = ulim;
  int int_ulim = (int)ulim;
  
  SetParamsArg(pos,bits);
}

void YAPOCMessage::CreateMEIDResetMsg(){
  type = MEID_RESET;
}

void YAPOCMessage::CreateZeroEncoderMsg(short axis){
 type = ZERO_ENCODER;
 axisvnumber = axis;
}

void YAPOCMessage::CreateMEIDCloseMsg(){
  type = MEID_CLOSE;
}
void YAPOCMessage::CreatePICResetMsg(){
  type = PIC_RESET;
}

void YAPOCMessage::CreateSetPICGainMsg(short *gains){
  type = SET_PICGAIN;
  SetGainsArg(gains);
}

void YAPOCMessage::CreateSetVelocityMsg(float *vels, short *bits){
  type = SET_VELOCITY;
  SetParamsArg(vels,bits);
}

void YAPOCMessage::ReadConvertParams(const char * filename){
  FILE *fp = fopen(filename, "r");
  assert(fp != NULL);
  if (DEBUG) printf("convert params: \n");
  for (int i = 0; i < MAXVIRTUALAXES; i++){
    fscanf (fp, "%lf ", &m_etoangles[i]);
    if (DEBUG) printf("i = %d conv = %f ", i, m_etoangles[i]);
  }
  fclose(fp);
  if (DEBUG) printf("\n");
}

float * YAPOCMessage::ConvertFromRadian(float *vals, float *newvals){
  
  for (int i = 0; i < MAXVIRTUALAXES; i++){
    newvals[i] = vals[i]/m_etoangles[i];
     double oldval = (double)vals[i];
     double newval = (double)newvals[i];
     fprintf(msg_outfp, "i = %d m_etoangles = %f Convert %f to %f\n",i,m_etoangles[i],oldval,newval);
  }
  return newvals;
}






