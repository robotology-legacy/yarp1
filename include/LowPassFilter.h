//
// LowPassFilter.h
//

#ifndef __LowPassFilterh__
#define __LowPassFilterh__

#define LENGTH 6

class CLowPass3
{
protected:
	double b[4];
	double a[4];
	double past_o[3];
	double past_i[4];

public:
	CLowPass3 ()
	{
		b[0] = 0.0029;
		b[1] = 0.0087;
		b[2] = 0.0087;
		b[3] = 0.0029;

		a[0] = 1.0;
		a[1] = -2.3741;
		a[2] = 1.9294;
		a[3] = -0.5321;

		memset (past_o, 0, sizeof(double) * 3);
		memset (past_i, 0, sizeof(double) * 4);
	}

	virtual ~CLowPass3 () {}

	void reset (void)
	{
		memset (past_o, 0, sizeof(double) * 3);
		memset (past_i, 0, sizeof(double) * 4);
	}

	double filter (double value)
	{
		past_i[0] = value;
		double yn = 0;
		
		for (int i = 0; i < 4; i++)
			yn += (b[i] * past_i[i]);

		for (i = 0; i < 3; i++)
			yn -= (a[i+1] * past_o[i]);

		for (i = 3; i >= 1; i--)
			past_o[i] = past_o[i-1];
		past_o[0] = yn;

		for (i = 2; i >= 1; i--)
			past_i[i] = past_i[i-1];

		return yn;
	}
};

class CMedian3
{
protected:
	double past_i[3];

public:
	CMedian3 ()
	{
		memset (past_i, 0, sizeof(double) * 3);
	}

	virtual ~CMedian3 () {}

	void reset () { memset (past_i, 0, sizeof(double) * 3); } 

	double filter (double value)
	{
		past_i[2] = past_i[1];
		past_i[1] = past_i[0];
		past_i[0] = value;

		double max = past_i[0], min = past_i[0];
		int imax = 0, imin = 0;
		for (int i = 1; i < 3; i++)
		{
			if (past_i[i] > max)
			{
				max = past_i[i];
				imax = i;
			}
			else
			if (past_i[i] < min)
			{
				min = past_i[i];
				imin = i;
			}
		}

		int median = 0;
		for (i = 0; i < 3; i++)
			if (i != imax && i != imin)
				median = i;

		return past_i[median];
	}
};

class CLowPassFIR10
{
protected:
	double b[LENGTH];
	double a[LENGTH];
	double past_o[LENGTH-1];
	double past_i[LENGTH];

public:
	CLowPassFIR10 ()
	{
		b[0] = 0.0264;
		b[1] = 0.1405;
		b[2] = 0.3331;
		b[3] = 0.3331;
		b[4] = 0.1405;
		b[5] = 0.0264;

		a[0] = 1.0;
		a[1] = 0.0;
		a[2] = 0.0;
		a[3] = 0.0;
		a[4] = 0.0;
		a[5] = 0.0;

		memset (past_o, 0, sizeof(double) * (LENGTH - 1));
		memset (past_i, 0, sizeof(double) * LENGTH);
	}

	virtual ~CLowPassFIR10 () {}

	void reset (void)
	{
		memset (past_o, 0, sizeof(double) * (LENGTH - 1));
		memset (past_i, 0, sizeof(double) * LENGTH);
	}

	double filter (double value)
	{
		past_i[0] = value;
		double yn = 0;
		
		for (int i = 0; i < LENGTH; i++)
			yn += (b[i] * past_i[i]);

		for (i = LENGTH-2; i >= 1; i--)
			past_i[i] = past_i[i-1];

		return yn;
	}
};

class CLowPassFilter10
{
protected:
	double b[LENGTH];
	double a[LENGTH];
	double past_o[LENGTH-1];
	double past_i[LENGTH];

public:
	CLowPassFilter10 ()
	{
		b[0] = 0.0598 * 1e-3;
		b[1] = 0.2990 * 1e-3;
		b[2] = 0.5980 * 1e-3;
		b[3] = 0.5980 * 1e-3;
		b[4] = 0.2990 * 1e-3;
		b[5] = 0.0598 * 1e-3;

		a[0] = 1.0;
		a[1] = -3.9845;
		a[2] = 6.4349;
		a[3] = -5.2536;
		a[4] = 2.1651;
		a[5] = -0.3599;

		memset (past_o, 0, sizeof(double) * (LENGTH - 1));
		memset (past_i, 0, sizeof(double) * LENGTH);
	}

	virtual ~CLowPassFilter10 () {}

	void reset (void)
	{
		memset (past_o, 0, sizeof(double) * (LENGTH - 1));
		memset (past_i, 0, sizeof(double) * LENGTH);
	}

	double filter (double value)
	{
		past_i[0] = value;
		double yn = 0;
		
		for (int i = 0; i < LENGTH; i++)
			yn += (b[i] * past_i[i]);

		for (i = 0; i < LENGTH-1; i++)
			yn -= (a[i+1] * past_o[i]);

		for (i = LENGTH-1; i >= 1; i--)
			past_o[i] = past_o[i-1];
		past_o[0] = yn;

		for (i = LENGTH-2; i >= 1; i--)
			past_i[i] = past_i[i-1];

		return yn;
	}
};

class CLowPassFilter30
{
protected:
	double b[LENGTH];
	double a[LENGTH];
	double past_o[LENGTH-1];
	double past_i[LENGTH];

public:
	CLowPassFilter30 ()
	{
		b[0] = 0.0069;
		b[1] = 0.0347;
		b[2] = 0.0693;
		b[3] = 0.0693;
		b[4] = 0.0347;
		b[5] = 0.0069;

		a[0] = 1.0;
		a[1] = -1.9759;
		a[2] = 2.0135;
		a[3] = -1.1026;
		a[4] = 0.3276;
		a[5] = -0.0407;

		memset (past_o, 0, sizeof(double) * (LENGTH - 1));
		memset (past_i, 0, sizeof(double) * LENGTH);
	}

	virtual ~CLowPassFilter30 () {}

	void reset (void)
	{
		memset (past_o, 0, sizeof(double) * (LENGTH - 1));
		memset (past_i, 0, sizeof(double) * LENGTH);
	}

	double filter (double value)
	{
		past_i[0] = value;
		double yn = 0;
		
		for (int i = 0; i < LENGTH; i++)
			yn += (b[i] * past_i[i]);

		for (i = 0; i < LENGTH-1; i++)
			yn -= (a[i+1] * past_o[i]);

		for (i = LENGTH-1; i >= 1; i--)
			past_o[i] = past_o[i-1];
		past_o[0] = yn;

		for (i = LENGTH-2; i >= 1; i--)
			past_i[i] = past_i[i-1];

		return yn;
	}
};

#endif