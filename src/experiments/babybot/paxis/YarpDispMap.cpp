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

#include "YARPDispMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPDispMap::YARPDispMap()
{
	_xSize = 128;
	_ySize = 128;
	_alpha = 0.40f;
	_beta  = 20.0f;

	_imgSize = _xSize*_ySize;

	_dilMask = 11;
	_epsScale = 4.0f;
	_epsVal = 10;
	_totSize = (4*_imgSize-1)/3; //Sono incluse la xSize*ySize e la 1*1
	_minDisp = -1;
	_maxDisp =  1;
	_deltaDisp = _maxDisp-_minDisp+1;

	_dispOffset = new int [_totSize];
	_tmpDisp    = new int [_totSize];
	_sobel		= new int [_imgSize];
	_oneLine = new unsigned char [_xSize];

	_coeffArray.Resize(_xSize,_ySize);
	_dilatImg.Resize(_xSize,_ySize);
	_fTempImg.Resize(_xSize,_ySize);
	_epsImg.Resize(_xSize,_ySize);
	_imaskImg.Resize(_xSize,_ySize);
	_idispImg.Resize(_xSize,_ySize);
	_DataSet1.Resize(_totSize,1);
	_DataSet2.Resize(_totSize,1);
}

YARPDispMap::~YARPDispMap()
{
	delete [] _dispOffset;
	delete [] _tmpDisp;
	delete [] _sobel;
	delete [] _oneLine;
}

void YARPDispMap::dynProgr(int totNodes, YARPDispMap::node * graph)
{
	bool *Q;
	bool done = false;
	int i,j;

	Q = new bool [totNodes];

	memset(Q,0,totNodes * sizeof(bool));

	Q[0] = true;

	while (!done)
	{
		for(i=0; i<totNodes; i++)
			if (Q[i])
				break;

		if (i==totNodes)
			done = true;
		else
		{
			Q[i] = false;

			for (j=0; j<graph[i].no_of_nodes; j++)
			{
				graph[graph[i].foll_nodes[j]].no_of_prev_nodes--;
				if (graph[graph[i].foll_nodes[j]].no_of_prev_nodes == 0)
					Q[graph[i].foll_nodes[j]]=true;

				if (graph[ graph[i].foll_nodes[j]].pathweight > graph[i].pathweight+graph[i].arcweight[j])
				{
					graph[ graph[i].foll_nodes[j]].pathweight = graph[i].pathweight+graph[i].arcweight[j];
					graph[graph[i].foll_nodes[j]].prevNode = i;
				}
			}
		}
	}
	delete [] Q;
}

void YARPDispMap::dispWrapper(	YARPImageOf<YarpPixelMono>  & left, 
								YARPImageOf<YarpPixelMono>  & right,
								YARPImageOf<YarpPixelMono>  & mask,
								YARPImageOf<YarpPixelMono>  & disparity)
{
	int i,j,m,n;

	unsigned char * lmask  = (unsigned char*) mask.GetRawBuffer();
	unsigned char * rmask = (unsigned char*) _imaskImg.GetRawBuffer();
	unsigned char * dilat = (unsigned char*) _dilatImg.GetRawBuffer();
	unsigned char * l  = (unsigned char*) left.GetRawBuffer();
	unsigned char * r  = (unsigned char*) right.GetRawBuffer();
	unsigned char * disp  = (unsigned char*) disparity.GetRawBuffer();
	unsigned char * idisp = (unsigned char*) _idispImg.GetRawBuffer();

	float * eps = (float*)_epsImg.GetRawBuffer();
	
	_dilatImg = mask;

	//Dilation
	for(j=_dilMask; j<_ySize-_dilMask; j++)
		for (i=_dilMask; i<_xSize-_dilMask; i++)
			if (lmask[j*_xSize+i]!=0)
				for (m=j-_dilMask; m<=j+_dilMask; m++)
					for (n=i-_dilMask; n<=i+_dilMask; n++)
						dilat[m*_xSize+n] = 255;
			else
				dilat[j*_xSize+i] = 0;


	if (_epsVal)
	{
 		edgePreservingSmooth(l);

		for (j=0; j<_imgSize; j++)
			if (dilat[j]==255)
				l[j] = (unsigned char)(eps[j]+0.5f);
			else
				l[j] = 0;


		edgePreservingSmooth(r);

		for (j=1; j<_ySize-1; j++)
			for (i=1; i<_xSize-1; i++)
				r[j*_xSize+i] = (unsigned char)(eps[j*_xSize+i]+0.5f);

	}

	else 
	{
		for (j=0; j<_imgSize; j++)
			if (dilat[j]==0)
					l[j] = 0;
	}

	getDisparity(l,r,disp); 


	for (j=0; j<_imgSize; j++)
	{
		if (lmask[j]==0)
			disp[j] = 0;
	}


	getRightSegm(disp,rmask);

	//Dilation
	for(j=_dilMask; j<_ySize-_dilMask; j++)
		for (i=_dilMask; i<_xSize-_dilMask; i++)
			if (rmask[j*_xSize+i]==255)
				for (m=j-_dilMask; m<=j+_dilMask; m++)
					for (n=i-_dilMask; n<=i+_dilMask; n++)
						dilat[m*_xSize+n] = 255;
			else
				dilat[j*_xSize+i] = 0;


	if(_epsVal)
	{
		for (j=0; j<_imgSize; j++)
			if (dilat[j]==255)
				r[j] = (unsigned char)(eps[j]);
			else
				r[j] = 0;
	}
	else
	{
		for (j=0; j<_imgSize; j++)
			if (dilat[j]==0)
				r[j] = 0;
	}

	getDisparity(r,l,idisp);

	invertDisparity(idisp);
	
	for (j=0; j<_imgSize; j++)
		if (idisp[j] != 0)
			if (disp[j] == 255)
				disp[j] = idisp[j];
		else if (disp[j] != 0)
			disp[j] = (idisp[j]+disp[j])/2;

}

