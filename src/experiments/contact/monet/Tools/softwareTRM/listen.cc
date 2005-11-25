
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include <fstream>
using namespace std;

#include "listen.h"

#include "fft.h"

#include "yarpy.h"

#define MAX_LEN 10000

void listen(unsigned char *sample, int len) {
  static ofstream fout("/tmp/snd.txt");
  static ofstream fout2("/tmp/mot.txt");
  static int base_len = len;
  static FFT fft(len);
  static FFT fft2(len/2);
  static FFT fft3(len/4);
  static float input[MAX_LEN];
  static float mag[MAX_LEN];
  static float phase[MAX_LEN];
  assert(len==base_len);
  assert(len<=MAX_LEN);
  float sum = 0;
  int p = 0;
  for (int i=0; i<len; i++) {
    input[i] = sample[i];
    mag[i] = 0;
    phase[i] = 0;
    sum += fabs(sample[i]);
    p = sample[i];
  }
  sum/=len;
  fft.apply(input,mag,phase);

  int out_len = len/2;
  float *out = mag;
  float *out2 = NULL;

  for (int i=0; i<out_len; i++) {
    sum += out[i];
  }
  for (int i=0; i<out_len; i++) {
    out[i] /= sum;
  }

  for (int i=1; i<len/2; i++) {
    fout << mag[i] << " ";
  }

  fft2.apply(mag,input,phase);
  out_len = len/16;
  out = input;
  out2 = phase;

#if 0
  sum = 0;
  for (int i=0; i<out_len; i++) {
    sum += out[i];
  }
  for (int i=0; i<out_len; i++) {
    out[i] /= sum;
  }

  fft3.apply(input,mag,phase);
  out_len = len/8;
  out = mag;
#endif

  for (int i=0; i<out_len; i++) {
    fout << out[i] << " ";
  }
  if (out2!=NULL) {
    for (int i=0; i<out_len; i++) {
      fout << out2[i] << " ";
    }
  }
  fout << endl;
  fout.flush();
  for (int i=0; i<TOTAL_REGIONS; i++) {
    fout2 << getParam(3,i) << " ";
  }
  fout2 << endl;
  fout2.flush();
  //printf("sum %g\n", sum);
}

