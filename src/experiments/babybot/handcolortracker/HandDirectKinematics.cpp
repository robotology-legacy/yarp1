#include "HandKinematics.h"

HandKinematics::HandKinematics()
{
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

HandKinematics::~HandKinematics()
{
	_log.close();
}

void HandKinematics::_dumpToDisk(const YVector &arm, const YVector &head, const YARPShapeEllipse &ellipse)
{
	_log.dump(arm);
	_log.dump(head);
	_log.dump(ellipse.rho);
	_log.dump(ellipse.theta);
	_log.dump(ellipse.a11);
	_log.dump(ellipse.a12);
	_log.dump(ellipse.a22);
	_log.newLine();

	_npoints++;

	printf("#%d got a new point\n", _npoints);
	if ((_npoints%10) == 0)
	{
		printf("#%d flushing points to disk\n", _npoints);
		_log.flush();
	}
}

void HandKinematics::learn(YVector &arm, YVector &head, YARPBottle &newPoint)
{
	YARPShapeEllipse ellipse;

	newPoint.readInt(&ellipse.rho);
	newPoint.readInt(&ellipse.theta);
	newPoint.readFloat(&ellipse.a11);
	newPoint.readFloat(&ellipse.a12);
	newPoint.readFloat(&ellipse.a22);

	_dumpToDisk(arm, head, ellipse);
}
