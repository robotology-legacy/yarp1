#ifndef __HANDKINEMATICS__
#define __HANDKINEMATICS__

#include <yarp/YARPLogFile.h>
#include <yarp/YARPConicFitter.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPBPNNet.h>
#include <yarp/YARPBabybotHeadKin.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPBottleContent.h>
#include <yarp/YARPConfigRobot.h>

class ArmPredictionPort: public YARPInputPortOf<YARPBabyBottle>
{
public:
	ArmPredictionPort(): YARPInputPortOf<YARPBabyBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
	{
		YARPString tmp = "/handtracker/prediction/i";
		Register(tmp.c_str());

		_position.Resize(6);
		_position = 0.0;
	}

	virtual void OnRead(void)
	{
		YBVocab	tmpVocab;

		Read();
		YARPBabyBottle &tmpBottle = Content();

		if (tmpBottle.getID() != YBVMotorLabel)
			return;

		if (tmpBottle.tryReadVocab(tmpVocab))
			tmpBottle.moveOn();
		
		if (tmpVocab == YBVArmIssuedCmd)
			tmpBottle.readYVector(_position);
		else
			return;
	
		ACE_OS::printf("DEBUG: received a new final position\n");
	}

	YVector _position;
	
};

class HandKinematics
{
public:
	// f1 and f2 are nnet config files; even on a non-trained network
	// they are required to speficy the network structur (#input, #nlayers, #noutput...)
	// if weights and biases are not specified the nnets are randomly initialized
	HandKinematics(const char *f1, const char *f2);
	~HandKinematics();

	void learn(YVector &arm, YVector &head, YARPBabyBottle &newPoint);
	
	void query(YVector &arm, YVector &head, YARPShapeEllipse &el)
	{
		_query(arm, head, el);
	}

	YARPShapeEllipse query(YVector &arm, YVector &head)
	{
		YARPShapeEllipse el;
		query(arm, head, el);
		return el;
	}

	YARPShapeEllipse queryPrediction()
	{
		YARPShapeEllipse el;
		_center.sim(_armPredictionPort._position.data(), _v.data());

		// compute retinal position
		int predx = 0, predy = 0;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _v, predx, predy);

		double p[3];
		_ellipse.sim(_armPredictionPort._position.data(), p);

		el.x = predx;		
		el.y = predy;	
		el.a11 = p[0]; 
		el.a12 = p[1]; 
		el.a22 = p[2]; 

		return el;
	}

	void loadCenter(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading center nnet from memory:");
		_center.load(p);
	}

	void loadEllipse(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading ellipse nnet from memory:");
		_ellipse.load(p);
	}

	void prepareRemoteTrainData(YARPBabyBottle &input, YARPBabyBottle &out1, YARPBabyBottle &out2);
	void update(const YVector &arm, const YVector &head);
	
private:
	void _query(const YVector &arm, const YVector &head, YARPShapeEllipse &el)
	{
		// compute _v()
		_center.sim(arm.data(),	// uses only the first 3 joints
				    _v.data());

		// compute retinal position
		int predx = 0, predy = 0;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _v, predx, predy);

		//////////////////////////////////////

		double p[3];
		_ellipse.sim(arm.data(), p);

		el.x = predx;		
		el.y = predy;	
		el.a11 = p[0]; 
		el.a12 = p[1]; 
		el.a22 = p[2]; 
	}

	void _dumpToDisk(const YVector &arm, const YVector &head, const YARPShapeEllipse &ellipse);

	YARPLpConicFitter _fitter;
	YARPLogFile		  _log;
	
	YARPBPNNet _center;
	YARPBPNNet _ellipse;

	ArmPredictionPort _armPredictionPort;

	YARPBabybotHeadKin _gaze;
	YVector _v;
	YVector _head;
	YVector _arm;
	int _npoints;
};

#endif