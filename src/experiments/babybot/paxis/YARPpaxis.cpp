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
///                            #Bremen#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id:
///
///
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ipl/ipl.h>

#include "YARPpaxis.h"
//#include "nrutil.h"


#define ROTATE(a,i,j,k,l) g=a(i,j);h=a(k,l);a(i,j)=g-s*(h+g*tau);\
	a(k,l)=h+s*(g-h*tau);

void jacobi(YMatrix &a, int n, YVector &d, YMatrix &v, int &nrot)
{
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c;
	
	YVector b(n);
	YVector z(n);
	
	for (ip=1;ip<=n;ip++) {
		for (iq=1;iq<=n;iq++) v(ip,iq)=0.0;
		v(ip,ip)=1.0;
	}
	for (ip=1;ip<=n;ip++) {
		b(ip)=d(ip)=a(ip,ip);
		z(ip)=0.0;
	}

	nrot=0;

	for (i=1;i<=50;i++) {
		sm=0.0;
		for (ip=1;ip<=n-1;ip++) {
		for (iq=ip+1;iq<=n;iq++)
			sm += fabs(a(ip,iq));
		}
		if (sm < DBL_EPSILON) {
			return;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n);
		else
			tresh=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++) {
				g=100.0*fabs(a(ip,iq));
				if (i > 4 && (double)(fabs(d(ip))+g) == fabs(d(ip))
					&& (double)(fabs(d(iq))+g) == fabs(d(iq)))
					a(ip,iq)=0.0;
				else if (fabs(a(ip,iq)) > tresh) {
					h=d(iq)-d(ip);
					if ((double)(fabs(h)+g) == fabs(h))
						t=(a(ip,iq))/h; //t = 1/(2.)
					else {
						theta=0.5*h/(a(ip,iq));
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a(ip,iq);
					z(ip) -= h;
					z(iq) += h;
					d(ip) -= h;
					d(iq) += h;
					a(ip,iq)=0.0;
					for (j=1;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<=n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1;j<=n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++nrot;
				}
			}
		}
		for (ip=1;ip<=n;ip++) {
			b(ip) += z(ip);
			d(ip)=b(ip);
			z(ip)=0.0;
		}
	}

	ACE_OS::printf("Too many iterations in routine jacobi");
}
#undef ROTATE


YARPPaxis::YARPPaxis()
{
	numPixel=0;
}


YARPPaxis::~YARPPaxis()
{
	
}


void YARPPaxis::subMatrixCopy(const YMatrix& S, YMatrix& D, int ri, int rf, int ci, int cf)
{
	for (int r=ri; r<=rf; r++)
		for (int c=ci; c<=cf; c++)
			D(r,c)=S(r,c);
}


