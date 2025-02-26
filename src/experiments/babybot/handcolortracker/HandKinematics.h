#ifndef __HANDKINEMATICS__
#define __HANDKINEMATICS__

#include <yarp/YARPLogFile.h>
#include <yarp/YARPConicFitter.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPBPNNet.h>
#include <yarp/RFNet.h>
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

struct trPoint
{
	int x;
	int y;
};

#include <vector>

class ArmTrajectory: public YARPInputPortOf<YARPBabyBottle>
{
public:
	ArmTrajectory(): YARPInputPortOf<YARPBabyBottle>(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
	{
		YARPString tmp = "/handtracker/trajectory/i";
		Register(tmp.c_str());
		pImage = NULL;

		image1.Resize(_logpolarParams::_xsize/2, _logpolarParams::_ysize/2);
		image2.Resize(_logpolarParams::_xsize/2, _logpolarParams::_ysize/2);

	}

	virtual void OnRead(void)
	{
		YBVocab	tmpVocab;

		Read();
		YARPBabyBottle &tmpBottle = Content();

		if (tmpBottle.tryReadVocab(tmpVocab))
			tmpBottle.moveOn();
		
		if (tmpVocab == YBVReachingReach)
		{
			// start acquisition
			printf("----> Starting acquisition\n");
			center.clear();
			fingers.clear();
			acquisition = true;
			
			if(pImage != NULL)
				image1 = *pImage;
		}
		else
		if (tmpVocab == YBVReachingDone)
		{
			// stop
			printf("----> Acquisition stopped\n");
			if(pImage != NULL)
				image2 = *pImage;
			
			acquisition = false;
			// dump to file
			dump("c:/center.txt", center);
			dump("c:/fingers.txt", fingers);

			YARPImageFile::Write("c:/start.ppm", image1);
			YARPImageFile::Write("c:/end.ppm", image2);
		}
		return;
	}

	void update(int cx, int cy, int fx, int fy)
	{
		if (acquisition)
		{
			trPoint tmpC,tmpF;
			tmpC.x = cx;
			tmpC.y = cy;

			tmpF.x = fx;
			tmpF.y = fy;

			center.push_back(tmpC);
			fingers.push_back(tmpF);

		}

	}

	void plotTrajectory(YARPImageOf<YarpPixelBGR> &img, YarpPixelBGR &color1, YarpPixelBGR &color2)
	{
		#ifndef DO_NOT_DRAW
			plotTrajectory(center, img, color1);
			plotTrajectory(fingers, img, color2);
		#endif
	}

	void setImagePointer(YARPImageOf<YarpPixelBGR> &img)
	{
		pImage = &img;
	}

private:
	inline void plotTrajectory(const std::vector<trPoint> &v, YARPImageOf<YarpPixelBGR> &img, YarpPixelBGR &color)
	{
		
		for (int i = 0; i < v.size(); i++)
		{
			int x = v[i].x;
			int y = v[i].y;
			YARPSimpleOperation::DrawCross(img, x, y, color, 1, 1);
		}

	}

	inline void dump(const char *fn, const std::vector<trPoint> &v)
	{
		FILE *fp = fopen(fn, "w");
		for (int i = 0; i < v.size(); i++)
		{
			int x = v[i].x;
			int y = v[i].y;
			fprintf(fp, "%d\t%d\n", x, y);
		}
		fclose(fp);
	}

	std::vector<trPoint> center;
	std::vector<trPoint> fingers;

	bool acquisition;

	YARPImageOf<YarpPixelBGR> *pImage;
	YARPImageOf<YarpPixelBGR> image1;
	YARPImageOf<YarpPixelBGR> image2;
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

		YVector tmp(3);
		tmp(1) = _armPredictionPort._position(1);
		tmp(2) = _armPredictionPort._position(2);
		tmp(3) = _armPredictionPort._position(3);
//		_rfnet.Simulate(tmp, 0.001, _v);

		// compute retinal position
		int predx = 0, predy = 0;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _v, predx, predy);
		// predx+=10;
		// predy+=10;

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
//		_center.load(p);
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
	/*	YVector tmp(3);
		tmp(1) = arm(1);
		tmp(2) = arm(2);
		tmp(3) = arm(3);*/
//		_rfnet.Simulate(tmp, 0.001, _v);

		// compute retinal position
		int predx = 0, predy = 0;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _v, predx, predy);
	//	predx+=10;
	//	predy+=10;

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

//	RFNet	   _rfnet;

	ArmPredictionPort _armPredictionPort;

	YARPBabybotHeadKin _gaze;
	YVector _v;
	YVector _head;
	YVector _arm;
	int _npoints;
};

#endif