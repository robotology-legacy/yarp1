/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                            #bremen#
///
///     "Licensed under the Academic Free License Version 1.0"
///


#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigRobot.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <ipl/ipl.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPList.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPImages.h>
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPMatrix.h>

#include <iostream>
using namespace std;

#include "ImgAtt.h"

#define DBGPF1 if (0)

#define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)


using namespace _logpolarParams;

///
/// global params.
char _inName1[512];
char _inName2[512];
char _inName3[512];
char _inName4[512];
char _outName1[512];
char _outName2[512];
char _outName3[512];
char _outName4[512];
char _netname0[512];
char _netname1[512];

const int numBoxes = 5;
bool isMoving;

YARPImgAtt att_mod(_stheta, _srho, _sfovea, numBoxes);

class mainthread : public YARPThread
{
public:
	virtual void Body (void);
	bool searching;
};


class secondthread : public YARPThread
{
	void inline _lock()
	{ _sema.Wait(); }

	void inline _unlock()
	{ _sema.Post(); }

public:
	secondthread():
	  _sema(1)
	  {}
	virtual void Body (void);
	YARPSemaphore _sema;
};


void secondthread::Body(void)
{
	YARPInputPortOf<YVector> inVector;
	
	inVector.Register(_inName3, _netname0);

	inVector.Read();
	YVector jointsOld = inVector.Content();
	printf("Head vector connected!\n");
	
	while (!IsTerminated()) {
		double diffJoints=0;
		inVector.Read();
		YVector &joints = inVector.Content();
		
		//cout<<joints(1)<<" "<<joints(2)<<" "<<joints(3)<<" "<<joints(4)<<" "<<joints(5)<<endl;
		//cout<<joints.Length()<<endl;
		bool changed=false;
		for (int i=1; i<=5;i++)
			if (fabs(joints(i)-jointsOld(i))>diffJoints) {
				diffJoints=fabs(joints(i)-jointsOld(i));
				changed=true;
			}
		/*if (changed)
			cout<<diffJoints<<endl;*/
		if (diffJoints<0.0001)
			isMoving=false;
		else
			isMoving=true;
		
		_lock();
		att_mod.setPosition(joints);
		_unlock();
		jointsOld = joints;
	}
}

