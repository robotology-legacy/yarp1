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

#include <YARPMath.h>

#include "ImgAtt.h"

#define DBGPF1 if (0)

#define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)


using namespace _logpolarParams;

///
/// global params.
char _name1[512];
char _name2[512];
char _name3[512];
char _name4[512];
char _name5[512];
char _netname[512];

YARPImgAtt att_mod(_stheta, _srho, _sfovea);

class mainthread : public YARPThread
{
public:
	virtual void Body (void);
};


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
	YARPInputPortOf<YARPGenericImage> inport(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	DeclareOutport(imageOut);
	DeclareOutport(imageOut2);
	YARPOutputPortOf<YVector> out_point;
	YARPInputPortOf<YARPBottle> _controlPort(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	YARPOutputPortOf<YARPBottle> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);

	YARPLogpolar mapper;

	double start = 0;
	double cur;
	
	char savename[512];

	YARPBox* pos_max;

	YVector v(2);

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

	//temp_16.Resize(_stheta, _srho, -2);

	//YARPConvKernelFile::Write("./prewitt.yck\0",prewitt);
	
	imageOut.Register(_name1, _netname);
	inport.Register(_name2, _netname);
	imageOut2.Register(_name3, _netname);
	_controlPort.Register(_name4, _netname);
	_outPort.Register(_name5, _netname);

	int frame_no = 0;

	const int numBoxes = 7;
	
	pos_max = new YARPBox[numBoxes];

	if (!inport.Read())
		ACE_OS::printf(">>> ERROR: frame not read\n");

	imgOld.Refer(inport.Content());
	iplRShiftS(imgOld, imgOld, 1);

	//start = YARPTime::GetTimeAsSeconds();
	
	bool isStarted = true;
	
	while (!IsTerminated())
	{
		
		////////// HANDLE MESSAGES
		////// wait untill start message is received
		if (_controlPort.Read(0))
		{
			// parse message
			ACE_OS::printf("Port received:");
			YARPBottle &bottle = _controlPort.Content();
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
					att_mod.updateIORTable(numBoxes, pos_max);
				}
				else if (message == YBVVAResetIORTable)
				{
					ACE_OS::printf("resetting IOR table\n");
					att_mod.resetIORTable(numBoxes, pos_max);
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

			if (inport.Read())
				cur = YARPTime::GetTimeAsSeconds();
			else
				ACE_OS::printf(">>> ERROR: frame not read\n");
		
			img.Refer(inport.Content());
			DBGPF1 ACE_OS::printf(">>> got a frame\n");

			iplRShiftS(img, img, 1);
			iplAdd(img, imgOld, imgOld);

			/*iplRShiftS(img, img, 1);
			iplRShiftS(imgOld, imgOld, 1);
			iplAdd(img, imgOld, imgOld);*/

			/*iplRShiftS(img, tmp, 2);
			iplSubtract(img, tmp, tmp);
			iplRShiftS(imgOld, imgOld, 2);
			iplAdd(tmp, imgOld, imgOld);*/

			frame_no++;

			DBGPF1 ACE_OS::printf(">>> reconstruct colors\n");
			mapper.ReconstructColor((const YARPImageOf<YarpPixelMono>&)imgOld, colored_s);
			//mapper.ReconstructColor((const YARPImageOf<YarpPixelMono>&)img, colored_s);

			imgOld.Refer(img);

			//ACE_OS::sprintf(savename, "./col.ppm\0");
			//YARPImageFile::Write(savename, colored);
			
			mapper.Sawt2Uniform(colored_s, colored_u);
			//mapper.Sawt2TriCentral(colored_s, colored_u);
			
			DBGPF1 ACE_OS::printf(">>> call attention module\n");
			att_mod.Apply(colored_u, numBoxes, pos_max);
			out.Refer(att_mod.Saliency());
			out2.Refer(att_mod.BlobFov());
		
			//att_mod.FindNMax(7, pos_max);

			//ACE_OS::printf("max position: x=%d y=%d max=%d\n", pos_x, pos_y, max);
			DBGPF1 ACE_OS::printf("max position: x=%d y=%d\n", pos_max[0].centroid_x, pos_max[0].centroid_y);

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

			imageOut.Content().Refer(out);
			imageOut.Write();

			imageOut2.Content().Refer(out2);
			imageOut2.Write();
			
			tmpBottle.writeInt(pos_max[0].centroid_x);
			tmpBottle.writeInt(pos_max[0].centroid_y);
			tmpBottle.writeInt(pos_max[0].meanRG);
			tmpBottle.writeInt(pos_max[0].meanGR);
			tmpBottle.writeInt(pos_max[0].meanBY);
			_outPort.Content() = tmpBottle;
			_outPort.Write();

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

	ACE_OS::sprintf(_name1, "/visualattention%s/o:img",basename);
	ACE_OS::sprintf(_name2, "/visualattention%s/i:img", basename);
	ACE_OS::sprintf(_name3, "/visualattention%s/o:img2", basename);
	ACE_OS::sprintf(_name4, "/visualattention%s/i:bottle", basename);
	ACE_OS::sprintf(_name5, "/visualattention%s/o:bottle", basename);
	ACE_OS::sprintf(_netname, "Net1");

	YARPScheduler::setHighResScheduling();

	mainthread _thread;
	_thread.Begin();

	char c = 0;

	do
	{
		cin >> c;
		if (c<='9' && c>='0')
			att_mod.setWatershedTh((c-'0')*3);
		else
			cout << "Type q+return to quit" << endl;
	}
	while (c != 'q');

	_thread.End(-1);

	return YARP_OK;
}