void YARPDispMap::edgePreservingSmooth(	unsigned char * inImg)
{
	float * floatImg = (float*) _fTempImg.GetRawBuffer();
	float * coeff    = (float*) _coeffArray.GetRawBuffer();
	float * out      = (float*) _epsImg.GetRawBuffer();

	memset(coeff,0,_imgSize*sizeof(float));
	memset(floatImg,0,_imgSize*sizeof(float));

	convertUCharToFloat(inImg,floatImg);

    int i;

	for (i=0; i<_epsVal;)
	{
		iterativeEps();
		i++ ;
		if (i < _epsVal)
			memcpy(floatImg,out,_imgSize*sizeof(float));

    }
}

void YARPDispMap::getDisparity(	unsigned char * img1, 
								unsigned char * img2,
								unsigned char * disparity)
{
	unsigned char * DSet1 = (unsigned char*) _DataSet1.GetRawBuffer();
	unsigned char * DSet2 = (unsigned char*) _DataSet2.GetRawBuffer();

//	unsigned char * image1 = (unsigned char*) img1.GetRawBuffer();
//	unsigned char * image2 = (unsigned char*) img2.GetRawBuffer();

//	int * dispOffset;
//	int * tmpDisparity;

	int imgOffset;
	node * graph;

	int top, bottom;

	int totNodes= 0; 
	int totArcs = 0;

	int length;


	int graphSize;

	int i,j,k,n,m;
	
	getImageSet(img1, DSet1);
	getImageSet(img2, DSet2);

	int levels = (int)(0.5+log(_xSize)/log(2.0));
	
	unsigned char * data1;
	unsigned char * data2;
	
	for (j=0; j<_totSize; j++)
	{
		_dispOffset[j] = 0;
		_tmpDisp[j] = INT_MAX;
	}

	bool found; 

	for (length = _xSize/8; length<=_xSize; length*=2)
	{
		graphSize = length*_deltaDisp+2;

		//Graph Allocation
			graph = new node [graphSize];
			graph[0].foll_nodes = new int   [4*graphSize*(_deltaDisp+2)];
			graph[0].arcweight  = new float [4*graphSize*(_deltaDisp+2)];
		//End Allocation

		memset(disparity,255,length*length);

		imgOffset = _totSize-(4*length*length-1)/3;

		data1 = DSet1 + imgOffset;
		data2 = DSet2 + imgOffset;

		top = 0;
		bottom = length-1;

		found = false;

		for (j=1; j<length; j++)
			for (i=1; i<length; i++)
			{
				if(data1[j*length+i]!=0)
				{
					top = j;
					j = length;
					found = true;
					break;
				}
			}

		if (!found)
		{
			delete [] graph[0].foll_nodes;
			delete [] graph[0].arcweight;
			delete [] graph;
//			delete [] dispOffset;
//			delete [] tmpDisparity;
//			delete [] leftDataSet;
///			delete [] rightDataSet;
//			delete [] Sobel;
			return;
		}

		for (j=top; j<length; j++)
			for (i=1; i<length; i++)
			{
				if(data2[j*length+i]!=0)
				{
					top = j;
					j = length;
					break;
				}
			}

		for (j=length-1; j>=top; j--)
			for (i=length-1; i>=0; i--)
			{
				if(data1[j*length+i]!=0)
				{
					bottom = j;
					j = 0;
					break;
				}
			}

		for (j=bottom; j>=top; j--)
			for (i=length-1; i>=0; i--)
			{
				if(data2[j*length+i]!=0)
				{
					bottom = j;
					j = 0;
					break;
				}
			}

		for (j=1; j<length-1; j++)
			for (i=1; i<length-1; i++)
			{
				_sobel[j*length+i]  = data1[(j-1)*length+(i-1)]+2*data1[(j-1)*length+(i)]+data1[(j-1)*length+(i+1)];
				_sobel[j*length+i] += -data1[(j+1)*length+(i-1)]-2*data1[(j+1)*length+(i)]-data1[(j+1)*length+(i+1)];
			}

		int ls,le;
		int rs,re;

		for (j=top; j<=bottom; j++)
		{
			data1 = DSet1 + imgOffset + j*length;
			data2 = DSet2 + imgOffset + j*length;

			ls = 0;
			le = length-1;
			rs = 0;
			re = length-1;

			for (i=0;i<length; i++)
			{
				if (data1[i] != 0)
				{
					ls = i;
					for (m=length-1;m>=0; m--)
						if(data1[m] != 0)
						{
							le = m;
							break;
						}
					break;
				}
			}

			for (i=0;i<length; i++)
			{
				if (data2[i] != 0)
				{
					rs = i;
					for (m=length-1;m>=0; m--)
						if(data2[m] != 0)
						{
							re = m;
							break;
						}
					break;
				}
			}

			int maxR = 0;
			int minR = length;
			int position;

			for (i=ls; i<=le; i++)
			{
				position = -(*(_dispOffset+imgOffset+j*length+i));
				position +=i;
				if (position+_maxDisp>maxR)
					maxR = position+_maxDisp;
				if (position+_minDisp<minR)
					minR = position+_minDisp;
			}

			buildGraph2D(graph, le-ls+1,maxR-minR+1,ls,minR, _dispOffset+imgOffset+j*length, &totNodes, &totArcs);

			if ((j==0)||(j==length-1))
				assignWeights(graph,length,data1,data2,_sobel, _tmpDisp, -1.0f, totArcs, totNodes);
			else
				assignWeights(graph,length,data1,data2,&_sobel[(j)*length], &_tmpDisp[imgOffset+(j-1)*length], _alpha, totArcs, totNodes) ;


			dynProgr(totNodes, graph);

			i = totNodes-1;
				//printf("Line: %d. Vertical Disparity: %d. Cost: %f\n",j,l,graph[i].pathweight);

//				if (graph[i].pathweight < cost)
//				{
//					cost = graph[i].pathweight;
//					vd = l;
//				}

			memset (&_tmpDisp[imgOffset+j*length],0, length*sizeof(int));

			for (k=0; k<=4*length; k++)
			{
				n = graph[i].prevNode;
				if (k!=0)
				{
					_tmpDisp[imgOffset+j*length+graph[i].left]= graph[i].left-graph[i].right;
//					if (j==length-2)
//						printf("Length: %d, Disparity=%d\n",length,tmpDisparity[imgOffset+j*length+graph[i].left]);

					disparity[j*length+graph[i].left] = (4*(graph[i].left-graph[i].right)+128);
					m = graph[i].left;
				}
				i=n;
				if((i<=0)||(i>totNodes-1))
					break;
			}//end k for

//			printf("Line: %d. Vertical Disparity: %d. Cost: %f\n",j,vd,cost);

			int a,b;

			//Process discontinuities in tmpDisparity
			if (disparity[j*length]==255) //Occlusion in zero//e se sono tutti zero???
			{
				for (k=0; k<length; k++)
				{
					if (disparity[j*length+k]!= 255)
						break;
				}
				if(k<length)
					for (m=0; m<k; m++)
						_tmpDisp[imgOffset+j*length+m]=_tmpDisp[imgOffset+j*length+k];
			}
			else if (disparity[j*length+length-1]==255)
			{
				for (k=length-1; k>=0; k--)
				{
					if (disparity[j*length+k]!= 255)
						break;
				}
				for (m=k+1; m<length; m++)
					_tmpDisp[imgOffset+j*length+m]=_tmpDisp[imgOffset+j*length+k];
			}
			else
			{
				for (k=0; k<length; k++)
				{
					if (disparity[j*length+k]== 255)
					{
						for (m=k; m<length; m++)
						{
							if (disparity[j*length+m]!= 255)
							{
								for (n=k; n<m; n++)
								{
									a = m-k+1;
									b = _tmpDisp[imgOffset+j*length+m]-_tmpDisp[imgOffset+j*length+k-1];
									if (((n-k+1)*b/a)+_tmpDisp[imgOffset+j*length+k-1]>0)
										_tmpDisp[imgOffset+j*length+n] = (int)( ((n-k+1)*b/a)+_tmpDisp[imgOffset+j*length+k-1]+0.5f);
									else
										_tmpDisp[imgOffset+j*length+n] = (int)( ((n-k+1)*b/a)+_tmpDisp[imgOffset+j*length+k-1]-0.5f);
								}
								m=length;
							}
						}
						k=n;
					}
				}
			}//endif

			if (length<_xSize)
			{
				for (k=0; k<length; k++)
				{
					if (_tmpDisp[imgOffset+j*length+k]!=INT_MAX)
					{
						_dispOffset[imgOffset-4*length*length+2*j*2*length+2*k] = 2*_tmpDisp[imgOffset+j*length+k];
						_dispOffset[imgOffset-4*length*length+2*j*2*length+2*k+1] = 2*_tmpDisp[imgOffset+j*length+k];
						_dispOffset[imgOffset-4*length*length+(2*j+1)*2*length+2*k] = 2*_tmpDisp[imgOffset+j*length+k];
						_dispOffset[imgOffset-4*length*length+(2*j+1)*2*length+2*k+1] = 2*_tmpDisp[imgOffset+j*length+k];
					}
				}
			}//endif
		}//endj
			
		

//		sprintf(File_Name,"%s%d%s","C:/Users/Fabio/Projects/DinamicDisparity/Images/_dpoutput",length,".bmp");
//		Save_Bitmap(dispImg,length,length,1,File_Name);

//		endTime = Get_Time();
//		printf("Time Elapsed: %d msec\n", endTime-startTime);
		//Graph Deallocation
			
		delete [] graph[0].foll_nodes;
		delete [] graph[0].arcweight;
		delete [] graph;
		
		//End Deallocation
	}
	
//	delete []  dispOffset;
//	delete [] tmpDisparity;
//	delete []  leftDataSet;
//	delete [] rightDataSet;
//	delete [] Sobel;

	//Disparity Inversion
/*
	unsigned char * invDisp;
//	invDisp = new unsigned char [xSize*ySize];
	memset(invDisp,255,xSize*ySize);

	for (j=0; j<ySize; j++)
		for (i=0; i<xSize; i++)
		{
			if (dispImg[j*xSize+i]!=255)
			k = (dispImg[j*xSize+i]-128)/8;
			
			if( ((i-k)>=0)&&((i-k)<xSize) )
				invDisp[j*xSize+(i-k)] = (-k*8)+128;
		}
						
	sprintf(File_Name,"%s%d%s","C:/Users/Fabio/Projects/DinamicDisparity/Images/_dpoutput_inv",xSize,".bmp");
	Save_Bitmap(invDisp,xSize,xSize,1,File_Name);
*/
//	endTime = Get_Time();

//	printf("Line : %d, Time Elapsed: %d msec\n",j, endTime-startTime);

}

