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

#include "YARPWatershed.h"

/*	i=0;
	for (int r=0; r<height; r++) {
		for (int c=0; c<width; c++)	{
			i++;
		}
		i+=padding;
	}*/


YARPWatershed::YARPWatershed(const int width1, const int height1, const int wstep, const YarpPixelMono th):
	_gaze(YMatrix(_dh_nrf, 5, DH_left[0]), YMatrix(_dh_nrf, 5, DH_right[0]), YMatrix(4, 4, TBaseline[0]) )
{
	neighborhood8=true;

	watershedColor=0;
	basinColor=255;

	neigh=NULL;

	resize(width1, height1, wstep, th);
}


YARPWatershed::YARPWatershed():
	_gaze(YMatrix(_dh_nrf, 5, DH_left[0]), YMatrix(_dh_nrf, 5, DH_right[0]), YMatrix(4, 4, TBaseline[0]) )
{
	neighborhood8=true;

	watershedColor=0;
	basinColor=255;

	neigh=NULL;
}


void YARPWatershed::resize(const int width1, const int height1, const int wstep, const YarpPixelMono th)
{
	width=width1;
	height=height1;

	imageSize=width*height;
	//imageSizePad=wstep*(height1+252;
	padding=wstep-width1;

	widthStep=wstep;

	threshold=th;
	
	createNeighborhood(wstep, neighborhood8);
	//initBorderLUT(width, height);

	m_boxes = new YARPBox[imageSize];
	ACE_ASSERT(m_boxes != NULL);

	m_attn = new YARPBox[imageSize];
	ACE_ASSERT(m_attn != NULL);

	
	tmpMsk = iplCreateImageHeader(
		1,
		0,
		IPL_DEPTH_1U,
		"GRAY",
		"G",
		IPL_DATA_ORDER_PLANE,
		IPL_ORIGIN_TL,
		IPL_ALIGN_QWORD,
		width1,
		height1,
		NULL,
		NULL,
		NULL,
		NULL);
	iplAllocateImage(tmpMsk,1,0);

	integralRG.resize(width1, height1);
	integralGR.resize(width1, height1);
	integralBY.resize(width1, height1);
}


void YARPWatershed::createNeighborhood(const int widthStep, const bool neigh8)
{
	if (!neigh8) {
		//neigh.resize(4,0,false,false);
		neighSize=4;
	} else {
		neighSize=8;
	}

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


/**
* Initialize a border LUT to save time detecting if a pixel is in
* the border.
*/
/*void YARPWatershed::initBorderLUT(const int width, const int height)
{
	//borderLUT.resize(size,0,false,true);
	borderLUT.Resize(width, height);
	borderLUT.Zero();

	p_borderLUT=(unsigned char *)borderLUT.GetRawBuffer();

	//borderLUT.getRow(0).fill(255);
	memset(borderLUT.GetRawBuffer(), 255, ((IplImage*)borderLUT)->widthStep);
	
	//borderLUT.getRow(borderLUT.lastRow()).fill(255);
	memset(borderLUT.GetArray()[height-1], 255, ((IplImage*)borderLUT)->widthStep);

	for (int y=0;y<height;++y) {
		borderLUT(0,y)=borderLUT(width-1,y)=255;
	}
}*/


/**
* return true if the given point corresponds to a valid neighbor point
* of the given current point
*/
/*inline bool YARPWatershed::invalidNeighbor(const int currentPoint, const int currentNeighbor) const
{
	return ( currentNeighbor<0 || currentNeighbor>=imageSizePad ||
		((p_borderLUT[currentPoint]!=0) &&
		(abs((currentPoint%widthStep) - (currentNeighbor%widthStep)) > 1)) ||
		(currentNeighbor%widthStep)>=width);
}*/


/**
* return true if the given point corresponds to a valid neighbor point
* of the given current point
*/
/*inline bool YARPWatershed::validNeighbor(const int currentPoint, const int currentNeighbor) const
{
	return (currentNeighbor>=0 && currentNeighbor<imageSizePad &&
		((p_borderLUT[currentPoint]==0) ||
		(abs((currentPoint%widthStep) - (currentNeighbor%widthStep)) <= 1)) &&
		(currentNeighbor%widthStep)<width);
}*/


void YARPWatershed::tags2Watershed(const YARPImageOf<YarpPixelInt>& src, YARPImageOf<YarpPixelMono>& dest)
{
	YarpPixelInt *p_src=(YarpPixelInt *)src.GetRawBuffer();
	YarpPixelMono *p_dst;
	int i,j,n,pos,p;

	const YarpPixelMono val = 25;

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


// create regions (numbers by "counter") which are lokal minima(s)
int YARPWatershed::markMinimas(YARPImageOf<YarpPixelInt>& result)
{
	// smelt points of same level and give it a number(counter)
	int* tempRegion = new int[imageSize];
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
						/*if( validNeighbor(tempi,tempiNeigh) && 
							p_result[tempiNeigh]==-1 && // unused
							p_src[tempiNeigh]==tempLevel ) //same level
						{*/
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
	
	delete[] tempRegion;

	return (counter-1);
}
  

void YARPWatershed::letsRain(YARPImageOf<YarpPixelInt>& result)
{
	int i,cc,tempi;
	int *tempRegion = new int [imageSize];
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
			for(cc=0; cc<regionC; cc++) {
				p_result[tempRegion[cc]] = numOfLokalMin;
			}
			i++;
		}
		i+=padding;
	}

	delete [] tempRegion;
}


