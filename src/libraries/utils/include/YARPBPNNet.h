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
///							#nat#
///
///	     "Licensed under the Academic Free License Version 1.0"
///
/// $Id: YARPBPNNet.h,v 1.1 2003-11-13 00:40:49 babybot Exp $
///  

/// Original implementation of the Matrix Back Propagation code by Davide
/// Anguita September 1992, University of Genova, DIBE -- Department
/// of Biophysical and Electronic Engineering
/// Spring 1999 -- First C++ porting by nat and bats
/// September 2003 -- Ported to YARP by nat

#ifndef __YARPBPNNET__
#define __YARPBPNNET__
#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <fstream>
#include <float.h>

#ifndef _REAL
#define _REAL
typedef double REAL;
#endif

#ifndef _BOOL
#define _BOOL
typedef int BOOL;
#endif

struct MBPOptions
{
	REAL   GradTh;        /* Threshold on gradient norm                     */
	REAL   AnaTh;         /* Threshold on analog error   (t-o)^2            */
	REAL   MaxTh;         /* Threshold on maximum error  max|t-o|           */
	REAL   DigTh;         /* Threshold on digital error  sign(t*o) %        */
	long   IterTh;        /* Threshold on # of iterations per run           */
	
	int    nPrints;       /* # of iterations/prints  0=no output            */

	BOOL   YProp;         /* YProp or Vogl algorithm ?                      */

	MBPOptions()
	{
		GradTh =  0.001F;
		AnaTh  =  0.0F;
		MaxTh  =  0.0F;
		DigTh  =  0.0F;
		IterTh =  200;
		nPrints = 1000;
		YProp   = 0;
	};

};

class YARPBPNNet  
{
protected:
                
  REAL** Status;         /* Neurons status for learning patterns            */
  REAL** Weight;         /* Weights                                         */
  REAL** Bias;           /* Biases                                          */
 
  int    nLayer;         /* # of layers                                     */
  int*   nUnit;          /* # of neurons per layer                          */

  REAL** _internal_status;

	REAL *max_input;
	REAL *min_input;

	REAL *max_output;
	REAL *min_output;

	REAL *max_limit;
	REAL *min_limit;

	REAL *tmp_input;
	REAL *tmp_output;

	int n_input;
	int n_output;

	//////// batch train vectors
	REAL** OldWeight;       /* Weights at step n-1                             */
	REAL** OldBias;         /* Biases at step n-1                              */
	REAL** Delta;           /* Error back-propagated                           */
	REAL** DeltaWeight;     /* Weight delta (gradient)                         */
	REAL** DeltaBias;       /* Bias delta (gradient)                           */
	REAL** OldDeltaWeight;  /* Weight delta at step n-1                        */
	REAL** OldDeltaBias;    /* Bias delta at step n-1                          */
	REAL*  Target;          /* Target of learning patterns                     */
	REAL** StepWeight;      /* Weight updating step                            */
	REAL** StepBias;        /* Bias updating step                              */

	bool _matrixAllocated;
	bool _batchAllocated;
	MBPOptions _batchTrainOptions;
	
	int nBatchPattern;

	unsigned int n_epoch;
	double analogCost;
	double gradient;

	char logfilename[128];
	bool savelog;
    
public:
	YARPBPNNet();
	YARPBPNNet(int nlayer,const int *nunit);
	YARPBPNNet(const char *filename);
	virtual ~YARPBPNNet();

	void init(long = 1,REAL = 2.0F,REAL = 0.9F,REAL = 1.47F);
	void trainByPattern(REAL *input, REAL *output, double step);
	void trainBatchInit(int, MBPOptions);
	void trainBatch(REAL *input,REAL *output);
	
	void sim(int nIPattern,REAL *input, REAL *output);
	void sim(const REAL *input, REAL *output);

	// get a random number
	double getRandom();
		
	inline REAL check_value (REAL val, REAL max, REAL min)
	{
		REAL ret = 0.0;
		if (val > max)
			ret = max;
		else if (val < min)
			ret = min;
		else
			ret = val;

		return ret;
	}

	void set_input(const REAL *max, const REAL *min);
	void set_output(const REAL *max, const REAL *min);
	void set_limits(const REAL *max, const REAL *min);

	int getInputSize()
	{ return n_input; }
	int getOutputSize()
	{ return n_output; }

	void save(const char *);
	int load(const char *);
private:
	void _allocMatrix();
	void _allocBatch();
	void _deallocBatch();
	void _deallocMatrix();

	void normalizeBatch(const double *in, double *out, const double *max, const double *min, int length)
	{
		int p;
		int i;
		for(p = 0; p<nBatchPattern; p++)
			for(i = 0; i < length; i++)
			{
				int index = i+p*length;
				out[index] = normalize(in[index], max[i], min[i]);
			}

	}

	void find_limits(const double *in, double *max, double *min, int length)
	{
		int p;
		int i;

		for(i = 0; i < length; i++)
		{
			max[i] = 0.0;
			min[i] = DBL_MAX;
		}

		// start search
		for(p = 0; p<nBatchPattern; p++)
			for(i = 0; i < length; i++)
			{
				double tmp = *in;
				if (tmp>max[i])
					max[i] = tmp;
				if (tmp<min[i])
					min[i] = tmp;
				in++;
			}
	}

	void de_normalizeBatch(const double *in, double *out, const double *max, const double *min, int length)
	{
		int p;
		int i;
		for(p = 0; p<nBatchPattern; p++)
			for(i = 0; i < length; i++)
			{
				int index = i+p*length;
				out[index] = de_normalize(in[index], max[i], min[i]);
			}
	}

	double de_normalize(double in, double max, double min)
	{
		double out;
		
		double a = (max - min)*0.5;
		double b = (max + min)*0.5;

		out = a*in + b;
		
		return out;
	}

	double normalize(double in, double max, double min)
	{
		double out;
		
		double a = (max - min)*0.5;
		double b = (max + min)*0.5;

		out = (in - b)/a;

		if (out>1)
			return 1;
		else if (out<-1)
			return -1;
		else	
			return out;
	}

	void appendLog()
	{
		if (!savelog)
			return;

		std::ofstream output(logfilename, std::ofstream::app);
		output << n_epoch << "\t" << analogCost << "\n";
		output.close();
	}
};
#endif // 
