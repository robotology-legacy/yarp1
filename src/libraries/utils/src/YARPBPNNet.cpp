#include "YARPBPNNet.h"
#include "../mbp/mbp.h"

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string>

extern "C"
{
	#include "../mbp/random.h"
	#include "../mbp/Mm.h"
}

#include <YARPConfigFile.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////

// default
YARPBPNNet::YARPBPNNet()
{
	nUnit = NULL;
	Status = NULL;
	Weight = NULL;
	Bias = NULL;
	nLayer = 0;
	Target = NULL;

	n_input = 0;
	n_output = 0;

	max_input = NULL; 
	min_input = NULL; 
	max_output = NULL; 
	min_output = NULL; 
	max_limit = NULL; 
	min_limit = NULL; 
	tmp_input = NULL; 
	tmp_output = NULL; 

	savelog = false;
	n_epoch = 0;
	analogCost = 0.0;
	gradient = 0.0;

	_matrixAllocated = false;
	_batchAllocated = false;
}

YARPBPNNet::YARPBPNNet(int nlayer,const int *nunit)
{         
	nLayer = nlayer;

	nUnit = NULL;
	Status = NULL;
	Weight = NULL;
	Bias = NULL;
	Target = NULL;

	n_input = 0;
	n_output = 0;

	max_input = NULL; 
	min_input = NULL; 
	max_output = NULL; 
	min_output = NULL; 
	max_limit = NULL; 
	min_limit = NULL; 
	tmp_input = NULL; 
	tmp_output = NULL; 

	_matrixAllocated = false;
	_batchAllocated = false;

	savelog = false;
	n_epoch = 0;
	analogCost = 0.0;
	gradient = 0.0;
		
	nUnit = (int *) calloc(nLayer+1,sizeof(int));
	CHECK_PTR(nUnit);

	int i;
	for(i=0;i<=nLayer;i++)
		nUnit[i] = nunit[i];

	n_input = nUnit[0];
	n_output = nUnit[nLayer];
  
	/* Check # of layers */

	if (nLayer LT 1)
	{
		ERROR_MSG("Insufficient number of layers");
	}

	/* Check # of units per layer */

	for (i=0; i LE nLayer; i++)
	{
		if ((nUnit)[i] LE 0)
		{
			ERROR_MSG("Layer without units");
		}
	}

	_allocMatrix();
}

//////////////////////////////////////////////////////////////////////
// Construction from a file
//////////////////////////////////////////////////////////////////////

YARPBPNNet::YARPBPNNet(const char* filename)
{
	nUnit = NULL;
	Status = NULL;
	Weight = NULL;
	Bias = NULL;
	nLayer = 0;
	Target = NULL;

	n_input = 0;
	n_output = 0;

	max_input = NULL; 
	min_input = NULL; 
	max_output = NULL; 
	min_output = NULL; 
	max_limit = NULL; 
	min_limit = NULL; 
	tmp_input = NULL; 
	tmp_output = NULL; 

	_matrixAllocated = false;
	_batchAllocated = false;

	load(filename);
}

int YARPBPNNet::load(const char *filename)
{
	YARPConfigFile cf;
	cf.set("", filename);

	// nLayer
	int ret = cf.get("[NET]", "NLayers", &nLayer);
	if (ret != YARP_OK)
		return ret;
	
	if (nUnit != NULL)
		free (nUnit);
	nUnit = (int*) calloc(nLayer+1,sizeof(int));
	CHECK_PTR(nUnit);

	// struct
	ret = cf.get("[NET]", "NUnits", nUnit, nLayer+1);
	if (ret != YARP_OK)
		return ret;

	n_input = nUnit[0];
	n_output = nUnit[nLayer];
	
	_allocMatrix();

	n_epoch = 0;
	// read limits and norm factors
	ret = cf.get("[NET]", "InputMax", max_input, nUnit[0]);
	if (ret != YARP_OK)
		return ret;
	ret = cf.get("[NET]", "InputMin", min_input, nUnit[0]);
	if (ret != YARP_OK)
		return ret;

	ret = cf.get("[NET]", "TargetMax", max_output, nUnit[nLayer]);
	if (ret != YARP_OK)
		return ret;

	ret = cf.get("[NET]", "TargetMin", min_output, nUnit[nLayer]);
	if (ret != YARP_OK)
		return ret;

	ret = cf.get("[NET]", "OutputMax", max_limit, nUnit[nLayer]);
	if (ret != YARP_OK)
		return ret;
	ret = cf.get("[NET]", "OutputMin", min_limit, nUnit[nLayer]);
	if (ret != YARP_OK)
		return ret;
	////////////////////////////////////////////////////////
	
	cf.get("[NET]", "Epoch", &n_epoch, 1);
	////////////////////////////////////////////////////////
	if (cf.getString("[NET]", "LogFile", logfilename) != YARP_OK)
		savelog = false;
	else
		savelog = true;

	bool savedOK = true;
	int i;
	for (i=1; i LE nLayer; i++)
	{

		char tmp[255];
		sprintf(tmp, "%s[%d]", "Weights", i);
		if (cf.get("[NET]", tmp, Weight[i], nUnit[i]*nUnit[i-1]) != YARP_OK)
			savedOK = false;
		sprintf(tmp, "%s[%d]", "Biases", i);
		if (cf.get("[NET]", tmp, Bias[i], nUnit[i]) != YARP_OK)
			savedOK = false;
	}

	if (!savedOK)
	{
		printf("No saved weights and biases found, perform random initialization\n");
		init((unsigned) time(NULL));	// random weigths init
	}

	return YARP_OK;
}

