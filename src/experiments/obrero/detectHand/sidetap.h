
#ifndef YARP_OS_CONFIG
#define YARP_OS_CONFIG LINUX
#endif

#include <yarp/YARPImage.h>

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

#ifndef SIDETAP_INC
#define SIDETAP_INC

#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>

#if 0
class SideTapPixel {
public:
  char r, g, b;
};


class SideTapImage {
private:
  char *buf;
  int w, h;
  SideTapPixel blank;

public:
  SideTapImage() {
    buf = NULL;
    w = h = -1;
    blank.r = 0;  blank.g = 0;  blank.b = 0;
  }

  void Set(char *buf, int w, int h) {
    this->buf = buf;
    this->w = w;
    this->h = h;
  }

  int GetWidth() { return w; }
  int GetHeight() { return h; }
  char *GetRawPointer() { return buf; }
  int IsValid() { return buf!=NULL; }
  
  SideTapPixel& Pixel(int x, int y) {
    return *(((SideTapPixel*)buf) + w*y + x);
  }
  
  SideTapPixel& operator()(int x, int y) {
    return Pixel(x,y);
  }  

  SideTapPixel& GetSafePixel(int x, int y) {
    if (x>=0 && x<w && y>=0 && y<h) {
      return Pixel(x,y);
    }
    return blank;
  }
};
#else
#define SideTapPixel YarpPixelRGB
class SideTapImage : public YARPImageOf<YarpPixelRGB> {
public:
  void Set(char *buf, int w, int h) {
    UncountedRefer(buf,w,h);
  }

  int IsValid() { return GetWidth()!=0; }
};
#endif


class SideTapSound {
private:
  int16_t **buf;
  int channels, len, freq, quantization;

public:
  SideTapSound() {
    buf = NULL;
    channels = len = freq = quantization = 0;
  }

  void Set(int16_t **buf, int channels, int len, 
	   int freq, int quantization) {
    this->buf = buf;
    this->channels = channels;
    this->len = len;
    this->freq = freq;
    this->quantization = quantization;
  }

  int GetChannels() { return channels; }
  int GetLength() { return len; }
  int GetFrequency() { return freq; }
  int GetQuantization() { return quantization; }
  int16_t **GetRawPointer() { return buf; }
  int IsValid() { return buf!=NULL; }
};





class SideTap {
public:
  // should images be unpacked and supplied in call to Apply?
  virtual bool ShouldSupplyImage() { return false; }

  // should sound be unpacked and supplied in call to Apply?
  virtual bool ShouldSupplySound() { return false; }

  // should movies be saved or discarded?
  virtual bool ShouldSave() { return false; }

  // after call to Apply, should image supplied be packed up again?
  // this is useful if you wish to save modified images into movie.
  virtual bool ShouldRecoverImage() { return false; }

  // after call to Apply, should sound supplied be packed up again?
  // this is useful if you wish to save modified sound into movie.
  virtual bool ShouldRecoverSound() { return false; }

  virtual void Apply(SideTapImage& image, SideTapSound& sound) {}
};

// main factory for your custom SideTap object
extern SideTap *CreateSideTap();

#endif
