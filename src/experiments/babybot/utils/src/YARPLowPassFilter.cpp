/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPLowPassFilter.cpp,v 1.1 2004-07-29 13:09:14 babybot Exp $
///
///

// LowPassFilter.cpp
//
// Digital low-pass filter
// 26/1/1998  by <Paolo & Emanuele>
// 1999 Pasa & Panerai

#include <yarp/YARPLowPassFilter.h>
#include <memory.h>

YARPLowPassFilter::YARPLowPassFilter(void)
{
	a[3]=0.0465;
	a[2]=0.1397;
	a[1]=0.1397;
	a[0]=0.0465;
	
	b[2]=0.1457;
	b[1]=-0.7245;
	b[0]=1.2045;
	
	c[3]=0.0013; // i coeff. c[] d[] ,usati per altra versione
	c[2]=0.0039; // del filtro
	c[1]=0.0039;
	c[0]=0.0013;
	
	d[2]=0.6241;
	d[1]=-2.1653;
	d[0]=2.5308;
	
	out3=0;

	memset (in, 0, sizeof(double) * 4);
	memset (out, 0, sizeof(double) * 4);
}

YARPLowPassFilter::~YARPLowPassFilter(void)
{
}

double YARPLowPassFilter::filter(double value)
{
	const int n=3;
	
	in[n-3]=in[n-2];
	in[n-2]=in[n-1];
	in[n-1]=in[n];
	in[n]=value;
	
	for(int h = 0; h < 3; h++)
	{
		out[h]=out[h+1];
	}

	out[n]=a[0]*in[n]+a[1]*in[n-1]+a[2]*in[n-2]+a[3]*in[n-3]+b[0]*out[n-1]+b[1]*out[n-2]+b[2]*out[n-3];

    return out[n];
}