/* Matrix allocation */
void YARPBPNNet::_allocMatrix()
{
	if (_matrixAllocated)
		_deallocMatrix();
	///////////////////////////////////////////////////
	max_input = new double [n_input];
	min_input = new double [n_input];

	max_output = new double [n_output];
	min_output = new double [n_output];

	max_limit = new double [n_output];
	min_limit = new double [n_output];
		
	
	tmp_input = new REAL [n_input];
	tmp_output = new REAL [n_output];

	Status = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(Status);

	Weight         = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(Weight);

	Bias           = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(Bias);

	int i;
	for (i=1; i LE nLayer; i++)
	{    

		(Weight)[i]         = (REAL*) calloc((nUnit)[i]*(nUnit)[i-1],sizeof(REAL));
		CHECK_PTR((Weight)[i]);

		(Bias)[i]           = (REAL*) calloc((nUnit)[i],sizeof(REAL));
		CHECK_PTR((Bias)[i]);

	} 

	// alloc iternal status vector that will be used to simulate the network with a single input pattern
	_internal_status = new REAL *[nLayer+1];

	for (i=1; i <= nLayer; i++)	//0 is not allocted because it is used directly the input vector
	   _internal_status[i] = new REAL [(nUnit)[i]];

	_matrixAllocated = true;
}

void YARPBPNNet::_allocBatch()
{
	if (_batchAllocated)
		_deallocBatch();

	Status[0]   = (REAL*) calloc((nUnit)[0]*(nBatchPattern),sizeof(REAL));
	
	Target = (REAL*) calloc((nUnit)[nLayer]*(nBatchPattern),sizeof(REAL));

	OldWeight      = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(OldWeight);

	OldBias        = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(OldBias);

	Delta          = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(Delta);

	DeltaWeight    = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(DeltaWeight);

	DeltaBias      = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(DeltaBias);

	OldDeltaWeight = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(OldDeltaWeight);

	OldDeltaBias   = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(OldDeltaBias);

	StepWeight     = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(StepWeight);

	StepBias       = (REAL**) calloc(nLayer+1,sizeof(REAL*));
	CHECK_PTR(StepBias);

	int i;
	for (i=1; i LE nLayer; i++)
    {
		Status[i]         = (REAL*) calloc((nUnit)[i]*(nBatchPattern),sizeof(REAL));
		CHECK_PTR((Status)[i]);

		Delta[i]          = (REAL*) calloc((nUnit)[i]*(nBatchPattern),sizeof(REAL));
		CHECK_PTR((Delta)[i]);

        (OldWeight)[i]      = (REAL*) calloc((nUnit)[i]*(nUnit)[i-1],sizeof(REAL));
        CHECK_PTR((Weight)[i]);

		(OldBias)[i]        = (REAL*) calloc((nUnit)[i],sizeof(REAL));
		CHECK_PTR((Bias)[i]);

		(DeltaWeight)[i]    = (REAL*) calloc((nUnit)[i]*(nUnit)[i-1],sizeof(REAL));
		CHECK_PTR((DeltaWeight)[i]);

		(DeltaBias)[i]      = (REAL*) calloc((nUnit)[i],sizeof(REAL));
		CHECK_PTR((DeltaBias)[i]);

		(OldDeltaWeight)[i] = (REAL*) calloc((nUnit)[i]*(nUnit)[i-1],sizeof(REAL));
		CHECK_PTR((OldDeltaWeight)[i]);

		(OldDeltaBias)[i]   = (REAL*) calloc((nUnit)[i],sizeof(REAL));
		CHECK_PTR((OldDeltaBias)[i]);

		(StepWeight)[i]     = (REAL*) calloc((nUnit)[i]*(nUnit)[i-1],sizeof(REAL));
		CHECK_PTR((StepWeight)[i]);

		(StepBias)[i]       = (REAL*) calloc((nUnit)[i],sizeof(REAL));
		CHECK_PTR((StepBias)[i]);
	} 

	_batchAllocated = true;
}

