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

#include "YARPWatershed.h"


YARPWatershed::YARPWatershed(const int width1, const int height1, const int wstep, const YarpPixelMono th)
{
	neighborhood8=true;

	watershedColor=0;
	basinColor=255;

	neigh=NULL;

	resize(width1, height1, wstep, th);
}


void YARPWatershed::resize(const int width1, const int height1, const int wstep, const YarpPixelMono th)
{
	width=width1;
	height=height1;

	imageSize=width*height;
	padding=wstep-width1;

	widthStep=wstep;

	threshold=th;
	
	createNeighborhood(wstep, neighborhood8);

	downPos.Resize(widthStep,height);
	downPos2.Resize(widthStep,height);
	
	tempRegion = new int[imageSize];
}


YARPWatershed::~YARPWatershed()
{
	if (tempRegion!=NULL) delete [] tempRegion;
	if (neigh!=NULL) delete [] neigh;
	if (neighL!=NULL) delete [] neighL;
	if (neighR!=NULL) delete [] neighR;
}


void YARPWatershed::createNeighborhood(const int widthStep, const bool neigh8)
{
	if (!neigh8)
		neighSize=4;
	else
		neighSize=8;

	if (neigh!=NULL)
		delete [] neigh;

	neigh = new int [neighSize];
	neighL = new int [neighSize];
	neighR = new int [neighSize];

	if (neigh8) {
		//	012
		//	3 4
		//	567

		neigh[4]= +1;			//right
		neigh[1]= -widthStep;	//up
		neigh[3]= -1;			//left
		neigh[6]= +widthStep;	//down
		neigh[2]= neigh[1]+neigh[4];	//up, right
		neigh[0]= neigh[1]+neigh[3];	//up, left
		neigh[5]= neigh[6]+neigh[3];	//down, left
		neigh[7]= neigh[6]+neigh[4];	//down, right

		neighL[4]= +1;			//right
		neighL[1]= -widthStep;	//up
		neighL[3]= +(width-1);	//left
		neighL[6]= +widthStep;	//down
		neighL[2]= neighL[1]+neighL[4];	//up, right
		neighL[0]= neighL[1]+neighL[3];	//up, left
		neighL[5]= neighL[6]+neighL[3];	//down, left
		neighL[7]= neighL[6]+neighL[4];	//down, right

		neighR[4]= -(width-1);	//right
		neighR[1]= -widthStep;	//up
		neighR[3]= -1;			//left
		neighR[6]= +widthStep;	//down
		neighR[2]= neighR[1]+neighR[4];	//up, right
		neighR[0]= neighR[1]+neighR[3];	//up, left
		neighR[5]= neighR[6]+neighR[3];	//down, left
		neighR[7]= neighR[6]+neighR[4];	//down, right
	} else {
		//	 0
		//	1 2
		//	 3

		neigh[1]= -widthStep;	//up
		neigh[2]= -1;			//left
		neigh[0]= +1;			//right
		neigh[3]= +widthStep;	//down

		neighL[0]= +1;			//right
		neighL[1]= -widthStep;	//up
		neighL[2]= +(width-1);	//left
		neighL[3]= +widthStep;	//down

		neighR[0]= -(width-1);	//right
		neighR[1]= -widthStep;	//up
		neighR[2]= -1;			//left
		neighR[3]= +widthStep;	//down
	}
}