int YARPPaxis::apply(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& msk, const YVector &joints)
{
	double thetaL=-0.1415;
	double thetaR=0.1334;
	double angxL;
	double angyL;
	double angzL;
	double angxR;
	double angyR;
	double angzR;

	const double A_left[3][3] = {
		{233, 0, 127},
		{0, 232, 128},
		{0, 0, 1},
	};
	
	const double A_right[3][3] = {
		{234, 0, 137},
		{0, 233, 124},
		{0, 0, 1},
	};

	YMatrix aL(3,3);
	YMatrix aR(3,3);
	YRotMatrix rL;
	YRotMatrix rR;
	YMatrix Rx(3,3);
	YMatrix Ry(3,3);
	YMatrix Rz(3,3);
	YMatrix tmp(3,3);
	YMatrix ppmL(3,4);
	YMatrix ppmR(3,4);

	
	angzL=0;
	//angyL=joints(5);
	angyL=0;
	//angxL=(joints(2)+joints(3));
	angxL=0;

	angzR=0;
	//angyR=-joints(4);
	angyR=-joints(5)-joints(4);
	//angxR=(joints(2)+joints(3));
	angxR=0;


	// Left PPM
	Rz(1,1)=cos(angzL);
	Rz(1,2)=-sin(angzL);
	Rz(1,3)=0;
	Rz(2,1)=sin(angzL);
	Rz(2,2)=cos(angzL);
	Rz(2,3)=0;
	Rz(3,1)=0;
	Rz(3,2)=0;
	Rz(3,3)=1;

	Ry(1,1)=cos(angyL);
	Ry(1,2)=0;
	Ry(1,3)=sin(angyL);
	Ry(2,1)=0;
	Ry(2,2)=1;
	Ry(2,3)=0;
	Ry(3,1)=-sin(angyL);
	Ry(3,2)=0;
	Ry(3,3)=cos(angyL);
	
	Rx(1,1)=1;
	Rx(1,2)=0;
	Rx(1,3)=0;
	Rx(2,1)=0;
	Rx(2,2)=cos(angxL);
	Rx(2,3)=-sin(angxL);
	Rx(3,1)=0;
	Rx(3,2)=sin(angxL);
	Rx(3,3)=cos(angxL);
	
	tmp=Rz*Ry*Rx;
	
	//saveMatrix("tmp.txt", tmp);
	//saveMatrix("el.txt", el);
			
	/*ppmL(1,1)=el(1,1);
	ppmL(1,2)=el(1,2);
	ppmL(1,3)=el(1,3);
	ppmL(2,1)=el(2,1);
	ppmL(2,2)=el(2,2);
	ppmL(2,3)=el(2,3);
	ppmL(3,1)=el(3,1);
	ppmL(3,2)=el(3,2);
	ppmL(3,3)=el(3,3);
	ppmL(1,4)=el(1,4);
	ppmL(2,4)=el(2,4);
	ppmL(3,4)=el(3,4);*/

	ppmL(1,1)=tmp(1,1);
	ppmL(1,2)=tmp(1,2);
	ppmL(1,3)=tmp(1,3);
	ppmL(2,1)=tmp(2,1);
	ppmL(2,2)=tmp(2,2);
	ppmL(2,3)=tmp(2,3);
	ppmL(3,1)=tmp(3,1);
	ppmL(3,2)=tmp(3,2);
	ppmL(3,3)=tmp(3,3);
	//ppmL(1,4)=64.5;
	//ppmL(2,4)=-2;
	//ppmL(3,4)=0;
	ppmL(1,4)=0;
	ppmL(2,4)=0;
	ppmL(3,4)=0;
	
	
	// Right PPM
	Rz(1,1)=cos(angzR);
	Rz(1,2)=-sin(angzR);
	Rz(1,3)=0;
	Rz(2,1)=sin(angzR);
	Rz(2,2)=cos(angzR);
	Rz(2,3)=0;
	Rz(3,1)=0;
	Rz(3,2)=0;
	Rz(3,3)=1;

	Ry(1,1)=cos(angyR);
	Ry(1,2)=0;
	Ry(1,3)=sin(angyR);
	Ry(2,1)=0;
	Ry(2,2)=1;
	Ry(2,3)=0;
	Ry(3,1)=-sin(angyR);
	Ry(3,2)=0;
	Ry(3,3)=cos(angyR);
	
	Rx(1,1)=1;
	Rx(1,2)=0;
	Rx(1,3)=0;
	Rx(2,1)=0;
	Rx(2,2)=cos(angxR);
	Rx(2,3)=-sin(angxR);
	Rx(3,1)=0;
	Rx(3,2)=sin(angxR);
	Rx(3,3)=cos(angxR);
	
	tmp=Rz*Ry*Rx;

	/*ppmR(1,1)=er(1,1);
	ppmR(1,2)=er(1,2);
	ppmR(1,3)=er(1,3);
	ppmR(2,1)=er(2,1);
	ppmR(2,2)=er(2,2);
	ppmR(2,3)=er(2,3);
	ppmR(3,1)=er(3,1);
	ppmR(3,2)=er(3,2);
	ppmR(3,3)=er(3,3);
	ppmR(1,4)=er(1,4);
	ppmR(2,4)=er(2,4);
	ppmR(3,4)=er(3,4);*/

	ppmR(1,1)=tmp(1,1);
	ppmR(1,2)=tmp(1,2);
	ppmR(1,3)=tmp(1,3);
	ppmR(2,1)=tmp(2,1);
	ppmR(2,2)=tmp(2,2);
	ppmR(2,3)=tmp(2,3);
	ppmR(3,1)=tmp(3,1);
	ppmR(3,2)=tmp(3,2);
	ppmR(3,3)=tmp(3,3);
	//ppmR(1,4)=-64.5;
	//ppmR(2,4)=2;
	//ppmR(3,4)=0;
	ppmR(1,4)=-129*cos(-joints(5)); //sin(pi/2-j)
	ppmR(2,4)=4;
	ppmR(3,4)=-129*(-sin(-joints(5))); //cos(pi/2+j)
	
	ppmL=YMatrix(3, 3, A_left[0])*ppmL;
	ppmR=YMatrix(3, 3, A_right[0])*ppmR;
	
	image2RealVectorsMsk(src, msk, ppmL, ppmR);
	if (numPixel>15*15) {
		YMatrix a=computeVarMatrix();
		
		/*ACE_OS::printf("%lf\t%lf\t%lf\n",a(1,1),a(1,2),a(1,3));
		ACE_OS::printf("%lf\t%lf\t%lf\n",a(2,1),a(2,2),a(2,3));
		ACE_OS::printf("%lf\t%lf\t%lf\n",a(3,1),a(3,2),a(3,3));
		ACE_OS::printf("\n");*/

		int nrot;
		YVector d(3);
		YMatrix v(3,3);
		//ACE_OS::printf("Factorizing matrix...\r");
		jacobi(a, 3, d, v, nrot);
		/*ACE_OS::printf("%lf\t%lf\t%lf\n",a(1,1),a(1,2),a(1,3));
		ACE_OS::printf("%lf\t%lf\t%lf\n",a(2,1),a(2,2),a(2,3));
		ACE_OS::printf("%lf\t%lf\t%lf\n",a(3,1),a(3,2),a(3,3));
		ACE_OS::printf("\n");*/
		/*ACE_OS::printf("%lf\t%lf\t%lf\n",v(1,1),v(1,2),v(1,3));
		ACE_OS::printf("%lf\t%lf\t%lf\n",v(2,1),v(2,2),v(2,3));
		ACE_OS::printf("%lf\t%lf\t%lf\n",v(3,1),v(3,2),v(3,3));
		ACE_OS::printf("\n");
		ACE_OS::printf("%lf\t%lf\t%lf\n",d(1),d(2),d(3));
		ACE_OS::printf("\n");*/
		
		//saveVectors("vec.txt");

		int maxAV=1;
		if (d(maxAV)<d(2))
			maxAV=2;
		if (d(maxAV)<d(3))
			maxAV=3;
		
		YVector pa(3);
		YVector pa2(3);
		pa(1)=v(1,maxAV);
		pa(2)=v(2,maxAV);
		pa(3)=v(3,maxAV);

		ACE_OS::printf("pa (camera) %lf\t%lf\t%lf\n",pa(1),pa(2),pa(3));

		int x,y;
		imageMoments.centerOfMass(msk, &x, &y);
		drawPAxis(pa,x,y,src);
		
		angxL=-joints(2)-joints(3);
		
		Rx(1,1)=1;
		Rx(1,2)=0;
		Rx(1,3)=0;
		Rx(2,1)=0;
		Rx(2,2)=cos(angxL);
		Rx(2,3)=-sin(angxL);
		Rx(3,1)=0;
		Rx(3,2)=sin(angxL);
		Rx(3,3)=cos(angxL);

		pa2=Rx.Transposed()*pa;
		
		ACE_OS::printf("pa (table) %lf\t%lf\t%lf\n",pa2(1),pa2(2),pa2(3));

		double pro=sqrt(pa2(1)*pa2(1)+pa2(3)*pa2(3));

		double m=pa2(2)/pro;

		double angZ=atan(m)/3.14159265*180;
		
		ACE_OS::printf("ang=%lf\n",angZ);

		if ( fabs(angZ) < 30 ) {
			ACE_OS::printf("IMO the object is laying on the table\n");
			return 0;
		}
		if ( fabs(angZ) > 60 ) {
			ACE_OS::printf("IMO the object is vertical on the table\n");
			return 1;
		}
		ACE_OS::printf("IMO the object is diagonal on the table\n");
		return 1;
	}
	ACE_OS::printf("Object too small\r");
	return -1;
}