//////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////

YARPBPNNet::~YARPBPNNet()
{
	if (_matrixAllocated)
		_deallocMatrix();
	
	if (_batchAllocated)
		_deallocBatch();
  
	if (nUnit != NULL)
		free(nUnit);
}

/////////////////////////////////////////////////////////////////////////////
// Random initialization of the weights
//////////////////////////////////////////////////////////////////////

void YARPBPNNet::init(long aSeed,REAL R,REAL Sat,REAL ASat)
{
	printf("Performing random initialization\n");
	InitRandom(aSeed);

	for (int i=1; i LE nLayer; i++ )
    {
		RandomWeights (Weight[i], nUnit[i-1], nUnit[i], ASat*R/(nUnit[i-1]+1.0F));
		RandomWeights (Bias[i],   nUnit[i], 1,          ASat*R/(nUnit[i-1]+1.0F));
    }

	n_epoch = 0;

	if (savelog)
	{
		printf("Erase previous log file (if any)\n");
		FILE *dummy = fopen(logfilename, "w");
		fclose(dummy);
	}
}

void YARPBPNNet::_deallocBatch()
{
	int i;
	for (i=1; i LE nLayer;i++)
	{
		free(Status[i]);
		free(Delta[i]);
		free(OldWeight[i]);
		free(OldBias[i]);
		free(DeltaWeight[i]);
		free(DeltaBias[i]);
		free(OldDeltaWeight[i]);
		free(OldDeltaBias[i]);
		free(StepWeight[i]);
		free(StepBias[i]);
    }

	free(OldWeight);
	free(OldBias);
	free(Delta);
	free(DeltaWeight);
	free(DeltaBias);
	free(OldDeltaWeight);
	free(OldDeltaBias);
	free(StepWeight);
	free(StepBias);
	free(Target);

	_batchAllocated = false;
}

void YARPBPNNet::_deallocMatrix()
{
	//////// firstly check if deallocation is needed
	if (max_input == NULL)
	{
		delete [] max_input;
		delete [] min_input;
		delete [] max_output;
		delete [] min_output;
		delete [] max_limit;
		delete [] min_limit;
	}
	
	int i;
	if (Weight != NULL)
	{
		for (i=1; i LE nLayer; i++)
			free(Weight[i]);
		free(Weight);
	}
	
	if (Bias != NULL)
	{
		for (i=1; i LE nLayer; i++)
			free(Bias[i]);
		free(Bias);
	}
		
	if (_internal_status != NULL)
	{
		for (i=1; i <= nLayer; i++)		//0 is not allocated
			delete [] _internal_status[i];
	
		delete [] _internal_status;
	}

	_matrixAllocated = false;
}

///////////////////////////////////////////////////////////////
// Training the net
//////////////////////////////////////////////////////////////////////

// call this  before you call the trainBatch function
void YARPBPNNet::trainBatchInit(int np, MBPOptions op)
{
	if (np LE 0)
    {
		ERROR_MSG("Insufficient number of patterns");
    }

	_batchTrainOptions = op;

	if (_batchAllocated)
		_deallocBatch();

	nBatchPattern = np;
	_allocBatch();
}

