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

	void reset()
	{
		_acc = 0.0;
	}

private:
	double _acc;

};

/*
typedef HistoEntry * HistoEntryPointer;

typedef ACE_Hash_Map_Entry<HistoKey, HistoEntryPointer> HASH_PIXEL_MAP_ENTRY;
typedef ACE_Hash_Map_Entry<unsigned char, HistoEntryPointer> HASH_1D_MAP_ENTRY;
typedef ACE_Hash_Map_Manager<HistoKey, HistoEntryPointer, ACE_Null_Mutex> HASH_PIXEL_MAP;
typedef ACE_Hash_Map_Const_Iterator<HistoKey, HistoEntryPointer, ACE_Null_Mutex> HASH_PIXEL_CONST_IT;
typedef ACE_Hash_Map_Manager<unsigned char, HistoEntryPointer, ACE_Null_Mutex> HASH_1D_MAP;
typedef ACE_Hash_Map_Const_Iterator<unsigned char, HistoEntryPointer, ACE_Null_Mutex> HASH_1D_CONST_IT;*/

class Histo3D /*: public HASH_PIXEL_MAP*/
{
public:

	Histo3D()
	{
		_nElem = 0;
		_lut = NULL;
	}
	~Histo3D()
	{
		if (_lut != NULL)
			delete [] _lut;
	}

	void resize(unsigned char max, unsigned char min, unsigned char size)
	{ 
		unsigned char tmpMax[3];
		unsigned char tmpMin[3];
		unsigned char tmpSize[3];

		int i;
		for (i = 0; i < 3; i++)
		{
			tmpMax[i] = max;
			tmpMin[i] = min;
			tmpSize[i] = size;
		}

		resize(tmpMax, tmpMin, tmpSize);
	}

	void resize(unsigned char max, unsigned char min, unsigned char *size)
	{ 
		unsigned char tmpMax[3];
		unsigned char tmpMin[3];
		
		int i;
		for (i = 0; i < 3; i++)
		{
			tmpMax[i] = max;
			tmpMin[i] = min;
		}

		resize(tmpMax, tmpMin, size);
	}
	
	void resize(unsigned char *max, unsigned char *min, unsigned char *size)
	{
		for(int i = 0; i < 3; i++)
		{
			_max[i] = max[i];
			_min[i] = min[i];
			_size[i] = size[i];
			_delta[i] = (_max[i]-_min[i])/_size[i];
			if (((_max[i]-_min[i])%_size[i]) != 0)
				_delta[i] += 1;
		
		}

		_nElem = _size[0]*_size[1]*_size[2];

		if (_lut != NULL)
			delete [] _lut;

		_lut = new HistoEntry[_nElem];
	}
	
	void clean()
	{
		int i;
		_maximum = 0.0;
		/*
		for (i = 0; i < 3; i++)
		{
			_max[i] = 0;
			_min[i] = 0;
			_delta[i] = 0;
			_size[i] = 0;
		}*/
		// HASH_PIXEL_MAP::unbind_all();

		if (_lut != NULL)
		{
			for(i = 0; i < _nElem; i++)
				_lut[i].reset();
		}
	}

	int find(unsigned int it, HistoEntry **v)
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

	int find(unsigned int it, HistoEntry &v)
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

	unsigned int begin()
	{
		return 0;
	}

private:
	HistoEntry	*_lut;
	unsigned int _nElem;

};

class Histo1D /*: public HASH_1D_MAP*/
{
public:
	Histo1D()
	{
		_nElem = 0;
		_lut = NULL;
	}

	void resize(unsigned char max, unsigned char min, unsigned char size)
	{
		_max = max;
		_min = min;
		_size = size;
		_delta = (_max-_min)/_size;
		if (((_max-_min)%_size) != 0)
			_delta += 1;
		
		if (_lut != NULL)
			delete [] _lut;

		_nElem = _size;

		_lut = new HistoEntry[_nElem];
	}

	void clean()
	{
		_maximum = 0.0;
		
		if (_lut != NULL)
		{
			int i;
			for(i = 0; i < _nElem; i ++)
				_lut[i].reset();
		}
	}

	int find(unsigned int it, HistoEntry **v)
	{
		if (it < _nElem)
		{
			*v = &_lut[it];
			return 0;
		}
		else
			return -1;
	}

	int find(unsigned int it, HistoEntry &v)
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

	void Resize(unsigned char max, unsigned char min, unsigned char n)
	{
    	clean();
		
		_3dlut.resize(max, min, n);

		_rlut.resize(max, min, n);
		_glut.resize(max, min, n);
		_blut.resize(max, min, n);
	}

	void Resize(unsigned char max, unsigned char min, unsigned char *n)
	{
		clean();
		
		_3dlut.resize(max, min, n);
		_rlut.resize(max, min, n[0]);
		_glut.resize(max, min, n[1]);
		_blut.resize(max, min, n[2]);
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

	inline double backProjection(const YarpPixelRGB &p)
	{ return _backProjection(p.r, p.g, p.b); }

	inline double backProjection(const YarpPixelBGR &p)
	{ return _backProjection(p.r, p.g, p.b); }

	inline double backProjection(unsigned char r, unsigned char g, unsigned char b)
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

	inline int _keyToPixel(unsigned int key, unsigned char *r, unsigned char *g, unsigned char *b)
	{
		/*unsigned char *r = key%(_size[0]*_size[1]*_size[2]);
		unsigned char *g = (key/(_size[1]*_size[2]))%_size[2];
		unsigned char *b = (key/(_size[1]*_size[2]*_size[2]);



		/*unsigned char tmpR = (r/_3dlut._delta[0]) + 0.5;
		unsigned char tmpG = (g/_3dlut._delta[1]) + 0.5;
		unsigned char tmpB = (b/_3dlut._delta[2]) + 0.5;*/

		return 0;
	}	

private:
	Histo3D _3dlut;
	Histo1D _rlut;
	Histo1D _glut;
	Histo1D _blut;
};


#endif