void YARPDispMap::getRightSegm(unsigned char * inImg, unsigned char * outImg)
{
	int j;

	memset(outImg,0,_imgSize*sizeof(unsigned char));
	
	int point;

	for (j=0; j<_imgSize; j++)
	{
		if ((inImg[j]!=255)&&(inImg[j]!=0))
		{
			point = (inImg[j]-128)/4;
			if ((j>=point)&&(j-point<_imgSize))
				outImg[j-point]=255;
		}
	}
}

void YARPDispMap::invertDisparity(unsigned char * idisp)
{
	int i,j,k;

	for (j=0; j<_ySize; j++)
	{
		memcpy(_oneLine,idisp+_xSize*j,_xSize*sizeof(unsigned char));
		memset(idisp+_xSize*j,0,_xSize*sizeof(unsigned char));

		for (i=0; i<_xSize; i++)
		{
			k = _oneLine[i];
			if (k!=255)
			{
				k = (k-128)/4;
				if( ((i-k)>=0)&&((i-k)<_xSize) )
					idisp[j*_xSize+(i-k)] = (-k*4)+128;
			}
		}
	}
}

void YARPDispMap::convertUCharToFloat(unsigned char * ucImg, float * fImg)
{
	int j;

	for (j=0; j<_imgSize; j++)
		fImg[j] = (float)(ucImg[j]);
}