void YARPBPNNet::trainBatch(REAL* input,REAL* output)
{
	BOOL   endRun;          /* End single run ?                               */
	
	REAL   E0    = 0.75F;    /* Initial learning step                          */
	REAL   A0    = 0.9F;     /* Initial momentum                               */
	REAL   Psi   = 1.05F;    /* Vogl's parameter psi                           */
	REAL   Beta  = 0.7F;     /* Vogl's parameter beta                          */
	REAL   G     = 1.05F;    /* Acceptance of error increase (Vogl)            */
	REAL   Ka    = 0.7F;     /* Acceleration factor (_batchTrainOptions.YProp)                    */
	REAL   Kd    = 0.07F;    /* Deceleration factor (_batchTrainOptions.YProp)                    */
	REAL   Eta;             /* Learning step                                  */
	REAL   Alpha;           /* Momentum                                       */

	clock_t Start,End;      /* Speed measurements                              */

	REAL   OldAnaCost;      /* Analog cost at step n-1                        */
	REAL   MaxCost;         /* Maximum absolute cost          max|t-o|        */
	REAL   DigCost;         /* Digital cost                   sign(t*o) %     */
  	  
    int i;

	unsigned int maxEpoch;
	maxEpoch = n_epoch + _batchTrainOptions.IterTh;
	
	// normalize internal input and output vectors
	normalizeBatch(input, Status[0], max_input, min_input, n_input);
	normalizeBatch(output, Target, max_output, min_output, n_output);
	
	find_limits(output, max_limit, min_limit, n_output);

			
///////////////////////////////////
// Insert here the training code
//////////////////////////////////
  
	OldAnaCost = MAXFLOAT;
    Eta        = E0;
    endRun     = FALSE;

    /* Start the timer */

    Start = clock();

    /* Start the learning */

    while (NOT endRun)
    {

		/* for each layer */

		for (i=1; i LE nLayer; i++)
		{

		/* compute the forward phase for the learning patterns */

		FeedForward (Status[i],   nBatchPattern,  nUnit[i],
		     Status[i-1], nBatchPattern,  nUnit[i-1],
		     Weight[i],   nUnit[i-1], nUnit[i],
		     Bias[i],     nUnit[i]);
	
		}

		/* Back-propagate the error of the output layer */

		EBPLastLayer (Delta[nLayer],  nBatchPattern, nUnit[nLayer],
		    Target,         nBatchPattern, nUnit[nLayer],
		    Status[nLayer], nBatchPattern, nUnit[nLayer]);
       
		/* and the hidden layers */
			
		for (i=nLayer-1; i GE 1; i--)
		{
			EBP (Delta[i],    nBatchPattern, nUnit[i],
				 Delta[i+1],  nBatchPattern, nUnit[i+1],
				 Weight[i+1], nUnit[i],  nUnit[i+1],
				 Status[i],   nBatchPattern, nUnit[i]);
			 
		}

		/* Compute the gradient */

		for (i=1; i LE nLayer; i++)
		{
			Step (DeltaWeight[i], nUnit[i-1], nUnit[i],
				  Status[i-1],    nBatchPattern,  nUnit[i-1],
				  Delta[i],       nBatchPattern,  nUnit[i],
				  DeltaBias[i],   nUnit[i]);
		}

		/* Compute the costs and the gradient norm */

		DigCost  = ComputeDigitalCost (Target,         nBatchPattern, nUnit[nLayer],
									   Status[nLayer], nBatchPattern, nUnit[nLayer]);
		analogCost  = ComputeAnalogCost  (Target,         nBatchPattern, nUnit[nLayer],
									   Status[nLayer], nBatchPattern, nUnit[nLayer]);
		MaxCost  = ComputeMaximumCost (Target,         nBatchPattern, nUnit[nLayer],
									   Status[nLayer], nBatchPattern, nUnit[nLayer]);
		gradient = ComputeGradientNorm (DeltaWeight, DeltaBias, nUnit,
											nLayer);
     		
		/* Print the starting step */

		if (n_epoch EQ 0)
		{
			PrintStep ("\n#", n_epoch, DigCost, analogCost, MaxCost, gradient);
			appendLog();
		}
			
		/* and every _batchTrainOptions.nPrints steps */

		if (_batchTrainOptions.nPrints NE 0 AND n_epoch GT 0 AND (n_epoch % _batchTrainOptions.nPrints EQ 0))
		{
			PrintStep ("#", n_epoch, DigCost, analogCost, MaxCost, gradient);
			appendLog();
		}
      
		/* Test if a threshold has been reached */

		if (gradient	 LE _batchTrainOptions.GradTh OR
			DigCost      LE _batchTrainOptions.DigTh  OR
			analogCost   LE _batchTrainOptions.AnaTh  OR
			MaxCost      LE _batchTrainOptions.MaxTh  OR
			n_epoch      GE maxEpoch )
		{

			/* Stop the timer */

			End = clock();

			/* and the current run */        
	
			endRun = TRUE;
	
			/* Print the last step */

			PrintStep("\n##", n_epoch, DigCost, analogCost, MaxCost, gradient);
			appendLog();

			/* Print the speed of the lerning */
	
			OutTime (n_epoch+1, nLayer, nBatchPattern, nUnit, (double)(End-Start)/CLOCKS_PER_SEC);

		}
			
		/* else makes a step following the Vogl's or _batchTrainOptions.YProp algorithm */
			
		else
		{
	
			/* If cost is better increase Eta */

			if (analogCost LT OldAnaCost)
			{
				if (_batchTrainOptions.YProp)
				{
					Eta *= 1+Ka/(Ka+Eta);
				}
				else
				{
					Eta *= Psi;            
				}
				Alpha      = A0;
				OldAnaCost = analogCost;
			}

	        /* If cost is worse than a few percent decrease Eta
			and zeroes momentum */

			else if (analogCost LE G*OldAnaCost)
			{
				if (_batchTrainOptions.YProp)
				{
					Eta *= Kd/(Kd+Eta);
				}
				else
				{
					Eta *= Beta;
				}
				Alpha       = 0.0;
				OldAnaCost = analogCost;
			}
 
			/* If cost is really worse, backstep */

			else
			{
				if (_batchTrainOptions.YProp)
				{
					Eta *= Kd/(Kd+Eta);
				}
				else
				{
					Eta *= Beta;
				}
				Alpha       = 0.0;
				for (i=1; i LE nLayer; i++)
				{
					BackStep (DeltaWeight[i],    nUnit[i-1], nUnit[i],
							DeltaBias[i],      nUnit[i],
							OldDeltaWeight[i], nUnit[i-1], nUnit[i],
							OldDeltaBias[i],   nUnit[i],
							Weight[i], nUnit[i-1], nUnit[i],
							Bias[i], nUnit[i],
							StepWeight[i], nUnit[i-1], nUnit[i],
							StepBias[i], nUnit[i]);
				}
			}

			/* Makes the learning step */

			for (i=1; i LE nLayer; i++)
			{
				UpdateWeights (StepWeight[i],     nUnit[i-1], nUnit[i],
								StepBias[i], nUnit[i], Eta, Alpha,
								DeltaWeight[i],    nUnit[i-1], nUnit[i],
								DeltaBias[i],      nUnit[i],
								OldDeltaWeight[i], nUnit[i-1], nUnit[i],
								OldDeltaBias[i],   nUnit[i],
								Weight[i],         nUnit[i-1], nUnit[i],
								Bias[i],           nUnit[i]);
			}
		}

		n_epoch++;
		
	}
   
}

