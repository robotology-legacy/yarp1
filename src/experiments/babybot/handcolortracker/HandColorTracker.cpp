// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>

#include <YARPHistoSegmentation.h>
#include <YARPConicFitter.h>

using namespace _logpolarParams;

YARPLogpolar _mapper;

class YARPHandSegmentation
{
public:
	YARPHandSegmentation (double lumaTh, unsigned char max, unsigned char min, unsigned char n):
	  histo(max, min, n)
	  {}
	  YARPHandSegmentation (double lumaTh, unsigned char max, unsigned char min, unsigned char *n):
	  histo(max, min, n)
	  {}

	void search(YARPImageOf<YarpPixelHSV> &src, YARPImageOf<YarpPixelMono> &backpr, YARPImageOf<YarpPixelMono> &out, YARPLpHistoSegmentation &target, double R)
	{
		// perform complete search over the image
		// just plot image
		int r;
		int t;
		for(r = 0; r<_srho; r++)
			for(t = 0; t<_stheta; t++)
			{
				if ( backpr(t,r) > 250)
				{
					_fitter.findCircle(t, r, R, points);
					cumulateRegion(src, points);
					// now histo is the histogram of the current circle
					double p = intersect(target);
					out(t,r) = unsigned char (p*255 + 0.5);
				}
				else
					out(t,r) = 0;

			}
	}
		
	void cumulateRegion(YARPImageOf<YarpPixelHSV> &src, circle &points)
	{
		int m;
		histo.clean();
		for(m = 0; m < points.n; m++)
		{
			YarpPixelHSV pixel = src(points.t[m], points.r[m]);
			// later check weight
			histo.Apply(pixel.h, pixel.s, 0, 1);
		}
	}

	double intersect(YARPLpHistoSegmentation &target)
	{
		HistoEntry tmpG;
		HistoEntry tmpH;
			
		int it = 0;
		double sumG = 0.0;
		double sumH = 0.0;
		double sum = 0.0;
		while  ( (target.find(it, tmpG)!=-1) && (histo.find(it, tmpH)!=-1) )
		{
			double g = tmpG.value()/target.maximum();
			double h = tmpH.value()/histo.maximum();
			
			sum += (g-h)*(g-h);
			it++;
		}

		return sum;
	}

	circle points;
	YARP3DHistogram histo;
	YARPLpConicFitter _fitter;
};

using namespace _logpolarParams;

int main(int argc, char* argv[])
{
	YARPLpHistoSegmentation _histo(5, 255, 0, 10);
	YARPHandSegmentation _hand(5, 255, 0, 10);

	YARPInputPortOf<YARPGenericImage> _inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> _inPortSeg(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	YARPOutputPortOf<YARPGenericImage> _outPortSeg(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	YARPBlobDetector _blobber(5.0);
	_blobber.resize(_stheta, _srho, _sfovea);
	
	_inPortImage.Register("/handtracker/i:img", "Net1");
	_inPortSeg.Register("/handtracker/segmentation/i:img");
	_outPortSeg.Register("/handtracker/segmentation/o:img");

	YARPImageOf<YarpPixelMono> _left;
	YARPImageOf<YarpPixelMono> _leftSeg;
	YARPImageOf<YarpPixelBGR> _leftColored;
	YARPImageOf<YarpPixelBGR> _leftSegColored;
	YARPImageOf<YarpPixelHSV> _leftHSV;
	YARPImageOf<YarpPixelHSV> _leftSegHSV;
	_left.Resize(_stheta, _srho);
	_leftSeg.Resize(_stheta, _srho);
	_leftColored.Resize(_stheta, _srho);
	_leftSegColored.Resize(_stheta, _srho);
	_leftSegHSV.Resize(_stheta, _srho);
	_leftHSV.Resize(_stheta, _srho);

	YARPImageOf<YarpPixelMono> _outSeg;
	YARPImageOf<YarpPixelMono> _outSeg2;
	// YARPImageOf<YarpPixelMono> _blobs;
	_outSeg.Resize(_stheta, _srho);
	_outSeg2.Resize(_stheta, _srho);

	// _blobs.Resize(_stheta, _srho);

	char tmp[128];
	sprintf(tmp, "%s%d", "y:\\zgarbage\\exp10\\histo", 37);
	_histo.load(YARPString(tmp));

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

			YARPColorConverter::RGB2HSV(_leftSegColored, _leftSegHSV);
			_histo.Apply(_leftSegHSV);

			// dump histo
			char tmp[128];
			_nHistos++;
			sprintf(tmp, "%s%d", "y:\\zgarbage\\histo", _nHistos);
			_histo.dump(YARPString(tmp));
		}

		_left.Refer (_inPortImage.Content());

		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);
		
		YARPColorConverter::RGB2HSV(_leftColored, _leftHSV);
		_histo.backProjection(_leftHSV, _outSeg);

		// _outSeg.Zero();

		_hand.search(_leftHSV, _outSeg, _outSeg2, _histo, 20);
				
		_outPortSeg.Content().Refer(_outSeg2);
		_outPortSeg.Write();

		/*_nHistos++;
		if ((_frame%100)==0)
			ACE_OS::printf("frame #%5d\r", _nHistos);*/
	}
	return 0;
}