void YARPPaxis::drawPAxis(YVector &d, int centerC, int centerR, YARPImageOf<YarpPixelMono>& img)
{
	int rmax=img.GetHeight();
	int cmax=img.GetWidth();

	double m;
	double q;

	if (fabs(d(1))>fabs(d(2))) {
		m=d(2)/d(1);
		q=centerR-m*centerC;
		for (int c=0; c<cmax; c++) {
			int r=m*c+q;
			if (r>rmax-1) r=rmax-1;
			if (r<0) r=0;
			img(c, r)=255;
		}
	} else {
		m=d(1)/d(2);
		q=q=centerC-m*centerR;
		for (int r=0; r<rmax; r++) {
			int c=m*r+q;
			if (c>cmax-1) c=cmax-1;
			if (c<0) c=0;
			img(c, r)=255;
		}
	}
}


void YARPPaxis::copyImageMsk(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelMono>& msk)
{
	int rmax=msk.GetHeight();
	int cmax=msk.GetWidth();
	
	for (int r=0; r<rmax; r++)
		for (int c=0; c<cmax; c++)
			if (msk(c, r)>127) dst(c,r)=src(c,r);
			else dst(c,r)=0;
}


float YARPPaxis::meanImageMsk(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& msk)
{
	int rmax=msk.GetHeight();
	int cmax=msk.GetWidth();
	int tmp=0;
	int cnt=0;

	for (int r=0; r<rmax; r++)
		for (int c=0; c<cmax; c++)
			if (msk(c, r)>127) {
				tmp+=src(c,r);
				cnt++;
			}

	return ((float)tmp/cnt);
}


