#ifndef __YARP3DHISTOHH__
#define __YARP3DHISTOHH__

#include <YARPImage.h>
#include <YARPString.h>
#include <./ace/functor.h>

#include <./ace/Hash_Map_Manager.h>

class HistoKey
{
public:
	HistoKey()
	{
		R = 0;
		G = 0;
		B = 0;
	}
	HistoKey(unsigned char r, unsigned char g, unsigned char b)
	{
		R = r;
		G = g;
		B = b;
	}
	
	bool operator == (const HistoKey &l) const
	{
		if ( (R == l.R) && (G == l.G) && (B == l.B) )
			return true;
		else
			return false;

	}

	unsigned long hash() const
	{
		int tmp;
		tmp = (R << 16) + (G << 8) + B;
		return tmp;
	}

	unsigned char R;
	unsigned char G;
	unsigned char B;
};

class HistoEntry
{
public:
	HistoEntry()
	{
		_acc = 0.0;
	}
	
	HistoEntry &operator ++ (int)
	{
		_acc++;
		return *this;
	}

	double value()
	{
		return _acc;
	}

private:
	double _acc;

};

typedef HistoEntry * HistoEntryPointer;

typedef ACE_Hash_Map_Entry<HistoKey, HistoEntryPointer> HASH_PIXEL_MAP_ENTRY;
typedef ACE_Hash_Map_Entry<unsigned char, HistoEntryPointer> HASH_1D_MAP_ENTRY;
typedef ACE_Hash_Map_Manager<HistoKey, HistoEntryPointer, ACE_Null_Mutex> HASH_PIXEL_MAP;
typedef ACE_Hash_Map_Const_Iterator<HistoKey, HistoEntryPointer, ACE_Null_Mutex> HASH_PIXEL_CONST_IT;
typedef ACE_Hash_Map_Manager<unsigned char, HistoEntryPointer, ACE_Null_Mutex> HASH_1D_MAP;
typedef ACE_Hash_Map_Const_Iterator<unsigned char, HistoEntryPointer, ACE_Null_Mutex> HASH_1D_CONST_IT;

class Histo3D: public HASH_PIXEL_MAP
{
public:
	void clean()
	{
		int i;
		_maximum = 0.0;
		for (i = 0; i < 3; i++)
		{
			_max[i] = 0;
			_min[i] = 0;
			_delta[i] = 0;
			_size[i] = 0;
		}
		HASH_PIXEL_MAP::unbind_all();
	}
	double _maximum;

	unsigned char _max[3];
	unsigned char _min[3];
	unsigned char _delta[3];
	unsigned char _size[3];

};

class Histo1D: public HASH_1D_MAP
{
public:
	void clean()
	{
		_maximum = 0.0;
		_max = 0;
		_min = 0;
		_delta = 0;
		_size = 0;
		HASH_1D_MAP::unbind_all();
	}

	double _maximum;

	unsigned char _max;
	unsigned char _min;
	unsigned char _delta;
	unsigned char _size;
};

class YARP3DHistogram
{
public:
	YARP3DHistogram();

	YARP3DHistogram(unsigned char max, unsigned char min, unsigned char n);
	
	~YARP3DHistogram(){};

	void Resize(unsigned char max, unsigned char min, unsigned char n)
	{
    	clean();
		for(int i = 0; i < 3; i++)
		{
			_3dlut._max[i] = max;
			_3dlut._min[i] = min;
			_3dlut._size[i] = n;
			_3dlut._delta[i] = (_3dlut._max[i]-_3dlut._min[i])/_3dlut._size[i];
		}

		
		_rlut._max = max;
		_rlut._min = min;
		_rlut._size = n;
		_rlut._delta = (_rlut._max-_rlut._min)/_rlut._size;

		_glut._max = max;
		_glut._min = min;
		_glut._size = n;
		_glut._delta = (_glut._max-_glut._min)/_glut._size;

		_blut._max = max;
		_blut._min = min;
		_blut._size = n;
		_blut._delta = (_blut._max-_blut._min)/_blut._size;

	}

	void Apply(unsigned char r, unsigned char g, unsigned char b);
	
	int dump(YARPString &basename)
	{
		_dumpFull((basename+".full").c_str());
		_dump1D((basename+".r").c_str(), _rlut);
		_dump1D((basename+".g").c_str(), _glut);
		_dump1D((basename+".b").c_str(), _blut);

		return YARP_OK;
 	}

	double backProjection(const YarpPixelRGB &p)
	{ return _backProjection(p.r, p.g, p.b); }

	double backProjection(const YarpPixelBGR &p)
	{ return _backProjection(p.r, p.g, p.b); }

	double backProjection(unsigned char r, unsigned char g, unsigned char b)
	{ return _backProjection(r,g,b); }

	void clean()
	{
		_3dlut.clean();
		_rlut.clean();
		_glut.clean();
		_blut.clean();
	}

private:
	int _dumpFull(const char *file);
	int _dump1D(const char *file, Histo1D &lut);
	double _backProjection(unsigned char r, unsigned char g, unsigned char b);
	int _pixelToKey(unsigned char r, unsigned char g, unsigned char b, HistoKey &key);
	
private:
	Histo3D _3dlut;
	Histo1D _rlut;
	Histo1D _glut;
	Histo1D _blut;
};


#endif