void YARPBPNNet::trainByPattern(REAL *input, REAL *output, double step)
{
	// REAL   E0    = 0.005F;////0.75F;    /* Initial learning step                          */
	REAL E0 = step;
	REAL   A0    = 0.9F;     /* Initial momentum                               */
	REAL   Psi   = 1.05F;    /* Vogl's parameter psi                           */
	REAL   Beta  = 0.7F;     /* Vogl's parameter beta                          */
	REAL   G     = 1.05F;    /* Acceptance of error increase (Vogl)            */
	REAL   Ka    = 0.7F;     /* Acceleration factor (_batchTrainOptions.YProp)                    */
	REAL   Kd    = 0.07F;    /* Deceleration factor (_batchTrainOptions.YProp)                    */
	REAL   Eta;             /* Learning step                                  */
	REAL   Alpha;           /* Momentum                                       */

	REAL   GradientNorm;    /* Gradient norm                                  */
	REAL   AnaCost;         /* Analog cost                    (t-o)^2         */
	REAL   OldAnaCost;      /* Analog cost at step n-1                        */
	REAL   MaxCost;         /* Maximum absolute cost          max|t-o|        */
	REAL   DigCost;         /* Digital cost                   sign(t*o) %     */
  	long   nIter;           /* Iteration #                                    */
  
    int i;
	
	// normalize internal input and output vectors
	normalizeBatch(input, Status[0], max_input, min_input, n_input);
	normalizeBatch(output, Target, max_output, min_output, n_output);

	// find max and min limits
	find_limits(output, max_limit, min_limit, n_output);
		
			
///////////////////////////////////
// Insert here the training code
//////////////////////////////////
  
	nIter      = 0;
    OldAnaCost = MAXFLOAT;
    Eta        = E0;
	Alpha = A0;

    /* for each layer */

	for (i=1; i LE nLayer; i++)
	{

		/* compute the forward phase for the learning patterns */

		FeedForward (Status[i],   nBatchPattern,  nUnit[i],
		     Status[i-1], nBatchPattern,  nUnit[i-1],
		     Weight[i],   nUnit[i-1], nUnit[i],
		     Bias[i],     nUnit[i]);
	
	}

	/* Back-propagate the error of the output layer */

	EBPLastLayer (Delta[nLayer],  nBatchPattern, nUnit[nLayer],
	    Target,         nBatchPattern, nUnit[nLayer],
	    Status[nLayer], nBatchPattern, nUnit[nLayer]);
       
	/* and the hidden layers */
			
	for (i=nLayer-1; i GE 1; i--)
	{
		EBP (Delta[i],    nBatchPattern, nUnit[i],
			 Delta[i+1],  nBatchPattern, nUnit[i+1],
			 Weight[i+1], nUnit[i],  nUnit[i+1],
			 Status[i],   nBatchPattern, nUnit[i]);
	}

	/* Compute the gradient */

	for (i=1; i LE nLayer; i++)
	{
		Step (DeltaWeight[i], nUnit[i-1], nUnit[i],
			  Status[i-1],    nBatchPattern,  nUnit[i-1],
			  Delta[i],       nBatchPattern,  nUnit[i],
			  DeltaBias[i],   nUnit[i]);
	}

	/* Compute the costs and the gradient norm */

	DigCost  = ComputeDigitalCost (Target,         nBatchPattern, nUnit[nLayer],
								   Status[nLayer], nBatchPattern, nUnit[nLayer]);
	AnaCost  = ComputeAnalogCost  (Target,         nBatchPattern, nUnit[nLayer],
									   Status[nLayer], nBatchPattern, nUnit[nLayer]);
	MaxCost  = ComputeMaximumCost (Target,         nBatchPattern, nUnit[nLayer],
									   Status[nLayer], nBatchPattern, nUnit[nLayer]);
	GradientNorm = ComputeGradientNorm (DeltaWeight, DeltaBias, nUnit,
											nLayer);

	/* Makes the learning step */
	for (i=1; i LE nLayer; i++)
	{
		UpdateWeights (StepWeight[i],     nUnit[i-1], nUnit[i],
    					StepBias[i], nUnit[i], Eta, Alpha,
						DeltaWeight[i],    nUnit[i-1], nUnit[i],
						DeltaBias[i],      nUnit[i],
						OldDeltaWeight[i], nUnit[i-1], nUnit[i],
						OldDeltaBias[i],   nUnit[i],
						Weight[i],         nUnit[i-1], nUnit[i],
						Bias[i],           nUnit[i]);
	}

	analogCost = AnaCost;
	gradient = GradientNorm;
}

