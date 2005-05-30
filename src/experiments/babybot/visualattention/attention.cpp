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
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPScheduler.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPParseParameters.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPMatrix.h>

#include <iostream>
using namespace std;

#include "ImgAtt.h"

#define DBGPF1 if (0)

//#define NOGRASP

#define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)


using namespace _logpolarParams;


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
	bool exploring;
	bool learnObject;
	bool learnHand;
	bool learnNotObject;
	bool graspIt;
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
		// It must skip the 4th joint, right camera
		for (int i=1; i<=3;i++)
			if (fabs(joints(i)-jointsOld(i))>diffJoints) {
				diffJoints=fabs(joints(i)-jointsOld(i));
				changed=true;
			}
		if (fabs(joints(5)-jointsOld(5))>diffJoints) {
			diffJoints=fabs(joints(5)-jointsOld(5));
			changed=true;
		}

		if (diffJoints<0.001)
		//if (diffJoints<0.0005)
		//if (diffJoints<0.0001)
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
	YARPImageOf<YarpPixelMono> out3;
	//YARPImageOf<YarpPixelBGR> out3;
	YARPImageOf<YarpPixelBGR> colored_s;
	YARPImageOf<YarpPixelBGR> colored_u;
	YARPImageOf<YarpPixelBGR> col_cart;
	YARPInputPortOf<YARPGenericImage> inImage(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPInputPortOf<YARPBottle> inBottle(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	YARPInputPortOf<YVector> inVector;
	DeclareOutport(outImage);
	DeclareOutport(outImage2);
	DeclareOutport(outImage3);
	YARPOutputPortOf<YARPBottle> outBottle(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);
	//YARPOutputPortOf<YVector> out_point;
	
	YARPLogpolar mapper;

	double start = 0;
	double cur;
	
	char savename[512];

	YARPConicFitter fit;
	YARPLpConicFitter fitlp;

	YARPBottle tmpBottle;
	
	//int bfX0, bfY0;
	//double bfA11, bfA12, bfA22;


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
	outImage3.Register(_outName4, _netname1);

	const int frameToStabilize = 5;
	
	int frame_no = 0;
	int moved = frameToStabilize;
	bool targetFound = false;
	bool diffFound = true;
	bool diffFoundValid = true;
	bool mustMove = false;
	int toMem = 0;
	bool noOutput = true;
	bool isStarted = true;
	bool reaching=false;
	searching = false;
	exploring = true;
	learnObject = false;
	learnHand = false;
	learnNotObject = false;
	graspIt=false;
	int boxesMem = 0;
	int mRG;
	int mGR;
	int mBY;
	int reachingAttempt=0;
	int saccades=0;

	if (!inImage.Read())
		ACE_OS::printf(">>> ERROR: frame not read\n"); // to stop the execution on this instruction

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
		for (int i=0; i<90; i++) {
			int xm,ym;
			bool changed=false;

			if (!inImage.Read())
				ACE_OS::printf(">>> ERROR: frame not read\n");
			img.Refer(inImage.Content());			

			iplSubtract(img, imgOld, tmp);
			iplSubtract(imgOld, img, tmp2);
			iplAdd(tmp2, tmp, tmp);
			
			for (int y=0; y<_srho; y++)
				for (int x=0; x<_stheta; x++)
					if (tmp(x,y)>maxDiff) {
						maxDiff=tmp(x,y);
						changed=true;
					}

			if (changed)
				att_mod.diffCenterOfMassAndMass(tmp, maxDiff, &xm, &ym, &maxMass);

			imgOld=img;
		}

		if (maxDiff<127) {
			maxMass=255*2*2;
			
#if 0
			// Note that the control on the area should be done in the log polar plane,
			// while the center of mass should be in cartesian coordinate.
			// However in this way it is faster.
			for (i=0; i<90; i++) {
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
#endif
		} else
			ACE_OS::printf("Warning! Max Diff=%d it is too high! I'm going to recalculate it.\n", maxDiff);

		//if (maxMass>255*5*5)
		//	ACE_OS::printf("Warning! Max Mass=%d, it is too high! I'm going to recalculate it.\n", maxMass);
	} while (maxDiff>127 || maxMass>255*5*5);
	
	ACE_OS::printf("done! Max Diff=%d, Max Mass=%d\n", maxDiff, maxMass);
	
	out2.Refer(att_mod.getBlobFov());
	out3.Refer(att_mod.getObjectFov());
	
	start = YARPTime::GetTimeAsSeconds();

	while (!IsTerminated())	{
		YARPTime::DelayInSeconds(0.010);
		
		// HANDLE MESSAGES
		YBVocab message;
		do {
			if (inBottle.Read(0)) {
				YARPBottle &bottle = inBottle.Content();
				//bottle.display();
				if (bottle.tryReadVocab(message)) {
					ACE_OS::printf("Message received\n");
					if (message == YBVVAStart) {
						ACE_OS::printf("starting\n");
						isStarted = true;
					} else if (message == YBVVAStop) {
						ACE_OS::printf("stopping\n");
						isStarted = false;
					} else if (message == YBVVAFreeze) {
						ACE_OS::printf("Freezing\n");
						noOutput=true;
					} else if (message == YBVVAUnFreeze) {
						ACE_OS::printf("Unfreezing\n");
						noOutput=false;
						mustMove=true;
					} else if (message == YBVVASet) {
						double cmp, ect;
						double salienceBU, salienceTD;

						ACE_OS::printf("set new parameters\n");
						bottle.readInt(&mRG);
						bottle.readInt(&mGR);
						bottle.readInt(&mBY);
						bottle.readFloat(&cmp);
						bottle.readFloat(&ect);
						bottle.readFloat(&salienceBU);
						bottle.readFloat(&salienceTD);
						att_mod.setParameters(mRG, mGR, mBY, salienceBU, cmp, ect, salienceTD);
					} else if (message == YBVVisMem) {
						toMem=1;
					} else if (message == YBVKFStart) {
						ACE_OS::printf("KF Starting...\n");
						noOutput=true;
						searching = false;
						exploring = true;
						att_mod.resetObject();
						att_mod.resetIORTable();
						reaching=false;
						reachingAttempt=0;
						if (learnHand)
							att_mod.resetHand();
					} else if (message == YBVKFTrainStart) {
						ACE_OS::printf("KF Training starting...\n");
						toMem=4;
					} else if (message == YBVKFTrainStop) {
						ACE_OS::printf("KF Training stopping...\n");
						toMem=0;
					} else if (message == YBVKFStop) {
						ACE_OS::printf("KF Stopping...\n");
						//mustMove=true;
						if (learnHand) {
							learnHand=false;
							att_mod.dumpLearnHand();
							att_mod.setParameters(0, 0, 0, 0, 0, 1, 0);
							searching=false;
							exploring=true;
						} else {
							att_mod.dumpLearnObject();
							if (boxesMem==0) {
								noOutput=true;
								ACE_OS::printf("I have not seen the object very well...\nPlese put it in my hand again.\n");
							} else {
								graspIt=true;
								noOutput=false;
							}
						}
					} else if (message == YBVReachingSuccess) {
						att_mod.setParameters(0, 0, 0, 0, 0, 1, 0);
						searching = false;
						exploring = true;
						noOutput=true;
						saccades=0;
						graspIt=false;
					} else if (message == YBVReachingFailure) {
						reachingAttempt++;
						if (reachingAttempt==3) {
							ACE_OS::printf("I give up!\n");
							reachingAttempt=0;
							att_mod.setParameters(0, 0, 0, 0, 0, 1, 0);
							searching = false;
							exploring = true;
							noOutput=true;
							graspIt=false;
						} else
							noOutput=false;
					} else if (message == YBVVAMove) {
						ACE_OS::printf("Moving to a new target\n");
						mustMove=true;
					} else if (message == YBVVAUpdateIORTable) {
						ACE_OS::printf("updating IOR table\n");
						att_mod.updateIORTable();
					} else if (message == YBVVAResetIORTable) {
						ACE_OS::printf("resetting IOR table\n");
						att_mod.resetIORTable();
					}
					else if (message == YBVVADump) {
						ACE_OS::printf("I'm searching:%d, %d, %d\n", mRG, mGR, mBY);
						ACE_OS::printf("Blob in the center:\n");
						ACE_OS::printf("areaCart: %lf\n",att_mod.fovBox.areaCart);
						ACE_OS::printf("meanRG: %d\n",(int)att_mod.fovBox.meanRG);
						ACE_OS::printf("meanGR: %d\n",(int)att_mod.fovBox.meanGR);
						ACE_OS::printf("meanBY: %d\n",(int)att_mod.fovBox.meanBY);
						ACE_OS::printf("error:%d\n", (mRG-att_mod.fovBox.meanRG)*(mRG-att_mod.fovBox.meanRG)+
							(mGR-att_mod.fovBox.meanGR)*(mGR-att_mod.fovBox.meanGR)+
							(mBY-att_mod.fovBox.meanBY)*(mBY-att_mod.fovBox.meanBY));
						att_mod.checkObject(img, 0.5);
						att_mod.dumpLearnObject();
						att_mod.dumpLearnHand();
					}
				}
			} 
		} while ((message!=YBVReachingAck && message!=YBVReachingAbort) && reaching);

		////////////////////
		if (reaching) {
			if (message==YBVReachingAbort) {
				ACE_OS::printf("-------------->Reaching aborted\n");
				mustMove=true;
				noOutput=false;
			} else if (message==YBVReachingAck) {
				ACE_OS::printf("-------------->Reaching started! YUU-HUU!!!\n");
				noOutput=true;
			}
		}
		reaching=false;
		// if I'm searching for a bottom-up target...
		if (exploring && targetFound) att_mod.setParameters(0, 0, 0, 0, 0, 1, 0);
		targetFound = false;
		////////////////////
		
		if (isStarted) {
			bool found;
			
			tmpBottle.reset();

			if (!inImage.Read())
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
				
				if (moved==frameToStabilize) {
					// to skip the difference test on the first stable frame
					// because I don't have a reference frame
					// and to skip the temporal filter for the same reason
					moved--;
				} else {
					if (moved>=0) moved--;

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
					// The robot pays attention only to moving objects in a window in the center
					if (mass>maxMass && (x>128-32 && x<128+32 && y>128-32 && y<128+32)) {
						int cartx, carty;
						mapper.Logpolar2Cartesian(y, x, cartx, carty);
						diffFound = true;
						
						moved=frameToStabilize;
												
						targetFound = false;

						// If the robot is searching the movement isn't displayed
						if (!searching && att_mod.isWithinRange(cartx, carty)) {
							out.Refer(tmp2);
													
							if (!noOutput) {
								ACE_OS::printf("Valid difference detected at (%d,%d)    \r",cartx,carty);
								tmpBottle.writeInt(cartx);
								tmpBottle.writeInt(carty);
								tmpBottle.writeInt(-1);
								outBottle.Content() = tmpBottle;
								outBottle.Write();
							} else 
								ACE_OS::printf("Valid difference detected, but freezed!\r");
							diffFoundValid = true;
						} else
							ACE_OS::printf("Difference detected at (%d,%d)          \r",cartx,carty);
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
				
				
#ifdef NUMBLOBLOG				
				char *root = GetYarpRoot();
				char logFile[256];
				ACE_OS::sprintf (logFile, "%s/zgarbage/va/log2.txt", root); 
				FILE *fp=ACE_OS::fopen(logFile,"a+");
				if (diffFound)
					ACE_OS::fprintf(fp, "1000\n");
				else
					ACE_OS::fprintf(fp, "0\n");
				ACE_OS::fclose(fp);
#endif
				
				if (diffFound) {
					att_mod.initMeanCol(colored_u);
					diffFound=false;
				}
				
				DBGPF1 ACE_OS::printf(">>> call attention module\n");
				// if I have found the target before and there isn't a difference, the
				// target is found again!
				found=(att_mod.Apply(colored_u, moved<=0) | targetFound) & searching & !mustMove;
				
				if (!diffFoundValid) {
					if (moved<=0 && toMem>0) {
						if (found | !searching) { // I'm searching and target blob found or this is the first time
							mRG=att_mod.fovBox.meanRG;
							mGR=att_mod.fovBox.meanGR;
							mBY=att_mod.fovBox.meanBY;

							double cmp=att_mod.fovBox.cmp;
							double ect=att_mod.fovBox.ect;

							cout<<"Searching for blobs similar to that in the fovea in this moment"<<endl;
							cout<<"mRG:"<<(int)mRG<<", mGR:"<<(int)mGR<<", mBY:"<<(int)mBY<<endl;
							cout<<"CMP:"<<cmp<<", ECT:"<<ect<<endl;
							att_mod.setParameters(mRG, mGR, mBY, cmp, ect, 0, 1);
							if (learnHand) {
								att_mod.learnHand();
							} else {
								boxesMem=att_mod.learnObject();
								att_mod.checkObject(img, 0);  //during learnig phase always shows the segmented object
							}
							toMem--;
							searching=true;
							targetFound = true;
							exploring = false;
							moved = 2;
							saccades=0;
						}
					} else if (moved==0) {
						// Special frame: now decision and data could be sent
						if (found) {
							mustMove=att_mod.checkObject(img, 0.5-(double)saccades/100)<(0.5-(double)saccades/100);
							//att_mod.dumpLearnObject();
							if (!mustMove) {
								ACE_OS::printf("Saccades: %d\n", saccades);
								if (!noOutput) {
									ACE_OS::printf("Target found, sending the center of the object\n");
									int xObj, yObj;
									att_mod.centerOfMassObject(&xObj, &yObj);
									ACE_OS::printf("Center of blob: (%d,%d)\nCenter of object: (%d,%d)\n",
										(int)att_mod.fovBox.centroid_x, (int)att_mod.fovBox.centroid_y,
										xObj,yObj);
									tmpBottle.writeInt(xObj);
									tmpBottle.writeInt(yObj);
#ifdef NOGRASP
									tmpBottle.writeInt(0);
#endif
									//tmpBottle.writeInt(att_mod.fovBox.centroid_x);
									//tmpBottle.writeInt(att_mod.fovBox.centroid_y);
									outBottle.Content() = tmpBottle;
									outBottle.Write();
									if (graspIt) reaching=true;
									noOutput=true;
								} else
									ACE_OS::printf("Target found but I'm freezed!\n");
							}
							targetFound = !mustMove;
						} else {
							out3.Zero();
							if (!noOutput) {
								ACE_OS::printf("Sending point: blob# %ld @ (%d,%d)\n", att_mod.max_boxes[0].id, (int)att_mod.max_boxes[0].centroid_x, (int)att_mod.max_boxes[0].centroid_y);
								if (exploring) {
									searching=true;
									YarpPixelMono mRG=att_mod.max_boxes[0].meanRG;
									YarpPixelMono mGR=att_mod.max_boxes[0].meanGR;
									YarpPixelMono mBY=att_mod.max_boxes[0].meanBY;

									double cmp=att_mod.fovBox.cmp;
									double ect=att_mod.fovBox.ect;

									att_mod.setParameters(mRG, mGR, mBY, cmp, ect, 0, 1);
								}

								tmpBottle.writeInt(att_mod.max_boxes[0].centroid_x);
								tmpBottle.writeInt(att_mod.max_boxes[0].centroid_y);
								tmpBottle.writeInt(0);
								outBottle.Content() = tmpBottle;
								outBottle.Write();
								saccades++;
							} else
								ACE_OS::printf("I should send a new point but I'm freezed!\n");
							mustMove = false;
						}
					} else if (moved==-1) {
						// Point already sended or not sended if target found
						if (found) {
							if (learnObject) {
								att_mod.learnObject();
								att_mod.checkObject(img, 0); //during learnig phase always shows the segmented object
								att_mod.dumpLearnObject();
								learnObject=false;
								saccades=0;
							}
						} else if (mustMove) {
							if (!noOutput) {
								if (exploring) {
									searching=true;
									YarpPixelMono mRG=att_mod.max_boxes[0].meanRG;
									YarpPixelMono mGR=att_mod.max_boxes[0].meanGR;
									YarpPixelMono mBY=att_mod.max_boxes[0].meanBY;

									double cmp=att_mod.fovBox.cmp;
									double ect=att_mod.fovBox.ect;

									att_mod.setParameters(mRG, mGR, mBY, cmp, ect, 0, 1);
								}

								ACE_OS::printf("Sending point: blob# %ld @ (%d,%d)\n", att_mod.max_boxes[0].id, (int)att_mod.max_boxes[0].centroid_x, (int)att_mod.max_boxes[0].centroid_y);
								tmpBottle.writeInt(att_mod.max_boxes[0].centroid_x);
								tmpBottle.writeInt(att_mod.max_boxes[0].centroid_y);
								tmpBottle.writeInt(0);
								outBottle.Content() = tmpBottle;
								outBottle.Write();
								saccades++;
							} else 
								ACE_OS::printf("I must move but I'm freezed!\n");
							mustMove = false;
						} else {
							/*tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);
							tmpBottle.writeInt(-1);*/
						}
					} else {
						//ACE_OS::printf("Waiting to stabilize\n");
					}

					// blob in fovea
					out.Refer(att_mod.Saliency());
					//ARRONZAMENTO
					YARPImageUtils::SetRed(out, colored_u);
					mapper.Uniform2Sawt(colored_u, colored_s);
					YARPImageUtils::GetRed(colored_s, out);
				}

				//DBGPF1 ACE_OS::printf("max position: x=%d y=%d\n", pos_max[0].centroid_x, pos_max[0].centroid_y);

				//start = start + (YARPTime::GetTimeAsSeconds() - cur);

				//ARRONZAMENTO
				YARPImageUtils::SetRed(out2, colored_u);
				mapper.Uniform2Sawt(colored_u, colored_s);
				YARPImageUtils::GetRed(colored_s, out2);
			} else {
				ACE_OS::printf("No point: the robot is moving\r");
				moved = frameToStabilize;
				targetFound = false;
			}
			
			//imgOld.Refer(img);
			imgOld = img;

			/*outBottle.Content() = tmpBottle;
			outBottle.Write();*/

			outImage.Content().Refer(out);
			outImage.Write();

			outImage2.Content().Refer(out2);
			/*if (moved==1) out2(0, 0)=127;
			else if (moved<=0) out2(0, 0)=80;*/
			outImage2.Write();

			//mapper.Logpolar2Cartesian(att_mod.ColorQuantiz(), out3);
			//MODIFY
			/*YARPImageUtils::SetRed(out3, colored_u);
			mapper.Uniform2Sawt(colored_u, colored_s);
			YARPImageUtils::GetRed(colored_s, out3);*/
			outImage3.Content().Refer(out3);
			outImage3.Write();
			
			if ((frame_no % 100) == 0 && noOutput) {
				cur = YARPTime::GetTimeAsSeconds();
				ACE_OS::fprintf(stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/frame_no, frame_no);
			}
		}
	}

	ACE_OS::fprintf(stdout, "returning smoothly\n");
}


int main (int argc, char *argv[])
{
	char filename[80]="";
	isMoving=false;
		
	if (YARPParseParameters::parse(argc, argv, "-file", filename)) {
		char name[80]="";
		char filename2[80]="";
		int found=0;
		int exa=0;
		int pos[100];
		int sum=0;
		
		YARPParseParameters::parse(argc, argv, "-mask", filename2);
		
		YARPImageOf<YarpPixelBGR> img;
		YARPImageOf<YarpPixelMono> msk;
		YARPImageOf<YarpPixelMono> out;
		YARPImageOf<YarpPixelMono> intersct;

		out.Resize(_stheta, _srho);
		intersct.Resize(_stheta, _srho);

		for (int l=0; l<100; l++) {
			ACE_OS::sprintf(name, "%s%04d_lp.ppm", filename, l);
			if (YARPImageFile::Read(name, img)>=0) {
				exa++;
				ACE_OS::printf("%s\n", name);
				
				att_mod.initMeanCol(img);
				att_mod.setWatershedTh(20);
				for (int i=0; i<5; i++) {
					att_mod.applyStatic(img, msk);
					out.Refer(att_mod.Saliency());
					//ACE_OS::sprintf(name, "out%2d_%2d.ppm", l,i);
					//YARPImageFile::Write(name, out);
				}
				ACE_OS::sprintf(name, "out%04d.ppm", l);
				YARPImageFile::Write(name, out);

				ACE_OS::sprintf(name, "%s%04d_lp.ppm", filename2, l);
				ACE_OS::printf("%s\n", name);
				if (YARPImageFile::Read(name, msk)>=0) {
					int p=att_mod.verifyMsk(out, msk, intersct);
					pos[exa-1]=p;
					sum+=p;
					if (p==0) found++;
					ACE_OS::printf("%d\n", p);
					ACE_OS::sprintf(name, "int%04d.ppm", l);
					YARPImageFile::Write(name, intersct);
				}
			}
		}
		ACE_OS::printf("%d/%d\n", found, exa);
		//for (l=0; l<exa; l++)
		//	ACE_OS::printf("%d\n", pos[l]);
		ACE_OS::printf("mean: %lf\n", (double)sum/exa);

		//att_mod.saveImages();
	} else {	
		char basename[80]="/";

		YARPParseParameters::parse(argc, argv, "-name", basename);
		
		ACE_OS::sprintf(_inName1, "/visualattention%s/i:img",basename);		//input log-polar img
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
				YarpPixelMonoSigned mRG=att_mod.fovBox.meanRG;
				YarpPixelMonoSigned mGR=att_mod.fovBox.meanGR;
				YarpPixelMonoSigned mBY=att_mod.fovBox.meanBY;

				double cmp=att_mod.fovBox.cmp;
				double ect=att_mod.fovBox.ect;

				cout<<"Searching for blobs similar to that in the fovea in this moment"<<endl;
				cout<<"mRG:"<<(int)mRG<<", mGR:"<<(int)mGR<<", mBY:"<<(int)mBY<<endl;
				cout<<"CMP:"<<cmp<<", ECT:"<<ect<<endl;
				att_mod.setParameters(mRG, mGR, mBY, cmp, ect, 0, 1);
				_thread.searching=true;
				_thread.exploring=false;
			} else if (c=='e') {
				cout<<"Exploring the scene"<<endl;
				att_mod.setParameters(0, 0, 0, 0, 0, 1, 0);
				_thread.searching=false;
				_thread.exploring=false;
			} else if (c=='E') {
				cout<<"Exploring the scene through grasping"<<endl;
				att_mod.setParameters(0, 0, 0, 0, 0, 1, 0);
				_thread.searching=false;
				_thread.exploring=true;
			} else if (c=='u') {
				ACE_OS::printf("Updating IOR table\n");
				att_mod.updateIORTable();
			} else if (c=='o') {
				ACE_OS::printf("Saving Mean Opponent Colors image\n");
				att_mod.saveMeanOppCol();
			} else if (c=='m') {
				ACE_OS::printf("Saving Mean Colors image\n");
				att_mod.saveMeanCol();
			} else if (c=='l') {
				ACE_OS::printf("Learn Object\n");
				_thread.learnObject=true;
			} else if (c=='h') {
				ACE_OS::printf("Learn Hand\n");
				_thread.learnHand=true;
			} else if (c=='n') {
				ACE_OS::printf("Learn counter example\n");
				_thread.learnNotObject=true;
			} else if (c=='i') {
				ACE_OS::printf("Saving all images\n");
				att_mod.saveImages();
			} else
				cout << "Type q+return to quit" << endl;
		} while (c != 'q');

		_thread.End(-1);
		_thread2.End(-1);
	}

	return YARP_OK;
}
