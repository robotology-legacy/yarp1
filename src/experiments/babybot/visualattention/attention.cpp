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

///
/// $Id:
///
///



#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPPort.h>
#include <YARPImages.h>
#include <YARPList.h>

#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>

#include <YARPParseParameters.h>

#include <./conf/YARPVocab.h>

#include <YARPBottle.h>
#include <YARPBottleContent.h>

#include <YARPImages.h>
#include <YARPLogpolar.h>

#include <YARPMatrix.h>
#include <YARPVectorPortContent.h>

#include "ImgAtt.h"

#define DBGPF1 if (0)

#define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)


using namespace _logpolarParams;

///
/// global params.
char _inName1[512];
char _inName2[512];
char _inName3[512];
char _outName1[512];
char _outName2[512];
char _outName3[512];
char _netname0[512];
char _netname1[512];

const int numBoxes = 5;

YARPImgAtt att_mod(_stheta, _srho, _sfovea, numBoxes);

class mainthread : public YARPThread
{
public:
	virtual void Body (void);
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

	while (!IsTerminated()) {
		inVector.Read();
		YVector &joints = inVector.Content();
		//cout<<joints(1)<<" "<<joints(2)<<" "<<joints(3)<<" "<<joints(4)<<" "<<joints(5)<<endl;
		//cout<<joints.Length()<<endl;
		_lock();
		att_mod.setPosition(joints);
		_unlock();
	}
}

