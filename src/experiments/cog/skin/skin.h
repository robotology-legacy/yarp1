#ifndef _SKIN_H
#define _SKIN_H

#include "vision.h"


class SkinGainSet {
public:
  float transformWeights[6];
  float transformDelta;
  float mask[6];
  
  SkinGainSet()
    {
      transformWeights[0] = 0.21742;     // r
      transformWeights[1] = -0.36386;    // g 
      transformWeights[2] = 0.90572;     // b
      transformWeights[3] = 0.00096756;  // r*r
      transformWeights[4] = -0.00050073; // g*g
      transformWeights[5] = -0.00287;    // b*b
      transformDelta = -50.1255;
      mask[0] = 1.05; // r > M*g
      mask[1] = 2;   // r < M*g
      mask[2] = 0.9; // r > M*g
      mask[3] = 2;   // r < M*b
      mask[4] = 20;  // r > M
      mask[5] = 250; // r < M      
    };
  
  Copy(SkinGainSet &newg)
    {
      transformWeights[0] = newg.transformWeights[0];
      transformWeights[1] = newg.transformWeights[1];
      transformWeights[2] = newg.transformWeights[2];
      transformWeights[3] = newg.transformWeights[3];
      transformWeights[4] = newg.transformWeights[4];
      transformWeights[5] = newg.transformWeights[5];
      transformDelta = newg.transformDelta;
      mask[0] = newg.mask[0];
      mask[1] = newg.mask[1];
      mask[2] = newg.mask[2];
      mask[3] = newg.mask[3];
      mask[4] = newg.mask[4];
      mask[5] = newg.mask[5];
    };
};

class SkinOperatorType
{
public:
  SkinGainSet gains;

  void Reset(SkinGainSet& newg)
    {
      gains.Copy(newg);
    };
  
  void operator() (PixelBGR& colorpixel, PixelMono& rawpixel)
    {
      int mask, r2, b2, g2, r, g, b;
      float judge;
      
      r = colorpixel.r;
      g = colorpixel.g;
      b = colorpixel.b;
      
      // This mask works pretty well in general
      // mask = (r>g) && (r<3*g) && (r>0.9*b) && (r<3*b) && (r>70);
      // This mask takes advantage of indoors
      // mask = (r>g) && (r<2*g) && (r>0.9*b) && (r<2*b) && (r>70);

      // this is the original   mask = (r>1.1*g) && (r<2*g) && (r>0.9*b) && (r<2*b);
      // this is the original   mask = mask && (r>20) && (r<250);

      mask = ((r > gains.mask[0] * g) &&
	      (r < gains.mask[1] * g) &&
	      (r > gains.mask[2] * b) &&
	      (r < gains.mask[3] * b) &&
	      (r > gains.mask[4]) &&
	      (r < gains.mask[5]));
      
      if (mask)
	{
	  r2 = r*r;
	  g2 = g*g;
	  b2 = b*b;
	  judge = 
	    r*gains.transformWeights[0] +
	    g*gains.transformWeights[1] +
	    b*gains.transformWeights[2] +
	    r2*gains.transformWeights[3] +
	    g2*gains.transformWeights[4] +
	    b2*gains.transformWeights[5] +
	    gains.transformDelta;
	  rawpixel = Clamp8bit(judge * 1.5 * 5);
	  // rawpixel = 0xff; // QQQ
	}
      else
	{
	  rawpixel = 0;
	}
    }
};

#endif