void YARPPaxis::image2VectorsMsk(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& msk)
{
	int rmax=msk.GetHeight();
	int cmax=msk.GetWidth();
	int tmp=0;
	int cnt=0;

	for (int r=0; r<rmax; r++)
		for (int c=0; c<cmax; c++)
			if (msk(c, r)>127 && src(c, r)!=255) {
				cnt++;
			}

	XV.Resize(cnt);
	YV.Resize(cnt);
	ZV.Resize(cnt);
	numPixel=cnt;

	cnt=1;
	for (r=0; r<rmax; r++)
		for (int c=0; c<cmax; c++)
			if (msk(c, r)>127 && src(c, r)!=255) {
				XV(cnt)=c;
				YV(cnt)=r;
				ZV(cnt)=src(c, r);
				cnt++;
			}
}


void YARPPaxis::image2RealVectorsMsk(YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& msk, const YMatrix& PPM1, const YMatrix& PPM2)
{
	int rmax=msk.GetHeight();
	int cmax=msk.GetWidth();
	int tmp=0;
	int cnt=0;
	YMatrix b(4,1);
	YMatrix A(4,3);
	YMatrix AP(3,4);
	YMatrix w(3,1);
	YVector tmp1(3);
	YVector tmp2(3);
	YVector tmp3(3);
	YVector tmp4(3);
	YVector tmp11(3);
	YVector tmp12(3);
	YVector tmp13(3);
	YVector tmp21(3);
	YVector tmp22(3);
	YVector tmp23(3);
	
	tmp11(1)=PPM1(1,1);
	tmp11(2)=PPM1(1,2);
	tmp11(3)=PPM1(1,3);
	tmp12(1)=PPM1(2,1);
	tmp12(2)=PPM1(2,2);
	tmp12(3)=PPM1(2,3);
	tmp13(1)=PPM1(3,1);
	tmp13(2)=PPM1(3,2);
	tmp13(3)=PPM1(3,3);
	
	tmp21(1)=PPM2(1,1);
	tmp21(2)=PPM2(1,2);
	tmp21(3)=PPM2(1,3);
	tmp22(1)=PPM2(2,1);
	tmp22(2)=PPM2(2,2);
	tmp22(3)=PPM2(2,3);
	tmp23(1)=PPM2(3,1);
	tmp23(2)=PPM2(3,2);
	tmp23(3)=PPM2(3,3);


	for (int r=0; r<rmax; r++)
		for (int c=0; c<cmax; c++)
			if (msk(c, r)>127 && src(c, r)!=255) {
				cnt++;
			}

	XVI.Resize(cnt);
	YVI.Resize(cnt);
	XV.Resize(cnt);
	YV.Resize(cnt);
	ZV.Resize(cnt);
	numPixel=cnt;

	/*for i = 1:numberOfPoints

		b = - [PPM1(1,4)-imageP1(i,1)*PPM1(3,4)
			   PPM1(2,4)-imageP1(i,2)*PPM1(3,4)
			   PPM2(1,4)-imageP2(i,1)*PPM2(3,4)
			   PPM2(2,4)-imageP2(i,2)*PPM2(3,4)];

		A = [PPM1(1,1:3) - imageP1(i,1)*PPM1(3,1:3)
			 PPM1(2,1:3) - imageP1(i,2)*PPM1(3,1:3)
			 PPM2(1,1:3) - imageP2(i,1)*PPM2(3,1:3)
			 PPM2(2,1:3) - imageP2(i,2)*PPM2(3,1:3)];

		w = A \ b;
		struc = [struc w];
	end
	xp = struc(1,:)';
	yp = struc(2,:)';
	zp = struc(3,:)';*/

	cnt=1;
	for (int r1=0; r1<rmax; r1++)
		for (int c1=0; c1<cmax; c1++)
			if (msk(c1, r1)>127 && src(c1, r1)!=255) {
				int c=c1+128;
				int r=r1+128;
				b(1,1) = - (PPM1(1,4)-c*PPM1(3,4));
				b(2,1) = - (PPM1(2,4)-r*PPM1(3,4));
				b(3,1) = - (PPM2(1,4)-(c - (src(c1, r1)-128)/4 )*PPM2(3,4));
				b(4,1) = - (PPM2(2,4)-r*PPM2(3,4));
				
				tmp1=tmp11-c*tmp13;
				tmp2=tmp12-r*tmp13;
				tmp3=tmp21-(c - (src(c1, r1)-128)/4 )*tmp23;
				tmp4=tmp22-r*tmp23;
				
				A(1,1)=tmp1(1); A(1,2)=tmp1(2); A(1,3)=tmp1(3);
				A(2,1)=tmp2(1); A(2,2)=tmp2(2); A(2,3)=tmp2(3);
				A(3,1)=tmp3(1); A(3,2)=tmp3(2); A(3,3)=tmp3(3);
				A(4,1)=tmp4(1); A(4,2)=tmp4(2); A(4,3)=tmp4(3);
				
				AP=(A.Transposed()*A).Inverted()*A.Transposed();
				
				w=AP*b;
				
				XV(cnt)=w(1,1);
				YV(cnt)=w(2,1);
				ZV(cnt)=w(3,1);

				XVI(cnt)=c;
				YVI(cnt)=r;

				cnt++;
			}
}


