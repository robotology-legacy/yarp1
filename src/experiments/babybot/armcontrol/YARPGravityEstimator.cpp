#include "YARPGravityEstimator.h"

YARPGravityEstimator::YARPGravityEstimator(int par):
_leastSquares(par, 1.0)
{
	_parSize = par;
	_parameters.Resize(_parSize);
	_input.Resize(_parSize);
	_k1 = 0.0;
	_k2 = 1.0;
	_deltaK1 = 1.0/__querySize;

	YMatrix SP0(_parSize, _parSize);
	SP0 = 0.0;
	for(int i = 1; i<=_parSize; i++)
		SP0(i,i) = 1.0;
		
	YVector ST0(_parSize);
	ST0 = 0.0;

	_leastSquares.Reset();
	_leastSquares.SetInitialState(SP0, ST0);
}

YARPGravityEstimator::~YARPGravityEstimator()
{

}

int YARPGravityEstimator::save(const char *filename)
{
	_lock();
	YMatrix p0(_parSize,_parSize);
	YVector par(_parSize);
	p0 = _leastSquares.GetP ();
	par = _parameters;
	int steps = _leastSquares.GetSteps();
	// end of the shared part
	_unlock();

	FILE *fp = fopen(filename, "wt");
	if (fp == NULL)
		return YARP_FAIL;

	fprintf(fp, "%d\n", steps);
	int i;
	for(i = 1; i<=_parSize; i++)
		fprintf (fp, "%lf\n", par(i));

	for (i = 1; i <= _parSize; i++)
		for (int j = 1; j <= _parSize; j++)
			fprintf (fp, "%lf ", p0 (i, j));
	
	fprintf (fp, "\n");

	fclose(fp);
	return YARP_OK;
}

int YARPGravityEstimator::load(const char *filename)
{
	_lock();
	FILE *fp = fopen(filename, "r");
	int nsteps;
	YVector t0(_parSize);
	YMatrix p0(_parSize,_parSize);

	fscanf (fp, "%d\n", &nsteps);

	int i,j; 
	for(i = 0; i < _parSize; i++)
	fscanf (fp, "%lf\n", t0.data()+i);

	for (i = 0; i < _parSize; i++)
		for (j = 0; j < _parSize; j++)
			fscanf (fp, "%lf ", p0.data()[i] + j);
	
	fscanf (fp, "\n");

	// set data.
	_leastSquares.SetInitialState (p0, t0);
	_leastSquares.SetSteps (nsteps);
	_parameters = t0;

	fclose (fp);

	_unlock();
	return YARP_OK;
}
