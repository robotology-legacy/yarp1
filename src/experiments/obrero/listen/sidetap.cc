/*
* sidetap.h -- classes to processes frames produced by dvgrab
* Copyright (C) 2004 Paul Fitzpatrick <paulfitz@ai.mit.edu>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*
*/

#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "sidetap.h"
#include "fft.h"

#include "signal_output.h"

class BoundedStat {
public:
  float *vals;
  double x;
  double x2;
  int ct;
  int n;
  int index;

  BoundedStat(int size) {
    vals = new float[size];
    n = size;
    ct = 0;
    index = 0;
    x = x2 = 0;
  }
  void add(float v) {
    if (ct>=n) {
      float r = vals[index];
      x -= r;
      x2 -= r*r;
    } else {
      ct++;
    }
    vals[index] = v;
    x += v;
    x2 += v*v;
    index = (index + 1) % n;
  }
  double getMean() {
    if (ct>0) {
      return x/ct;
    }
    return 0;
  }
  double getStd() {
    if (ct>0) {
      double m = getMean();
      double s = x2/ct - m*m;
      if (s<0) s = 0;
      return sqrt(s);
    }
    return 1;
  }
};



void AddSound(SideTapSound& sound) {
  //static FFT fft;
  static SignalOutput output("/out");
  static FILE *draw = NULL;
  /*
  if (draw==NULL) {
    draw = fopen("/plot/draw","w");
  }
  */
  float q_top = 0;
  float q_total = 0;
  int q_ct = 0;

  static long int index = 0;
  static BoundedStat s(1000);
  static BoundedStat s_long(10000);
  if (sound.GetChannels()>=1) {
    int16_t **channel = sound.GetRawPointer();
    int len = sound.GetLength();
    int16_t *sample = channel[0];
    for (int i=0; i<len; i++) {
      float v = fabs(sample[i]);
      s.add(v);
      s_long.add(v);
      index++;
      double mean = s.getMean();
      double var = s_long.getStd();
      double q = (v-mean)/var;
      if (q<0) q = 0;
      if (fabs(q)>q_top) {
	q_top = fabs(q);
      }
      q_total += q_top;
      q_ct++;
      if (fabs(q)>2.5) {
	//printf("%9ld %9ld %9ld\n",(int)(q*1000),(int)(mean*1000),(int)(mean*1000));

      }
    }
  }

  q_total /= q_ct;
  static long int ct = 0;
  if (draw!=NULL) {
    fprintf(draw,"%ld %g\n",ct,q_total);
    fflush(draw);
  }
  output.add(q_total);
  ct++;
  if (ct>=250) {
    //ct = 0;
    //printf("RESET\n");
    //fclose(draw);
    //draw = fopen("/plot/draw","a");
  }
}


class MySideTap : public SideTap {
public:
  //virtual bool ShouldSupplyImage() { return true; }
  virtual bool ShouldSupplySound() { return true; }
  //virtual bool ShouldRecoverImage() { return true; }
  //virtual bool ShouldSave() { return true; }
  virtual void Apply(SideTapImage& image, SideTapSound& sound);
};

void MySideTap::Apply(SideTapImage& image, SideTapSound& sound) {
  if (sound.IsValid()) {
    //printf("MySideTap::Apply() in sidetap.cc received a sound! %d len\n", sound.GetLength());
    AddSound(sound);
  }
}

SideTap *CreateSideTap() {
  SideTap *tap = new MySideTap;
  assert(tap!=NULL);
  return tap;
}
