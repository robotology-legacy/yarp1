#include <YARPLogFile.h>
#include <YARPConicFitter.h>
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
		char filename1[256];
		char filename2[256];

		#if defined(__WIN32__)
			ACE_OS::sprintf (filename, "%s\\conf\\babybot\\handforward.dat\0", root);
			ACE_OS::sprintf (filename1, "%s\\conf\\babybot\\netc.ini\0", root);
			ACE_OS::sprintf (filename2, "%s\\conf\\babybot\\netp.ini\0", root);
		#elif defined (__QNX6__)
			ACE_OS::sprintf (path, "%s/conf\babybot\\0", root);
			ACE_OS::sprintf (filename, "%s/conf/babybot/handforward.dat\0", root);
			ACE_OS::sprintf (filename1, "%s/conf/babybot/netc.ini\0", root);
			ACE_OS::sprintf (filename2, "%s/conf/babybot/netp.ini\0", root);
		#endif
		_log.append(filename);
		_npoints = 0;

		// nnets
		center.load(filename1);
		parameters.load(filename2);

	}
	~HandLocalization()
	{
		_log.close();
	}

	void learn(YARPBottle &newPoint)
	{
		newPoint.readYVector(_armPosition);
		newPoint.readYVector(_headPosition);
		newPoint.readInt(&_r0);
		newPoint.readInt(&_t0);
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
		// _fitter.plotEllipse(t, r, a11, a12, a22, in);
	}

	void query(int *t, int *r, double *a11, double *a12, double *a22)
	{
		if (_armPort.Read(0))
		{
			_armPosition = _armPort.Content()._current_position;
		}
		_query(_armPosition, t, r, a11, a12, a22);
	}

	void _query(const YVector &position, int *t0, int *r0, double *a11, double *a12, double *a22)
	{
		double tmp[3];
		double c[2];
		double p[3];
		tmp[0] = position(1);
		tmp[1] = position(2);
		tmp[2] = position(3);
		center.sim(tmp, c);
		parameters.sim(tmp, p);
	
		*r0 = c[0];
		*t0 = c[1];
		*a11 = /*p[0];*/ 0.005;
		*a12 = /*p[1];*/ 0;
		*a22 = /*p[2];*/ 0.005;

		_npoints++;
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

	nnet center;
	nnet parameters;

	int _npoints;
};