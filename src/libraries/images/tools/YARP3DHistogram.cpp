#include "YARP3DHistogram.h"

////////////////////
// Histo3D class

Histo3D::Histo3D()
{
	_nElem = 0;
	_lut = NULL;
}

Histo3D::~Histo3D()
{
	if (_lut != NULL)
		delete [] _lut;
}

void Histo3D::clean()
{
	int i;
	_maximum = 0.0;
	
	if (_lut != NULL)
	{
		for(i = 0; i < _nElem; i++)
			_lut[i].reset();
	}
}

void Histo3D::resize(unsigned char max, unsigned char min, unsigned char size)
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

void Histo3D::resize(unsigned char max, unsigned char min, unsigned char *size)
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

void Histo3D::resize(unsigned char *max, unsigned char *min, unsigned char *size)
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

/////////////
// Histo1D
Histo1D::Histo1D()
{
	_nElem = 0;
	_lut = NULL;
}

void Histo1D:: resize(unsigned char max, unsigned char min, unsigned char size)
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

void Histo1D::clean()
{
	_maximum = 0.0;
		
	if (_lut != NULL)
	{
		int i;
		for(i = 0; i < _nElem; i ++)
			_lut[i].reset();
	}
}

////////////////
// YARP3DHistogram class

YARP3DHistogram::YARP3DHistogram()
{
	clean();
}

YARP3DHistogram::YARP3DHistogram(unsigned char max, unsigned char min, unsigned char n)
{
	clean();

	Resize(max, min, n);
}

YARP3DHistogram::YARP3DHistogram(unsigned char max, unsigned char min, unsigned char *n)
{
	clean();

	Resize(max, min, n);
}

int YARP3DHistogram::_dumpFull(const char *file)
{
	FILE *fp;
	fp = ACE_OS::fopen(file, "w");

	ACE_OS::fprintf(fp, "%lf\n", _3dlut._maximum);
	int i;
	for(i = 0; i<3; i++)
		ACE_OS::fprintf(fp, "%d %d %d\n", _3dlut._max[i], _3dlut._min[i], _3dlut._size[i]);

	HistoEntry tmpEntry;
	HistoKey tmpKey;
			
	unsigned int it = _3dlut.begin();

	while  (_3dlut.find(it, tmpEntry)!=-1)
	{
		tmpKey.b++;

		if (tmpKey.b == _3dlut._size[2])
		{
			tmpKey.b = 0;
			tmpKey.g++;
			if (tmpKey.g == _3dlut._size[1])
			{
				tmpKey.g = 0;
				tmpKey.r++;
			}
		}

		ACE_OS::fprintf(fp, "%d %d %d %lf\n", tmpKey.r, tmpKey.g, tmpKey.b, tmpEntry.value());
		it++;
	}

	ACE_OS::fclose(fp);

	return YARP_OK;
}

void YARP3DHistogram::Resize(unsigned char max, unsigned char min, unsigned char n)
{
   	clean();
	
	_3dlut.resize(max, min, n);

	_rlut.resize(max, min, n);
	_glut.resize(max, min, n);
	_blut.resize(max, min, n);
}

void YARP3DHistogram::Resize(unsigned char max, unsigned char min, unsigned char *n)
{
	clean();
		
	_3dlut.resize(max, min, n);
	_rlut.resize(max, min, n[0]);
	_glut.resize(max, min, n[1]);
	_blut.resize(max, min, n[2]);
}

int YARP3DHistogram::dump(YARPString &basename)
{
	_dumpFull((basename+".full").c_str());
	_dump1D((basename+".r").c_str(), _rlut);
	_dump1D((basename+".g").c_str(), _glut);
	_dump1D((basename+".b").c_str(), _blut);

	return YARP_OK;
}

int YARP3DHistogram::_dump1D(const char *file, Histo1D &lut)
{
	FILE *fp;
	fp = ACE_OS::fopen(file, "w");

	ACE_OS::fprintf(fp, "%lf\n", lut._maximum);
	ACE_OS::fprintf(fp, "%d %d %d\n", lut._max, lut._min, lut._size);

	HistoEntry tmpEntry;
				
	unsigned int it = lut.begin();

	while  (lut.find(it, tmpEntry)!=-1)
	{
		ACE_OS::fprintf(fp, "%d \t%lf\n", it, tmpEntry.value());
		it++;
	}

	ACE_OS::fclose(fp);

	return YARP_OK;
}

void YARP3DHistogram::clean()
{
	_3dlut.clean();
	_rlut.clean();
	_glut.clean();
	_blut.clean();
}

void YARP3DHistogram::Apply(unsigned char r, unsigned char g, unsigned char b, double w)
{
	HistoEntry *tmpEntryP = NULL;
	unsigned int it;

	// check int threshold
	if ((r+g+b) < 5)
		return;

	_pixelToKey(r, g, b, &it);

	/////////////////// 3d histo
	_3dlut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _3dlut._maximum)
			_3dlut._maximum = tmpMax;
	}

	_pixelToKey(r, 0, 0, &it);
	
	/////////////////// r histo
	_rlut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _rlut._maximum)
			_rlut._maximum = tmpMax;
	}
	
	_pixelToKey(0, g, 0, &it);

	/////////////////// g histo
	_glut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _glut._maximum)
			_glut._maximum = tmpMax;
	}
			
	_pixelToKey(0, 0, b, &it);

	/////////////////// b histo
	_blut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP).accumulate(w);
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _blut._maximum)
			_blut._maximum = tmpMax;
	}

}