void YARPDispMap::iterativeEps()
{
	int i,j;
	float dx,dy;
	float sum;

	float * input  = (float*) _fTempImg.GetRawBuffer();
	float * coefs  = (float*) _coeffArray.GetRawBuffer();
	float * output = (float*) _epsImg.GetRawBuffer();

		for (j=1; j<_ySize-1; j++) 
		{
			for (i=1; i<_xSize-1; i++) 
			{
				dx = (input[(j-1)*_xSize+i+1] - 
					  input[(j-1)*_xSize+i-1] + 
					  2 * (input[(j)*_xSize+i+1] - input[(j)*_xSize+i-1]) +     
					  input[(j+1)*_xSize+i+1] - input[(j+1)*_xSize+i-1]);

				dy = (input[(j+1)*_xSize+i-1] - 
					  input[(j-1)*_xSize+i-1])+ 
					  2 * (input[(j+1)*_xSize+i] - input[(j-1)*_xSize+i]) +     
					  (input[(j+1)*_xSize+i+1] - input[(j-1)*_xSize+i+1]);

				dx /= 4.0;
				dy /= 4.0;
				
				coefs[j*_xSize+i] = (float)(1.0/(8.0 * sqrt(1.0+(dx*dx+dy*dy)/(_epsScale*_epsScale))) );
			}
		}

		for (j=1; j<_ySize-1; j++) 
		{
			for (i=1; i<_xSize-1; i++) 
			{
				output[j*_xSize+i]  = input[(j-1)*_xSize+i-1] * coefs[(j-1)*_xSize+i-1] ;
				output[j*_xSize+i] += input[(j-1)*_xSize+i] * coefs[(j-1)*_xSize+i] ;
				output[j*_xSize+i] += input[(j-1)*_xSize+i+1] * coefs[(j-1)*_xSize+i+1] ;

				output[j*_xSize+i] += input[(j)*_xSize+i-1] * coefs[(j)*_xSize+i-1] ;
				output[j*_xSize+i] += input[(j)*_xSize+i+1] * coefs[(j)*_xSize+i+1] ;

				output[j*_xSize+i] += input[(j+1)*_xSize+i-1] * coefs[(j+1)*_xSize+i-1] ;
				output[j*_xSize+i] += input[(j+1)*_xSize+i] * coefs[(j+1)*_xSize+i] ;
				output[j*_xSize+i] += input[(j+1)*_xSize+i+1] * coefs[(j+1)*_xSize+i+1] ;

				sum  = coefs[(j-1)*_xSize+i-1] ;
				sum += coefs[(j-1)*_xSize+i] ;
				sum += coefs[(j-1)*_xSize+i+1] ;

				sum += coefs[(j)*_xSize+i-1] ;
				sum += coefs[(j)*_xSize+i+1] ;

				sum += coefs[(j+1)*_xSize+i-1] ;
				sum += coefs[(j+1)*_xSize+i] ;
				sum += coefs[(j+1)*_xSize+i+1] ;

				output[j*_xSize+i] += (1.0f - sum) * input[j*_xSize+i] ;
			}
		}
}

