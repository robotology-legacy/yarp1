// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>
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

class YARPRGBHistogram
{
public:
	YARPRGBHistogram()
	{
		_3dMaximum = 0.0;
		_rMaximum = 0.0;
		_gMaximum = 0.0;
		_bMaximum = 0.0;

		_3dN = 0;
		_rN = 0;
		_gN = 0;
		_bN = 0;

		for(int i = 0; i < 3; i++)
		{
			_max[i] = 0;
			_min[i] = 0;
			_size[i] = 0;
			_delta[i] = 0;
		}
	}
	YARPRGBHistogram(unsigned char max, unsigned char min, unsigned char n)
	{
		_3dMaximum = 0.0;
		_rMaximum = 0.0;
		_gMaximum = 0.0;
		_bMaximum = 0.0;

		_3dN = 0;
		_rN = 0;
		_gN = 0;
		_bN = 0;

		for(int i = 0; i < 3; i++)
		{
			_max[i] = max;
			_min[i] = min;
			_size[i] = n;
			_delta[i] = (_max[i]-_min[i])/_size[i];
		}
	};

	~YARPRGBHistogram(){};

	void Resize(unsigned char max, unsigned char min, unsigned char n)
	{
    	clean();
		for(int i = 0; i < 3; i++)
		{
			_max[i] = max;
			_min[i] = min;
			_size[i] = n;
			_delta[i] = (_max[i]-_min[i])/_size[i];
		}
	}

	inline void Apply(unsigned char r, unsigned char g, unsigned char b)
	{
	
		HistoEntryPointer tmpEntryP;
		HistoKey tmpKey;

		// check int threshold
		if ((r+g+b) < 5)
			return;

		if ( (r == 85) && (g == 85) && (b == 85) )
			return;

		_pixelToKey(r, g, b, tmpKey);

		/////////////////// 3d histo
		if (_3dlut.find(tmpKey, tmpEntryP) == -1)
		{
			tmpEntryP = new HistoEntry;
			(*tmpEntryP)++;
			_3dlut.bind(tmpKey, tmpEntryP);
			if (_3dMaximum < 1)
				_3dMaximum=1;
		}
		else
		{
			(*tmpEntryP)++;
			double tmpMax = tmpEntryP->value();
			if (tmpMax > _3dMaximum)
				_3dMaximum = tmpMax;
		}
		_3dN++;

		/////////////////// r histo
		if (_rlut.find(tmpKey.R, tmpEntryP) == -1)
		{
			tmpEntryP = new HistoEntry;
			(*tmpEntryP)++;
			_rlut.bind(tmpKey.R, tmpEntryP);
			if (_rMaximum < 1)
				_rMaximum=1;
		}
		else
		{
			(*tmpEntryP)++;
			double tmpMax = tmpEntryP->value();
			if (tmpMax > _rMaximum)
				_rMaximum = tmpMax;
		}
		
		/////////////////// g histo
		if (_glut.find(tmpKey.R, tmpEntryP) == -1)
		{
			tmpEntryP = new HistoEntry;
			(*tmpEntryP)++;
			_glut.bind(tmpKey.G, tmpEntryP);
			if (_gMaximum < 1)
				_gMaximum=1;
		}
		else
		{
			(*tmpEntryP)++;
			double tmpMax = tmpEntryP->value();
			if (tmpMax > _rMaximum)
				_rMaximum = tmpMax;
		}
				
		/////////////////// b histo
		if (_blut.find(tmpKey.B, tmpEntryP) == -1)
		{
			tmpEntryP = new HistoEntry;
			(*tmpEntryP)++;
			_blut.bind(tmpKey.B, tmpEntryP);
			if (_rMaximum < 1)
				_rMaximum=1;
		}
		else
		{
			(*tmpEntryP)++;
			double tmpMax = tmpEntryP->value();
			if (tmpMax > _rMaximum)
				_rMaximum = tmpMax;
		}

		_rN++;
		_gN++;
		_bN++;
	}
	
	void dump(YARPString &basename)
	{
		_dumpFull((basename+".full").c_str());
		_dump1D((basename+".r").c_str(), _rlut);
		_dump1D((basename+".g").c_str(), _glut);
		_dump1D((basename+".b").c_str(), _blut);
 	}

	double backProjection(const YarpPixelRGB &p)
	{ return _backProjection(p.r, p.g, p.b); }

	double backProjection(const YarpPixelBGR &p)
	{ return _backProjection(p.r, p.g, p.b); }

	double backProjection(unsigned char r, unsigned char g, unsigned char b)
	{ return _backProjection(r,g,b); }