///////////////////////////////////////////////////////////////
// Simulate the net, from a single input pattern (fast !)
//////////////////////////////////////////////////////////////////////

void YARPBPNNet::sim(const REAL *input, REAL *output)
{
	int i = 0;
		
	for(i = 0; i < n_input; i++)
	  tmp_input[i] = normalize(input[i], max_input[i], min_input[i]);
			
	_internal_status[0] = tmp_input;//const_cast<REAL *>(input);

	for (i=1; i LE nLayer; i++)
    {
		FeedForward (_internal_status[i],   1,  nUnit[i],
					 _internal_status[i-1], 1,  nUnit[i-1],
					 Weight[i],   nUnit[i-1], nUnit[i],
					 Bias[i],     nUnit[i]);
    }

	memcpy(tmp_output, _internal_status[nLayer], sizeof(REAL)*nUnit[nLayer]);

	for(i = 0; i < n_output; i++)
	{
		output[i] = de_normalize(tmp_output[i], max_output[i], min_output[i]);
		output[i] = check_value(output[i], max_limit[i], min_limit[i]);
	}
}

// simulate the network from a specified number of input patterns
// slow !!, but do we need it fast ?
void YARPBPNNet::sim(int nIPattern,REAL *input, REAL *output)
{
	nBatchPattern = nIPattern;
	Status[0] = (REAL *) calloc(n_input*nBatchPattern, sizeof(REAL));
	normalizeBatch(input, Status[0], max_input, min_input, n_input);
	
	for (int i=1; i LE nLayer; i++)
    {
		Status[i]         = (REAL*) calloc((nUnit)[i]*(nIPattern),sizeof(REAL));
		CHECK_PTR((Status)[i]);
	}

	for (i=1; i LE nLayer; i++)
    {
		FeedForward (Status[i],   nIPattern,  nUnit[i],
					 Status[i-1], nIPattern,  nUnit[i-1],
					 Weight[i],   nUnit[i-1], nUnit[i],
					 Bias[i],     nUnit[i]);
    }

	de_normalizeBatch(Status[nLayer], output, max_output, min_output, n_output);

	for (i=0; i <= nLayer;i++)
	{
		free(Status[i]);
	}
}