void YARPWatershed::tags2Watershed(const YARPImageOf<YarpPixelInt>& src, YARPImageOf<YarpPixelMono>& dest)
{
	YarpPixelInt *p_src=(YarpPixelInt *)src.GetRawBuffer();
	YarpPixelMono *p_dst;
	int i,j,n,pos,p;

	const YarpPixelMono val = 255;

	//dest.Resize(width,height);
	p_dst=(YarpPixelMono *)dest.GetRawBuffer();

	dest.Zero();

	// first row
	// first pixel
	p=0;
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighL[n];
		if (p_src[p]!=p_src[pos]) {
			p_dst[p] = val;
			break;
		}
	}
	p++;
	// first row, 1->150
	// Note that the pixel of the first row are all the same, so it is
	// useless to check the "far" neighborhoods
	for(i=1; i<width-1; i++) {
		for(n=neighSize/2-1; n<neighSize; n++) { // no top Neighbor
			pos = p + neigh[n];
			if (p_src[p]!=p_src[pos]) {
				p_dst[p] = val;
				break;
			}
		}
		p++;
	}
	// last pixel of the first row
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighR[n];
		if (p_src[p]!=p_src[pos]) {
			p_dst[p] = val;
			break;
		}
	}
	p++;
	p+=padding;

	// inner part
	for(j=1; j<height-1; j++) {
		// first pixel of the inner part
		for(n=0; n<neighSize; n++) {
			pos = p + neighL[n];
			if (p_src[p]!=p_src[pos]) {
				p_dst[p] = val;
				break;
			}
		}
		p++;
		
		for(i=1; i<width-1; i++) {
			for(n=0; n<neighSize; n++) {
				pos = p + neigh[n];
				if (p_src[p]!=p_src[pos]) {
					p_dst[p] = val;
					break;
				}
			}
			p++;
		}
		
		//last pixel of the inner part
		for(n=0; n<neighSize; n++) {
			pos = p + neighR[n];
			if (p_src[p]!=p_src[pos]) {
				p_dst[p] = val;
				break;
			}
		}
		p++;
		
		p+=padding;
	}

	// last row
	// first pixel
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighL[n];
		if (p_src[p]!=p_src[pos]) {
			p_dst[p] = val;
			break;
		}
	}
	p++;
	// last row, 1->150
	for(i=1; i<width-1; i++) {
		for(n=0; n<neighSize/2+1; n++) { // no top Neighbor
			pos = p + neigh[n];
			if (p_src[p]!=p_src[pos]) {
				p_dst[p] = val;
				break;
			}
		}
		p++;
	}
	// last pixel of the last row
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighR[n];
		if (p_src[p]!=p_src[pos]) {
			p_dst[p] = val;
			break;
		}
	}
}


/*void YARPWatershed::tags2Watershed(const YARPImageOf<YarpPixelInt>& src, YARPImageOf<YarpPixelMono>& dest)
{
	const int WSHED =  0; // watersheds have value 0
	
	int *p_src=(int *)src.GetRawBuffer();
	unsigned char *p_dest;

	// 4-neighborhood is sufficient here, so the watersheds won't get too thick
	// only for visualisation (calculation by user-parameter)
	createNeighborhood(((IplImage *)src)->widthStep, false);

	dest.Resize(width, height);
	p_dest=(unsigned char *)dest.GetRawBuffer();

	int currentPoint=0;
	for (int r=0; r<height; r++) {
		for (int c=0; c<width; c++)	{
			int currentValue = p_src[currentPoint];

			if (currentValue == WSHED) // watershed
				p_dest[currentPoint] = watershedColor;
			else {                     // point is labeled
				// assume point is in basin
				p_dest[currentPoint] = basinColor;
				// check for adjacent basins
				int n;
				for(n=0;n<neighSize;n++) {
					int currentNeighbor = currentPoint + neigh[n];
					// skip invalid neighbors (pixel outside of image)
					if (invalidNeighbor(currentPoint,currentNeighbor)) continue;
					if (currentValue != p_src[currentNeighbor]) {
						// different basin bordering => declare point as watershed
						p_dest[currentPoint] = watershedColor;
						break; // next currentPoint
					}
				}
			}
			currentPoint++;
		}
		currentPoint+=padding;
	}
}*/