void YARPDispMap::getImageSet(	unsigned char * inImg,
								unsigned char * outDataSet)
{
	int j;
	int  inOffset = 0;
	int outOffset = 0;
	int levels = (int)(0.5+log(_xSize)/log(2));

	memcpy(outDataSet,inImg,_imgSize);
	
	for (j=levels; j>=1; j--)
	{
		outOffset += (int)(pow(4,j));
		downSampleImages(outDataSet+inOffset,outDataSet+outOffset,(int)pow(2,j));
		inOffset += (int)(pow(4,j));
	}
}

void YARPDispMap::buildGraph2D(node * graph, int length1, int length2,int Offs1, int Offs2, int * prevDisp, int * totNodes, int * totArcs)
{
	int i,j,k;
	int a,b;
	int lf,rf;
	int counter = 1;
	int arcCounter = 0;
	int tag1;
	int size = length1*length2;

	*totArcs = 0;
	*totNodes = 0;

	int * Table = new int [size];

	for (j=0; j<size; j++)
		Table [j] = -1;

	//Step 1: Mark Nodes with allowed disparity
	for (i=Offs1; i<Offs1+length1; i++)
	{
		lf = i;
		for (rf=i-Offs1+Offs2; rf<length2+Offs2; rf++)
		{
			if(lf-rf>=prevDisp[lf]+_minDisp)
				if(lf-rf<=prevDisp[lf]+_maxDisp)
				{
					graph[counter].valid = true;
					graph[counter].left  = lf;
					graph[counter].right = rf;
					graph[counter].no_of_prev_nodes = 0;
					graph[counter].pathweight = 100000000.0;
					graph[counter].prevNode = -1;
					Table[(rf-Offs2)*length1+lf-Offs1] = counter;
					counter++;
				}
		}

		rf = i-Offs1+Offs2;
		for (lf=i+1; lf<length1+Offs1; lf++)
		{
			if(lf-rf>=prevDisp[lf]+_minDisp)
				if(lf-rf<=prevDisp[lf]+_maxDisp)
					if (rf-Offs2<length2)
				{
					graph[counter].valid = true;
					graph[counter].left  = lf;
					graph[counter].right = rf;
					graph[counter].no_of_prev_nodes = 0;
					graph[counter].pathweight = 100000000.0;
					graph[counter].prevNode = -1;
					Table[(rf-Offs2)*length1+lf-Offs1] = counter;
					counter++;
				}
		}
		if (i==Offs1)
			tag1 = counter;
	}

	*totNodes = counter+1;

	//Step 2: Write first node
		graph[0].valid = true;
		graph[0].left= -1;
		graph[0].right = -1;
		graph[0].no_of_nodes = tag1-1;
		*totArcs += graph[0].no_of_nodes;
		graph[0].no_of_prev_nodes = 0;
		graph[0].pathweight = 0;
		graph[0].prevNode = -1;
		for (j=0; j<graph[0].no_of_nodes; j++)
		{
			graph[0].foll_nodes[j] = j+1;
			graph[j+1].no_of_prev_nodes ++;
		}

	int arcIndex;

	//Step 3: Assign arcs
	for (i=1; i<counter; i++)
	{
		arcIndex = 0;
		a = graph[i].left+1;
		b = graph[i].right+1;

		graph[i].foll_nodes = graph[i-1].foll_nodes + graph[i-1].no_of_nodes;
		graph[i].arcweight  = graph[i-1].arcweight  + graph[i-1].no_of_nodes;

		if ((graph[i-1].left==Offs1+length1-1)||(graph[i-1].right==Offs2+length2-1))
		{
			graph[i].foll_nodes++;
			graph[i].arcweight++;
		}

		if ((a<Offs1+length1)&&(b<Offs2+length2))
		{
			k = a;

			for (j=b; j<Offs2+length2; j++)
			{
				if (Table[(j-Offs2)*length1+k-Offs1]!=-1)
				{
					graph[i].foll_nodes[arcIndex] = Table[(j-Offs2)*length1+k-Offs1];
					graph[Table[(j-Offs2)*length1+k-Offs1]].no_of_prev_nodes++;
					arcIndex++;
				}
			}

			k = b;

			for (j=a+1; j<Offs1+length1; j++)
			{
				if (Table[(k-Offs2)*length1+j-Offs1]!=-1)
				{
					graph[i].foll_nodes[arcIndex] = Table[(k-Offs2)*length1+j-Offs1];
					graph[Table[(k-Offs2)*length1+j-Offs1]].no_of_prev_nodes++;
					arcIndex++;
				}
			}
		}

#ifdef DEFORMATION

		int defindex = 1;

		a = a-1;
		b = b;

		for (j=b; j<b+defindex; j++)
		{
			if ((a<Offs1+length1)&&(j<Offs2+length2))
				if (Table[(j-Offs2)*length1+a-Offs1]!=-1)
				{
					graph[i].foll_nodes[arcIndex] = Table[(j-Offs2)*length1+a-Offs1];
					graph[Table[(j-Offs2)*length1+a-Offs1]].no_of_prev_nodes++;
					arcIndex ++;
				}
		}


		a = a+1;
		b = b-1;

		for (j=a; j<a+defindex; j++)
		{
			if ((j<Offs1+length1)&&(b<Offs2+length2))
				if (Table[(b-Offs2)*length1+j-Offs1]!=-1)
				{
					graph[i].foll_nodes[arcIndex] = Table[(b-Offs2)*length1+j-Offs1];
					graph[Table[(b-Offs2)*length1+j-Offs1]].no_of_prev_nodes++;
					arcIndex ++;
				}
		}

#endif				
		
		graph[i].no_of_nodes = arcIndex;
		*totArcs += graph[i].no_of_nodes;
	}

	//Step 4: Write last node and arcs to it

	graph[counter].valid = true;
	graph[counter].left= length1+Offs1;
	graph[counter].right = length2+Offs2;
	graph[counter].no_of_nodes = 0;
	graph[counter].no_of_prev_nodes = 0;
	graph[counter].pathweight = 100000000.0;
	graph[counter].prevNode = -1;

	graph[0].no_of_prev_nodes = 0;
//	graph[0].pathweight = 100000000.0;
	graph[0].prevNode = -1;
/*
	if (Table[length*length-1] != -1)
	{
		graph[counter].no_of_prev_nodes ++;
		graph[Table[length*length-1]].foll_nodes[graph[Table[length*length-1]].no_of_nodes] = counter;
		graph[Table[length*length-1]].no_of_nodes++;
		(*totArcs) ++;
	}

*/
	for (j=0; j<length2; j++)
	{
		if (Table[j*length1+(length1-1)] != -1)
		{
			graph[counter].no_of_prev_nodes ++;
			graph[Table[j*length1+(length1-1)]].foll_nodes[graph[Table[j*length1+(length1-1)]].no_of_nodes] = counter;
			graph[Table[j*length1+(length1-1)]].no_of_nodes++;
			(*totArcs) ++;
		}
	}	
	for (j=0; j<length1; j++)
	{
		if (Table[(length2-1)*length1+j] != -1)
			if (j!= length1-1)
			{
				graph[counter].no_of_prev_nodes ++;
				graph[Table[(length2-1)*length1+j]].foll_nodes[graph[Table[(length2-1)*length1+j]].no_of_nodes] = counter;
				graph[Table[(length2-1)*length1+j]].no_of_nodes++;
				(*totArcs) ++;
			}
	}
	

	graph[counter].foll_nodes = graph[counter-1].foll_nodes + graph[counter-1].no_of_nodes;
	graph[counter].arcweight  = graph[counter-1].arcweight  + graph[counter-1].no_of_nodes;

		//Purge Graph
	bool done = false;

	
	while (!done)
	{	
		done = true;
		for (j=1; j<*totNodes; j++)
		{
			if ((graph[j].no_of_prev_nodes==0)&&(graph[j].valid))
			{
				graph[j].valid = false;
				for (i=0; i<graph[j].no_of_nodes; i++)
				{
					graph[graph[j].foll_nodes[i]].no_of_prev_nodes--;
					done = false;
				}
			}
		}
	}
	delete[] Table;
}

