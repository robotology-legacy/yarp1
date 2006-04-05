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
#include <yarp/YARPBabybotHeadKin.h>
#include <yarp/YARPBabyBottle.h>
#include "YARPpaxis.h"
#include "YARPDispMap.h"

//#define FROMFILE


using namespace std;



#define DBGPF1 if (0)

#define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)


using namespace _logpolarParams;


/// global params.
char _inName1[512];
char _inName2[512];
char _inName3[512];
char _inName4[512];

char _outName1[512];
char _outName2[512];

char _netname0[512];
char _netname1[512];


YVector joints(5);

class mainthread : public YARPThread
{
	YARPSemaphore _sema;

	void inline _lock()
	{ _sema.Wait(); }

	void inline _unlock()
	{ _sema.Post(); }

	YARPRobotKinematics _leftCamera;
	YARPRobotKinematics _rightCamera;

	YVector _leftJoints;
	YVector _rightJoints;

	YHmgTrsf el;
	YHmgTrsf er;

	void saveMatrix(const char *file, const YMatrix& a);
	void cropImages(YARPImageOf<YarpPixelMono> &src, unsigned char * dst, int newRow, int newCol);
	void cropImages(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelMono> &dst, int newRow, int newCol);
	void uchar2YImg(unsigned char * src, YARPImageOf<YarpPixelMono> &dst, int row, int col);
	
public:
	mainthread():
		_sema(1),
		_leftCamera(YMatrix(_dh_nrf, 5, DH_left[0]), YMatrix(4, 4, TBaseline[0]) ),
		_rightCamera(YMatrix(_dh_nrf, 5, DH_right[0]), YMatrix(4, 4, TBaseline[0]) )
		{}
	
