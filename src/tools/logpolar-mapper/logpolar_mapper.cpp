// mapper.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPString.h>
#include <yarp/YARPLogPolar.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPParseParameters.h>
// #include <yarp/YARPColorConverter.h>
#include <yarp/YARPSimpleOperations.h>
#include "DIBConverter.h"
#include "colorconverter.h"

using namespace _logpolarParams;

YARPLogpolar _mapper;
YARPDIBConverter _converter;

int _findName(const char *in, YARPString &out)
{
	int i = 0;
	while(in[i] != '.')
	{
		if (in[i] == '\0')
			return 0;
		out.append(in[i]);
		i++;
	}
	return 1;
}

// just reconstruct colors
int _logpolar(const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelBGR> &out)
{
	out.Resize(in.GetWidth(), in.GetHeight());
	_mapper.ReconstructColor(in, out);
	return 0;
}

// reconstruct colors and convert to cartesian
int _cartesian(const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelBGR> &out)
{
	YARPImageOf<YarpPixelBGR> inTmp;
	inTmp.Resize(in.GetWidth(), in.GetHeight());
	out.Resize(_xsize,_ysize);
	_mapper.ReconstructColor(in, inTmp);
	_mapper.Logpolar2Cartesian(inTmp, out);
	return 0;
}

// reconstruct colors and convert to cartesian
int _cartesianNoCol(const YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelBGR> &out)
{
	out.Resize(_xsize,_ysize);
	_mapper.Logpolar2Cartesian(in, out);
	return 0;
}

// reconstruct colors and convert to cartesian, plus extract the fovea
int _fovea(const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelBGR> &out)
{
	YARPImageOf<YarpPixelBGR> inTmp;
	inTmp.Resize(in.GetWidth(), in.GetHeight());
	out.Resize(_xsizefovea,_ysizefovea);
	_mapper.ReconstructColor(in, inTmp);
	_mapper.Logpolar2CartesianFovea(inTmp, out);
	return 0;
}

int _normalize(const YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelBGR> &out)
{
	out.Resize(in.GetWidth(), in.GetHeight());
	RGB2Normalized(in, out, 20);
	return 0;
}

int _grayscale(const YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &out)
{
	out.Resize(in.GetWidth(), in.GetHeight());
	RGB2Grayscale(in, out);
	return 0;
}

void _saveImage(YARPGenericImage &im, const YARPString &inName, bool bmp = false)
{
	if (!bmp)
	{
		// save ppm
		YARPImageFile::Write(inName.c_str(), im);
	}
	else
	{
		_converter.Resize(im);
		_converter.ConvertToDIB(im);
		_converter.SaveDIB(inName.c_str());
	}
}

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Use: " << argv[0] << " inputfile outputfile [--mono] [--norm] [--lp] [--bmp] [--fov] [--nocol]\n";
		return 0;
	}

	YARPImageOf<YarpPixelMono> _input;
	YARPImageOf<YarpPixelBGR> _inputCol;
	YARPImageOf<YarpPixelBGR> _tmp1;
	YARPImageOf<YarpPixelBGR> _flipped;
	YARPImageOf<YarpPixelBGR> _outputColor;
	YARPImageOf<YarpPixelMono> _outputMono;

	bool grayscale = false;
	bool normalized = false;
	bool lp = false;
	bool fovea = false;
	bool bmp = false;
	bool color = true;
	
	if (YARPParseParameters::parse(argc, argv, "-mono"))
	{
		grayscale = true;
	}
	if (YARPParseParameters::parse(argc, argv, "-norm"))
	{
		normalized = true;
	}
	if (YARPParseParameters::parse(argc, argv, "-lp"))
	{
		lp = true;
	}
	if (YARPParseParameters::parse(argc, argv, "-fov"))
	{
		fovea = true;
	}
	if (YARPParseParameters::parse(argc, argv, "-bmp"))
	{
		bmp = true;
	}
	if (YARPParseParameters::parse(argc, argv, "-nocol"))
	{
		cout << "You chose not to build colors, assuming input image is RGB\n";
		color = false;
	}
	else
	{
		cout << "You chose to build colors, assuming input image is MONO\n";
		color = true;
	}
		
	YARPString name;
	YARPString outName;
	
	outName.append(argv[2]);
	
	if (color)
	{
		if (YARPImageFile::Read(argv[1], _input) == -1)
		{
			ACE_OS::printf("Error: problem reading mono image %s\n", argv[1]);
			return 0;
		}
	}
	else
	{
		if (YARPImageFile::Read(argv[1], _inputCol) == -1)
		{
			ACE_OS::printf("Error: problem reading RGB image %s\n", argv[1]);
			return 0;
		}
	}


	if (lp)
		_logpolar(_input, _tmp1);
	else if (fovea)
		_fovea(_input, _tmp1);
	else if(color)
		_cartesian(_input, _tmp1);
	else
		_cartesianNoCol(_inputCol, _tmp1);
	
	if (bmp)
	{
		_flipped.Resize(_tmp1.GetWidth(), _tmp1.GetHeight());
		YARPSimpleOperation::Flip(_tmp1, _flipped);
		_tmp1 = _flipped;
	}


	if (grayscale)
	{
		_grayscale(_tmp1, _outputMono);
		_saveImage(_outputMono, outName, bmp);
		// YARPImageFile::Write(outName.c_str(), _outputMono);
	}
	else if (normalized)
	{
		_normalize(_tmp1, _outputColor);
		_saveImage(_outputColor, outName, bmp);
		// YARPImageFile::Write(outName.c_str(), _outputColor);
	}
	else
	{
		_outputColor = _tmp1;
		_saveImage(_outputColor, outName, bmp);
		// YARPImageFile::Write(outName.c_str(), _outputColor);
	}

	return 0;
}