void YARPWatershed::findLowerNeigh(const YARPImageOf<YarpPixelMono>& src)
{
	static const int lokalMin = -1;
	static const int saddle = -2;
	unsigned char *p_src=(unsigned char *)src.GetRawBuffer();
	YarpPixelInt *p_downPos;
	//unsigned char *p_tSrc;
	int i,j,n,max,pos,diff,p;

	downPos.Resize(widthStep,height);
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
		//if (invalidNeighbor(p,pos)) continue;
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
			//if (invalidNeighbor(p,pos)) continue;
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
		//if (invalidNeighbor(p,pos)) continue;
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
			//if (invalidNeighbor(p,pos)) continue;
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
				//if (invalidNeighbor(p,pos)) continue;
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
			//if (invalidNeighbor(p,pos)) continue;
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
		//if (invalidNeighbor(p,pos)) continue;
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
			//if (invalidNeighbor(p,pos)) continue;
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
		//if (invalidNeighbor(p,pos)) continue;
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
		// first row, 1->150
		for(i=1; i<width-1; i++) {
			if(p_downPos[p] == saddle) {
				for(n=neighSize/2-1; n<neighSize; n++) {
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
		// last pixel of the first row
		if(p_downPos[p] == saddle) {
			for(n=neighSize/2-1; n<neighSize; n++) {
				pos = p + neighR[n];
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
		p+=padding;

		// inner part
		for(j=1; j<height-1; j++) {
			// first pixel inner part
			if(p_downPos[p] == saddle) {
				for(n=0; n<neighSize; n++) {
					pos = p + neighL[n];
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

			// 1->150
			for(i=1; i<width-1; i++) {
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

			// last pixel inner part
			if(p_downPos[p] == saddle) {
				for(n=0; n<neighSize; n++) {
					pos = p + neighR[n];
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

			p+=padding;
		}
		

		// last row
		// first pixel
		if(p_downPos[p] == saddle) {
			for(n=0; n<neighSize/2+1; n++) {
				pos = p + neighL[n];
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
		// 1->150
		for(i=1; i<width-1; i++) {
			if(p_downPos[p] == saddle) {
				for(n=0; n<neighSize/2+1; n++) {
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

		// last pixel
		if(p_downPos[p] == saddle) {
			for(n=0; n<neighSize/2+1; n++) {
				pos = p + neighR[n];
				//if (invalidNeighbor(p,pos)) continue;
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
	downPos2.Resize(widthStep,height);

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

	/*
	* according to idea of the rainfallingWatersheds from
	* P. De Smet and Rui Luis V.P.M.Pires
	* http://telin.rug.ac.be/ipi/watershed
	*/

	createTmpImage(src);
	num_tags=markMinimas(result);
	letsRain(result);

	return num_tags;
}


void YARPWatershed::blobCatalog(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r1, YARPImageOf<YarpPixelMono> &g1, YARPImageOf<YarpPixelMono> &b1, int last_tag)
{
	for(int i = 0; i <= last_tag; i++) {
		m_boxes[i].cmax = m_boxes[i].rmax = 0;
		m_boxes[i].cmin = width;
		m_boxes[i].rmin = height;

		m_boxes[i].xmax = m_boxes[i].ymax = 0;
		m_boxes[i].xmin = m_lp.GetCWidth();
		m_boxes[i].ymin = m_lp.GetCHeight();

		//m_boxes[i].total_sal = 0;
		m_boxes[i].areaLP = 0;
		//m_boxes[i].areaCart = 0;
		m_boxes[i].xsum = m_boxes[i].ysum = 0;
		m_boxes[i].valid = false;
		
		m_boxes[i].id = i;
		
		//m_boxes[i].edge = false;

		m_boxes[i].rgSum=0;
		m_boxes[i].grSum=0;
		m_boxes[i].bySum=0;

		m_boxes[i].rSum=0;
		m_boxes[i].gSum=0;
		m_boxes[i].bSum=0;

	}
  
	// special case for the null tag (0)
	// null tag is not used!!!
	m_boxes[0].rmax = m_boxes[0].rmin = height/2;
	m_boxes[0].cmax = m_boxes[0].cmin = width/2;
	m_boxes[0].xmax = m_boxes[0].xmin = m_lp.GetCWidth() / 2;
	m_boxes[0].ymax = m_boxes[0].ymin = m_lp.GetCHeight() / 2;
	m_boxes[0].salienceTotal=0;
	m_boxes[0].valid = true;

	// pixels are logpolar, averaging is done in cartesian.
	//unsigned char *source = (unsigned char *)img.GetArray();
	//short *tmp = tagged;
	for(int r = 0; r < height; r++)
		for(int c = 0; c < width; c++) {
			long int tag_index = tagged(c, r);
			if (tag_index != 0) {
				m_boxes[tag_index].areaLP++;
				//m_boxes[tag_index].areaCart+=pixelSize[r];

				m_boxes[tag_index].valid = true;

				// euristic to check if the blob is an edge
				//if (img(c,r)>70) m_boxes[tag_index].edge = true;
				
				int x, y;
				m_lp.Logpolar2Cartesian(r, c, x, y);

				if (m_boxes[tag_index].ymax < y) m_boxes[tag_index].ymax = y;
				if (m_boxes[tag_index].ymin > y) m_boxes[tag_index].ymin = y;
				if (m_boxes[tag_index].xmax < x) m_boxes[tag_index].xmax = x;
				if (m_boxes[tag_index].xmin > x) m_boxes[tag_index].xmin = x;

				m_boxes[tag_index].ysum += y;
				m_boxes[tag_index].xsum += x;

				if (m_boxes[tag_index].rmax < r) m_boxes[tag_index].rmax = r;
				if (m_boxes[tag_index].rmin > r) m_boxes[tag_index].rmin = r;
				if (m_boxes[tag_index].cmax < c) m_boxes[tag_index].cmax = c;
				if (m_boxes[tag_index].cmin > c) m_boxes[tag_index].cmin = c;

				m_boxes[tag_index].rgSum += rg(c, r);
				m_boxes[tag_index].grSum += gr(c, r);
				m_boxes[tag_index].bySum += by(c, r);

				m_boxes[tag_index].rSum += r1(c, r);
				m_boxes[tag_index].gSum += g1(c, r);
				m_boxes[tag_index].bSum += b1(c, r);
			}
		}
}	


void YARPWatershed::SortAndComputeSalience(int num_tag, int last_tag)
{	
	//double max_areaCart;
	int max_areaCart;
	int i, max_tag;

	bool *taken = new bool[last_tag];

	memset(taken, false, sizeof(bool)*last_tag);
	
	// create now the subset of attentional boxes.
	for (int box_num = 0; box_num < num_tag; box_num++) {
		// find the most frequent tag, zero does not count 
		max_tag = 0;
		max_areaCart = 0;
		for(i = 1; i <= last_tag; i++) {
			int area = TotalArea(m_boxes[i]);
			if (area > max_areaCart && !taken[i] && m_boxes[i].valid)
			//if(m_boxes[i].areaCart > max_areaCart)
			{
				max_areaCart= area;
				//max_areaCart= m_boxes[i].areaCart;
				max_tag = i;
			}
		}
	
		if (max_tag != 0) {
			m_attn[box_num] = m_boxes[max_tag];

			//m_attn[box_num].valid = true;
			m_attn[box_num].centroid_y = (double)m_boxes[max_tag].ysum / m_boxes[max_tag].areaLP;
			m_attn[box_num].centroid_x = (double)m_boxes[max_tag].xsum / m_boxes[max_tag].areaLP;

			// mean values are calculated only for biggest blobs
			m_attn[box_num].meanRG = m_boxes[max_tag].rgSum / m_boxes[max_tag].areaLP;
			m_attn[box_num].meanGR = m_boxes[max_tag].grSum / m_boxes[max_tag].areaLP;
			m_attn[box_num].meanBY = m_boxes[max_tag].bySum / m_boxes[max_tag].areaLP;
			
			//m_attn[box_num].id = max_tag;
			
			taken[max_tag]=true;
		} else {
			// return the center, no motion
			m_attn[box_num].valid = false;
			//m_attn[box_num].centroid_x = m_lp.GetCWidth() / 2;
			//m_attn[box_num].centroid_y = m_lp.GetCHeight() / 2;
		}
	}

	//PrintBoxes (m_attn, MaxBoxes);

	// I've here MaxBoxes boxes accounting for the largest 
	// regions in the image.
	// remember that there's a bias because of logpolar.

	delete [] taken;
}


// Total number of blobs
// Only for valid boxes
void YARPWatershed::ComputeSalience(int num_blob, int last_tag)
{	
	int i=1;

	m_attn[0].valid=false;
	m_attn[0].salienceTotal=0;
	
	for (; i <= last_tag; i++) {
	//while (i<=last_tag && num_blob>0) {
		if (m_boxes[i].valid) {
			m_attn[i] = m_boxes[i];

			// Maybe I can calculate this values only when it is useful...
			//int area=TotalArea(m_boxes[i]);
			m_attn[i].centroid_y = (double)m_boxes[i].ysum / m_boxes[i].areaLP;
			m_attn[i].centroid_x = (double)m_boxes[i].xsum / m_boxes[i].areaLP;

			// mean values are calculated only for biggest blobs
			m_attn[i].meanRG = m_boxes[i].rgSum / m_boxes[i].areaLP;
			m_attn[i].meanGR = m_boxes[i].grSum / m_boxes[i].areaLP;
			m_attn[i].meanBY = m_boxes[i].bySum / m_boxes[i].areaLP;
			
			//m_attn[box_num].id = max_tag;
		} else
			m_attn[i].valid=false;
		//i++;
	}
}


void YARPWatershed::ComputeSalienceAll(int num_blob, int last_tag)
{	
	int i=1;

	m_attn[0].valid=false;
	m_attn[0].salienceTotal=0;
	
	for (; i <= last_tag; i++) {
	//while (i<=last_tag && num_blob>0) {
		if (m_boxes[i].areaLP) {
			m_attn[i] = m_boxes[i];

			// Maybe I can calculate this values only when it is useful...
			//int area=TotalArea(m_boxes[i]);
			m_attn[i].centroid_y = (double)m_boxes[i].ysum / m_boxes[i].areaLP;
			m_attn[i].centroid_x = (double)m_boxes[i].xsum / m_boxes[i].areaLP;

			// mean values are calculated only for biggest blobs
			m_attn[i].meanRG = m_boxes[i].rgSum / m_boxes[i].areaLP;
			m_attn[i].meanGR = m_boxes[i].grSum / m_boxes[i].areaLP;
			m_attn[i].meanBY = m_boxes[i].bySum / m_boxes[i].areaLP;
			
			//m_attn[box_num].id = max_tag;
		} else
			m_attn[i].valid=false;
		//i++;
	}
}


void YARPWatershed::IOR(YARPImageOf<YarpPixelInt>& tagged, YARPBox* boxes, int num)
{
	for (int i=0; i<num; i++) {
		if (boxes[i].valid) {
			int r, c;
			int x, y;
			//TO DO: transform to "local" axis
			cout<<"box #"<<i<<endl;
			
			_gaze.intersectRay(YARPBabybotHeadKin::KIN_LEFT_PERI, boxes[i].v, x, y);
			m_lp.Cartesian2Logpolar(x, y, r, c);
			if (r>=height) {
				cout<<"out of sight!"<<endl;	
				continue;
			}
			YarpPixelInt index=tagged(c, r);
			
			cout<<"RG : "<<(int)m_attn[index].meanRG<<endl;
			cout<<"GR : "<<(int)m_attn[index].meanGR<<endl;
			cout<<"BY : "<<(int)m_attn[index].meanBY<<endl;
			cout<<"RG diff: "<<abs((int)m_attn[index].meanRG-(int)boxes[i].meanRG)<<endl;
			cout<<"GR diff: "<<abs((int)m_attn[index].meanGR-(int)boxes[i].meanGR)<<endl;
			cout<<"BY diff: "<<abs((int)m_attn[index].meanBY-(int)boxes[i].meanBY)<<endl;
			// the log area changes due to log polare mapping and distance
			cout<<"areaLP diff: "<<abs(m_attn[index].areaLP-boxes[i].areaLP)<<endl;
			cout<<endl;
		}
	}
}


void YARPWatershed::drawIOR(YARPImageOf<YarpPixelMono>& out, YARPBox* boxes, int num)
{
	for (int i=0; i<num; i++) {
		if (boxes[i].valid) {
			int r, c;
			int x, y;
			_gaze.intersectRay(YARPBabybotHeadKin::KIN_LEFT_PERI, boxes[i].v, x, y);
			m_lp.Cartesian2Logpolar(x, y, r, c);
			if (r>=height) {
				continue;
			}
			out(c, r)=255;
		}
	}
}


void YARPWatershed::ComputeMeanColors(int last_tag)
{	
	// create the subset of attentional boxes.
	for (int i = 1; i <= last_tag; i++)
		if (m_boxes[i].areaLP!=0) {
			m_boxes[i].meanColors.r = m_boxes[i].rSum / m_boxes[i].areaLP;
			m_boxes[i].meanColors.g = m_boxes[i].gSum / m_boxes[i].areaLP;
			m_boxes[i].meanColors.b = m_boxes[i].bSum / m_boxes[i].areaLP;
		}
}


void YARPWatershed::RemoveNonValid(int last_tag, const int max_size, const int min_size)
{
	//const int max_size = m_lp.GetCWidth() * m_lp.GetCHeight() / 10;
	//const int max_size = m_lp.GetCWidth() * m_lp.GetCHeight() / 13; // ~area/10
	//const int min_size = 100;

	for (int i = 1; i <= last_tag;i++) {
		int area = TotalArea(m_boxes[i]);
		if (// in effetti eliminare i blob troppo piccoli nn è molto utile se
			// i blob vengono comunque ordinati x grandezza
			area < min_size ||
			area > max_size )
		{
			m_boxes[i].valid=false;
		}
	}

	/*for (int i = 1; i < last_tag;) {
		int area = TotalArea(m_boxes[i]);
		//if (m_boxes[i].valid == false || 
		//	m_boxes[i].areaLP < 5 ||
		//	area < min_size ||
		//	area > max_size)
		// If I delete the boxes with .areaLP > 255 than sum of color can be an int?
		//if (m_boxes[i].valid == false || 
		//	m_boxes[i].areaCart < min_size ||
		//	m_boxes[i].areaCart > max_size ||
		//	m_boxes[i].edge==true)
		if (m_boxes[i].valid == false || 
			area < min_size ||
			area > max_size ||
			m_boxes[i].edge==true)
		{
			if (i == (last_tag-1))
				last_tag--;
			else {
				memcpy(&(m_boxes[i]), &(m_boxes[i+1]), sizeof(YARPBox) * (last_tag - i - 1));
				last_tag--;
			}
		} else
			i++;
	}*/
}

int YARPWatershed::DrawMeanColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged)
{
	/*if (last_tag<numBlob || numBlob==-1) numBlob=last_tag;
	
	for (int i = 0; i < numBlob; i++) {
	//for (int i = 0; i < MaxBoxes; i++) {
		for (int r=m_boxes[i].rmin; r<=m_boxes[i].rmax; r++)
			for (int c=m_boxes[i].cmin; c<=m_boxes[i].cmax; c++)
				if (tagged(c, r)==m_boxes[i].id) {
					id(c ,r)=m_boxes[i].meanColors;
				}
	}

	return numBlob;*/

	for (int r=0; r<height; r++)
		for (int c=0; c<width; c++)
			id(c ,r)=m_boxes[tagged(c, r)].meanColors;
	
	return 1;
}


int YARPWatershed::DrawMeanOpponentColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged)
{
	for (int r=0; r<height; r++)
		for (int c=0; c<width; c++)
			if (m_boxes[tagged(c, r)].valid) {
				id(c ,r).r=m_attn[tagged(c, r)].meanRG;
				id(c ,r).g=m_attn[tagged(c, r)].meanGR;
				id(c ,r).b=m_attn[tagged(c, r)].meanBY;
			}
	
	return 1;
}


int YARPWatershed::DrawContrastLP(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float pBU, float pTD, YarpPixelMono prg, YarpPixelMono pgr, YarpPixelMono pby)
{
	//IplROI zdi;
	int salienceBU, salienceTD;
	int borderRG;
	int borderGR;
	int borderBY;

	int a1,b1,a2,b2;

	int minSalienceBU=INT_MAX;
	int maxSalienceBU=INT_MIN;

	int minSalienceTD=INT_MAX;
	int maxSalienceTD=INT_MIN;

	int pixel0=0;
	int pixel1=1;
	
	tmp.Resize(width, height);
	tmp.Zero();
	
	/*zdi.coi=0;
	zdi.width=1;
	zdi.height=1;*/

	//((IplImage *)tmp)->roi=&zdi;
	((IplImage *)tmp)->maskROI=tmpMsk;

	borderRG=((IplImage *)rg)->BorderMode[IPL_SIDE_BOTTOM_INDEX];
	iplSetBorderMode((IplImage *)rg, IPL_BORDER_REPLICATE, IPL_SIDE_BOTTOM, 0);
	borderGR=((IplImage *)gr)->BorderMode[IPL_SIDE_BOTTOM_INDEX];
	iplSetBorderMode((IplImage *)gr, IPL_BORDER_REPLICATE, IPL_SIDE_BOTTOM, 0);
	borderBY=((IplImage *)by)->BorderMode[IPL_SIDE_BOTTOM_INDEX];
	iplSetBorderMode((IplImage *)by, IPL_BORDER_REPLICATE, IPL_SIDE_BOTTOM, 0);

	// ARRONZAMENTO
	//memset(dst.GetRawBuffer(), 255, ((IplImage*)dst)->imageSize);
	dst.Zero();
	
	if (numBlob>imageSize) numBlob=imageSize;
	
	for (int i = 0; i < numBlob; i++) {
		if (m_attn[i].valid) {
			int rdim=(m_attn[i].rmax-m_attn[i].rmin+1);
			int cdim=(m_attn[i].cmax-m_attn[i].cmin+1);

			//zdi.xOffset=m_attn[i].cmin;
			//zdi.yOffset=m_attn[i].rmin;

			iplPutPixel((IplImage*) tmpMsk, m_attn[i].cmin, m_attn[i].rmin, &pixel1);

			iplBlur((IplImage*) rg, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_attn[i].cRG=abs(tmp(m_attn[i].cmin, m_attn[i].rmin)-m_attn[i].meanRG);
			//m_attn[i].cRG=min(tmp(m_attn[i].cmin, m_attn[i].rmin), m_attn[i].meanRG)*prg;

			iplBlur((IplImage*) gr, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_attn[i].cGR=abs(tmp(m_attn[i].cmin, m_attn[i].rmin)-m_attn[i].meanGR);
			//m_attn[i].cGR=min(tmp(m_attn[i].cmin, m_attn[i].rmin), m_attn[i].meanGR)*pgr;
			
			iplBlur((IplImage*) by, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_attn[i].cBY=abs(tmp(m_attn[i].cmin, m_attn[i].rmin)-m_attn[i].meanBY);
			//m_attn[i].cBY=min(tmp(m_attn[i].cmin, m_attn[i].rmin), m_attn[i].meanBY)*pby;

			// con il max se un blob è rosso e verde nn va molto bene (????)
			salienceBU=m_attn[i].cRG;

			if (salienceBU<m_attn[i].cGR)
				salienceBU=m_attn[i].cGR;

			if (salienceBU<m_attn[i].cBY)
				salienceBU=m_attn[i].cBY;

			//salienceBU=m_attn[i].cRG+m_attn[i].cGR+m_attn[i].cBY;

			/*salienceTD=abs(m_attn[i].meanRG-prg);
							
			if (salienceTD<abs(m_attn[i].meanGR-pgr))
				salienceTD=abs(m_attn[i].meanGR-pgr);
				
			if (salienceTD<abs(m_attn[i].meanBY-pby))
				salienceTD=abs(m_attn[i].meanBY-pby);

			salienceTD=255-salienceTD;*/

			salienceTD=sqrt((m_attn[i].meanRG-prg)*(m_attn[i].meanRG-prg)+
			                (m_attn[i].meanGR-pgr)*(m_attn[i].meanGR-pgr)+
			                (m_attn[i].meanBY-pby)*(m_attn[i].meanBY-pby));
			salienceTD=255-salienceTD/sqrt(3);


			//if (salienceTD<230) salienceTD=0;

			//if (salienceTD<0) salienceTD=0;
			
			m_attn[i].salienceBU=salienceBU;
			m_attn[i].salienceTD=salienceTD;

			if (salienceBU<minSalienceBU)
				minSalienceBU=salienceBU;
			else if (salienceBU>maxSalienceBU)
				maxSalienceBU=salienceBU;

			if (salienceTD<minSalienceTD)
				minSalienceTD=salienceTD;
			else if (salienceTD>maxSalienceTD)
				maxSalienceTD=salienceTD;

			iplPutPixel((IplImage*) tmpMsk, m_attn[i].cmin, m_attn[i].rmin, &pixel0);
		}
	}

	const int maxDest=200;
	
	if (maxSalienceBU!=minSalienceBU) {
		a1=255*(maxDest-1)/(maxSalienceBU-minSalienceBU);
		b1=1-a1*minSalienceBU/255;
	} else {
		a1=0;
		b1=0;
	}

	if (maxSalienceTD!=minSalienceTD) {
		a2=255*(maxDest-1)/(maxSalienceTD-minSalienceTD);
		b2=1-a2*minSalienceTD/255;
		//a2=255*254/(minSalienceTD-maxSalienceTD);
		//b2=1-a2*maxSalienceTD/255;
	} else {
		a2=0;
		b2=0;
	}

	for (i = 0; i < numBlob; i++) {
		if (m_attn[i].valid) {
			m_attn[i].salienceTotal=pBU*(a1*m_attn[i].salienceBU/255+b1)+pTD*(a2*m_attn[i].salienceTD/255+b2);
			for (int r=m_attn[i].rmin; r<=m_attn[i].rmax; r++)
				for (int c=m_attn[i].cmin; c<=m_attn[i].cmax; c++)
					if (tagged(c, r)==m_attn[i].id) {
						//unsigned char sal=(a1*m_attn[i].salienceBU/255+b1+a2*m_attn[i].salienceTD/255+b2)/2;
						//if (sal>th) dst(c ,r)=sal;
						//else dst(c ,r)=0;
						dst(c ,r)=m_attn[i].salienceTotal;
						//dst(c ,r)=a1*m_attn[i].salienceBU/255+b1;
						/*dst(c ,r)=a2*m_attn[i].salienceTD/255+b2;
						if (dst(c ,r)<240) dst(c ,r)=1;
						else dst(c ,r)=dst(c ,r)-240;*/
					}
		}
	}
	
	((IplImage *)rg)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=borderRG;
	((IplImage *)gr)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=borderGR;
	((IplImage *)by)->BorderMode[IPL_SIDE_BOTTOM_INDEX]=borderBY;

	return numBlob;
}


int YARPWatershed::DrawContrastLP2(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float pBU, float pTD, YarpPixelMono prg, YarpPixelMono pgr, YarpPixelMono pby)
{
	//IplROI zdi;
	int salienceBU, salienceTD;

	int a1,b1,a2,b2;

	int minSalienceBU=INT_MAX;
	int maxSalienceBU=INT_MIN;

	int minSalienceTD=INT_MAX;
	int maxSalienceTD=INT_MIN;

	dst.Zero();
	
	if (numBlob>imageSize) numBlob=imageSize;

	integralRG.computeCartesian(rg);
	integralGR.computeCartesian(gr);
	integralBY.computeCartesian(by);
	
	for (int i = 0; i < numBlob; i++) {
		if (m_attn[i].valid) {
			int tmp;
			
			int rdim=(m_attn[i].rmax-m_attn[i].rmin+1);
			int cdim=(m_attn[i].cmax-m_attn[i].cmin+1); //BUG: se va da parte a parte esce una dimensione enorme!

			//zdi.xOffset=m_attn[i].cmin;
			//zdi.yOffset=m_attn[i].rmin;

			tmp=255*252*152*integralRG.getSaliencyLp(m_attn[i].cmax+cdim,
				m_attn[i].cmin-cdim,
				m_attn[i].rmax+rdim,
				m_attn[i].rmin-rdim)/(rdim*cdim);
			m_attn[i].cRG=abs(tmp-m_attn[i].meanRG);

			tmp=255*252*152*integralGR.getSaliencyLp(m_attn[i].cmax+cdim,
				m_attn[i].cmin-cdim,
				m_attn[i].rmax+rdim,
				m_attn[i].rmin-rdim)/(rdim*cdim);
			m_attn[i].cGR=abs(tmp-m_attn[i].meanGR);
			
			tmp=255*252*152*integralBY.getSaliencyLp(m_attn[i].cmax+cdim,
				m_attn[i].cmin-cdim,
				m_attn[i].rmax+rdim,
				m_attn[i].rmin-rdim)/(rdim*cdim);
			m_attn[i].cBY=abs(tmp-m_attn[i].meanBY);

			// con il max se un blob è rosso e verde nn va molto bene (????)
			salienceBU=m_attn[i].cRG;

			if (salienceBU<m_attn[i].cGR)
				salienceBU=m_attn[i].cGR;

			if (salienceBU<m_attn[i].cBY)
				salienceBU=m_attn[i].cBY;

			//salienceBU=m_attn[i].cRG+m_attn[i].cGR+m_attn[i].cBY;

			salienceTD=abs(m_attn[i].meanRG-prg);
							
			if (salienceTD<abs(m_attn[i].meanGR-pgr))
				salienceTD=abs(m_attn[i].meanGR-pgr);
				
			if (salienceTD<abs(m_attn[i].meanBY-pby))
				salienceTD=abs(m_attn[i].meanBY-pby);

			salienceTD=255-salienceTD;

			//if (salienceTD<230) salienceTD=0;

			//if (salienceTD<0) salienceTD=0;
			
			m_attn[i].salienceBU=salienceBU;
			m_attn[i].salienceTD=salienceTD;

			if (salienceBU<minSalienceBU)
				minSalienceBU=salienceBU;
			else if (salienceBU>maxSalienceBU)
				maxSalienceBU=salienceBU;

			if (salienceTD<minSalienceTD)
				minSalienceTD=salienceTD;
			else if (salienceTD>maxSalienceTD)
				maxSalienceTD=salienceTD;
		}
	}

	if (maxSalienceBU!=minSalienceBU) {
		a1=255*254/(maxSalienceBU-minSalienceBU);
		b1=1-a1*minSalienceBU/255;
	} else {
		a1=0;
		b1=0;
	}

	if (maxSalienceTD!=minSalienceTD) {
		a2=255*254/(maxSalienceTD-minSalienceTD);
		b2=1-a2*minSalienceTD/255;
		//a2=255*254/(minSalienceTD-maxSalienceTD);
		//b2=1-a2*maxSalienceTD/255;
	} else {
		a2=0;
		b2=0;
	}

	for (i = 0; i < numBlob; i++) {
		if (m_attn[i].valid) {
			m_attn[i].salienceTotal=pBU*(a1*m_attn[i].salienceBU/255+b1)+pTD*(a2*m_attn[i].salienceTD/255+b2);
			for (int r=m_attn[i].rmin; r<=m_attn[i].rmax; r++)
				for (int c=m_attn[i].cmin; c<=m_attn[i].cmax; c++)
					if (tagged(c, r)==m_attn[i].id) {
						//unsigned char sal=(a1*m_attn[i].salienceBU/255+b1+a2*m_attn[i].salienceTD/255+b2)/2;
						//if (sal>th) dst(c ,r)=sal;
						//else dst(c ,r)=0;
						dst(c ,r)=m_attn[i].salienceTotal;
						//dst(c ,r)=a1*m_attn[i].salienceBU/255+b1;
						/*dst(c ,r)=a2*m_attn[i].salienceTD/255+b2;
						if (dst(c ,r)<240) dst(c ,r)=1;
						else dst(c ,r)=dst(c ,r)-240;*/
					}
		}
	}
	
	return numBlob;
}


int YARPWatershed::DrawGrayLP(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int numBlob)
{
	if (numBlob>256) numBlob=256;
	
	for (int i = 0; i < numBlob; i++) {
		if (m_attn[i].valid) { // Is this check necessary?
			for (int r=m_attn[i].rmin; r<=m_attn[i].rmax; r++)
				for (int c=m_attn[i].cmin; c<=m_attn[i].cmax; c++)
					if (tagged(c, r)==m_attn[i].id)
						id(c ,r)=255-i;
		}
	}

	return numBlob;
}


void YARPWatershed::DrawFoveaBlob(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, const YarpPixelMono gray)
{
	id.Zero();
	
	int tag=tagged(0, 0);
	
	for (int r=m_boxes[tag].rmin; r<=m_boxes[tag].rmax; r++)
		for (int c=m_boxes[tag].cmin; c<=m_boxes[tag].cmax; c++)
			if (tagged(c, r)==tag)
				id(c ,r)=gray;
}


void YARPWatershed::SeedColor(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int x, int y, int col)
{
	long int seed=tagged(x,y);

	//if (m_boxes[seed].valid && m_boxes[seed].areaLP > 5 && m_boxes[seed].areaLP < 250) {
		for (int r=m_boxes[seed].rmin; r<=m_boxes[seed].rmax; r++)
			for (int c=m_boxes[seed].cmin; c<=m_boxes[seed].cmax; c++)
				if (tagged(c, r)==m_boxes[seed].id)
					id(c ,r)=col;
	//}
}


void YARPWatershed::findNeighborhood(YARPImageOf<YarpPixelInt>& tagged, int x, int y, bool *blobList, int max_tag)
{
	int i,j,n,pos,p;
	YarpPixelInt seed=tagged(x,y);
	YarpPixelInt *p_tagged=(YarpPixelInt *)tagged.GetRawBuffer();

	memset(blobList, false, sizeof(bool)*max_tag);
	
	// first row
	// first pixel
	p=0;
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighL[n];
		if (p_tagged[pos]==seed) {
			blobList[p_tagged[p]]=true;
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
				blobList[p_tagged[p]]=true;
				break;
			}
		}
		p++;
	}
	// last pixel of the first row
	for(n=neighSize/2-1; n<neighSize; n++) {
		pos = p + neighR[n];
		if (p_tagged[pos]==seed) {
			blobList[p_tagged[p]]=true;
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
				blobList[p_tagged[p]]=true;
				break;
			}
		}
		p++;
		
		for(i=1; i<width-1; i++) {
			for(n=0; n<neighSize; n++) {
				pos = p + neigh[n];
				if (p_tagged[pos]==seed) {
					blobList[p_tagged[p]]=true;
					break;
				}
			}
			p++;
		}
		
		//last pixel of the inner part
		for(n=0; n<neighSize; n++) {
			pos = p + neighR[n];
			if (p_tagged[pos]==seed) {
				blobList[p_tagged[p]]=true;
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
			blobList[p_tagged[p]]=true;
			break;
		}
	}
	p++;
	// last row, 1->150
	for(i=1; i<width-1; i++) {
		for(n=0; n<neighSize/2+1; n++) { // no top Neighbor
			pos = p + neigh[n];
			if (p_tagged[pos]==seed) {
				blobList[p_tagged[p]]=true;
				break;
			}
		}
		p++;
	}
	// last pixel of the last row
	for(n=0; n<neighSize/2+1; n++) {
		pos = p + neighR[n];
		if (p_tagged[pos]==seed) {
			blobList[p_tagged[p]]=true;
			break;
		}
	}
}


// If I use the function I shouldn't strecth the color opponency maps
void YARPWatershed::fuseFoveaBlob(YARPImageOf<YarpPixelInt>& tagged, bool *blobList, int max_tag)
{
	memset(blobList, false, sizeof(bool)*max_tag);
	
	/*const YarpPixelMono rg0=m_attn[1].meanRG;
	const YarpPixelMono gr0=m_attn[1].meanGR;
	const YarpPixelMono by0=m_attn[1].meanBY;*/
	
	for (int c=0; c<width; c++) {
		YarpPixelInt seed=1;
		YarpPixelMono rg0;
		YarpPixelMono gr0;
		YarpPixelMono by0;

		int r=1;
		while(1) {
			rg0=m_attn[seed].meanRG;
			gr0=m_attn[seed].meanGR;
			by0=m_attn[seed].meanBY;

			while(r<height && tagged(c, r)==seed)
				r++;
			if (r==height) break;
			seed=tagged(c, r);
			//if (abs(m_attn[seed].meanRG-rg0)+abs(m_attn[seed].meanGR-gr0)+abs(m_attn[seed].meanBY-by0)<13 )
			if ((m_attn[seed].meanRG-rg0)*(m_attn[seed].meanRG-rg0)+(m_attn[seed].meanGR-gr0)*(m_attn[seed].meanGR-gr0)+(m_attn[seed].meanBY-by0)*(m_attn[seed].meanBY-by0)<150 )
				blobList[seed]=true;
			else
				break;
		}
	}
				
}


// If I use the function I shouldn't strecth the color opponency maps
void YARPWatershed::fuseFoveaBlob2(YARPImageOf<YarpPixelInt>& tagged, bool *blobList, int max_tag)
{
	const YarpPixelMono rg0=m_attn[1].meanRG;
	const YarpPixelMono gr0=m_attn[1].meanGR;
	const YarpPixelMono by0=m_attn[1].meanBY;
	
	for (int i=0; i<max_tag; i++) {
		if (blobList[i])
			if ((m_attn[i].meanRG-rg0)*(m_attn[i].meanRG-rg0)+(m_attn[i].meanGR-gr0)*(m_attn[i].meanGR-gr0)+(m_attn[i].meanBY-by0)*(m_attn[i].meanBY-by0)>=110 )
				blobList[i]=false;
	}
}


void YARPWatershed::drawBlobList(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, bool *blobList, int max_tag, const YarpPixelMono gray)
{
	//id.Zero();
	
	for (int tag=1; tag<max_tag; tag++)
		if (blobList[tag])
			for (int r=m_boxes[tag].rmin; r<=m_boxes[tag].rmax; r++)
				for (int c=m_boxes[tag].cmin; c<=m_boxes[tag].cmax; c++)
					if (tagged(c, r)==tag)
						id(c ,r)=gray;
}


void YARPWatershed::statBlobList(YARPImageOf<YarpPixelInt>& tagged, bool *blobList, int max_tag, YARPBox &blob)
{
	int area=0;
	
	unsigned long int rgSum=0;
	unsigned long int grSum=0;
	unsigned long int bySum=0;

	for (int tag=1; tag<max_tag; tag++)
		if (blobList[tag]) {
			area+=m_attn[tag].areaLP;
			rgSum=m_attn[tag].rgSum;
			grSum=m_attn[tag].grSum;
			bySum=m_attn[tag].bySum;
		}
	
	blob.areaLP=area;
	if (area!=0) {
		blob.rgSum=rgSum;
		blob.grSum=grSum;
		blob.bySum=bySum;
		blob.meanRG=rgSum/area;
		blob.meanGR=grSum/area;
		blob.meanBY=bySum/area;
	}
}


void YARPWatershed::getBlob(YARPImageOf<YarpPixelInt>& tagged, int x, int y, YARPBox &blob)
{
	blob=m_attn[tagged(x, y)];
}


void YARPWatershed::maxSalienceBlobs(YARPImageOf<YarpPixelInt>& tagged, int max_tag, YARPBox* boxes, int num)
{
	int *pos = new int [num];
	
	memset(pos, 0, sizeof(int)*num);
	
	for (int l = 0; l < max_tag; l++) {
		if (m_attn[l].valid) {
			for (int i=0; i<num; i++) {
				if (m_attn[l].salienceTotal>m_attn[pos[i]].salienceTotal) {
					for (int j=num-1; j>i; j--)
						pos[j]=pos[j-1];
					pos[i]=l;
					break;
				}
			}
		}
	}

	for (l=0; l<num; l++) {
		boxes[l]=m_attn[pos[l]];
		_gaze.computeRay(YARPBabybotHeadKin::KIN_LEFT_PERI, boxes[l].v , (int)boxes[l].centroid_x, (int)boxes[l].centroid_y);
	}

	delete [] pos;
	// If the numbers of valid blobs are less than num???
}


void YARPWatershed::maxSalienceBlob(YARPImageOf<YarpPixelInt>& tagged, int max_tag, YARPBox &box)
{
	int max=0;
	
	for (int l = 1; l < max_tag; l++) {
		if (m_attn[l].valid)
			if (m_attn[l].salienceTotal>m_attn[max].salienceTotal)
				max=l;
	}

	box=m_attn[max];
	_gaze.computeRay(YARPBabybotHeadKin::KIN_LEFT_PERI, box.v , (int)box.centroid_x, (int)box.centroid_y);
}
