
#include <stdio.h>

#include "structs.h"

static TRMData fake_data;
static TRMParameters fake_parameters;

int isRemote() {
  return remote;
}

INPUT *remoteInput(INPUT *input) {
  static int first = 1;
  static INPUT inputs[2];
  int i;
  if (!isRemote()) {
    return input;
  }
  if (first) {
    printf("Remote input procedure initializing\n");
    for (i=0; i<2; i++) {
      inputs[i].previous = inputs[i].next = &inputs[(i+1)%2];
    }
    if (input!=NULL) {
      fake_parameters = input->parameters;
    }
    fake_data.inputHead = &inputs[0];
    fake_data.inputTail = NULL; // invariant is broken for now since not needed
    first = 0;
    input = &inputs[0];
  }
  // must set up input
  input->parameters = fake_parameters;

  return input;
}
