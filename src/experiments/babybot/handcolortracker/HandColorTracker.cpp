// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>

#include "YARPHistoSegmentation.h"
#include "YARP3DHistogram.h"

using namespace _logpolarParams;

int main(int argc, char* argv[])
{
	YARPHistoSegmentation _histo(5, 255, 0, 10);

	YARPInputPortOf<YARPGenericImage> _inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> _inPortSeg(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	YARPOutputPortOf<YARPGenericImage> _outPortSeg(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	YARPBlobDetector _blobber(5.0);
	_blobber.resize(_stheta, _srho, _sfovea);
	
	_inPortImage.Register("/handtracker/i:img");
	_inPortSeg.Register("/handtracker/segmentation/i:img");
	_outPortSeg.Register("/handtracker/segmentation/o:img");

	YARPImageOf<YarpPixelMono> _left;
	YARPImageOf<YarpPixelMono> _leftSeg;
	YARPImageOf<YarpPixelBGR> _leftColored;
	YARPImageOf<YarpPixelBGR> _leftSegColored;
	_left.Resize(_stheta, _srho);
	_leftSeg.Resize(_stheta, _srho);
	_leftColored.Resize(_stheta, _srho);
	_leftSegColored.Resize(_stheta, _srho);

	YARPImageOf<YarpPixelMono> _outSeg;
	// YARPImageOf<YarpPixelMono> _blobs;
	_outSeg.Resize(_stheta, _srho);
	// _blobs.Resize(_stheta, _srho);

	YARPLogpolar _mapper;

	int _nHistos = 0;
	while (true)
	{
		_inPortImage.Read();
		
		if (_inPortSeg.Read(0))
		{
			ACE_OS::printf("Updating histogram\n");
			_leftSeg.Refer(_inPortSeg.Content());
			_mapper.ReconstructColor(_leftSeg, _leftSegColored);
			_histo.Apply(_leftSegColored);

			// dump histo
			char tmp[128];
			_nHistos++;
			sprintf(tmp, "%s%d", "y:\\zgarbage\\histo", _nHistos);
			_histo.dump(YARPString(tmp));
		}

		_left.Refer (_inPortImage.Content());

		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);

		_histo.backProjection(_leftColored, _outSeg);
		// _blobber.ApplyLp(_outSeg);
		
		// iplMultiplyS(_outSeg, _outSeg, 4);
				
		// _outPortSeg.Content().Refer(_blobber.getSegmented());
		_outPortSeg.Content().Refer(_outSeg);
		_outPortSeg.Write();

		/*_nHistos++;
		if ((_frame%100)==0)
			ACE_OS::printf("frame #%5d\r", _nHistos);*/
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