	virtual void Body (void);
	inline void saveMatrixLeft(const char *file) {saveMatrix(file, el);}
	inline void saveMatrixRight(const char *file) {saveMatrix(file, er);}
	void saveJoints(const char *file);
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


void mainthread::Body(void)
{
	YARPInputPortOf<YARPGenericImage> inImage1(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> inImage2(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> inImage3(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	DeclareOutport(outImage1);
	YARPOutputPortOf<YARPBottle> outBottle(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST);

	YARPBottle tmpBottle;

	// log-polar images
	YARPImageOf<YarpPixelMono> imgML1;
	YARPImageOf<YarpPixelMono> imgML2;
	YARPImageOf<YarpPixelBGR> imgCL1;
	
	// cartesian images
	YARPImageOf<YarpPixelMono> imgMC1;
	YARPImageOf<YarpPixelMono> imgMC2;
	YARPImageOf<YarpPixelMono> imgMC3;
	YARPImageOf<YarpPixelMono> imgMC4;
	YARPImageOf<YarpPixelMono> imgMC5;
	YARPImageOf<YarpPixelMono> imgMC6;
	YARPImageOf<YarpPixelMono> imgMC7;
	YARPImageOf<YarpPixelMono> imgMC8;
	YARPImageOf<YarpPixelMono> leftImg;
	YARPImageOf<YarpPixelMono> rightImg;
	YARPImageOf<YarpPixelMono> mskImg;
	YARPImageOf<YarpPixelMono> dispImg;
	YARPImageOf<YarpPixelBGR> imgCC1;
	YARPImageOf<YarpPixelBGR> imgCC2;
	YARPImageOf<YarpPixelBGR> imgCC3;
	YARPImageOf<YarpPixelBGR> imgCC4;

	YARPLogpolar mapper;

	YARPPaxis axis;

	YARPDispMap disp;
	
	//disparity
	const int newCol=128;
	const int newRow=128;

	inImage1.Register(_inName1, _netname1);
	inImage2.Register(_inName2, _netname1);
	inImage3.Register(_inName3, _netname1);
	outImage1.Register(_outName1, _netname1);
	outBottle.Register(_outName2, _netname0);

	
	imgML1.Resize(_stheta, _srho);
	imgML2.Resize(_stheta, _srho);
	imgCL1.Resize(_stheta, _srho);
	
	imgMC1.Resize(_xsize, _ysize);
	imgMC2.Resize(_xsize, _ysize);
	imgMC3.Resize(_xsize, _ysize);
	imgMC4.Resize(_xsize, _ysize);
	imgMC5.Resize(newCol, newRow);
	imgMC6.Resize(newCol, newRow);
	imgMC7.Resize(_xsize, _ysize);
	imgMC8.Resize(newCol, newRow);
	leftImg.Resize(newCol, newRow);
	rightImg.Resize(newCol, newRow);
	dispImg.Resize(newCol, newRow);
	mskImg.Resize(newCol, newRow);

	imgCC1.Resize(_xsize, _ysize);
	imgCC2.Resize(_xsize, _ysize);
	imgCC3.Resize(_xsize, _ysize);
	imgCC4.Resize(_xsize, _ysize);

#ifndef FROMFILE
	inImage1.Read();
	inImage2.Read();
	inImage3.Read();
	ACE_OS::printf("Cameras connected!\n");
#endif

	_leftJoints.Resize (5);
	_rightJoints.Resize (5);

	_leftJoints = 0;
	_rightJoints = 0;

	while (!IsTerminated()) {
		/*joints(1)=0.000000;
		joints(2)=-0.184741;
		joints(3)=-0.696125;
		joints(4)=-0.097498;
		joints(5)=-0.182050;*/
		/*joints(1)=0.000000;
		joints(2)=0;
		joints(3)=0;
		joints(4)=-0.097498;
		joints(5)=-0.182050;*/

		//YARPTime::DelayInSeconds(0.050);
		YARPTime::DelayInSeconds(0.10);

		ACE_OS::printf("%lf %lf %lf %lf %lf\n", joints(1), joints(2), joints(3), joints(4), joints(5));
		
		//saveJoints("joints.txt");

		_leftJoints(1) = joints(1);
		_leftJoints(2) = joints(2);
		_leftJoints(3) = joints(3);
		_leftJoints(4) = joints(5);

		_rightJoints(1) = joints(1);
		_rightJoints(2) = joints(2);
		_rightJoints(3) = joints(3);
		_rightJoints(4) = joints(4);
		
		_leftCamera.computeDirect(_leftJoints);
		_rightCamera.computeDirect(_rightJoints);
		
		//el = _leftCamera.endFrame();
		//er = _rightCamera.endFrame();
		el = _leftCamera._Ti[4];
		er = _rightCamera._Ti[4];

		//saveMatrix("EL.txt",el);
		//saveMatrix("ER.txt",er);
		
#ifdef FROMFILE
		YARPImageFile::Read("disparity.pgm", imgMC1);
		YARPImageFile::Read("mask.pgm", imgMC2);
#else
		//ACE_OS::printf("Reading image 1...");
		inImage1.Read();
		//ACE_OS::printf("done!\n");

		//ACE_OS::printf("Reading image 2...");
		inImage2.Read();
		//ACE_OS::printf("done!\n");

		//ACE_OS::printf("Reading image 3...");
		inImage3.Read();
		//ACE_OS::printf("done!\n");

		imgML1.Refer(inImage1.Content());
		//mapper.ReconstructColor((const YARPImageOf<YarpPixelMono>&)imgML1, imgCL1);
		mapper.ReconstructGrays((const YARPImageOf<YarpPixelMono>&)imgML1, imgML2);
		YARPImageUtils::SetRed(imgML2, imgCL1);
		mapper.Logpolar2Cartesian(imgCL1, imgCC1);
		YARPImageUtils::GetRed(imgCC1, imgMC1);
		//iplMedianFilter(imgMC4, imgMC1, 5, 5, 1, 1);

		imgML1.Refer(inImage2.Content());
		mapper.ReconstructGrays((const YARPImageOf<YarpPixelMono>&)imgML1, imgML2);
		YARPImageUtils::SetRed(imgML2, imgCL1);
		mapper.Logpolar2Cartesian(imgCL1, imgCC1);
		YARPImageUtils::GetRed(imgCC1, imgMC2);
		//iplMedianFilter(imgMC4, imgMC2, 5, 5, 1, 1);

		imgML1.Refer(inImage3.Content());
		mapper.ReconstructGrays((const YARPImageOf<YarpPixelMono>&)imgML1, imgML2);
		YARPImageUtils::SetRed(imgML2, imgCL1);
		mapper.Logpolar2Cartesian(imgCL1, imgCC1);
		YARPImageUtils::GetRed(imgCC1, imgMC3);
#endif

		char *root = GetYarpRoot();
		char path[256];
		char savename[512];
	
		ACE_OS::sprintf (path, "%s/zgarbage/", root);

		/*ACE_OS::sprintf(savename, "%sleft.ppm", path);
		YARPImageFile::Write(savename, imgMC1);

		ACE_OS::sprintf(savename, "%sright.ppm", path);
		YARPImageFile::Write(savename, imgMC2);*/

		
		//iplDilate(imgMC3, imgMC7,10);
		//axis.copyImageMsk(imgMC1, imgMC4, imgMC7); //masked left
		
		cropImages(imgMC1, leftImg, newRow, newCol);
		cropImages(imgMC2, rightImg, newRow, newCol);
		cropImages(imgMC3, mskImg, newRow, newCol);
		cropImages(imgMC3, imgMC6, newRow, newCol);

		// saving the inputs to the disparity
		/*ACE_OS::sprintf(savename, "%scleft.ppm", path);
		YARPImageFile::Write(savename, imgMC8);
		ACE_OS::sprintf(savename, "%scright.ppm", path);
		YARPImageFile::Write(savename, imgMC8);
		ACE_OS::sprintf(savename, "%scmask.ppm", path);
		YARPImageFile::Write(savename, imgMC6);*/


		//ACE_OS::printf("Computing disparity...");
		disp.dispWrapper(leftImg, rightImg,	mskImg, dispImg);
		//ACE_OS::printf("done!\n");
		
		//ACE_OS::sprintf(savename, "%sdisparity.ppm", path);
		//YARPImageFile::Write(savename, imgMC5);
		
		//ACE_OS::printf("Computing principal axis...");
		int paa=axis.apply(dispImg, mskImg, joints);
		//ACE_OS::printf("done!\n");
		
		if (paa>=0) {
			outImage1.Content().Refer(dispImg);
			outImage1.Write();

			tmpBottle.reset();
			tmpBottle.writeInt(paa);
			outBottle.Content() = tmpBottle;
			outBottle.Write();
		}
	}
}

void mainthread::saveMatrix(const char *file, const YMatrix& a)
{
	char *root = GetYarpRoot();
	char path[256];
	
	ACE_OS::sprintf (path, "%s/zgarbage/%s", root, file); 
	
	FILE  *fp = ACE_OS::fopen(path, "w");
	
	if (!fp) {
		ACE_OS::fprintf(stderr, "Error - cannot open file for writing\n");
		return;
	}

	//ACE_OS::fwrite(&ep(1,1), sizeof(double), 1, fp);
	ACE_OS::fprintf(fp,"%lf\t%lf\t%lf\t%lf\n",a(1,1),a(1,2),a(1,3),a(1,4));
	ACE_OS::fprintf(fp,"%lf\t%lf\t%lf\t%lf\n",a(2,1),a(2,2),a(2,3),a(2,4));
	ACE_OS::fprintf(fp,"%lf\t%lf\t%lf\t%lf\n",a(3,1),a(3,2),a(3,3),a(3,4));
	/*ACE_OS::fprintf(fp,"%lf\t%lf\t%lf\n",a(1,1),a(1,2),a(1,3));
	ACE_OS::fprintf(fp,"%lf\t%lf\t%lf\n",a(2,1),a(2,2),a(2,3));
	ACE_OS::fprintf(fp,"%lf\t%lf\t%lf\n",a(3,1),a(3,2),a(3,3));*/

	ACE_OS::fclose(fp);
}


void mainthread::saveJoints(const char *file)
{
	char *root = GetYarpRoot();
	char path[256];
	
	ACE_OS::sprintf (path, "%s/zgarbage/%s", root, file); 
	
	FILE  *fp = ACE_OS::fopen(path, "w");
	
	if (!fp) {
		ACE_OS::fprintf(stderr, "Error - cannot open file for writing\n");
		return;
	}

	ACE_OS::fprintf(fp,"%lf\n",joints(1));
	ACE_OS::fprintf(fp,"%lf\n",joints(2));
	ACE_OS::fprintf(fp,"%lf\n",joints(3));
	ACE_OS::fprintf(fp,"%lf\n",joints(4));
	ACE_OS::fprintf(fp,"%lf\n",joints(5));

	ACE_OS::fclose(fp);
}


void mainthread::cropImages(YARPImageOf<YarpPixelMono> &src, unsigned char * dst, int newRow, int newCol)
{
	int rmax=src.GetHeight();
	int cmax=src.GetWidth();

	int rOff=(rmax-newRow)/2;
	int cOff=(cmax-newCol)/2;
	
	for (int r=0; r<newRow; r++)
		for (int c=0; c<newCol; c++)
			dst[newCol*r+c]=src(c+cOff, r+rOff);
}

void mainthread::cropImages(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelMono> &dst, int newRow, int newCol)
{
	int rmax=src.GetHeight();
	int cmax=src.GetWidth();

	int rOff=(rmax-newRow)/2;
	int cOff=(cmax-newCol)/2;
	
	for (int r=0; r<newRow; r++)
		for (int c=0; c<newCol; c++)
			dst(c, r)=src(c+cOff, r+rOff);
}

void mainthread::uchar2YImg(unsigned char * src, YARPImageOf<YarpPixelMono> &dst, int row, int col)
{
	dst.Resize(col, row);
	
	for (int r=0; r<row; r++)
		for (int c=0; c<col; c++)
			dst(c, r)=src[col*r+c];
}


void secondthread::Body(void)
{
	YARPInputPortOf<YVector> inVector;
	
	inVector.Register(_inName4, _netname0);

	inVector.Read();
	printf("Head vector connected!\n");
	
	while (!IsTerminated()) {
		double diffJoints=0;
		inVector.Read();
		joints = inVector.Content();
	}
}


int main (int argc, char *argv[])
{
	char basename[80]="";
		
	YARPParseParameters::parse(argc, argv, "-name", basename);

	ACE_OS::sprintf(_inName1, "/paxis%s/i:img",basename);
	ACE_OS::sprintf(_inName2, "/paxis%s/i:img2", basename);
	ACE_OS::sprintf(_inName3, "/paxis%s/i:img3", basename);
	ACE_OS::sprintf(_inName4, "/paxis%s/i:vec", basename);
	ACE_OS::sprintf(_outName1, "/paxis%s/o:img", basename);
	ACE_OS::sprintf(_outName2, "/paxis%s/o:bot", basename);
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
		if (c=='s') {
			cout<<"Saving matrix, left and right"<<endl;
			_thread.saveMatrixLeft("L.txt");
			_thread.saveMatrixRight("R.txt");
		}
	} while (c != 'q');

	_thread.End(-1);
	_thread2.End(-1);

	return YARP_OK;
}
