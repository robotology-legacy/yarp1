// YARPSusanFilter.cpp: implementation of the YARPSusanFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPSusanFilter.h"

#define  FTOI(a) ( (a) < 0 ? ((int)(a-0.5)) : ((int)(a+0.5)) )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPSusanFilter::YARPSusanFilter()
{
	x_size = 0;
	y_size = 0;
	r = NULL;
	pLUTData = new unsigned char[516];
	pMono = NULL;
}

YARPSusanFilter::YARPSusanFilter(int sizeX, int sizeY)
{
	x_size = sizeX;
	y_size = sizeY;
	r = new int[x_size * y_size];
	pLUTData = new unsigned char[516];
	pMono = new unsigned char[x_size * y_size];
}

YARPSusanFilter::~YARPSusanFilter()
{
	if (r != NULL)
		delete [] r;
	if (pLUTData != NULL)
		delete [] pLUTData;
	if (pMono != NULL)
		delete [] pMono;
}

bool YARPSusanFilter::apply(YARPImageOf<YarpPixelBGR> *inImg, YARPImageOf<YarpPixelMono> *outImg, float brightnessThreshold, bool smallMask)
{
	unsigned char *inData;
	unsigned char *outData;
		
	if ( (pMono == NULL) || (r == NULL) )
		return false;

	if ( (x_size != inImg->GetWidth()) || (y_size != inImg->GetHeight()) )
		return false;

	if ( (x_size != outImg->GetWidth()) || (y_size != outImg->GetHeight()) )
		outImg->Resize(x_size, y_size);

	inData = (unsigned char *)inImg->GetRawBuffer();
	outData = (unsigned char *)outImg->GetRawBuffer();
	
	bp = pLUTData;
	_setupBrightnessLUT(brightnessThreshold, 6);

	_bgr2Mono(pMono, inData);
		
	if (smallMask)
		_susanSmall(pMono, MAX_N_EDGES);
	else
		_susan(pMono, MAX_N_EDGES);
	_int2uchar(r, outData, x_size*y_size);

	return true;
}

bool YARPSusanFilter::apply(YARPImageOf<YarpPixelMono> *inImg, YARPImageOf<YarpPixelMono> *outImg, float brightnessThreshold, bool smallMask)
{
	unsigned char *inData;
	unsigned char *outData;
	
	if ( (x_size != inImg->GetWidth()) || (y_size != inImg->GetHeight()) )
		return false;
	
	if ( (x_size != outImg->GetWidth()) || (y_size != outImg->GetHeight()) )
		outImg->Resize(x_size, y_size);

	inData = (unsigned char *)inImg->GetRawBuffer();
	outData = (unsigned char *)outImg->GetRawBuffer();
	
	bp = pLUTData;
	_setupBrightnessLUT(brightnessThreshold, 6);
		
	if (smallMask)
		_susanSmall(inData, MAX_N_EDGES);
	else
		_susan(inData, MAX_N_EDGES);
	_int2uchar(r, outData, x_size*y_size);
	
	return false;
}

void YARPSusanFilter::_setupBrightnessLUT(float thresh, int form)
{
	int   k;
	float temp;
	
	bp = new unsigned char[516];

	bp=bp+258;

	for(k=-256;k<257;k++)
	{
		temp=((float)k)/(thresh);
		temp=temp*temp;
		if (form==6)
			temp=temp*temp*temp;
		temp=100.0*exp(-temp);
		*(bp+k)= (uchar)temp;
	}

}

void YARPSusanFilter::_susan( unsigned char *in, int max_no)
{
	int   i, j, n;
	unsigned char *p,*cp;

	memset (r,0,x_size * y_size * sizeof(int));

	for (i=3;i<y_size-3;i++)
		for (j=3;j<x_size-3;j++)
		{
			n=100;
			p=in + (i-3)*x_size + j - 1;
			cp=bp + in[i*x_size+j];

			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);
			p+=x_size-3; 

			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);
			p+=x_size-5;

			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);
			p+=x_size-6;

			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);
			p+=2;
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);
			p+=x_size-6;

			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);
			p+=x_size-5;

			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);
			p+=x_size-3;

			n+=*(cp-*p++);
			n+=*(cp-*p++);
			n+=*(cp-*p);

			if (n<=max_no)
				r[i*x_size+j] = max_no - n;
		}


}

void YARPSusanFilter::_susanSmall(unsigned char *in, int max_no)
{
	int   i, j, n;
	unsigned char *p,*cp;

	memset (r,0,x_size * y_size * sizeof(int));

	max_no = 730; /* ho hum ;) */
	for (i=1;i<y_size-1;i++)
		for (j=1;j<x_size-1;j++)
		{
		  n=100;
		  p=in + (i-1)*x_size + j - 1;
		  cp=bp + in[i*x_size+j];

		  n+=*(cp-*p++);
		  n+=*(cp-*p++);
		  n+=*(cp-*p);
		  p+=x_size-2; 

		  n+=*(cp-*p);
		  p+=2;
		  n+=*(cp-*p);
		  p+=x_size-2;

		  n+=*(cp-*p++);
		  n+=*(cp-*p++);
		  n+=*(cp-*p);

		  if (n<=max_no)
			r[i*x_size+j] = max_no - n;
		}
}

void YARPSusanFilter::_int2uchar(int *r, unsigned char *in, int size)
{
	int i,
    max_r=r[0],
    min_r=r[0];

  for (i=0; i<size; i++)
    {
      if ( r[i] > max_r )
        max_r=r[i];
      if ( r[i] < min_r )
        min_r=r[i];
    }

  max_r-=min_r;

  for (i=0; i<size; i++)
  {
    if (max_r != 0)
    in[i] = (unsigned char)((int)((int)(r[i]-min_r)*255)/max_r);
	else
		in[i] = 0;
  }
}

void YARPSusanFilter::resize(int sizeX, int sizeY)
{
	if (r != NULL)
		delete [] r;
	if (pMono != NULL)
		delete [] pMono;
	x_size = sizeX;
	y_size = sizeY;
	r = new int[x_size * y_size];
	pMono = new unsigned char[x_size * y_size];

}

void YARPSusanFilter::_bgr2Mono(unsigned char *dstMono, unsigned char *srcBGR)
{
	unsigned char *pBGR = srcBGR;
	unsigned char *pGray = dstMono;
	unsigned char b,g,r;
#ifndef NO_F2I_ROUNDING
	float value;
#endif
	for (int i=1;i<y_size-1;i++)
			for (int j=1;j<x_size-1;j++)
			{	
				b = *(pBGR++);
				g = *(pBGR++);
				r = *(pBGR++);
#ifdef NO_F2I_ROUNDING
				*(dstMono++) = unsigned char((b+g+r)/3);
#else
				value = (float(b)+float(g)+float(r))/3;
				*(dstMono++) = unsigned char(FTOI(value));
#endif
			}
}