void YARPDispMap::assignWeights(node * graph, int length, unsigned char * data1, unsigned char * data2,int * Sobel, int * tmpDisparity, float alpha, int totArcs, int totNodes)
{
	int i,j;
//	int graphSize = length * length;
	float arcCost;

	int nodedisp;
	int nextdisp;
	float occlWeight;

	unsigned char left, pright, cright, nright;

	for (j=0; j<totNodes; j++)
	{
		for (i=0; i<graph[j].no_of_nodes; i++)
		{
			nodedisp = graph[j].left-graph[j].right;

			if(graph[j].foll_nodes[i]==totNodes-1)
			{
				occlWeight = _beta * abs(nodedisp);
				graph[j].arcweight[i] = occlWeight;
			}
			else
			{
				left = data1[graph[graph[j].foll_nodes[i]].left];
				cright = data2[graph[graph[j].foll_nodes[i]].right];

				if (graph[graph[j].foll_nodes[i]].right>0)
					pright = data2[graph[graph[j].foll_nodes[i]].right-1];
				else
					pright = cright;

				if (graph[graph[j].foll_nodes[i]].right<length-1)
					nright = data2[graph[graph[j].foll_nodes[i]].right+1];
				else
					nright = cright;

				nextdisp = graph[graph[j].foll_nodes[i]].left-graph[graph[j].foll_nodes[i]].right;
				occlWeight = _beta * abs(nodedisp-nextdisp);

#ifdef DEFORMATION
				if ((graph[j].left==graph[graph[j].foll_nodes[i]].left)||(graph[j].right==graph[graph[j].foll_nodes[i]].right))
					occlWeight /= 1.5f;
#endif

				arcCost = computeCost(left,cright,nright,pright)+occlWeight;
				graph[j].arcweight[i] = arcCost;
				if (alpha> -0.5)
					if (graph[graph[j].foll_nodes[i]].left!= 0)
						if (graph[graph[j].foll_nodes[i]].left!= length-1) 
						{
							graph[j].arcweight[i] += computeInterCost(alpha,Sobel[graph[graph[j].foll_nodes[i]].left],tmpDisparity[graph[graph[j].foll_nodes[i]].left],nextdisp);
						}
			}
			
		}
	}
	
	//First node
	for (i=0; i<graph[0].no_of_nodes; i++)
	{
		nodedisp = 0;
		
		left = data1[graph[graph[0].foll_nodes[i]].left];
		cright = data2[graph[graph[0].foll_nodes[i]].right];

		if (graph[graph[0].foll_nodes[i]].right>0)
			pright = data2[graph[graph[0].foll_nodes[i]].right-1];
		else
			pright = cright;
		
		if (graph[graph[0].foll_nodes[i]].right<length-1)
			nright = data2[graph[graph[0].foll_nodes[i]].right+1];
		else
			nright = cright;

		nextdisp = (graph[graph[0].foll_nodes[i]].left-graph[graph[0].foll_nodes[i]].right);

		occlWeight = _beta * abs(nodedisp-nextdisp);
		
		graph[0].arcweight[i] = computeCost(left,cright,nright,pright)+occlWeight;

		if (alpha> -0.5)
			if (graph[graph[0].foll_nodes[i]].left!= 0)
				if (graph[graph[0].foll_nodes[i]].left!= length-1) 
				{
					graph[0].arcweight[i] += computeInterCost(alpha,Sobel[graph[graph[0].foll_nodes[i]].left],tmpDisparity[graph[graph[0].foll_nodes[i]].left],nextdisp);
				}
	}
}

