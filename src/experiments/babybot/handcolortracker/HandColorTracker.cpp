// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>

#include <YARPHistoSegmentation.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPConicFitter.h>

#include <YARPLogFile.h>

using namespace _logpolarParams;

YARPLogpolar _mapper;

class YARPHandSegmentation
{
public:
	YARPHandSegmentation (double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char n):
	  histo(max, min, n)
	  {}
	  YARPHandSegmentation (double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char *n):
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

class HandLocalization
{
public:
	HandLocalization():
	_armPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST)
	{
		_armPort.Register("/handtracker/i:arm");
		_armPosition.Resize(6);
		_headPosition.Resize(2);
		_r0 = 0;
		_t0 = 0;
		_a11 = 0;
		_a12 = 0;
		_a22 = 0;

		
 		char *root = GetYarpRoot();
		char filename[256];

		#if defined(__WIN32__)
			ACE_OS::sprintf (filename, "%s\\conf\\babybot\\handforward.dat\0", root);
		#elif defined (__QNX6__)
			ACE_OS::sprintf (path, "%s/conf\babybot\\0", root);
			ACE_OS::sprintf (filename, "%s/conf/babybot/handforward.dat\0", root);
		#endif
		_log.append(filename);
		_npoints = 0;

	}
	~HandLocalization()
	{
		_log.close();
	}

	void learn(YARPBottle &newPoint)
	{
		newPoint.readYVector(_armPosition);
		newPoint.readYVector(_headPosition);
		newPoint.readInt(&_t0);
		newPoint.readInt(&_r0);
		newPoint.readFloat(&_a11);
		newPoint.readFloat(&_a12);
		newPoint.readFloat(&_a22);

		_dumpToDisk();
	}

	void query(YARPImageOf<YarpPixelMono> &in)
	{
		if (_armPort.Read(0))
		{
			_armPosition = _armPort.Content()._current_position;
		}
		int r, t;
		double a11, a12, a22;
		_query(_armPosition, &t, &r, &a11, &a12, &a22);
		_fitter.plotEllipse(t, r, a11, a12, a22, in);
	}
	void _query(const YVector &position, int *t0, int *r0, double *a11, double *a12, double *a22)
	{
		*r0 = _r0;
		*t0 = _t0;
		*a11 = _a11;
		*a12 = _a12;
		*a22 = _a22;
	}

	void _dumpToDisk()
	{
		_log.dump(_armPosition);
		_log.dump(_headPosition);
		_log.dump(_r0);
		_log.dump(_t0);
		_log.dump(_a11);
		_log.dump(_a12);
		_log.dump(_a22);
		_log.newLine();

		_npoints++;

		printf("#%d got a new point\n", _npoints);
		if ((_npoints%10) == 0)
		{
			printf("#%d flushing points to disk\n", _npoints);
			_log.flush();
		}
	}

	YARPInputPortOf<YARPControlBoardNetworkData>  _armPort;
	YVector _armPosition;
	YVector _headPosition;
	YARPLpConicFitter _fitter;
	YARPLogFile		  _log;
	int _r0;
	int _t0;
	double _a11;
	double _a12;
	double _a22;

	int _npoints;
};

using namespace _logpolarParams;

int main(int argc, char* argv[])
{
	YARPLpHistoSegmentation _histo(65, 50, 255, 0, 15);
	YARPHandSegmentation _hand(65, 50, 255, 0, 15);

	YARPInputPortOf<YARPGenericImage> _inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> _inPortSeg(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	YARPOutputPortOf<YARPGenericImage> _outPortSeg(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	YARPInputPortOf<YARPBottle>  _armSegmentationPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPBlobDetector _blobber(5.0);
	_blobber.resize(_stheta, _srho, _sfovea);
	
	_inPortImage.Register("/handtracker/i:img", "Net1");
	_inPortSeg.Register("/handtracker/segmentation/i:img");
	_outPortSeg.Register("/handtracker/segmentation/o:img");
	_armSegmentationPort.Register("/handtracker/segmentation/i:armdata");
	

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

	HandLocalization _handLocalization;

	// _blobs.Resize(_stheta, _srho);

	char tmp[128];
	sprintf(tmp, "%s%d", "y:\\zgarbage\\exp10\\histo", 37);
//	_histo.load(YARPString(tmp));

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

		if(_armSegmentationPort.Read(0))
		{
			_armSegmentationPort.Content().display();
			_handLocalization.learn(_armSegmentationPort.Content());
		}

		_left.Refer (_inPortImage.Content());

		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);


		YARPColorConverter::RGB2HSV(_leftColored, _leftHSV);
		_histo.backProjection(_leftHSV, _outSeg);

		_handLocalization.query(_outSeg);

		_outPortSeg.Content().Refer(_outSeg);
		_outPortSeg.Write();
		

		/*_nHistos++;
		if ((_frame%100)==0)
			ACE_OS::printf("frame #%5d\r", _nHistos);*/
	}
	return 0;
}