void YARPWatershed::connectivityGraph(const YARPImageOf<YarpPixelInt>& src, bool *matr, int max_tag)
{
	YarpPixelInt *p_src=(YarpPixelInt *)src.GetRawBuffer();
	int i,j,n,pos,p;

	memset(matr, false, sizeof(bool)*max_tag*max_tag);

	// first row
	// first pixel
	/*p=0;
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighL[n];
		if (p_src[p]!=p_src[pos]) {
			p_dst[p] = val;
			break;
		}
	}*/
	p=1;

	// first row, 1->150
	// Note that the pixel of the first row are all the same, so it is
	// useless to check the "far" neighborhoods...
	for(i=1; i<width-1; i++) {
		for(n=neighSize/2-1; n<neighSize; n++) { // no top Neighbor
			pos = p + neigh[n];
			if (p_src[p]!=p_src[pos]) {
				matr[max_tag*p_src[p]+p_src[pos]]=true;
				matr[max_tag*p_src[pos]+p_src[p]]=true;
			}
		}
		p++;
	}
	// last pixel of the first row
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighR[n];
		if (p_src[p]!=p_src[pos]) {
			matr[max_tag*p_src[p]+p_src[pos]]=true;
			matr[max_tag*p_src[pos]+p_src[p]]=true;
		}
	}
	p++;
	p+=padding;

	// inner part
	for(j=1; j<height-1; j++) {
		// first pixel of the inner part
		for(n=0; n<neighSize; n++) {
			pos = p + neighL[n];
			if (p_src[p]!=p_src[pos]) {
				matr[max_tag*p_src[p]+p_src[pos]]=true;
				matr[max_tag*p_src[pos]+p_src[p]]=true;
			}
		}
		p++;
		
		for(i=1; i<width-1; i++) {
			for(n=0; n<neighSize; n++) {
				pos = p + neigh[n];
				if (p_src[p]!=p_src[pos]) {
					matr[max_tag*p_src[p]+p_src[pos]]=true;
					matr[max_tag*p_src[pos]+p_src[p]]=true;
				}
			}
			p++;
		}
		
		//last pixel of the inner part
		for(n=0; n<neighSize; n++) {
			pos = p + neighR[n];
			if (p_src[p]!=p_src[pos]) {
				matr[max_tag*p_src[p]+p_src[pos]]=true;
				matr[max_tag*p_src[pos]+p_src[p]]=true;
			}
		}
		p++;
		
		p+=padding;
	}

	// last row
	// first pixel
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighL[n];
		if (p_src[p]!=p_src[pos]) {
			matr[max_tag*p_src[p]+p_src[pos]]=true;
			matr[max_tag*p_src[pos]+p_src[p]]=true;
		}
	}
	p++;
	// last row, 1->150
	for(i=1; i<width-1; i++) {
		n = neighSize/2;
		pos = p + neigh[n];
		if (p_src[p]!=p_src[pos]) {
			matr[max_tag*p_src[p]+p_src[pos]]=true;
			matr[max_tag*p_src[pos]+p_src[p]]=true;
		}
		p++;
	}
	// last pixel of the last row
	/*for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighR[n];
		if (p_src[p]!=p_src[pos]) {
			matr[max_tag*p_src[p]+p_src[pos]]=true;
			matr[max_tag*p_src[pos]+p_src[p]]=true;
		}
	}*/
}


// create regions (numbers by "counter") which are lokal minima(s)
int YARPWatershed::markMinimas(YARPImageOf<YarpPixelInt>& result)
{
	// smelt points of same level and give it a number(counter)
	//int* tempRegion = new int[imageSize];
	const int MASK = -2;
	int countF, countB ;
	int counter = 1; // number of region
	int tempi,tempiNeigh;
	int n,i;
	int *p_result=(int *)result.GetRawBuffer();
	int *p_downPos=(int *)downPos2.GetRawBuffer();
	unsigned char *p_src=(unsigned char *)tSrc.GetRawBuffer();

	int maxNeigh;
	int minNeigh;
	int *fixNeigh;

	i=0;
	for (int r=0; r<height; r++) {
		for (int c=0; c<width; c++)	{
			if (p_result[i]==-1 && p_downPos[i]<0) { //unused & minima
				// i is point of a new found region
				countF = countB = 0;
				tempRegion[countB++] = i; // put point in queue
				int tempLevel = p_src[i];
				// find all points which have the same tempLevel like point i
				// and mark them with counter
				while(countF<countB) {
					tempi = tempRegion[countF++]; //get point from queue
					p_result[tempi] = counter; // label point
					int rT=tempi/widthStep;
					int cT=tempi%widthStep;
					if (rT==0) {
						minNeigh=neighSize/2-1;
						maxNeigh=neighSize;
					} else if (rT==height-1) {
						minNeigh=0;
						maxNeigh=neighSize/2+1;
					} else {
						minNeigh=0;
						maxNeigh=neighSize;
					}

					if (cT==0)
						fixNeigh=neighL;
					else if (cT==width-1)
						fixNeigh=neighR;
					else
						fixNeigh=neigh;
					for(n=minNeigh; n<maxNeigh; n++) {
						tempiNeigh = tempi + fixNeigh[n];
						//tempiNeigh a valid image point
						if( p_result[tempiNeigh]==-1 && // unused
							p_src[tempiNeigh]==tempLevel ) //same level
						{
							//each point only once in queue
							tempRegion[countB++] = tempiNeigh; // put point in queue
							p_result[tempiNeigh] = MASK; // prevent double in queue
						}
					}
				}
				counter++;
			}
			i++;
		}
		i+=padding;
	}
	
	//delete[] tempRegion;

	return (counter-1);
}
  