	void clean()
	{
		_3dMaximum = 0.0;
		_rMaximum = 0.0;
		_gMaximum = 0.0;
		_bMaximum = 0.0;

		_3dN = 0;
		_rN = 0;
		_gN = 0;
		_bN = 0;
		
		_3dlut.unbind_all();
		_rlut.unbind_all();
		_glut.unbind_all();
		_blut.unbind_all();
	}

private:
	void _dumpFull(const char *file)
	{
		FILE *fp;
		fp = fopen(file, "w");

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
	}

	void _dump1D(const char *file, HASH_1D_MAP &lut)
	{
		FILE *fp;
		fp = fopen(file, "w");

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
	};

	double _backProjection(unsigned char r, unsigned char g, unsigned char b)
	{
		HistoKey tmpKey;
		HistoEntryPointer tmpEntryP;
		_pixelToKey(r, g, b, tmpKey);

		if (_3dlut.find(tmpKey, tmpEntryP) != -1)
		{
			return tmpEntryP->value()/_3dN;
		}
		else
		{
			return 0;
		}
	}

	int _pixelToKey(unsigned char r, unsigned char g, unsigned char b, HistoKey &key)
	{
		key.R = double (r/_delta[0]) + 0.5;
		key.G = double (g/_delta[1]) + 0.5;
		key.B = double (b/_delta[2]) + 0.5;

		return 0;
	}

private:
	HASH_PIXEL_MAP _3dlut;
	HASH_1D_MAP _rlut;
	HASH_1D_MAP _glut;
	HASH_1D_MAP _blut;

	unsigned char _max[3];
	unsigned char _min[3];
	unsigned char _delta[3];
	unsigned char _size[3];

	double _3dMaximum;
	int _3dN;
	double _rMaximum;
	int _rN;
	double _gMaximum;
	int _gN;
	double _bMaximum;
	int _bN;
};

class YARPHistogramSegmentation: public YARPRGBHistogram
{
public:
	YARPHistogramSegmentation(double lumaTh, unsigned char max, unsigned char min, unsigned char n):
	 YARPRGBHistogram(max, min, n)
	{
		_lumaThreshold = lumaTh;
	
	}
	
	inline void _normalize (unsigned char r, unsigned char g, unsigned char b,
							unsigned char *rp, unsigned char *gp, unsigned char *bp)
	{
	//	*rp = r;
	//	*gp = g;
	//	*bp = b;

		float luma = r+g+b;
		if (luma < _lumaThreshold)
		{
			*rp = 0;
			*gp = 0;
			*bp = 0;
		}
		else
		{
			*rp = (r/luma)*255 + 0.5;
			*gp = (g/luma)*255 + 0.5;
			*bp = (b/luma)*255 + 0.5;
		}
	}
	
	inline void _normalize (const YarpPixelRGB &in, YarpPixelRGB &out)
	{
		_normalize(in.r, in.g, in.b, &out.r, &out.g, &out.b);
	}

	inline void _normalize (const YarpPixelBGR &in, YarpPixelRGB &out)
	{
		_normalize(in.r, in.g, in.b, &out.r, &out.g, &out.b);
	}

	void Apply(YARPImageOf<YarpPixelBGR> &src)
	{
		int i;
		int j;
		unsigned char *r;
		unsigned char *g;
		unsigned char *b;

		for(i = 0; i < src.GetHeight(); i++)
		{
			b = (unsigned char *) src.GetArray()[i];
			g = b+1;
 			r = b+2;

			unsigned char rp, gp, bp;
		
			
			for(j = 0; j < src.GetWidth(); j++)
			{
				_normalize(*r, *g, *b, &rp, &gp, &bp);
				YARPRGBHistogram::Apply(rp, gp, bp);
				b += 3;
				g += 3;
				r += 3;
			}
		}

	}

	void Apply(YARPImageOf<YarpPixelRGB> &src)
	{
		int i;
		int j;
		unsigned char *r;
		unsigned char *g;
		unsigned char *b;
			
		for(i = 0; i < src.GetHeight(); i++)
		{
			r = (unsigned char *) src.GetArray()[i];
			g = r+1;
 			b = r+2;

			unsigned char rp, gp, bp;
					
			for(j = 0; j < src.GetWidth(); j++)
			{
				_normalize(*r, *g, *b, &rp, &gp, &bp);
				YARPRGBHistogram::Apply(rp, gp, bp);
				
				b += 3;
				g += 3;
				r += 3;
			}
		}
	}

