#include "HandKinematics.h"

HandKinematics::HandKinematics():
_gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) )
{
	char *root = GetYarpRoot();
	char filename[256];
	char filename1[256];
	char filename2[256];

	// ACE_OS::sprintf (path, "%s/conf\babybot\\0", root);
	ACE_OS::sprintf (filename, "%s/conf/babybot/handforward.dat\0", root);
	ACE_OS::sprintf (filename1, "%s/conf/babybot/handfk1.ini\0", root);
	ACE_OS::sprintf (filename2, "%s/conf/babybot/handfk2.ini\0", root);
	
//	_log.append(filename);
	_npoints = 0;

	// 
	_v.Resize(3);
	// nnets
	if (_center.load(filename1)!=YARP_OK)
	{
		ACE_OS::printf("Error, cannot read neural network file %s", filename1);
		exit(-1);
	}
	if (_ellipse.load(filename2)!=YARP_OK)
	{
		ACE_OS::printf("Error, cannot read neural network file %s", filename2);
		exit(-1);
	}
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
}

void HandKinematics::learn(YVector &arm, YVector &head, YARPBottle &newPoint)
{
	YARPShapeEllipse ellipse;

	newPoint.readInt(&ellipse.rho);
	newPoint.readInt(&ellipse.theta);
	newPoint.readFloat(&ellipse.a11);
	newPoint.readFloat(&ellipse.a12);
	newPoint.readFloat(&ellipse.a22);
}