void mainthread::Body (void)
{
	YARPGenericImage img;
	YARPGenericImage imgOld;
	YARPImageOf<YarpPixelMono> tmp;

	YARPImageOf<YarpPixelMono> out;
	YARPImageOf<YarpPixelMono> out2;
	YARPImageOf<YarpPixelMono> out3;
	YARPImageOf<YarpPixelBGR> colored_s;
	YARPImageOf<YarpPixelBGR> colored_u;
	YARPImageOf<YarpPixelBGR> col_cart;
	YARPInputPortOf<YARPGenericImage> inImage(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPInputPortOf<YARPBottle> inBottle(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	//YARPInputPortOf<YVector> inVector;
	DeclareOutport(outImage);
	DeclareOutport(outImage2);
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

	double salienceBU;
	double salienceTD;
	
	
	colored_s.Resize(_stheta, _srho);
	colored_u.Resize(_stheta, _srho);
	tmp.Resize(_stheta, _srho);
	out.Resize(_stheta, _srho);
	out2.Resize(_stheta, _srho);
	out3.Resize(_xsize, _ysize);
	col_cart.Resize(_xsize, _ysize);

	//YARPConvKernelFile::Write("./prewitt.yck\0",prewitt);
	
	inImage.Register(_inName1, _netname1);
	inBottle.Register(_inName2, _netname0);
	//inVector.Register(_inName3, _netname0);
	outImage.Register(_outName1, _netname1);
	outImage2.Register(_outName2, _netname1);
	outBottle.Register(_outName3, _netname0);

	int frame_no = 0;

	if (!inImage.Read())
		ACE_OS::printf(">>> ERROR: frame not read\n");

	imgOld.Refer(inImage.Content());
	//iplRShiftS(imgOld, imgOld, 1);

	//start = YARPTime::GetTimeAsSeconds();
	
	bool isStarted = true;
	
	//att_mod.setParameters(109, 0, 18, 0, 1);
	
	while (!IsTerminated())
	{
		
		YARPTime::DelayInSeconds(0.020);
		////////// HANDLE MESSAGES
		////// wait untill start message is received
		if (inBottle.Read(0))
		{
			// parse message
			ACE_OS::printf("Port received:");
			YARPBottle &bottle = inBottle.Content();
			bottle.display();
			YBVocab message;
			if (bottle.tryReadVocab(message))
			{
				ACE_OS::printf("Message received: ");
				if (message == YBVVAStart)
				{
					ACE_OS::printf("starting\n");
					isStarted = true;
				}
				else if (message == YBVVAStop)
				{
					ACE_OS::printf("stopping\n");
					isStarted = false;
				}
				else if (message == YBVVASet)
				{
					ACE_OS::printf("set new parameters\n");
					bottle.readInt(&searchRG);
					bottle.readInt(&searchGR);
					bottle.readInt(&searchBY);
					
					bottle.readFloat(&salienceBU);
					bottle.readFloat(&salienceTD);

					att_mod.setParameters(searchRG, searchGR, searchBY, salienceBU, salienceTD);
				}
				else if (message == YBVVAUpdateIORTable)
				{
					ACE_OS::printf("updating IOR table\n");
					att_mod.updateIORTable();
				}
				else if (message == YBVVAResetIORTable)
				{
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
		if (isStarted)
		{
			tmpBottle.reset();

			/*inVector.Read();
			YVector &joints = inVector.Content();
			//cout<<joints(1)<<" "<<joints(2)<<" "<<joints(3)<<joints(4)<<" "<<joints(5)<<endl;
			att_mod.setPosition(joints);*/
			
			if (inImage.Read())
				cur = YARPTime::GetTimeAsSeconds();
			else
				ACE_OS::printf(">>> ERROR: frame not read\n");
		
			img.Refer(inImage.Content());
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

			// 0.25*frame corrente + 0.75*frame precedente
			iplRShiftS(imgOld, tmp, 2);
			iplSubtract(imgOld, tmp, tmp);
			iplRShiftS(img, imgOld, 2);
			iplAdd(tmp, imgOld, img);

			frame_no++;

			DBGPF1 ACE_OS::printf(">>> reconstruct colors\n");
			//mapper.ReconstructColor((const YARPImageOf<YarpPixelMono>&)imgOld, colored_s);
			mapper.ReconstructColor((const YARPImageOf<YarpPixelMono>&)img, colored_s);

			imgOld.Refer(img);

			//ACE_OS::sprintf(savename, "./col.ppm\0");
			//YARPImageFile::Write(savename, colored);
			
			mapper.Sawt2Uniform(colored_s, colored_u);
			//mapper.Sawt2TriCentral(colored_s, colored_u);
			
			DBGPF1 ACE_OS::printf(">>> call attention module\n");
			att_mod.Apply(colored_u);
			out.Refer(att_mod.Saliency());
			out2.Refer(att_mod.BlobFov());
		
			//att_mod.FindNMax(7, pos_max);

			//ACE_OS::printf("max position: x=%d y=%d max=%d\n", pos_x, pos_y, max);
			//DBGPF1 ACE_OS::printf("max position: x=%d y=%d\n", pos_max[0].centroid_x, pos_max[0].centroid_y);

			//ARRONZAMENTO
			YARPImageUtils::SetRed(out, colored_u);
			mapper.Uniform2Sawt(colored_u, colored_s);
			YARPImageUtils::GetRed(colored_s, out);

			YARPImageUtils::SetRed(out2, colored_u);
			mapper.Uniform2Sawt(colored_u, colored_s);
			YARPImageUtils::GetRed(colored_s, out2);
			
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

			outImage.Content().Refer(out);
			outImage.Write();

			outImage2.Content().Refer(out2);
			outImage2.Write();
			
			// next target
			tmpBottle.writeInt(att_mod.max_boxes[0].centroid_x);
			tmpBottle.writeInt(att_mod.max_boxes[0].centroid_y);
			tmpBottle.writeInt(att_mod.max_boxes[0].meanRG);
			tmpBottle.writeInt(att_mod.max_boxes[0].meanGR);
			tmpBottle.writeInt(att_mod.max_boxes[0].meanBY);
			// blob in fovea
			tmpBottle.writeInt(att_mod.fovBox.meanRG);
			tmpBottle.writeInt(att_mod.fovBox.meanGR);
			tmpBottle.writeInt(att_mod.fovBox.meanBY);
			outBottle.Content() = tmpBottle;
			outBottle.Write();

			//v(1) = pos_max[0].centroid_x;
			//v(2) = pos_max[0].centroid_y;
			//out_point.Content() = v;
			//out_point.Write();

			start = start + (YARPTime::GetTimeAsSeconds() - cur);

			if ((frame_no % 100) == 0) {
				cur = YARPTime::GetTimeAsSeconds();
				//ACE_OS::fprintf(stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/frame_no, frame_no);
				ACE_OS::fprintf(stdout, "average frame time: %f frame #%d acquired\r", start/frame_no, frame_no);
				//start = cur;
			}
		}
	}

	ACE_OS::fprintf(stdout, "returning smoothly\n");
}


int main (int argc, char *argv[])
{
	char basename[80]="/";
	YARPParseParameters::parse(argc, argv, "name", basename);

	ACE_OS::sprintf(_inName1, "/visualattention%s/i:img",basename);
	ACE_OS::sprintf(_inName2, "/visualattention%s/i:bot", basename);
	ACE_OS::sprintf(_inName3, "/visualattention%s/i:vec", basename);
	ACE_OS::sprintf(_outName1, "/visualattention%s/o:img", basename);
	ACE_OS::sprintf(_outName2, "/visualattention%s/o:img2", basename);
	ACE_OS::sprintf(_outName3, "/visualattention%s/o:bot", basename);
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

			cout<<"Searching for blobs similar to that in the fovea in this moment"<<endl;
			cout<<"mRG: "<<(int)mRG<<", mGR: "<<(int)mGR<<", mBY: "<<(int)mBY<<endl;
			att_mod.setParameters(mRG, mGR, mBY, 0, 1);
		} else if (c=='e') {
			cout<<"Exploring the scene"<<endl;
			att_mod.setParameters(0, 0, 0, 1, 0);
		} else if (c=='u') {
			ACE_OS::printf("updating IOR table\n");
			att_mod.updateIORTable();
		} else
			cout << "Type q+return to quit" << endl;
	} while (c != 'q');

	_thread.End(-1);
	_thread2.End(-1);

	return YARP_OK;
}