void YARPWatershed::letsRain(YARPImageOf<YarpPixelInt>& result)
{
	int i,cc,tempi;
	//int *tempRegion = new int [imageSize];
	int *p_result=(int *)result.GetRawBuffer();
	int *p_downPos=(int *)downPos2.GetRawBuffer();

	int regionC;
	i=0;
	for (int r=0; r<height; r++) {
		for (int c=0; c<width; c++)	{
			regionC = 0;
			tempi = i;
			while(p_result[tempi] == -1) { // unassigned pixel
				tempRegion[regionC++] = tempi;
				tempi = p_downPos[tempi];
			}
			// a way found down to a lokalMin(lake/point)
			// set all points belong to the way down := tempi,
			// which is the counterNumber of the lokalMin
			int numOfLokalMin = p_result[tempi];
			for(cc=0; cc<regionC; cc++)
				p_result[tempRegion[cc]] = numOfLokalMin;
			i++;
		}
		i+=padding;
	}

	//delete [] tempRegion;
}


void YARPWatershed::findLowerNeigh(const YARPImageOf<YarpPixelMono>& src)
{
	static const int lokalMin = -1;
	static const int saddle = -2;
	unsigned char *p_src=(unsigned char *)src.GetRawBuffer();
	YarpPixelInt *p_downPos;
	//unsigned char *p_tSrc;
	int i,j,n,max,pos,diff,p;

	//downPos.Resize(widthStep,height);
	p_downPos=(YarpPixelInt *)downPos.GetRawBuffer();

	p=0;
	for(j=0; j<height; j++) {
		for(i=0; i<width; i++) {
			p_downPos[p]=lokalMin;
			p++;
		}
		p+=padding;
	}
	
	// first row
	// first pixel
	p=0;
	max = -1;
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighL[n];
		diff = p_src[p]-p_src[pos];
		if(diff > max) { // neigh with lower level
			max = diff;
			p_downPos[p] = pos;
		}
	}
	if( max == 0 ) { // all neighours are on the same level = saddlePoint
		p_downPos[p] = saddle;
	}
	p++;
	// first row, 1->150
	// Note that the pixel of the first row are all the same, so it is
	// useless to check the "far" neighborhoods
	for(i=1; i<width-1; i++) {
		max = -1;
		for(n=neighSize/2-1; n<neighSize; n++) { // no top Neighbor
			pos = p + neigh[n];
			diff = p_src[p]-p_src[pos];
			if(diff > max) { // neigh with lower level
				max = diff;
				p_downPos[p] = pos;
			}
		}
		if( max == 0 ) { // all neighours are on the same level = saddlePoint
			p_downPos[p] = saddle;
		}
		p++;
	}
	// last pixel of the first row
	max = -1;
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighR[n];
		diff = p_src[p]-p_src[pos];
		if(diff > max) { // neigh with lower level
			max = diff;
			p_downPos[p] = pos;
		}
	}
	if( max == 0 ) { // all neighours are on the same level = saddlePoint
		p_downPos[p] = saddle;
	}
	p++;
	p+=padding;

	// inner part
	for(j=1; j<height-1; j++) {
		// first pixel of the inner part
		max = -1;
		for(n=0; n<neighSize; n++) {
			pos = p + neighL[n];
			diff = p_src[p]-p_src[pos];
			if(diff > max) { // neigh with lower level
				max = diff;
				p_downPos[p] = pos;
			}
		}
		if( max == 0 ) { // all neighours are on the same level = saddlePoint
			p_downPos[p] = saddle;
		}
		p++;
		
		for(i=1; i<width-1; i++) {
			max = -1;
			for(n=0; n<neighSize; n++) {
				pos = p + neigh[n];
				diff = p_src[p]-p_src[pos];
				if(diff > max) { // neigh with lower level
					max = diff;
					p_downPos[p] = pos;
				}
			}
			if( max == 0 ) { // all neighours are on the same level = saddlePoint
				p_downPos[p] = saddle;
			}
			p++;
		}
		
		//last pixel of the inner part
		max = -1;
		for(n=0; n<neighSize; n++) {
			pos = p + neighR[n];
			diff = p_src[p]-p_src[pos];
			if(diff > max) { // neigh with lower level
				max = diff;
				p_downPos[p] = pos;
			}
		}
		if( max == 0 ) { // all neighours are on the same level = saddlePoint
			p_downPos[p] = saddle;
		}
		p++;
		
		p+=padding;
	}

	// last row
	// first pixel
	max = -1;
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighL[n];
		diff = p_src[p]-p_src[pos];
		if(diff > max) { // neigh with lower level
			max = diff;
			p_downPos[p] = pos;
		}
	}
	if( max == 0 ) { // all neighours are on the same level = saddlePoint
		p_downPos[p] = saddle;
	}
	p++;
	// last row, 1->150
	for(i=1; i<width-1; i++) {
		max = -1;
		for(n=0; n<neighSize/2+1; n++) { // no top Neighbor
			pos = p + neigh[n];
			diff = p_src[p]-p_src[pos];
			if(diff > max) { // neigh with lower level
				max = diff;
				p_downPos[p] = pos;
			}
		}
		if( max == 0 ) { // all neighours are on the same level = saddlePoint
			p_downPos[p] = saddle;
		}
		p++;
	}
	// last pixel of the last row
	max = -1;
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighR[n];
		diff = p_src[p]-p_src[pos];
		if(diff > max) { // neigh with lower level
			max = diff;
			p_downPos[p] = pos;
		}
	}
	if( max == 0 ) { // all neighours are on the same level = saddlePoint
		p_downPos[p] = saddle;
	}
	
	// try if a saddlePoint have lower "neigh"
	bool change = true;
	while (change) {
		change = false;
		
		// first row
		// first pixel
		p=0;
		if(p_downPos[p] == saddle) {
			for(n=neighSize/2-1; n<neighSize; n++) {
				pos = p + neighL[n];
				if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
					// no more saddle, no lokalMin
					p_downPos[p] = p_downPos[pos];
					change = true;
					break; // next i
				}
			}
		}
		p++;
		// first row, 1->150
		for(i=1; i<width-1; i++) {
			if(p_downPos[p] == saddle) {
				for(n=neighSize/2-1; n<neighSize; n++) {
					pos = p + neigh[n];
					if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
						// no more saddle, no lokalMin
						p_downPos[p] = p_downPos[pos];
						change = true;
						break; // next i
					}
				}
			}
			p++;
		}
		// last pixel of the first row
		if(p_downPos[p] == saddle) {
			for(n=neighSize/2-1; n<neighSize; n++) {
				pos = p + neighR[n];
				if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
					// no more saddle, no lokalMin
					p_downPos[p] = p_downPos[pos];
					change = true;
					break; // next i
				}
			}
		}
		p++;
		p+=padding;

		// inner part
		for(j=1; j<height-1; j++) {
			// first pixel inner part
			if(p_downPos[p] == saddle) {
				for(n=0; n<neighSize; n++) {
					pos = p + neighL[n];
					if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
						// no more saddle, no lokalMin
						p_downPos[p] = p_downPos[pos];
						change = true;
						break; // next i
					}
				}
			}
			p++;

			// 1->150
			for(i=1; i<width-1; i++) {
				if(p_downPos[p] == saddle) {
					for(n=0; n<neighSize; n++) {
						pos = p + neigh[n];
						if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
							// no more saddle, no lokalMin
							p_downPos[p] = p_downPos[pos];
							change = true;
							break; // next i
						}
					}
				}
				p++;
			}

			// last pixel inner part
			if(p_downPos[p] == saddle) {
				for(n=0; n<neighSize; n++) {
					pos = p + neighR[n];
					if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
						// no more saddle, no lokalMin
						p_downPos[p] = p_downPos[pos];
						change = true;
						break; // next i
					}
				}
			}
			p++;

			p+=padding;
		}
		

		// last row
		// first pixel
		if(p_downPos[p] == saddle) {
			for(n=0; n<neighSize/2+1; n++) {
				pos = p + neighL[n];
				if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
					// no more saddle, no lokalMin
					p_downPos[p] = p_downPos[pos];
					change = true;
					break; // next i
				}
			}
		}
		p++;
		// 1->150
		for(i=1; i<width-1; i++) {
			if(p_downPos[p] == saddle) {
				for(n=0; n<neighSize/2+1; n++) {
					pos = p + neigh[n];
					if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
						// no more saddle, no lokalMin
						p_downPos[p] = p_downPos[pos];
						change = true;
						break; // next i
					}
				}
			}
			p++;
		}

		// last pixel
		if(p_downPos[p] == saddle) {
			for(n=0; n<neighSize/2+1; n++) {
				pos = p + neighR[n];
				if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
					// no more saddle, no lokalMin
					p_downPos[p] = p_downPos[pos];
					change = true;
					break; // next i
				}
			}
		}

		
		/*p=0;
		for(j=0; j<height; j++) {
			for(i=0; i<width; i++) {
				if(p_downPos[p] == saddle) {
					for(n=0; n<neighSize; n++) {
						pos = p + neigh[n];
						//if (invalidNeighbor(p,pos)) continue;
						if(p_src[p]==p_src[pos] && p_downPos[pos] >= 0) { 
							// no more saddle, no lokalMin
							p_downPos[p] = p_downPos[pos];
							change = true;
							break; // next i
						}
					}
				}
				p++;
			}
			p+=padding;
		}*/
	}

}
	

