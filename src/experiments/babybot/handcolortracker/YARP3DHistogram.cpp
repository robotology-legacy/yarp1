#include "YARP3DHistogram.h"

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

void YARP3DHistogram::Apply(unsigned char r, unsigned char g, unsigned char b)
{
	HistoEntry *tmpEntryP = NULL;
	// HistoKey tmpKey;
	unsigned int it;

	// check int threshold
	if ((r+g+b) < 5)
		return;

	/*if ( (r == 85) && (g == 85) && (b == 85) )
		return;*/

	_pixelToKey(r, g, b, &it);

	/////////////////// 3d histo
	_3dlut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP)++;
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _3dlut._maximum)
			_3dlut._maximum = tmpMax;
	}

	_pixelToKey(r, 0, 0, &it);
	
	/////////////////// r histo
	_rlut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP)++;
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _rlut._maximum)
			_rlut._maximum = tmpMax;
	}
	
	_pixelToKey(0, g, 0, &it);

	/////////////////// g histo
	_glut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP)++;
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _glut._maximum)
			_glut._maximum = tmpMax;
	}
			
	_pixelToKey(0, 0, b, &it);

	/////////////////// b histo
	_blut.find(it, &tmpEntryP);
	if (tmpEntryP != NULL)
	{
		(*tmpEntryP)++;
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _blut._maximum)
			_blut._maximum = tmpMax;
	}

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
		tmpKey.B++;

		if (tmpKey.B == _3dlut._size[2])
		{
			tmpKey.B = 0;
			tmpKey.G++;
			if (tmpKey.G == _3dlut._size[1])
			{
				tmpKey.G = 0;
				tmpKey.R++;
			}
		}

		ACE_OS::fprintf(fp, "%d %d %d %lf\n", tmpKey.R, tmpKey.G, tmpKey.B, tmpEntry.value());
		it++;
	}

	ACE_OS::fclose(fp);

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