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

#include <YARPImages.h>
#include <YARPLogpolar.h>

#include <YARPMath.h>

#include "ImgAtt.h"

#define DBGPF1 if (0)

#define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)


///
/// global params.
char _name1[512];
char _name2[512];
char _name3[512];
char _netname[512];


class mainthread : public YARPThread
{
public:
	virtual void Body (void);
};


void mainthread::Body (void)
{
	using namespace _logpolarParams;

	YARPImgAtt att_mod(_stheta, _srho, _sfovea);
	
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
	DeclareOutport(outport);
	DeclareOutport(outport2);
	YARPOutputPortOf<YVector> out_point;

	YARPLogpolar mapper;

	double start = 0;
	double cur;
	
	char savename[512];

	Vett* pos_max;

	YVector v(2);

	YARPConicFitter fit;
	YARPLpConicFitter fitlp;

	//int bfX0, bfY0;
	//double bfA11, bfA12, bfA22;

	
	colored_s.Resize(_stheta, _srho);
	colored_u.Resize(_stheta, _srho);
	tmp.Resize(_stheta, _srho);
	out.Resize(_stheta, _srho);
	out2.Resize(_stheta, _srho);
	out3.Resize(_xsize, _ysize);
	col_cart.Resize(_xsize, _ysize);

	//temp_16.Resize(_stheta, _srho, -2);

	//YARPConvKernelFile::Write("./prewitt.yck\0",prewitt);
	
	ACE_OS::sprintf(_name1, "/bremen/va/o:img");
	ACE_OS::sprintf(_name2, "/bremen/va/i:img");
	ACE_OS::sprintf(_name3, "/bremen/va/o:img2");
	//ACE_OS::sprintf(_name, "/%s/o:img", argv[0]);
	ACE_OS::sprintf(_netname, "Net1");
	
	outport.Register(_name1, _netname);
	inport.Register(_name2, _netname);
	outport2.Register(_name3, _netname);

	int frame_no = 0;

	pos_max = new Vett[7];

	if (!inport.Read())
		ACE_OS::printf(">>> ERROR: frame not read\n");

	imgOld.Refer(inport.Content());
	iplRShiftS(imgOld, imgOld, 1);

	while (!IsTerminated())
	{
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
		att_mod.Apply(colored_u, 7, pos_max);
		out.Refer(att_mod.Saliency());
		out2.Refer(att_mod.BlobFov());
	
		//att_mod.FindNMax(7, pos_max);

		//ACE_OS::printf("max position: x=%d y=%d max=%d\n", pos_x, pos_y, max);
		DBGPF1 ACE_OS::printf("max position: x=%d y=%d\n", pos_max[0].x, pos_max[0].y);

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

		outport.Content().Refer(out);
		outport.Write();

		outport2.Content().Refer(out2);
		outport2.Write();
		
		v(1) = pos_max[0].x;
		v(2) = pos_max[0].y;
		//out_point.Content() = v;
		//out_point.Write();

		start = start + (YARPTime::GetTimeAsSeconds() - cur);

		if ((frame_no % 20) == 0) {
			//cur = YARPTime::GetTimeAsSeconds();
			//ACE_OS::fprintf(stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			ACE_OS::fprintf(stdout, "average frame time: %f frame #%d acquired\r", start/frame_no, frame_no);
			//start = cur;
		}

	}

	ACE_OS::fprintf(stdout, "returning smoothly\n");
}


int main (int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling();

	mainthread _thread;
	_thread.Begin();

	char c = 0;

	do
	{
		cout << "Type q+return to quit" << endl;
		cin >> c;
	}
	while (c != 'q');

	_thread.End(-1);

	return YARP_OK;
}