void YARPWatershed::createTmpImage(const YARPImageOf<YarpPixelMono>& src)
{
	static const int lokalMin = -1;
	static const int saddle = -2;
	unsigned char *p_src=(unsigned char *)src.GetRawBuffer();
	YarpPixelInt *p_downPos;
	YarpPixelInt *p_downPos2;
	unsigned char *p_tSrc;
	int i,j,p;

	//downPos2=downPos;
	//downPos2.Resize(widthStep,height);

	p_downPos=(YarpPixelInt *)downPos.GetRawBuffer();
	p_downPos2=(YarpPixelInt *)downPos2.GetRawBuffer();
	
	// remaining saddle points must be lokalMins
	// and all points<threshold are lokalMins
	tSrc=src;
	p_tSrc=(unsigned char *)tSrc.GetRawBuffer();
	p=0;
	for(j = 0; j < height ; j++) {
		for(i = 0; i < width; i++) {
			if (p_tSrc[p] < threshold) {
				p_downPos2[p] = lokalMin;
				p_tSrc[p] = threshold;
			}
			else if(p_downPos[p] == saddle)
				p_downPos2[p] = lokalMin;
			else
				p_downPos2[p]=p_downPos[p];
			p++;
		}
		p+=padding;
	}
}


// On place apply
/*bool YARPWatershed::apply(YARPImageOf<YarpPixelMono>& srcdest)
{
    return apply(srcdest, srcdest);
}*/


