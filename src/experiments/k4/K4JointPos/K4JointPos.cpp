/* K4JointPos functions */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "K4JointPos.h"
#define DEBUG 0

//constructor
K4JointPos::K4JointPos(){
  //read conversion parameters from file
  ReadConvertParams(CONVERT_FILENAME);
}

K4JointPos::~K4JointPos(){
  
}

//this functions set individual axis's encoder value 
void K4JointPos::SetPosition(int axis, float newpos){
  positions[axis] = newpos;
  //printf("Set positions[%d] to %f\n",axis,newpos);
}

//this function set encoder values for all axes to be values in newpos array
void K4JointPos::SetPosition(float *newpos){
  for (int i = 0; i < MAXVIRTUALAXES; i++)
    positions[i] = newpos[i];
}

//this function reads conversion parameter from an input file
void K4JointPos::ReadConvertParams(const char * filename){
  FILE *fp = fopen(filename, "r");
  assert(fp != NULL);
  if (DEBUG) printf("convert params: \n");
  for (int i = 0; i < MAXVIRTUALAXES; i++){
    fscanf (fp, "%lf ", &m_etoangles[i]);
    if (DEBUG) printf(" %f ", m_etoangles[i]);
  }
  fclose(fp);
  if (DEBUG) printf("\n");
}

/* this function is to be used by client for converting positions from 
tick to radians */
void K4JointPos::ConvertToRadians(){
  for (int i = 0; i < MAXVIRTUALAXES; i++){
    positions[i] = positions[i] * m_etoangles[i];

	}
}