	void backProjection(YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelMono> &out)
	{	
		// complete histogram backprojection
		int i;
		int j;
		for(i = 0; i < in.GetHeight(); i++)
			for(j = 0; j < in.GetWidth(); j++)
			{
				YarpPixelRGB tmp;
				_normalize(in(i,j), tmp);
				out(i,j) = YARPRGBHistogram::backProjection(tmp)*255 + 0.5;
			}
	}

	void backProjection(YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &out)
	{	
		// complete histogram backprojection
		int i;
		int j;
		for(i = 0; i < in.GetHeight(); i++)
			for(j = 0; j < in.GetWidth(); j++)
			{
				YarpPixelRGB tmp;
				_normalize(in(i,j), tmp);
				out(i,j) = YARPRGBHistogram::backProjection(tmp)*255 + 0.5;
			}
	}

private:
	double _lumaThreshold;
};

int main(int argc, char* argv[])
{
	YARPHistogramSegmentation _histo(5, 255, 0, 32);

	YARPInputPortOf<YARPGenericImage> _inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> _inPortSeg(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	YARPOutputPortOf<YARPGenericImage> _outPortSeg(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	
	_inPortImage.Register("/handtracker/i:img");
	_inPortSeg.Register("/handtracker/segmentation/i:img");
	_outPortSeg.Register("/handtracker/segmentation/o:img");

	YARPImageOf<YarpPixelMono> _left;
	YARPImageOf<YarpPixelMono> _leftSeg;
	YARPImageOf<YarpPixelBGR> _leftColored;
	YARPImageOf<YarpPixelBGR> _leftSegColored;

	YARPImageOf<YarpPixelMono> _outSeg;

	YARPLogpolar _mapper;

	while (false)
	{
		_inPortImage.Read();

		if (_inPortSeg.Read(0))
		{
			_leftSeg.Refer(_inPortSeg.Content());
			_mapper.ReconstructColor(_leftSeg, _leftSegColored);
			_histo.Apply(_leftSegColored);
		}

		_left.Refer (_inPortImage.Content());

		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);

		_histo.backProjection(_leftColored, _outSeg);
		
		_outPortSeg.Content().Refer(_outSeg);
		_outPortSeg.Write();
	}
	
		
	/*
	YARPLogpolar _mapper;

	YARPImageOf<YarpPixelMono> input;

	YARPImageOf<YarpPixelMono> output;*/
	// YARPImageOf<YarpPixelBGR> outputLpC;
/*	YARPDIBConverter bmp;
	YARPImageOf<YarpPixelRGB> inputCart;
	YARPImageOf<YarpPixelRGB> testCart;
	YARPImageOf<YarpPixelRGB> testCartFlipped;
	inputCart.Resize(256,256);
	testCart.Resize(256,256);
	testCartFlipped.Resize(256,256);
	
	// compute histo
	for(int i = 1; i <= 3; i++)
	{
		char tmp[255];
		sprintf(tmp,"d:\\nat\\yarp\\zgarbage\\handsegmented%d.bmp", i);
		bmp.LoadDIB(tmp);
		bmp.ConvertFromDIB(inputCart);
		_histo.Apply(inputCart);
	}

	// test histo
	bmp.LoadDIB("d:\\nat\\yarp\\zgarbage\\sample1Cart.bmp");
	bmp.ConvertFromDIB(testCart);
	YARPSimpleOperation::Flip(testCart, testCartFlipped);
		
	_histo.dump(YARPString("d:\\nat\\yarp\\zgarbage\\handhisto"));

	YARPImageOf<YarpPixelMono> out;
	out.Resize(testCartFlipped.GetHeight(), testCartFlipped.GetWidth());
	_histo.backProjection(testCartFlipped, out);
	
	YARPImageFile::Write("d:\\nat\\yarp\\zgarbage\\backprojected.ppm", out);


	/*
	
	
	YARPImageFile::Read("y:\\zgarbage\\detected.ppm",input);
	output.Resize(input.GetWidth(), input.GetHeight());
	outputCart.Resize(256,256);
	outputLpC.Resize(input.GetWidth(), input.GetHeight());
	
	YARPBlobDetector _blob(4.0);
	_blob.resize(input.GetWidth(), input.GetHeight(), _logpolarParams::_sfovea);

	// YARPIntegralImage _int;
	// _int.resize(input.GetWidth(), input.GetHeight(), _logpolarParams::_sfovea);

	_blob.filterLp(input);
	// _int.get(output);
	output = _blob.getSegmented();

	_mapper.ReconstructColor(output, outputLpC);
	_mapper.Logpolar2Cartesian(outputLpC, outputCart);

	YARPImageFile::Write("y:\\zgarbage\\blob.ppm",output);
	YARPImageFile::Write("y:\\zgarbage\\blobC.ppm",outputCart);*/
	
	return 0;
}

