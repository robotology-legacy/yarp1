// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>

int main(int argc, char* argv[])
{
	YARPLogpolar _mapper;

	YARPImageOf<YarpPixelMono> input;

	YARPImageOf<YarpPixelMono> output;
	YARPImageOf<YarpPixelBGR> outputLpC;
	YARPImageOf<YarpPixelBGR> outputCart;
	
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
	YARPImageFile::Write("y:\\zgarbage\\blobC.ppm",outputCart);
	
	return 0;
}

