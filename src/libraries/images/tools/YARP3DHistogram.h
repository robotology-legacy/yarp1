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
///                    #nat
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARP3DHistogram.h,v 1.3 2003-09-04 16:57:40 babybot Exp $ 
///
/// August 2003 -- by nat

#ifndef __YARP3DHISTOHH__
#define __YARP3DHISTOHH__

#include <YARPImage.h>
#include <YARPString.h>

// use HistoKey for historical reasons
typedef YarpPixelRGB HistoKey;

class HistoEntry
{
public:
	HistoEntry()
	{ _acc = 0.0; }
	
/*	HistoEntry &operator ++ (int)
	{ 
		_acc++;
		return *this;
	}*/

	void accumulate(double w)
	{ _acc += w; }

	double value()
	{ return _acc; }

	void reset()
	{ _acc = 0.0; }

private:
	double _acc;
};

class Histo3D
{
public:
	Histo3D();
	~Histo3D();
	
	// resize methods
	void resize(unsigned char max, unsigned char min, unsigned char size);
	void resize(unsigned char max, unsigned char min, unsigned char *size);
	void resize(unsigned char *max, unsigned char *min, unsigned char *size);
	
	// clean histo
	void clean();

	// query histo
	inline int find(unsigned int it, HistoEntry **v)
	{
		if (it < _nElem)
		{
			*v = &_lut[it];
			return 0;
		}
		else
		{
			return -1;
		}
	}

	// query histo
	inline int find(unsigned int it, HistoEntry &v)
	{
		// LATER: CHECK INDEX 
		if (it < _nElem)
		{
			v = _lut[it];
			return 0;
		}
		else
			return -1;
	}

	double _maximum;

	unsigned char _max[3];
	unsigned char _min[3];
	unsigned char _delta[3];
	unsigned char _size[3];

	// return first element
	unsigned int begin()
	{ return 0; }

private:
	HistoEntry	*_lut;
	unsigned int _nElem;

};

class Histo1D
{
public:
	Histo1D();
	
	void resize(unsigned char max, unsigned char min, unsigned char size);
	void clean();
	
	// find methods
	inline int find(unsigned int it, HistoEntry **v)
	{
		if (it < _nElem)
		{
			*v = &_lut[it];
			return 0;
		}
		else
			return -1;
	}
	
	inline int find(unsigned int it, HistoEntry &v)
	{
		if (it < _nElem)
		{
			v = _lut[it];
			return 0;
		}
		else
			return -1;
	}

	unsigned int begin()
	{ return 0; }

	double _maximum;

	unsigned char _max;
	unsigned char _min;
	unsigned char _delta;
	unsigned char _size;

	HistoKey _key;
private:
	HistoEntry *_lut;
	unsigned int _nElem;
};

class YARP3DHistogram
{
public:
	YARP3DHistogram();

	YARP3DHistogram(unsigned char max, unsigned char min, unsigned char n);
	YARP3DHistogram(unsigned char max, unsigned char min, unsigned char *n);
	
	~YARP3DHistogram(){};

	void Resize(unsigned char max, unsigned char min, unsigned char n);
	void Resize(unsigned char max, unsigned char min, unsigned char *n);
	void Apply(unsigned char r, unsigned char g, unsigned char b, double w = 1.0);
		
	int dump(YARPString &basename);
	
	inline double backProjection(const YarpPixelRGB &p)
	{ return _backProjection(p.r, p.g, p.b); }

	inline double backProjection(const YarpPixelBGR &p)
	{ return _backProjection(p.r, p.g, p.b); }

	inline double backProjection(unsigned char r, unsigned char g, unsigned char b)
	{ return _backProjection(r,g,b); }

	void clean();
	
private:
	int _dumpFull(const char *file);
	int _dump1D(const char *file, Histo1D &lut);
	inline double _backProjection(unsigned char r, unsigned char g, unsigned char b)
	{
		HistoEntry *tmpEntryP = NULL;
		unsigned int it;
		
		_pixelToKey(r, g, b, &it);

		_3dlut.find(it, &tmpEntryP);
		if (tmpEntryP != NULL)
		{
			return tmpEntryP->value()/_3dlut._maximum;
		}
		else
		{
			cout << "Warning, index out of limit this should not happen. Check bin size.\n";
			return 0;
		}
		return 0;
	}

	inline int _pixelToKey(unsigned char r, unsigned char g, unsigned char b, unsigned int *key)
	{
		int tmpR = (r/_3dlut._delta[0]);
		int tmpG = (g/_3dlut._delta[1]);
		int tmpB = (b/_3dlut._delta[2]);

		unsigned int tmp;
		tmp = (tmpR)*_3dlut._size[1]*_3dlut._size[2];
		tmp += (tmpG)*_3dlut._size[2];
		tmp += (tmpB);

		*key = tmp;

		return tmp;
	}

private:
	Histo3D _3dlut;
	Histo1D _rlut;
	Histo1D _glut;
	Histo1D _blut;
};

#endif