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

using namespace _logpolarParams;

YARPLogpolar _mapper;

struct circle
{
	circle()
	{
		r = new int [_srho*_stheta];
		t = new int [_srho*_stheta];
	}
	~circle()
	{
		delete [] r;
		delete [] t;
	}

	void reset()
	{ n = 0; }

	void add(int T, int R)
	{ 
		r[n] = R;
		t[n] = T;
		n++;
	}

	int *r;
	int *t;
	int n;
};

void findCircle(int T0, int R0, double R, circle &out)
{
	out.reset();
	int theta;
	int rho2;
	int rho1;
	int r0;
	r0 = _mapper.CsiToRo(R0);
	for(theta = 0; theta < _logpolarParams::_stheta; theta++)
	{
		double c = cos((theta-T0)/_q);
		double DELTA = (r0*r0*(c*c-1) + R*R);
		if (DELTA>=0)
		{
			int r = (r0*c+sqrt(DELTA)) + 0.5;
			if (r > 0)
			{
				rho2 = _mapper.RoToCsi(r);
				if ((rho2>(_logpolarParams::_srho-1)))
				{
					rho2 = _logpolarParams::_srho-1;
				}
				else if (rho2<0)
					rho2 = 0;

				int p;
				for(p = r0; p<=rho2; p++)
					out.add(theta, p);
			}
			
			r = (r0*c-sqrt(DELTA)) + 0.5;
			if (r > 0)
			{
				rho1 = _mapper.RoToCsi(r);
				if ((rho1>(_logpolarParams::_srho-1)))
				{
					rho1 = _logpolarParams::_srho-1;
				}
				else if (rho1<0)
					rho1 = 0;
			
				int p;
				for(p = rho1; p<=r0; p++)
					out.add(theta, p);
			}
		}
	}
}

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
				if ( backpr(t,r) > 200)
				{
					findCircle(t, r, R, points);
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
		while  ( (target._3dlut.find(it, tmpG)!=-1) && (histo._3dlut.find(it, tmpH)!=-1) )
		{
			double g = tmpG.value()/target._maximum;
			double h = tmpH.value()/histo._maximum;
			
			sumG += g;
			sumH += h;

			if (g>h)
				sum += h;
			else
				sum += g;
			it++;
		}

		if (sumG>sumH)
			return sum/sumH;
		else
			return sum/sumG;
	}

	circle points;
	YARP3DHistogram histo;
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
				
		_outPortSeg.Content().Refer(_outSeg);
		_outPortSeg.Write();

		/*_nHistos++;
		if ((_frame%100)==0)
			ACE_OS::printf("frame #%5d\r", _nHistos);*/
	}
	return 0;
}