float YARPDispMap::computeCost(unsigned char left, unsigned char right, unsigned char nRight, unsigned char pRight)
{
	float cost;
	float maxval;
	float minval;

	float sample1 = (right+pRight)/2.0f;
	float sample2 = (right+nRight)/2.0f;

	maxval = max (sample1,sample2);
	maxval = max (maxval,right);

	minval = min (sample1,sample2);
	minval = min (minval,right);

	cost = max (0,left-maxval);
	cost = max (cost, minval-left);

	cost += (float)fabs(left-right)/1024.0f;
	
	return cost;
}

float YARPDispMap::computeInterCost(float alpha, int Sobel,int prevdisp, int currdisp)
{
	float cost = 0.0f;
	float a = 134.0f; 
//	a = 81.5f;//DEFAULT

	if (abs(prevdisp)<1024)
	{
//		cost = max(1.0f,a/(64.0f+0.25f*abs(Sobel)+0.000001f));
		cost = max(1.0f,a/(64.0f+0.25f*abs(Sobel)));//DEFAULT 0.25*abs
		cost *= alpha;
		cost *= (float)fabs(currdisp-prevdisp);
	}

	return cost;
}

void YARPDispMap::downSampleImages(unsigned char * inImg, unsigned char * outImg, int imgSize)
{
	int i,j;

	for (j=0; j<imgSize/2; j++)
	{
		for (i=0; i<imgSize/2; i++)
		{
			outImg[j*(imgSize/2)+i] = (inImg[2*j*imgSize+2*i]+inImg[2*j*imgSize+2*i+1]+
												inImg[(2*j+1)*imgSize+2*i]+inImg[(2*j+1)*imgSize+2*i+1])/4;
		}
	}
}