/*bool YARPWatershed::apply(const YARPImageOf<YarpPixelMono>& src, YARPImageOf<YarpPixelMono>& dest)
{
    //matrix<int> result;
	YARPImageOf<YarpPixelInt> result;

    if (apply(src, result)) {
      tags2Watershed(result, dest);
      return true;
    }

    return false;
}*/


int YARPWatershed::apply(const YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &result)
{
	int num_tags;
	int i;
	int *p_result;

	//result.Resize(widthStep, height);
	p_result=(int *)result.GetRawBuffer();
	
	i=0;
	for (int r=0; r<height; r++) {
		for (int c=0; c<width; c++)	{
			p_result[i]=-1;
			i++;
		}
		i+=padding;
	}

	/*
	* according to idea of the rainfallingWatersheds from
	* P. De Smet and Rui Luis V.P.M.Pires
	* http://telin.rug.ac.be/ipi/watershed
	*/

	findLowerNeigh(src);
	createTmpImage(src);
	num_tags=markMinimas(result);
	letsRain(result);

	return num_tags;
}


int YARPWatershed::applyOnOld(const YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &result)
{
	int num_tags;
	int i;
	int *p_result;

	//result.Resize(widthStep, height);
	p_result=(int *)result.GetRawBuffer();
	
	i=0;
	for (int r=0; r<height; r++) {
		for (int c=0; c<width; c++)	{
			p_result[i]=-1;
			i++;
		}
		i+=padding;
	}

	createTmpImage(src);
	num_tags=markMinimas(result);
	letsRain(result);

	return num_tags;
}