void YARPPaxis::saveVectors(const char *file)
{
	char *root = GetYarpRoot();
	char path[256];
	
	ACE_OS::sprintf (path, "%s/zgarbage/%s", root, file); 
	
	FILE  *fp = ACE_OS::fopen(path, "w");
	
	if (!fp) {
		ACE_OS::fprintf(stderr, "Error - cannot open file for writing\n");
		return;
	}

	for (int i=1; i<=numPixel; i++) {
		//ACE_OS::fprintf(fp,"(%d,%d) (%d,%d) -> (%lf,%lf,%lf)\n", );
		//ACE_OS::fprintf(fp,"(%d,%d)  ->  %lf %lf %lf\n", (int)XVI(i), (int)YVI(i), XV(i), YV(i), ZV(i));
		ACE_OS::fprintf(fp,"%lf %lf %lf\n", XV(i), YV(i), ZV(i));
	}

	ACE_OS::fclose(fp);


}


YMatrix YARPPaxis::computeVarMatrix()
{
	YMatrix res(3,3);

	if (numPixel!=0) {
		double xmean=0;
		double ymean=0;
		double zmean=0;
		
		nXV.Resize(numPixel);
		nYV.Resize(numPixel);
		nZV.Resize(numPixel);

		for (int i=1; i<=numPixel; i++) {
			xmean+=XV(i);
			ymean+=YV(i);
			zmean+=ZV(i);
		}
		xmean/=numPixel;
		ymean/=numPixel;
		zmean/=numPixel;

		for (i=1; i<=numPixel; i++) {
			nXV(i)=XV(i)-xmean;
			nYV(i)=YV(i)-ymean;
			nZV(i)=ZV(i)-zmean;
		}
		
		res(1,1)=(nXV*nXV)/(numPixel-1);
		res(1,2)=(nXV*nYV)/(numPixel-1);
		res(1,3)=(nXV*nZV)/(numPixel-1);
		res(2,1)=res(1,2);
		res(2,2)=(nYV*nYV)/(numPixel-1);
		res(2,3)=(nYV*nZV)/(numPixel-1);
		res(3,1)=res(1,3);
		res(3,2)=res(2,3);
		res(3,3)=(nZV*nZV)/(numPixel-1);
	}

	return res;
}