/////////////////////////////////////////////////
// Save the net
////////////////////////////////////////////////

void YARPBPNNet::save(const char* filename)
{
	ofstream output(filename);
	if(!output)
	{
		cout << "Error: cannot write or create file " << filename << "\n"; 
		exit(2);
	}
	
	output << "[NET]\n";
	output << "NLayers= " << nLayer << '\n';
	output << "NUnits= ";
	int i;
	for (i=0; i LE nLayer; i++)
    {
		output << nUnit[i] << " ";
	}
	output << "\n";

	//////// MAX/MIN INPUT
	output << "InputMax= ";
	for(i = 0; i LT nUnit[0]; i++)
		output << max_input[i] << " ";
	output << "\n";
	
	output << "InputMin= ";
	for(i = 0; i LT nUnit[0]; i++)
		output << min_input[i] << " ";
	output << "\n";
	////////////////////////////

	//////// MAX/MIN OUTPUT
	output << "TargetMax= ";
	for(i = 0; i LT nUnit[nLayer]; i++)
		output << max_output[i] << " ";
	output << "\n";
	
	output << "TargetMin= ";
	for(i = 0; i LT nUnit[nLayer]; i++)
		output << min_output[i] << " ";
	output << "\n";
	/////////////////////////////

	///////// LIMITS
	output << "OutputMax= ";
	for(i = 0; i LT nUnit[nLayer]; i++)
		output << max_limit[i] << " ";
	output << "\n";
	
	output << "OutputMin= ";
	for(i = 0; i LT nUnit[nLayer]; i++)
		output << min_limit[i] << " ";
	output << "\n";
	//////////////////////////////////

	// #epochs
	output << "Epoch= ";
	output << (n_epoch-1);
	output << "\n";

	// cost gradient
	output << "AnalogCost= " << analogCost << "\n";
	output << "Grad= " << gradient << "\n";

	// logfile
	output << "LogFile= " << logfilename << "\n";

	///////// WEIGHTS and BIASES
    for (i=1; i LE nLayer; i++)
	{
		char tmp[255];
		sprintf(tmp, "%s[%d]= ", "Weights", i);
		output << tmp;

		for (int j=0; j LT nUnit[i]*nUnit[i-1]; j++)
		{
			output << Weight[i][j] << " ";
		}
		output << "\n";

		sprintf(tmp, "%s[%d]= ", "Biases", i);
		output<<tmp;

		for (j=0; j LT nUnit[i]; j++)
		{
			output << Bias[i][j] << " ";
		}
		output << "\n";
	} 

	output << "[END]\n";
	output.close();
}

void YARPBPNNet::set_input(const REAL *max, const REAL *min)
{
	memcpy(max_input, max, sizeof(REAL) * n_input);
	memcpy(min_input, min, sizeof(REAL) * n_input);
}

void YARPBPNNet:: set_output(const REAL *max, const REAL *min)
{
	memcpy(max_output, max, sizeof(REAL) * n_output);
	memcpy(min_output, min, sizeof(REAL) * n_output);
}

void YARPBPNNet::set_limits(const REAL *max, const REAL *min)
{
	memcpy(max_limit, max, sizeof(REAL) * n_output);
	memcpy(min_limit, min, sizeof(REAL) * n_output);
}

double YARPBPNNet::getRandom()
{
	return RandomNumber();
}