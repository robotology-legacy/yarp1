// mapper.cpp : Defines the entry point for the console application.
//

#include <YARPString.h>
#include <YARPLogPolar.h>
#include <YARPImage.h>
#include <YARPImageFile.h>
#include <YARPParseParameters.h>
#include <YARPColorConverter.h>

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

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Use: " << argv[0] << " inputfile outputfile [-mono] [-norm]\n";
		return 0;
	}

	bool grayscale = false;
	bool normalized = false;

	if (YARPParseParameters::parse(argc, argv, "mono"))
	{
		grayscale = true;
	}
	if (YARPParseParameters::parse(argc, argv, "norm"))
	{
		normalized = true;
	}
		
	YARPString name;
	YARPString cartName;
	
	cartName.append(argv[2]);
	
	YARPLogpolar _mapper;
	YARPImageOf<YarpPixelMono> _input;
	YARPImageOf<YarpPixelBGR> _inputC;
	YARPImageOf<YarpPixelBGR> _inputCNorm;
	YARPImageOf<YarpPixelMono> _inputMono;
	YARPImageOf<YarpPixelBGR> _output;

	YARPImageFile::Read(argv[1], _input);
	_inputC.Resize(_input.GetWidth(), _input.GetHeight());
	_inputCNorm.Resize(_input.GetWidth(), _input.GetHeight());
	_inputMono.Resize(_input.GetWidth(), _input.GetHeight());
	_output.Resize(_logpolarParams::_xsize, _logpolarParams::_ysize);
	
	if ( (!grayscale) && (!normalized) )
	{
		_mapper.ReconstructColor(_input, _inputC);
		_mapper.Logpolar2Cartesian(_inputC, _output);
	}
	else if (!grayscale)
	{
		_mapper.ReconstructColor(_input, _inputC);
		YARPColorConverter::RGB2Normalized(_inputC, _inputCNorm, 20);
		_mapper.Logpolar2Cartesian(_inputCNorm, _output);
	}
	else
	{
		_mapper.ReconstructGrays(_input, _inputMono);
		_mapper.Logpolar2Cartesian(_inputMono, _output);
	}
	
	YARPImageFile::Write(cartName.c_str(), _output);
	return 0;
}

