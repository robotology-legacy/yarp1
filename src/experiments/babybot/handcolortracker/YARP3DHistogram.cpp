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

void YARP3DHistogram::Apply(unsigned char r, unsigned char g, unsigned char b)
{
	HistoEntryPointer tmpEntryP;
	HistoKey tmpKey;

	// check int threshold
	if ((r+g+b) < 5)
		return;

	/*if ( (r == 85) && (g == 85) && (b == 85) )
		return;*/

	_pixelToKey(r, g, b, tmpKey);

	/////////////////// 3d histo
	if (_3dlut.find(tmpKey, tmpEntryP) == -1)
	{
		tmpEntryP = new HistoEntry;
		(*tmpEntryP)++;
		_3dlut.bind(tmpKey, tmpEntryP);
		if (_3dlut._maximum < 1)
			_3dlut._maximum=1;
	}
	else
	{
		(*tmpEntryP)++;
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _3dlut._maximum)
			_3dlut._maximum = tmpMax;
	}

	/////////////////// r histo
	if (_rlut.find(tmpKey.R, tmpEntryP) == -1)
	{
		tmpEntryP = new HistoEntry;
		(*tmpEntryP)++;
		_rlut.bind(tmpKey.R, tmpEntryP);
		if (_rlut._maximum < 1)
			_rlut._maximum=1;
	}
	else
	{
		(*tmpEntryP)++;
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _rlut._maximum)
			_rlut._maximum = tmpMax;
	}
	
	/////////////////// g histo
	if (_glut.find(tmpKey.R, tmpEntryP) == -1)
	{
		tmpEntryP = new HistoEntry;
		(*tmpEntryP)++;
		_glut.bind(tmpKey.G, tmpEntryP);
		if (_glut._maximum < 1)
			_glut._maximum=1;
	}
	else
	{
		(*tmpEntryP)++;
		double tmpMax = tmpEntryP->value();
		if (tmpMax > _glut._maximum)
			_glut._maximum = tmpMax;
	}
			
	/////////////////// b histo
	if (_blut.find(tmpKey.B, tmpEntryP) == -1)
	{
		tmpEntryP = new HistoEntry;
		(*tmpEntryP)++;
		_blut.bind(tmpKey.B, tmpEntryP);
		if (_blut._maximum < 1)
			_blut._maximum=1;
	}
	else
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
	fp = fopen(file, "w");

	ACE_OS::fprintf(fp, "%lf\n", _3dlut._maximum);
	int i;
	for(i = 0; i<3; i++)
		ACE_OS::fprintf(fp, "%d %d %d\n", _3dlut._max[i], _3dlut._min[i], _3dlut._size[i]);

	HistoEntryPointer tmpEntryP;
	HistoKey *tmpKeyP;
		
	HASH_PIXEL_MAP_ENTRY *entry;
		
	for (HASH_PIXEL_CONST_IT hash_iter (_3dlut);
		 hash_iter.next (entry) != 0;
		 hash_iter.advance ())
	{
		tmpEntryP = entry->int_id_;
		tmpKeyP = &entry->ext_id_;
		fprintf(fp, "%d %d %d\t%lf\n", tmpKeyP->R, tmpKeyP->G, tmpKeyP->B, tmpEntryP->value());
	}

	fclose(fp);

	return YARP_OK;
}

int YARP3DHistogram::_dump1D(const char *file, Histo1D &lut)
{
	FILE *fp;
	fp = fopen(file, "w");

	ACE_OS::fprintf(fp, "%lf\n", lut._maximum);
	ACE_OS::fprintf(fp, "%d %d %d\n", lut._max, lut._min, lut._size);

	HistoEntryPointer tmpEntryP;
	unsigned char *tmpKeyP;
		
	HASH_1D_MAP_ENTRY *entry;
		
	for (HASH_1D_CONST_IT hash_iter (lut);
		 hash_iter.next (entry) != 0;
		 hash_iter.advance ())
	{
		tmpEntryP = entry->int_id_;
		tmpKeyP = &entry->ext_id_;
		fprintf(fp, "%d\t%lf\n", *tmpKeyP, tmpEntryP->value());
	}

	fclose(fp);

	return YARP_OK;
}

double YARP3DHistogram::_backProjection(unsigned char r, unsigned char g, unsigned char b)
{
	HistoKey tmpKey;
	HistoEntryPointer tmpEntryP;
	_pixelToKey(r, g, b, tmpKey);

	if (_3dlut.find(tmpKey, tmpEntryP) != -1)
	{
		return tmpEntryP->value()/_3dlut._maximum;
	}
	else
	{
		return 0;
	}
}

int YARP3DHistogram::_pixelToKey(unsigned char r, unsigned char g, unsigned char b, HistoKey &key)
{
	key.R = double (r/_3dlut._delta[0]) + 0.5;
	key.G = double (g/_3dlut._delta[1]) + 0.5;
	key.B = double (b/_3dlut._delta[2]) + 0.5;

	return 0;
}