void YARPWatershed::findNeighborhood(YARPImageOf<YarpPixelInt>& tagged, int x, int y, char *blobList)
{
	int i,j,n,pos,p;
	YarpPixelInt seed=tagged(x,y);
	YarpPixelInt *p_tagged=(YarpPixelInt *)tagged.GetRawBuffer();

	// first row
	// first pixel
	p=0;
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighL[n];
		if (p_tagged[pos]==seed) {
			blobList[p_tagged[p]]=1;
			break;
		}
	}
	p++;
	// first row, 1->150
	// Note that the pixel of the first row are all the same, so it is
	// useless to check the "far" neighborhoods
	for(i=1; i<width-1; i++) {
		for(n=neighSize/2-1; n<neighSize; n++) { // no top Neighbor
			pos = p + neigh[n];
			if (p_tagged[pos]==seed) {
				blobList[p_tagged[p]]=1;
				break;
			}
		}
		p++;
	}
	// last pixel of the first row
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighR[n];
		if (p_tagged[pos]==seed) {
			blobList[p_tagged[p]]=1;
			break;
		}
	}
	p++;
	p+=padding;

	// inner part
	for(j=1; j<height-1; j++) {
		// first pixel of the inner part
		for(n=0; n<neighSize; n++) {
			pos = p + neighL[n];
			if (p_tagged[pos]==seed) {
				blobList[p_tagged[p]]=1;
				break;
			}
		}
		p++;
		
		for(i=1; i<width-1; i++) {
			for(n=0; n<neighSize; n++) {
				pos = p + neigh[n];
				if (p_tagged[pos]==seed) {
					blobList[p_tagged[p]]=1;
					break;
				}
			}
			p++;
		}
		
		//last pixel of the inner part
		for(n=0; n<neighSize; n++) {
			pos = p + neighR[n];
			if (p_tagged[pos]==seed) {
				blobList[p_tagged[p]]=1;
				break;
			}
		}
		p++;
		
		p+=padding;
	}

	// last row
	// first pixel
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighL[n];
		if (p_tagged[pos]==seed) {
			blobList[p_tagged[p]]=1;
			break;
		}
	}
	p++;
	// last row, 1->150
	for(i=1; i<width-1; i++) {
		for(n=0; n<neighSize/2+1; n++) { // no top Neighbor
			pos = p + neigh[n];
			if (p_tagged[pos]==seed) {
				blobList[p_tagged[p]]=1;
				break;
			}
		}
		p++;
	}
	// last pixel of the last row
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighR[n];
		if (p_tagged[pos]==seed) {
			blobList[p_tagged[p]]=1;
			break;
		}
	}
}