void mainthread::Body (void)
{
	YARPImageOf<YarpPixelMono> img;
	YARPImageOf<YarpPixelMono> imgOld;
	YARPImageOf<YarpPixelMono> tmp;
	YARPImageOf<YarpPixelMono> tmp2;

	YARPImageOf<YarpPixelMono> out;
	YARPImageOf<YarpPixelMono> out2;
	YARPImageOf<YarpPixelBGR> out3;
	YARPImageOf<YarpPixelBGR> colored_s;
	YARPImageOf<YarpPixelBGR> colored_u;
	YARPImageOf<YarpPixelBGR> col_cart;
	YARPInputPortOf<YARPGenericImage> inImage(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPInputPortOf<YARPBottle> inBottle(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	YARPInputPortOf<YVector> inVector;
	DeclareOutport(outImage);
	DeclareOutport(outImage2);
	//DeclareOutport(outImage3);
	YARPOutputPortOf<YARPBottle> outBottle(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);
	//YARPOutputPortOf<YVector> out_point;
	
	YARPLogpolar mapper;

	double start = 0;
	double cur;
	
	char savename[512];

	//YARPBox* pos_max;

	//YVector v(2);

	YARPConicFitter fit;
	YARPLpConicFitter fitlp;

	YARPBottle tmpBottle;
	
	//int bfX0, bfY0;
	//double bfA11, bfA12, bfA22;

	int searchRG;
	int searchGR;
	int searchBY;

	double cmp, ect;

	double salienceBU, salienceTD;



	colored_s.Resize(_stheta, _srho);
	colored_u.Resize(_stheta, _srho);
	tmp.Resize(_stheta, _srho);
	tmp2.Resize(_stheta, _srho);
	out.Resize(_stheta, _srho);
	out2.Resize(_stheta, _srho);
	out3.Resize(_xsize, _ysize);
	col_cart.Resize(_xsize, _ysize);

	inImage.Register(_inName1, _netname1);
	inBottle.Register(_inName2, _netname0);
	inVector.Register(_inName4, _netname0);
	
	outImage.Register(_outName1, _netname1);
	outImage2.Register(_outName2, _netname1);
	outBottle.Register(_outName3, _netname0);
	//outImage3.Register(_outName4, _netname1);

	int frame_no = 0;
	//bool moved = true;
	int moved = 5;
	bool targetFound = false;
	bool diffFound = true;
	bool diffFoundValid = true;

	if (!inImage.Read())
		ACE_OS::printf(">>> ERROR: frame not read\n"); // to stop the execution on this instruction
	YARPTime::DelayInSeconds(0.5);

	YarpPixelMono maxDiff;
	int maxMass;
	do {
		ACE_OS::printf("Inizialization of the motion detector...");
		maxDiff=0;
		maxMass=0;

		if (!inImage.Read())
			ACE_OS::printf(">>> ERROR: frame not read\n");

		img.Refer(inImage.Content());
		imgOld=img;
		
		// check for difference in "original" image or in grayscale?
		for (int i=0; i<90; i++)
		{
			int xm,ym;
			int mass;

			if (!inImage.Read())
				ACE_OS::printf(">>> ERROR: frame not read\n");
			img.Refer(inImage.Content());			

			iplSubtract(img, imgOld, tmp);
			iplSubtract(imgOld, img, tmp2);
			iplAdd(tmp2, tmp, tmp);
			
			for (int y=0; y<_srho; y++)
				for (int x=0; x<_stheta; x++)
					if (tmp(x,y)>maxDiff)
						maxDiff=tmp(x,y);
			
			att_mod.diffCenterOfMassAndMass(tmp, maxDiff, &xm, &ym, &mass);
			if (maxMass<mass) maxMass=mass;
						
			/*if (maxDiff==255) {
				ACE_OS::sprintf(savename, "./diff.ppm");
				YARPImageFile::Write(savename, tmp);
				ACE_OS::sprintf(savename, "./img.ppm");
				YARPImageFile::Write(savename, img);
				ACE_OS::sprintf(savename, "./imgold.ppm");
				YARPImageFile::Write(savename, imgOld);
			}*/

			imgOld=img;
		}

		if (maxDiff<127) {
			// Note that the control on the area should be done in the log polar plane,
			// while the center of mass should be in cartesian coordinate.
			// However in this way it is faster.
			for (i=0; i<90; i++)
			{
				int xm,ym;
				int mass;

				if (!inImage.Read())
					ACE_OS::printf(">>> ERROR: frame not read\n");
				img.Refer(inImage.Content());			

				iplSubtract(img, imgOld, tmp);
				iplSubtract(imgOld, img, tmp2);
				iplAdd(tmp2, tmp, tmp);
				
				att_mod.diffCenterOfMassAndMass(tmp, maxDiff, &xm, &ym, &mass);
				if (maxMass<mass) maxMass=mass;
							
				imgOld=img;
			}
		} else
			ACE_OS::printf("Warning! Max Diff=%d it is too high! I'm going to recalculate it.\n", maxDiff);

		if (maxMass>255*5*5)
			ACE_OS::printf("Warning! Max Mass=%d, it is too high! I'm going to recalculate it.\n", maxMass);
	} while (maxDiff>127 || maxMass>255*5*5);
	ACE_OS::printf("done! Max Diff=%d, Max Mass=%d\n", maxDiff, maxMass);
	
	//att_mod.setParameters(109, 0, 18, 0, 1);
	searching=false;
	bool isStarted = true;
			
	out2.Refer(att_mod.BlobFov());
	
	start = YARPTime::GetTimeAsSeconds();

	while (!IsTerminated())	{
		YARPTime::DelayInSeconds(0.010);
		////////// HANDLE MESSAGES
		////// wait untill start message is received
		if (inBottle.Read(0)) {
			// parse message
			ACE_OS::printf("Port received:");
			YARPBottle &bottle = inBottle.Content();
			bottle.display();
			YBVocab message;
			if (bottle.tryReadVocab(message)) {
				ACE_OS::printf("Message received: ");
				if (message == YBVVAStart) {
					ACE_OS::printf("starting\n");
					isStarted = true;
				} else if (message == YBVVAStop) {
					ACE_OS::printf("stopping\n");
					isStarted = false;
				} else if (message == YBVVASet) {
					ACE_OS::printf("set new parameters\n");
					bottle.readInt(&searchRG);
					bottle.readInt(&searchGR);
					bottle.readInt(&searchBY);
					
					bottle.readFloat(&cmp);
					bottle.readFloat(&ect);
					
					bottle.readFloat(&salienceBU);
					bottle.readFloat(&salienceTD);

					att_mod.setParameters(searchRG, searchGR, searchBY, salienceBU, cmp, ect, salienceTD);
				} else if (message == YBVVAUpdateIORTable) {
					ACE_OS::printf("updating IOR table\n");
					att_mod.updateIORTable();
				} else if (message == YBVVAResetIORTable) {
					ACE_OS::printf("resetting IOR table\n");
					att_mod.resetIORTable();
				}
				/*else if (message == YBVDump)
				{
					
				}*/
				else
					ACE_OS::printf("nothing done\n");

			}
		}
		/////////////////////////////////////
		//////// MAIN LOOP
		if (isStarted) {
			bool found;
			
			tmpBottle.reset();

			if (!inImage.Read())
				//cur = YARPTime::GetTimeAsSeconds();
			//else
				ACE_OS::printf(">>> ERROR: frame not read\n");
		
			img.Refer(inImage.Content());
			frame_no++;
			DBGPF1 ACE_OS::printf(">>> got a frame\n");

			/*iplRShiftS(img, img, 1);
			iplAdd(img, imgOld, imgOld);*/

			/*iplRShiftS(img, img, 1);
			iplRShiftS(imgOld, imgOld, 1);
			iplAdd(img, imgOld, imgOld);*/

			/*iplRShiftS(img, tmp, 2);
			iplSubtract(img, tmp, tmp);
			iplRShiftS(imgOld, imgOld, 2);
			iplAdd(tmp, imgOld, imgOld);*/

			//inVector.Read();
			//YVector &checkFix = inVector.Content();
			
			//if (checkFix(1)==1) {
			if (!isMoving) {
				
				//if (moved) {
				if (moved==5) {
					// to skip the difference test on the first stable frame
					// because I don't have a reference frame
					// and to skip the temporal filter for the same reason
					//moved = false;
					moved--;
				} else {
					if (moved>0) moved--;

					iplSubtract(img, imgOld, tmp);
					iplSubtract(imgOld, img, tmp2);
					iplAdd(tmp2, tmp, tmp2);

					// Posso
					//1) mettere a 1 tutti i pixel > di maxDiff, taggare
					//2) sottrarre maxDiff, filtrare e taggare
					// se c'è stato un movimento lo dovresti ricordare e spostarsi lì in ogni caso?
					diffFound = false;
					diffFoundValid = false;
					int x,y;
					int mass;
					att_mod.diffCenterOfMassAndMass(tmp2, maxDiff, &x, &y, &mass);
					if (mass>maxMass) {
						int cartx, carty;
						mapper.Logpolar2Cartesian(y, x, cartx, carty);
						diffFound = true;
						moved=5;
						targetFound = false;
						if (!searching && att_mod.isWithinRange(cartx, carty)) {
							out.Refer(tmp2);
							ACE_OS::printf("Valid difference detected at (%d,%d)\n",cartx,carty);
						
							tmpBottle.writeInt(cartx);
							tmpBottle.writeInt(carty);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							diffFoundValid = true;
						} else
							ACE_OS::printf("Difference detected at (%d,%d)\n",cartx,carty);
						goto endDiffCheck;
					}

					// 0.25*current frame + 0.75*previous frame
					iplRShiftS(imgOld, tmp, 2);
					iplSubtract(imgOld, tmp, tmp);
					iplRShiftS(img, imgOld, 2);
					iplAdd(tmp, imgOld, img);
				}

endDiffCheck:
				DBGPF1 ACE_OS::printf(">>> reconstruct colors\n");
				mapper.ReconstructColor((const YARPImageOf<YarpPixelMono>&)img, colored_s);
				mapper.Sawt2Uniform(colored_s, colored_u);
				//mapper.Sawt2TriCentral(colored_s, colored_u);
				
				if (diffFound) {
					att_mod.initMeanCol(colored_u);
					diffFound=false;
				}
				
				DBGPF1 ACE_OS::printf(">>> call attention module\n");
				found=(att_mod.Apply(colored_u) | targetFound);

				if (!diffFoundValid) {
					if (moved==0) {
						if (found) {
							ACE_OS::printf("No point: target found\n");
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							targetFound = true;
						} else {
							//ACE_OS::printf("Sending point: blob# %ld @ (%d,%d)\n", att_mod.max_boxes[0].id, (int)att_mod.max_boxes[0].centroid_x, (int)att_mod.max_boxes[0].centroid_y);
							tmpBottle.writeInt(att_mod.max_boxes[0].centroid_x);
							tmpBottle.writeInt(att_mod.max_boxes[0].centroid_y);
							tmpBottle.writeInt(att_mod.max_boxes[0].meanRG);
							tmpBottle.writeInt(att_mod.max_boxes[0].meanGR);
							tmpBottle.writeInt(att_mod.max_boxes[0].meanBY);
						}
					} else {
						ACE_OS::printf("Waiting to stabilize\n");
						tmpBottle.writeInt(-1);
						tmpBottle.writeInt(-1);
						tmpBottle.writeInt(-1);
						tmpBottle.writeInt(-1);
						tmpBottle.writeInt(-1);
					}

					// blob in fovea
					tmpBottle.writeInt(att_mod.fovBox.meanRG);
					tmpBottle.writeInt(att_mod.fovBox.meanGR);
					tmpBottle.writeInt(att_mod.fovBox.meanBY);
					//out = att_mod.Saliency();
					out.Refer(att_mod.Saliency());
					//ARRONZAMENTO
					YARPImageUtils::SetRed(out, colored_u);
					mapper.Uniform2Sawt(colored_u, colored_s);
					YARPImageUtils::GetRed(colored_s, out);
				}

				//att_mod.FindNMax(7, pos_max);

				//ACE_OS::printf("max position: x=%d y=%d max=%d\n", pos_x, pos_y, max);
				//DBGPF1 ACE_OS::printf("max position: x=%d y=%d\n", pos_max[0].centroid_x, pos_max[0].centroid_y);

				/*out2.Zero();
				memset(out2.GetRawBuffer(), 255, 50*((IplImage*)out2)->widthStep);
				fitlp.fitEllipse(out2, &bfX0, &bfY0, &bfA11, &bfA12, &bfA22);
				fitlp.plotEllipse(bfX0, bfY0, bfA11, bfA12, bfA22, out2, 127);
				ACE_OS::sprintf(savename, "./ellipse.ppm");
				YARPImageFile::Write(savename, out2);

				YARPImageUtils::SetRed(out2, colored_s);
				mapper.Logpolar2Cartesian(colored_s, col_cart);
				YARPImageUtils::GetRed(col_cart, out3);
				//fit.fitEllipse(out3, &bfX0, &bfY0, &bfA11, &bfA12, &bfA22);
				fit.plotEllipse(bfX0, bfY0, bfA11, bfA12, bfA22, out3, 127);
				ACE_OS::sprintf(savename, "./ellipse.ppm");
				YARPImageFile::Write(savename, out3);*/

				//start = start + (YARPTime::GetTimeAsSeconds() - cur);

				//ARRONZAMENTO
				YARPImageUtils::SetRed(out2, colored_u);
				mapper.Uniform2Sawt(colored_u, colored_s);
				YARPImageUtils::GetRed(colored_s, out2);
			} else {
				ACE_OS::printf("No point: the robot is moving\n");
				//moved = true;
				moved = 5;
				targetFound = false;
				tmpBottle.writeInt(-1);
				tmpBottle.writeInt(-1);
				tmpBottle.writeInt(-1);
				tmpBottle.writeInt(-1);
				tmpBottle.writeInt(-1);
				tmpBottle.writeInt(-1);
				tmpBottle.writeInt(-1);
				tmpBottle.writeInt(-1);
			}
			
			//imgOld.Refer(img);
			imgOld = img;

			outBottle.Content() = tmpBottle;
			outBottle.Write();

			outImage.Content().Refer(out);
			outImage.Write();

			outImage2.Content().Refer(out2);
			outImage2.Write();

			//mapper.Logpolar2Cartesian(att_mod.ColorQuantiz(), out3);
			//outImage3.Content().Refer(out3);
			//outImage3.Write();
			
			if ((frame_no % 100) == 0) {
				cur = YARPTime::GetTimeAsSeconds();
				ACE_OS::fprintf(stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/frame_no, frame_no);
				//ACE_OS::fprintf(stdout, "average frame time: %f frame #%d acquired\r", start/frame_no, frame_no);
				//start = cur;
			}
		}
	}

	ACE_OS::fprintf(stdout, "returning smoothly\n");
}


int main (int argc, char *argv[])
{
	char basename[80]="/";
	isMoving=false;
		
	YARPParseParameters::parse(argc, argv, "name", basename);

	ACE_OS::sprintf(_inName1, "/visualattention%s/i:img",basename);
	ACE_OS::sprintf(_inName2, "/visualattention%s/i:bot", basename);	//input commands
	ACE_OS::sprintf(_inName3, "/visualattention%s/i:vec", basename);	//motor 
	ACE_OS::sprintf(_inName4, "/visualattention%s/i:vec2", basename);	//checkfixation
	ACE_OS::sprintf(_outName1, "/visualattention%s/o:img", basename);
	ACE_OS::sprintf(_outName2, "/visualattention%s/o:img2", basename);
	ACE_OS::sprintf(_outName3, "/visualattention%s/o:bot", basename);
	ACE_OS::sprintf(_outName4, "/visualattention%s/o:img3", basename);
	ACE_OS::sprintf(_netname0, "Net0");
	ACE_OS::sprintf(_netname1, "Net1");

	YARPScheduler::setHighResScheduling();

	mainthread _thread;
	secondthread _thread2;

	_thread.Begin();
	_thread2.Begin();

	char c = 0;

	do {
		cin >> c;
		if (c<='9' && c>='0')
			att_mod.setWatershedTh((c-'0')*3);
		else if (c=='r') {
			cout<<"Resetting IOR table"<<endl;
			att_mod.resetIORTable();
		} else if (c=='s') {
			YarpPixelMono mRG=att_mod.fovBox.meanRG;
			YarpPixelMono mGR=att_mod.fovBox.meanGR;
			YarpPixelMono mBY=att_mod.fovBox.meanBY;

			double cmp=att_mod.fovBox.cmp;
			double ect=att_mod.fovBox.ect;

			cout<<"Searching for blobs similar to that in the fovea in this moment"<<endl;
			cout<<"mRG:"<<(int)mRG<<", mGR:"<<(int)mGR<<", mBY:"<<(int)mBY<<endl;
			cout<<"CMP:"<<cmp<<", ECT:"<<ect<<endl;
			att_mod.setParameters(mRG, mGR, mBY, cmp, ect, 0, 1);
			_thread.searching=true;
		} else if (c=='e') {
			cout<<"Exploring the scene"<<endl;
			att_mod.setParameters(0, 0, 0, 0, 0, 1, 0);
			_thread.searching=false;
		} else if (c=='u') {
			ACE_OS::printf("Updating IOR table\n");
			att_mod.updateIORTable();
		} else
			cout << "Type q+return to quit" << endl;
	} while (c != 'q');

	_thread.End(-1);
	_thread2.End(-1);

	return YARP_OK;
}
