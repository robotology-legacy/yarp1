#ifndef __hallparameters__
#define __hallparameters__

const double const __hallParameters[15][8] = {
	{-0.0032,	0.0105,		-0.1066,	0.3084,		0.1,	3.0,	0.3,	0.0},
	{0.0104,	-0.0640,	0.3085,		0.2725,		0.45,	3.0,	0.0,	0.9},
	{-0.6639,	2.8048,		-4.1735,	2.581,		0.85,	2.10,	0.6,	0.0},

	{0.0083,	-0.0608,	0.2141,		0.2388,		0.11,	4.78,	0.0,	0.8},
	{0.0465,	-0.2325,	0.4810,		0.3343,		0.30,	2.85,	0.0,	0.9},
	{0.0,		0.0,		0.0,		0.0,		5.0,	0.0,	0.0,	0.0},
	
	{-0.2075,	0.5624,		-0.5253,	0.3895,		0.05,	1.9,	0.4,	0.0},
	{0.0442,	-0.4648,	1.7217,		-1.6221,	1.4,	4.76,	0.0,	0.8},
	{0.0,		0.0,		0.0,		0.0,		5.0,	0.0,	0.0,	0.0},
	
	{0.0028,	-0.0210,	0.1084,		0.2253,		0.8,	4.0,	0.0,	0.5},
	{0.0041,	-0.0373,	0.2370,		0.0828,		0.25,	4.76,	0.0,	1.0},
	{0.0,		0.0,		0.0,		0.0,		5.0,	0.0,	0.0,	0.0},
	
	{-0.0094,	0.0741,		-0.0949,	0.2835,		0.7,	4.76,	0.0,	0.5},
	{-0.0922,	1.0980,		-4.1249,	5.0067,		2.95,	4.7,	0.0,	0.5},
	{0.0,		0.0,		0.0,		0.0,		5.0,	0.0,	0.0,	0.0}
};

const double const __offsets[16] = {-90*degToRad, -45*degToRad,	0, 0,
									0, -10*degToRad, 0,
									0, -20*degToRad, 0,
									0, -20*degToRad, 0,
									0, -15*degToRad, 0};

class Polynomial
{
public:
	Polynomial()
	{
		int i = 0;
		for (i = 0; i < 4; i++)
			_parameters[i] = 0;
		for (i = 0; i < 4; i++)
			_limits[i] = 0;
	}

	~Polynomial(){}

	inline double operator() (double input)
	{
		if (input <= _limits[0])
			return _limits[2];

		if (input >= _limits[1])
			return _limits[3];

		double tmp;
		tmp = _parameters[0]*input*input*input + _parameters[1] *input*input + _parameters[2]*input + _parameters[3];

		return tmp;
	}

	void setParameters(const double *p)
	{
		memcpy(_parameters, p, sizeof(double)*4);	// first four polynomial params
		memcpy(_limits, p+4, sizeof(double)*4);	// limits params
	}

	double _parameters[3+1];
	double _limits[4];
};

#endif