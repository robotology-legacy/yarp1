#include <fstream>
#include <vector>
#include <YARPBabybotHeadKin.h>

using namespace std;

#include <YARPLogpolar.h>

YARPLogpolar _mapper;

int read_set (const char *filename, double **in, double **t, int inSize, int outSize)
{
	int np = 0;

	bool sequenceOK = false;

	ifstream input_file(filename);
		
	vector<double> input_v;
	vector<double> target_v;

	double *inTmp = new double [inSize];
	double *outTmp = new double [outSize];
	while (!input_file.eof())
	{		
		for(int i = 0; i < inSize; i++)
			input_file >> inTmp[i];
			
		for(int j = 0; j < outSize; j++)
			input_file >> outTmp[j];
		
		if (!input_file.eof())
			sequenceOK = true;
		else
			sequenceOK = false;

		if (sequenceOK)
		{
			for(i = 0; i < inSize; i++)
				input_v.push_back(inTmp[i]);

			for(j = 0; j < outSize; j++)
				target_v.push_back(outTmp[j]);
			np++;
		}
	} 

	delete [] inTmp;
	delete [] outTmp;

	input_file.close();

	// alloc struct
	*in  = new double[np*inSize];
	*t = new double[np*outSize];

	int i = 0;
	while(i<input_v.size())
	{
		for(int j = 0; j < inSize; j++)
		{
			(*in)[i] = input_v[i];
			i++;
		}
	}
	
	i=0;
	while(i<target_v.size())
	{
		for(int j = 0; j < outSize; j++)
		{
			(*t)[i] = target_v[i];
			i++;
		}
	}


	return np;

}

void parsePatternsCenters(double *iRaw, double *tRaw, double **in, double **out, int n)
{
	*in = new double [3*n];		// 3 arm joints
	*out = new double [3*n];	// output vector

	double *inTmp = (*in);
	double *outTmp = (*out);

	YARPBabybotHeadKin _gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) );

	YVector tmpHeadJoints(5);
	YVector tmpV(3);
	int tmpX;
	int tmpY;

	// for each pattern
	for(int i = 0; i < n; i++)
	{
		*inTmp++ = *iRaw++;
		*inTmp++ = *iRaw++;
		*inTmp++ = *iRaw++;
		
		iRaw+=3;	// skip 3 wrist joints

		for(int j = 1; j <= 5; j++)
			tmpHeadJoints(j) = *iRaw++;
		
		int rho = *tRaw++;
		int theta = *tRaw++;
		_mapper.Logpolar2Cartesian(rho, theta, tmpX, tmpY);
		//tmpX = tmpX + _logpolarParams::_xsize/2;
		//tmpY = _logpolarParams::_ysize/2 - tmpY;
		
		tRaw+=3;	// skip 3 conic parameters
		_gaze.update (tmpHeadJoints);
		_gaze.computeRay (YARPBabybotHeadKin::KIN_LEFT_PERI, tmpV, (double) tmpX, (double) tmpY);
		
		*outTmp++ = tmpV(1);
		*outTmp++ = tmpV(2);
		*outTmp++ = tmpV(3);
	}
}

void parsePatternsParams(double *iRaw, double *tRaw, double **in, double **out, int n)
{
	*in = new double [3*n];		// 3 arm joints
	*out = new double [3*n];	// output vector

	double *inTmp = (*in);
	double *outTmp = (*out);

	// for each pattern
	for(int i = 0; i < n; i++)
	{
		*inTmp++ = *iRaw++;
		*inTmp++ = *iRaw++;
		*inTmp++ = *iRaw++;
		
		iRaw+=3;	// skip 3 wrist joints

		tRaw++;		// skip retinal X pos
		tRaw++;		// skip retinal Y pos
		
		*outTmp++ = *tRaw++;	// 3 conic parameters
		*outTmp++ = *tRaw++;
		*outTmp++ = *tRaw++;
	}
}
