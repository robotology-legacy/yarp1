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

#include <iostream>
using namespace std;

#include "YARPSalience.h"


YARPSalience::YARPSalience(const int width1, const int height1):
	_gaze( YMatrix(_dh_nrf, 5, DH_left[0]), YMatrix(_dh_nrf, 5, DH_right[0]), YMatrix(4, 4, TBaseline[0]) )
{
	resize(width1, height1);
}


/*YARPSalience::YARPSalience():
	_gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) )
{
}*/


void YARPSalience::resize(const int width1, const int height1)
{
	width=width1;
	height=height1;

	imageSize=width*height;

	m_boxes = new YARPBox[imageSize];
	ACE_ASSERT(m_boxes != NULL);

	_checkCutted = new bool[imageSize];
	ACE_ASSERT(_checkCutted != NULL);

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


YARPSalience::~YARPSalience()
{
	if (m_boxes!=NULL)
		delete [] m_boxes;
	if (_checkCutted!=NULL)
		delete [] _checkCutted;
}


void YARPSalience::blobCatalog(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, YARPImageOf<YarpPixelMono> &r1, YARPImageOf<YarpPixelMono> &g1, YARPImageOf<YarpPixelMono> &b1, int last_tag)
{
	for (int i = 0; i <= last_tag; i++) {
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
		
		m_boxes[i].cutted = false;
		
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
	m_boxes[0].valid = false;

	memset(_checkCutted, 0, sizeof(bool)*width*height);
	
	for (int r = 0; r < height; r++)
		if (tagged(0, r)==tagged(width-1, r)) {
			m_boxes[tagged(0, r)].cutted=true;
			m_boxes[tagged(0, r)].indexCutted=r;
		}
		
	// pixels are logpolar, averaging is done in cartesian
	for (r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {
			long int tag_index = tagged(c, r);
			m_boxes[tag_index].areaLP++;
			//m_boxes[tag_index].areaCart+=pixelSize[r];

			m_boxes[tag_index].valid = true;

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
			if (!m_boxes[tag_index].cutted) {
				if (m_boxes[tag_index].cmax < c) m_boxes[tag_index].cmax = c;
				if (m_boxes[tag_index].cmin > c) m_boxes[tag_index].cmin = c;
			} else
				_checkCutted[m_boxes[tag_index].indexCutted*height+c]=true;

			m_boxes[tag_index].rgSum += rg(c, r);
			m_boxes[tag_index].grSum += gr(c, r);
			m_boxes[tag_index].bySum += by(c, r);

			m_boxes[tag_index].rSum += r1(c, r);
			m_boxes[tag_index].gSum += g1(c, r);
			m_boxes[tag_index].bSum += b1(c, r);
		}
	}
	
	for (i = 1; i <= last_tag; i++) {
		if (m_boxes[i].cutted) {
			int index = m_boxes[i].indexCutted;
			int c = 1;
			while (c<width && _checkCutted[index*height+c]==true)
				c++;
			if (c<width) {
				m_boxes[i].cmax=c-1;
				c++;
				while (_checkCutted[index*height+c]==false)
					c++;
				m_boxes[i].cmin=c-width;
			} else {
				m_boxes[i].cmin=0;
				m_boxes[i].cmax=width;
			}
		}
	}
}	


/*void YARPWatershed::SortAndComputeSalience(int num_tag, int last_tag)
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
}*/


// Total number of blobs
// Only for valid boxes
void YARPSalience::ComputeSalience(int num_blob, int last_tag)
{	
	int i=1;

	for (; i <= last_tag; i++) {
	//while (i<=last_tag && num_blob>0) {
		if (m_boxes[i].valid) {

			//int area=TotalArea(m_boxes[i]);
			m_boxes[i].centroid_y = (double)m_boxes[i].ysum / m_boxes[i].areaLP;
			m_boxes[i].centroid_x = (double)m_boxes[i].xsum / m_boxes[i].areaLP;

			m_boxes[i].meanRG = m_boxes[i].rgSum / m_boxes[i].areaLP;
			m_boxes[i].meanGR = m_boxes[i].grSum / m_boxes[i].areaLP;
			m_boxes[i].meanBY = m_boxes[i].bySum / m_boxes[i].areaLP;
			
			//m_boxes[box_num].id = max_tag;
		} else
			m_boxes[i].valid=false;
		//i++;
	}
}


//Valid and not valid boxes
void YARPSalience::ComputeSalienceAll(int num_blob, int last_tag)
{	
	int i=1;

	for (; i <= last_tag; i++) {
	//while (i<=last_tag && num_blob>0) {
		if (m_boxes[i].areaLP) {

			//m_boxes[i].areaCart=TotalArea(m_boxes[i]);
			m_boxes[i].centroid_y = (double)m_boxes[i].ysum / m_boxes[i].areaLP;
			m_boxes[i].centroid_x = (double)m_boxes[i].xsum / m_boxes[i].areaLP;

			m_boxes[i].meanRG = m_boxes[i].rgSum / m_boxes[i].areaLP;
			m_boxes[i].meanGR = m_boxes[i].grSum / m_boxes[i].areaLP;
			m_boxes[i].meanBY = m_boxes[i].bySum / m_boxes[i].areaLP;
			
			//m_boxes[box_num].id = max_tag;
		} else
			m_boxes[i].valid=false;
		//i++;
	}
}


void YARPSalience::checkIOR(YARPImageOf<YarpPixelInt>& tagged, YARPBox* boxes, int num)
{
	for (int i=0; i<num; i++) {
		if (boxes[i].valid) {
			int r, c;
			int x, y;
			cout<<"box #"<<i<<endl;
			
			_gaze.intersectRay(YARPBabybotHeadKin::KIN_LEFT_PERI, boxes[i].elev, boxes[i].az, x, y);
			m_lp.Cartesian2Logpolar(x, y, r, c);
			if (r>=height) {
				cout<<"out of sight!"<<endl;	
				continue;
			}
			YarpPixelInt index=tagged(c, r);
			
			//cout<<"RG : "<<(int)m_boxes[index].meanRG<<endl;
			//cout<<"GR : "<<(int)m_boxes[index].meanGR<<endl;
			//cout<<"BY : "<<(int)m_boxes[index].meanBY<<endl;
			cout<<"RG diff:"<<abs((int)m_boxes[index].meanRG-(int)boxes[i].meanRG);
			cout<<" GR diff:"<<abs((int)m_boxes[index].meanGR-(int)boxes[i].meanGR);
			cout<<" BY diff:"<<abs((int)m_boxes[index].meanBY-(int)boxes[i].meanBY)<<endl;
			// the log area changes due to log polar mapping and distance
			//cout<<"areaLP diff: "<<abs(m_boxes[index].areaLP-boxes[i].areaLP)<<endl;
			cout<<endl;
		}
	}
}


void YARPSalience::doIOR(YARPImageOf<YarpPixelInt>& tagged, YARPBox* boxes, int num)
{
	for (int l=0; l<num; l++) {
		if (boxes[l].valid) {
			int r1, c1, r, c;
			int x, y;
			
			_gaze.intersectRay(YARPBabybotHeadKin::KIN_LEFT_PERI, boxes[l].elev, boxes[l].az, x, y);
			m_lp.Cartesian2Logpolar(x, y, r1, c1);
			for (int i=-3; i<=3; i++)
				for (int j=-3; j<=3; j++) {
					r=r1+i;
					c=c1+j;
					if (r<height && r>=0) {
						if (c<0) c=width+c;
						else if (c>width-1) c=c-width;
						YarpPixelInt index=tagged(c, r);
						int crg=m_boxes[index].meanRG-boxes[l].meanRG;
						int cgr=m_boxes[index].meanGR-boxes[l].meanGR;
						int cby=m_boxes[index].meanBY-boxes[l].meanBY;

						if (crg*crg+cgr*cgr+cby*cby<1500) {
							m_boxes[index].valid=false;
						}
					}
				}
		}
	}
}


void YARPSalience::drawIOR(YARPImageOf<YarpPixelMono>& out, YARPBox* boxes, int num)
{
	for (int i=0; i<num; i++) {
		if (boxes[i].valid) {
			int r, c;
			int x, y;
			_gaze.intersectRay(YARPBabybotHeadKin::KIN_LEFT_PERI, boxes[i].elev, boxes[i].az, x, y);
			m_lp.Cartesian2Logpolar(x, y, r, c);
			if (r>=height)
				continue;
			out(c, r)=255;
		}
	}
}


void YARPSalience::ComputeMeanColors(int last_tag)
{	
	// create the subset of attentional boxes.
	for (int i = 1; i <= last_tag; i++)
		if (m_boxes[i].areaLP!=0) {
			m_boxes[i].meanColors.r = m_boxes[i].rSum / m_boxes[i].areaLP;
			m_boxes[i].meanColors.g = m_boxes[i].gSum / m_boxes[i].areaLP;
			m_boxes[i].meanColors.b = m_boxes[i].bSum / m_boxes[i].areaLP;
		}
}


void YARPSalience::RemoveNonValid(int last_tag, const int max_size, const int min_size)
{
	//const int max_size = m_lp.GetCWidth() * m_lp.GetCHeight() / 10;
	//const int max_size = m_lp.GetCWidth() * m_lp.GetCHeight() / 13; // ~area/10
	//const int min_size = 100;

	for (int i = 1; i <= last_tag;i++) {
		int area = TotalArea(m_boxes[i]);
		// eliminare i blob troppo piccoli nn è molto utile se
		// i blob vengono comunque ordinati x grandezza.
		if ( area < min_size ||	area > max_size ) {
			m_boxes[i].valid=false;
		} else {
			if ( !isWithinRange((int)m_boxes[i].centroid_x, (int)m_boxes[i].centroid_y, m_boxes[i].elev, m_boxes[i].az) )
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


void YARPSalience::removeBlobList(char *blobList, int max_tag)
{
	for (int i=0; i<max_tag; i++) {
		if (blobList[i]==2)
			m_boxes[i].valid=false;
	}
}


void YARPSalience::DrawMeanColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged)
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
}


void YARPSalience::DrawMeanOpponentColorsLP(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged)
{
	for (int r=0; r<height; r++)
		for (int c=0; c<width; c++) {
			//if (m_boxes[tagged(c, r)].valid) {
				id(c ,r).r=m_boxes[tagged(c, r)].meanRG;
				id(c ,r).g=m_boxes[tagged(c, r)].meanGR;
				id(c ,r).b=m_boxes[tagged(c, r)].meanBY;
			//}
		}
}


int YARPSalience::DrawContrastLP(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float pBU, float pTD, YarpPixelMono prg, YarpPixelMono pgr, YarpPixelMono pby)
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

	dst.Zero();
	
	if (numBlob>imageSize) numBlob=imageSize;
	
	for (int i = 1; i < numBlob; i++) {
		if (m_boxes[i].valid) {
			int rdim=(m_boxes[i].rmax-m_boxes[i].rmin+1);
			int cdim=(m_boxes[i].cmax-m_boxes[i].cmin+1);

			//zdi.xOffset=m_boxes[i].cmin;
			//zdi.yOffset=m_boxes[i].rmin;

			iplPutPixel((IplImage*) tmpMsk, m_boxes[i].cmin, m_boxes[i].rmin, &pixel1);

			iplBlur((IplImage*) rg, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_boxes[i].cRG=abs(tmp(m_boxes[i].cmin, m_boxes[i].rmin)-m_boxes[i].meanRG);
			//m_boxes[i].cRG=min(tmp(m_boxes[i].cmin, m_boxes[i].rmin), m_boxes[i].meanRG)*prg;

			iplBlur((IplImage*) gr, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_boxes[i].cGR=abs(tmp(m_boxes[i].cmin, m_boxes[i].rmin)-m_boxes[i].meanGR);
			//m_boxes[i].cGR=min(tmp(m_boxes[i].cmin, m_boxes[i].rmin), m_boxes[i].meanGR)*pgr;
			
			iplBlur((IplImage*) by, (IplImage*) tmp, 3*cdim, 3*rdim, cdim, rdim);
			m_boxes[i].cBY=abs(tmp(m_boxes[i].cmin, m_boxes[i].rmin)-m_boxes[i].meanBY);
			//m_boxes[i].cBY=min(tmp(m_boxes[i].cmin, m_boxes[i].rmin), m_boxes[i].meanBY)*pby;

			salienceBU=sqrt(m_boxes[i].cRG*m_boxes[i].cRG+
			                m_boxes[i].cGR*m_boxes[i].cGR+
			                m_boxes[i].cBY*m_boxes[i].cBY);
			salienceBU=salienceBU/sqrt(3);

			/*salienceBU=m_boxes[i].cRG;

			if (salienceBU<m_boxes[i].cGR)
				salienceBU=m_boxes[i].cGR;

			if (salienceBU<m_boxes[i].cBY)
				salienceBU=m_boxes[i].cBY;*/

			//salienceBU=m_boxes[i].cRG+m_boxes[i].cGR+m_boxes[i].cBY;


			/*salienceTD=abs(m_boxes[i].meanRG-prg);
							
			if (salienceTD<abs(m_boxes[i].meanGR-pgr))
				salienceTD=abs(m_boxes[i].meanGR-pgr);
				
			if (salienceTD<abs(m_boxes[i].meanBY-pby))
				salienceTD=abs(m_boxes[i].meanBY-pby);

			salienceTD=255-salienceTD;*/

			salienceTD=sqrt((m_boxes[i].meanRG-prg)*(m_boxes[i].meanRG-prg)+
			                (m_boxes[i].meanGR-pgr)*(m_boxes[i].meanGR-pgr)+
			                (m_boxes[i].meanBY-pby)*(m_boxes[i].meanBY-pby));
			salienceTD=255-salienceTD/sqrt(3);

			// if the color is too different it isn't in the scene!
			if (salienceTD<200) salienceTD=0;

			//if (salienceTD<0) salienceTD=0;
			
			m_boxes[i].salienceBU=salienceBU;
			m_boxes[i].salienceTD=salienceTD;

			if (salienceBU<minSalienceBU)
				minSalienceBU=salienceBU;
			else if (salienceBU>maxSalienceBU)
				maxSalienceBU=salienceBU;

			if (salienceTD<minSalienceTD)
				minSalienceTD=salienceTD;
			else if (salienceTD>maxSalienceTD)
				maxSalienceTD=salienceTD;

			iplPutPixel((IplImage*) tmpMsk, m_boxes[i].cmin, m_boxes[i].rmin, &pixel0);
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
		if (m_boxes[i].valid) {
			m_boxes[i].salienceTotal=pBU*(a1*m_boxes[i].salienceBU/255+b1)+pTD*(a2*m_boxes[i].salienceTD/255+b2);
			for (int r=m_boxes[i].rmin; r<=m_boxes[i].rmax; r++)
				for (int c=m_boxes[i].cmin; c<=m_boxes[i].cmax; c++)
					if (tagged(c, r)==m_boxes[i].id) {
						//unsigned char sal=(a1*m_boxes[i].salienceBU/255+b1+a2*m_boxes[i].salienceTD/255+b2)/2;
						//if (sal>th) dst(c ,r)=sal;
						//else dst(c ,r)=0;
						dst(c ,r)=m_boxes[i].salienceTotal;
						//dst(c ,r)=a1*m_boxes[i].salienceBU/255+b1;
						/*dst(c ,r)=a2*m_boxes[i].salienceTD/255+b2;
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


int YARPSalience::DrawContrastLP2(YARPImageOf<YarpPixelMono>& rg, YARPImageOf<YarpPixelMono>& gr, YARPImageOf<YarpPixelMono>& by, YARPImageOf<YarpPixelMono>& dst, YARPImageOf<YarpPixelInt>& tagged, int numBlob, float pBU, float pTD, YarpPixelMono prg, YarpPixelMono pgr, YarpPixelMono pby)
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
	
	for (int i = 1; i < numBlob; i++) {
		// The salience should not change if a blob is eliminated because it is too small
		// or too big
		// I could not change the scale of the salience and change only the color of the
		// most salient blob
		if (m_boxes[i].valid) {
			int tmp;
			
			/*int r, c;
			int xdim, ydim;
			int xmax, xmin ,ymin, ymax;
			
			Cartesian2Logpolar(m_boxes[i].centroid_x, m_boxes[i].centroid_y, r, c);
			Cartesian2Logpolar(m_boxes[i].xmin, m_boxes[i].ymin, r, c);
			Cartesian2Logpolar(m_boxes[i].xmin, m_boxes[i].ymin, r, c);
			Cartesian2Logpolar(m_boxes[i].xmin, m_boxes[i].ymin, r, c);
			Cartesian2Logpolar(m_boxes[i].xmin, m_boxes[i].ymin, r, c);*/

			int rdim=(double)(m_boxes[i].rmax-m_boxes[i].rmin+1)*.75;
			int cdim=(double)(m_boxes[i].cmax-m_boxes[i].cmin+1)*.75;

			int a,b,c,d;
			c = m_boxes[i].rmax+rdim;
			d = m_boxes[i].rmin-rdim;
			if (cdim<width/3) {
				a = m_boxes[i].cmin+cdim;
				b = m_boxes[i].cmin-cdim;
			} else {
				a = 0;
				b = 251;
			}

			/*tmp=255*height*width*integralRG.getSaliencyLp(m_boxes[i].cmax+cdim,
				m_boxes[i].cmin-cdim,
				m_boxes[i].rmax+rdim,
				m_boxes[i].rmin-rdim)/(rdim*cdim);
			m_boxes[i].cRG=abs(tmp-m_boxes[i].meanRG);
			tmp=255*height*width*integralGR.getSaliencyLp(m_boxes[i].cmax+cdim,
				m_boxes[i].cmin-cdim,
				m_boxes[i].rmax+rdim,
				m_boxes[i].rmin-rdim)/(rdim*cdim);
			m_boxes[i].cGR=abs(tmp-m_boxes[i].meanGR);
			tmp=255*height*width*integralBY.getSaliencyLp(m_boxes[i].cmax+cdim,
				m_boxes[i].cmin-cdim,
				m_boxes[i].rmax+rdim,
				m_boxes[i].rmin-rdim)/(rdim*cdim);
			m_boxes[i].cBY=abs(tmp-m_boxes[i].meanBY);*/

			tmp=255*height*width*integralRG.getMeanLp(a,b,c,d);
			m_boxes[i].cRG=abs(tmp-m_boxes[i].meanRG);

			tmp=255*height*width*integralGR.getMeanLp(a,b,c,d);
			m_boxes[i].cGR=abs(tmp-m_boxes[i].meanGR);
			
			tmp=255*height*width*integralBY.getMeanLp(a,b,c,d);
			m_boxes[i].cBY=abs(tmp-m_boxes[i].meanBY);

			/*salienceBU=sqrt(m_boxes[i].cRG*m_boxes[i].cRG+
			                m_boxes[i].cGR*m_boxes[i].cGR+
			                m_boxes[i].cBY*m_boxes[i].cBY);
			salienceBU=salienceBU/sqrt(3);*/

			/*salienceBU=m_boxes[i].cRG;
			if (salienceBU<m_boxes[i].cGR)
				salienceBU=m_boxes[i].cGR;
			if (salienceBU<m_boxes[i].cBY)
				salienceBU=m_boxes[i].cBY;*/

			// sum of abs of contrast differences
			salienceBU=m_boxes[i].cRG+m_boxes[i].cGR+m_boxes[i].cBY;


			salienceTD=sqrt((m_boxes[i].meanRG-prg)*(m_boxes[i].meanRG-prg)+
			                (m_boxes[i].meanGR-pgr)*(m_boxes[i].meanGR-pgr)+
			                (m_boxes[i].meanBY-pby)*(m_boxes[i].meanBY-pby));
			salienceTD=255-salienceTD/sqrt(3);
			
			/*salienceTD=abs(m_boxes[i].meanRG-prg);
							
			if (salienceTD<abs(m_boxes[i].meanGR-pgr))
				salienceTD=abs(m_boxes[i].meanGR-pgr);
				
			if (salienceTD<abs(m_boxes[i].meanBY-pby))
				salienceTD=abs(m_boxes[i].meanBY-pby);

			salienceTD=255-salienceTD;*/

			//if the color is too different it isn't in the scene!
			if (salienceTD<200) salienceTD=200;

			//if (salienceTD<0) salienceTD=0;
			
			m_boxes[i].salienceBU=salienceBU;
			m_boxes[i].salienceTD=salienceTD;

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

	const int maxDest=170;

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

	for (i = 1; i < numBlob; i++) {
		if (m_boxes[i].valid) {
			if ((m_boxes[i].salienceBU==maxSalienceBU && pBU==1) || (m_boxes[i].salienceTD==maxSalienceTD && pTD==1))
				m_boxes[i].salienceTotal=255;
			else
				m_boxes[i].salienceTotal=pBU*(a1*m_boxes[i].salienceBU/255+b1)+pTD*(a2*m_boxes[i].salienceTD/255+b2);
			//m_boxes[i].salienceTotal=pBU*m_boxes[i].salienceBU+pTD*m_boxes[i].salienceTD;
			//m_boxes[i].salienceTotal=pBU*m_boxes[i].salienceBU+pTD*(a2*m_boxes[i].salienceTD/255+b2);
			//m_boxes[i].salienceTotal=pBU*255+pTD*(a2*m_boxes[i].salienceTD/255+b2);
			for (int r=m_boxes[i].rmin; r<=m_boxes[i].rmax; r++)
				for (int c=m_boxes[i].cmin; c<=m_boxes[i].cmax; c++)
					if (tagged(c, r)==m_boxes[i].id) {
						//if (sal>th) dst(c ,r)=sal;
						//else dst(c ,r)=0;
						dst(c ,r)=m_boxes[i].salienceTotal;
					}
		}
	}
	
	return numBlob;
}


/*int YARPSalience::DrawGrayLP(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int numBlob)
{
	if (numBlob>256) numBlob=256;
	
	for (int i = 0; i < numBlob; i++) {
		if (m_boxes[i].valid) { // Is this check necessary?
			for (int r=m_boxes[i].rmin; r<=m_boxes[i].rmax; r++)
				for (int c=m_boxes[i].cmin; c<=m_boxes[i].cmax; c++)
					if (tagged(c, r)==m_boxes[i].id)
						id(c ,r)=255-i;
		}
	}

	return numBlob;
}*/


void YARPSalience::DrawFoveaBlob(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, const YarpPixelMono gray)
{
	int tag=tagged(0, 0);
	
	id.Zero();
	
	for (int r=m_boxes[tag].rmin; r<=m_boxes[tag].rmax; r++)
		for (int c=0; c<width; c++)
			if (tagged(c, r)==tag)
				id(c ,r)=gray;
}


void YARPSalience::SeedColor(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, int x, int y, int col)
{
	long int seed=tagged(x,y);

	//if (m_boxes[seed].valid && m_boxes[seed].areaLP > 5 && m_boxes[seed].areaLP < 250) {
		for (int r=m_boxes[seed].rmin; r<=m_boxes[seed].rmax; r++)
			for (int c=m_boxes[seed].cmin; c<=m_boxes[seed].cmax; c++)
				if (tagged(c, r)==m_boxes[seed].id)
					id(c ,r)=col;
	//}
}


// If I use the function I shouldn't strecth the color opponency maps
void YARPSalience::fuseFoveaBlob(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag)
{
	memset(blobList, false, sizeof(char)*max_tag);
	
	/*const YarpPixelMono rg0=m_boxes[1].meanRG;
	const YarpPixelMono gr0=m_boxes[1].meanGR;
	const YarpPixelMono by0=m_boxes[1].meanBY;*/
	
	for (int c=0; c<width; c++) {
		YarpPixelInt seed=1;
		YarpPixelMono rg0;
		YarpPixelMono gr0;
		YarpPixelMono by0;

		int r=1;
		while(1) {
			rg0=m_boxes[seed].meanRG;
			gr0=m_boxes[seed].meanGR;
			by0=m_boxes[seed].meanBY;

			while(r<height && tagged(c, r)==seed)
				r++;
			if (r==height) break;
			seed=tagged(c, r);
			//if (abs(m_boxes[seed].meanRG-rg0)+abs(m_boxes[seed].meanGR-gr0)+abs(m_boxes[seed].meanBY-by0)<13 )
			if ((m_boxes[seed].meanRG-rg0)*(m_boxes[seed].meanRG-rg0)+(m_boxes[seed].meanGR-gr0)*(m_boxes[seed].meanGR-gr0)+(m_boxes[seed].meanBY-by0)*(m_boxes[seed].meanBY-by0)<50 )
				blobList[seed]=1;
			else
				break;
		}
	}
				
}


// If I use the function I shouldn't strecth the color opponency maps
void YARPSalience::fuseFoveaBlob2(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag)
{
	const YarpPixelMono rg0=m_boxes[1].meanRG;
	const YarpPixelMono gr0=m_boxes[1].meanGR;
	const YarpPixelMono by0=m_boxes[1].meanBY;
	
	for (int i=0; i<max_tag; i++) {
		if (blobList[i])
			if ((m_boxes[i].meanRG-rg0)*(m_boxes[i].meanRG-rg0)+(m_boxes[i].meanGR-gr0)*(m_boxes[i].meanGR-gr0)+(m_boxes[i].meanBY-by0)*(m_boxes[i].meanBY-by0)>=50 )
				blobList[i]=0;
	}
}


// Variance method
void YARPSalience::fuseFoveaBlob3(YARPImageOf<YarpPixelInt>& tagged, char *blobList, YarpPixelBGR var, int max_tag)
{
	const YarpPixelMono rg0=m_boxes[1].meanRG;
	const YarpPixelMono gr0=m_boxes[1].meanGR;
	const YarpPixelMono by0=m_boxes[1].meanBY;
	
	for (int i=0; i<max_tag; i++) {
		if (blobList[i])
			if (abs(m_boxes[i].meanRG-rg0)/var.r+abs(m_boxes[i].meanGR-gr0)/var.g+abs(m_boxes[i].meanBY-by0)/var.b<=2)
				blobList[i]=2;
	}
}


void YARPSalience::drawBlobList(YARPImageOf<YarpPixelMono>& id, YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag, const YarpPixelMono gray)
{
	//id.Zero();
	
	for (int tag=1; tag<max_tag; tag++)
		if (blobList[tag]==2)
			for (int r=m_boxes[tag].rmin; r<=m_boxes[tag].rmax; r++)
				for (int c=m_boxes[tag].cmin; c<=m_boxes[tag].cmax; c++)
					if (tagged(c, r)==tag)
						id(c ,r)=gray;
}


void YARPSalience::centerOfMassAndMass(YARPImageOf<YarpPixelInt> &in, YarpPixelInt tag, int *x, int *y, double *mass)
{
	int r,t;
	double areaT = 0.0;
	double sumX = 0.0;
	double sumY = 0.0;
	YarpPixelInt *src;
	for(r = 0; r < height; r++) {
		int sumTmpX = 0;
		int sumTmpY = 0;
		int areaTmp = 0;

		src = (YarpPixelInt *)in.GetArray()[r];
		for(t = 0; t < width; t++) {
			if (*src==tag) {
				int tmpX;
				int tmpY;

				m_lp.Logpolar2Cartesian(r, t, tmpX, tmpY);

				sumTmpX += tmpX;
				sumTmpY += tmpY;
				areaTmp++;
			}
			src++;
		}
		double J=m_lp.Jacobian(r, 0);
		sumX+=sumTmpX*J;
		sumY+=sumTmpY*J;
		areaT+=areaTmp*J;
	}
	
	*mass=areaT;
	
	if (areaT != 0)	{
		*x = (int)(sumX/areaT + .5);
		*y = (int)(sumY/areaT + .5);
	} else {
		*x = 0;
		*y = 0;
	}
}


YarpPixelBGR YARPSalience::varBlob(YARPImageOf<YarpPixelInt>& tagged, YARPImageOf<YarpPixelMono> &rg, YARPImageOf<YarpPixelMono> &gr, YARPImageOf<YarpPixelMono> &by, int tag)
{
	int tmpr, tmpg, tmpb;
	YarpPixelBGR tmp;

	tmpr=0;
	tmpg=0;
	tmpb=0;

	for (int r=m_boxes[tag].rmin; r<=m_boxes[tag].rmax; r++)
		for (int c=m_boxes[tag].cmin; c<=m_boxes[tag].cmax; c++)
			if (tagged(c, r)==tag) {
				tmpr+=rg(c, r)*rg(c, r);
				tmpg+=gr(c, r)*gr(c, r);
				tmpb+=by(c, r)*by(c, r);
			}
	
	tmpr = tmpr / m_boxes[tag].areaLP;
	tmpg = tmpg / m_boxes[tag].areaLP;
	tmpb = tmpb / m_boxes[tag].areaLP;

	tmp.r = sqrt(tmpr - m_boxes[tag].meanRG*m_boxes[tag].meanRG);
	tmp.g = sqrt(tmpg - m_boxes[tag].meanGR*m_boxes[tag].meanGR);
	tmp.b = sqrt(tmpb - m_boxes[tag].meanBY*m_boxes[tag].meanBY);

	if (tmp.r==0) tmp.r=1;
	if (tmp.g==0) tmp.g=1;
	if (tmp.b==0) tmp.b=1;
	
	return tmp;
}


void YARPSalience::statBlobList(YARPImageOf<YarpPixelInt>& tagged, char *blobList, int max_tag, YARPBox &blob)
{
	unsigned long int area=0;
	unsigned long int areaCart=0;
	unsigned long int rgSum=0;
	unsigned long int grSum=0;
	unsigned long int bySum=0;
	unsigned long int xSum=0;
	unsigned long int ySum=0;

	for (int tag=1; tag<max_tag; tag++)
		if (blobList[tag]==2) {
			area+=m_boxes[tag].areaLP;
			areaCart+=TotalArea(m_boxes[tag]);
			rgSum+=m_boxes[tag].rgSum;
			grSum+=m_boxes[tag].grSum;
			bySum+=m_boxes[tag].bySum;
			xSum+=m_boxes[tag].xsum;
			ySum+=m_boxes[tag].ysum;
		}
	
	blob.areaLP=area;
	blob.areaCart=areaCart;
	blob.rgSum=rgSum;
	blob.grSum=grSum;
	blob.bySum=bySum;
	if (area!=0) {
		blob.meanRG=rgSum/area;
		blob.meanGR=grSum/area;
		blob.meanBY=bySum/area;
		blob.centroid_x=xSum/area;
		blob.centroid_y=ySum/area;
	}
}


void YARPSalience::maxSalienceBlobs(YARPImageOf<YarpPixelInt>& tagged, int max_tag, YARPBox* boxes, int num)
{
	int *pos = new int [num];
	
	memset(pos, 0, sizeof(int)*num);
	
	for (int l = 1; l < max_tag; l++) {
		if (m_boxes[l].valid) {
			for (int i=0; i<num; i++) {
				if (m_boxes[l].salienceTotal>m_boxes[pos[i]].salienceTotal) {
					for (int j=num-1; j>i; j--)
						pos[j]=pos[j-1];
					pos[i]=l;
					break;
				}
			}
		}
	}

	for (l=0; l<num; l++) {
		boxes[l]=m_boxes[pos[l]];
		_gaze.computeRay(YARPBabybotHeadKin::KIN_LEFT_PERI, boxes[l].elev, boxes[l].az , (int)boxes[l].centroid_x, (int)boxes[l].centroid_y);
	}

	delete [] pos;
	// If the numbers of valid blobs are less than num???
}


void YARPSalience::maxSalienceBlob(YARPImageOf<YarpPixelInt>& tagged, int max_tag, YARPBox &box)
{
	int max=1;
	int xcart;
	int ycart;
	
	for (int l = 1; l < max_tag; l++)
		if (m_boxes[l].valid)
			if (m_boxes[l].salienceTotal>m_boxes[max].salienceTotal)
				max=l;

	box=m_boxes[max];
	centerOfMassAndMass(tagged, box.id, &xcart, &ycart, &box.areaCart);
	box.centroid_x=xcart;
	box.centroid_y=ycart;
	_gaze.computeRay(YARPBabybotHeadKin::KIN_LEFT_PERI, box.elev, box.az, (int)box.centroid_x, (int)box.centroid_y);
}


void YARPSalience::foveaBlob(YARPImageOf<YarpPixelInt>& tagged, YARPBox &box)
{
	box=m_boxes[1];
	_gaze.computeRay(YARPBabybotHeadKin::KIN_LEFT_PERI, box.elev, box.az , (int)box.centroid_x, (int)box.centroid_y);
}


void YARPSalience::DrawVQColor(YARPImageOf<YarpPixelBGR>& id, YARPImageOf<YarpPixelInt>& tagged)
{
	for (int r=0; r<height; r++)
		for (int c=0; c<width; c++)
			colorVQ.DominantQuantization(m_boxes[tagged(c, r)].meanColors, id(c,r), 0.3*255);
}
