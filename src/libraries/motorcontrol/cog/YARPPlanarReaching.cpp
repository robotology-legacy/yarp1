//
// YARPPlanarReaching.cpp
//
//
//
//

#include "YARPPlanarReaching.h"
#include "YARPRandomNumber.h"
#include <time.h>

YARPPlanarReaching::YARPPlanarReaching (int inputsize, int outputsize, int numbases) 
{
	int i;

	m_isize = inputsize;
	m_osize = outputsize;
	m_bases = numbases;


	m_linear_approx = new CVisDVector[m_osize];
	assert (m_linear_approx != NULL);

	m_v = new CVisDVector[m_bases-1];
	assert (m_v != NULL);

	m_g = new CVisDVector[m_bases];
	assert (m_g != NULL);

	m_vergence = new CVisDVector[m_bases];
	assert (m_vergence != NULL);

	m_r = new CVisDVector[m_bases];
	assert (m_r != NULL);


	for (i = 0; i < m_osize; i++)
	{
		m_linear_approx[i].Resize (m_isize+1);
		m_linear_approx[i] = 0;
	}

	m_linear_vergence.Resize (m_isize+1);
	m_linear_vergence = 0;

	m_alpha = m_beta = m_gamma = 0;

	for (i = 0; i < m_bases; i++)
	{
		m_g[i].Resize(m_isize); m_g[i] = 0;			// fixation point described as version, vergence and tilt.
		m_r[i].Resize(m_osize); m_r[i] = 0;			// 6 dof of the arm.
		m_vergence[i].Resize(1); m_vergence[i] = 0;	// 1 number only.
	}

	for (i = 0; i < m_bases-1; i++)
	{
		m_v[i].Resize(m_isize); m_v[i] = 0;
	}

	for (i = 0; i < N_ACTIONS; i++)
	{
		m_gaze_shift[i].Resize(2); m_gaze_shift[i] = 0;

		m_position_shift[i].Resize(m_osize); m_position_shift[i] = 0;
		m_pushing_values[i].Resize(m_osize); m_pushing_values[i] = 0;
	}

	m_sideposition.Resize (m_osize);
	m_sideposition = 0;

	m_calibrated = false;

	YARPRandom::Seed (time(NULL));
}

YARPPlanarReaching::~YARPPlanarReaching () 
{
	if (m_linear_approx != NULL) delete[] m_linear_approx;

	if (m_v != NULL) delete[] m_v;
	if (m_g != NULL) delete[] m_g;
	if (m_r != NULL) delete[] m_r;
	if (m_vergence != NULL) delete[] m_vergence;
}

void YARPPlanarReaching::PRV (const CVisDVector& v)
{
	for (int i = 1; i <= v.Length(); i++)
		printf ("%lf ", v(i));
	printf ("\n");
}

void YARPPlanarReaching::BuildTemporaryVectors (void)
{
	// reconstruct other temporary vectors.
	for (int i = 1; i <= m_bases-1; i++)
	{
		m_v[i-1] = m_g[i] - m_g[0];
	}

	// m_v are the basis vectors (non-orthogonal, non-normalized).
	m_alpha = m_v[0].norm2square();
	m_beta = m_v[1].norm2square();
	m_gamma = m_v[0] * m_v[1];

	//
	CVisDMatrix A(m_bases, m_isize+1);
	CVisDMatrix At(m_isize+1, m_bases);
	CVisDMatrix sqA(m_isize+1,m_isize+1);
	CVisDVector sqB(m_isize+1);
	A = 0;
	At = 0;
	CVisDVector b(m_bases);
	b = 0;

	for (int comp = 0; comp < m_osize; comp++)
	{
		A = 0;
		b = 0;

		// build A,b for each component.
		for (int rows = 1; rows <= m_bases; rows++)
		{
			for (int cols = 1; cols <= m_isize; cols++)
			{
				A(rows,cols) = m_g[rows-1](cols);
			}
			A(rows,m_isize+1) = 1;
			b(rows) = m_r[rows-1](comp+1);
		}

		// solve by LU.
		At = A.Transposed();
		sqA = At * A;
		sqB = At * b;
		VisDMatrixLU(sqA,sqB,m_linear_approx[comp]);

		// solution.
		printf ("solution comp % d : ",comp); PRV(m_linear_approx[comp]);
	}

	// for vergence.
	A = 0;
	b = 0;

	// build A,b for each component.
	for (int rows = 1; rows <= m_bases; rows++)
	{
		for (int cols = 1; cols <= m_isize; cols++)
		{
			A(rows,cols) = m_g[rows-1](cols);
		}
		A(rows,m_isize+1) = 1;
		b(rows) = m_vergence[rows-1](1);
	}

	// solve by LU.
	At = A.Transposed();
	sqA = At * A;
	sqB = At * b;
	VisDMatrixLU(sqA,sqB,m_linear_vergence);

	// solution.
	printf ("vergence : "); PRV(m_linear_vergence);

	m_calibrated = true;
}

void YARPPlanarReaching::LoadCalibration (const char *filename) 
{
	FILE *fp = fopen (filename, "r");
	assert (fp != NULL);

	for (int b = 0; b < m_bases; b++)
	{
		double tmp;
		for (int i = 1; i <= m_isize; i++)
		{
			fscanf (fp, "%lf ", &tmp);
			m_g[b](i) = tmp;
		}
		fscanf (fp, "\n");

		for (i = 1; i <= m_osize; i++)
		{
			fscanf (fp, "%lf ", &tmp);
			m_r[b](i) = tmp;
		}
		fscanf (fp, "\n");
	}

	printf ("g[0]: "); PRV(m_g[0]);
	printf ("g[1]: "); PRV(m_g[1]);
	printf ("g[2]: "); PRV(m_g[2]);

	double tmp;
	for (int i = 1; i <= m_osize; i++)
	{
		fscanf (fp, "%lf ", &tmp);
		m_sideposition(i) = tmp;
	}
	fscanf (fp, "\n");

	for (i = 0; i < m_bases; i++)
	{
		fscanf (fp, "%lf ", &tmp);
		m_vergence[i](1) = tmp;
	}
	fscanf (fp, "\n");

	fscanf (fp, "\n");

	//
	for (i = 0; i < N_ACTIONS; i++)
	{
		fscanf (fp, "%lf %lf\n", m_gaze_shift[i].data(), m_gaze_shift[i].data()+1);  
		for (int j = 0; j < m_osize; j++)
			fscanf (fp, "%lf ", m_position_shift[i].data()+j);
		fscanf (fp, "\n");
		for (j = 0; j < m_osize; j++)
			fscanf (fp, "%lf ", m_pushing_values[i].data()+j);
		fscanf (fp, "\n\n");
	}

	if (fp != NULL)
		fclose (fp);

	BuildTemporaryVectors ();
}

void YARPPlanarReaching::StoreCalibration (const char *filename)
{
	FILE *fp = fopen (filename, "w");
	assert (fp != NULL);

	for (int b = 0; b < m_bases; b++)
	{
		for (int i = 1; i <= m_isize; i++)
		{
			fprintf (fp, "%lf ", m_g[b](i));
		}
		fprintf (fp, "\n");

		for (i = 1; i <= m_osize; i++)
		{
			fprintf (fp, "%lf ", m_r[b](i));
		}
		fprintf (fp, "\n");
	}
	
	for (int i = 1; i <= m_osize; i++)
		fprintf (fp, "%lf ", m_sideposition(i));
	fprintf (fp, "\n");

	for (i = 0; i < m_bases; i++)
	{
		fprintf (fp, "%lf ", m_vergence[i](1));
	}
	fprintf (fp, "\n");

	if (fp != NULL)
		fclose (fp);

	// WARNING: don't save action description vectors!!!!
}


// returns direction of gaze in radians.
void YARPPlanarReaching::ForwardKinHead (const JointPos& head, CVisDVector& out)
{
	// combine and return in out vector.
	m_gaze.Apply ((JointPos&)head);

	out(1) = m_gaze.theta_middle;
	out(2) = m_gaze.phi_middle;
}

void YARPPlanarReaching::ComputeVergence (const CVisDVector& gaze, double& expected)
{
	// expected vergence given orientation.
	expected = 0;
	for (int i = 1; i <= m_isize; i++)
		expected += (gaze(i) * m_linear_vergence(i));
	expected += m_linear_vergence(m_isize+1);
}

void YARPPlanarReaching::ComputeFixationPoint (const CVisDVector& gaze, double& c1, double& c2)
{
	// project gaze onto the basis vectors.
	double p1 = gaze * m_v[0];
	double p2 = gaze * m_v[1];

	c1 = (m_beta * p1 - m_gamma * p2) / (-m_gamma * m_gamma + m_alpha * m_beta);
	c2 = (m_alpha * p2 - m_gamma * p1) / (-m_gamma * m_gamma + m_alpha * m_beta);
}

void YARPPlanarReaching::ComputeReaching (const JointPos& head, int action_no, CVisDVector& control, double noise)
{
	if (action_no >= 0)
	{
		CVisDVector gaze (m_isize);

		ForwardKinHead (head, gaze);

		// add a shift to the current gaze.
		gaze += (m_gaze_shift[action_no] * degToRad);

		// compute the reaching map (planar).
		//
		for (int comp = 0; comp < m_osize; comp++)
		{
			control(comp+1) = 0;
			for (int i = 1; i <= m_isize; i++)
				control(comp+1) += (gaze(i) * m_linear_approx[comp](i));
			control(comp+1) += m_linear_approx[comp](m_isize+1);
		}

		// add a displacement from the reaching position.
		control += (m_position_shift[action_no]);
	}
	else
	if (action_no >= -1)
	{
		CVisDVector gaze (m_isize);

		ForwardKinHead (head, gaze);
		if (noise != 0.0)
		{
			gaze(1) += (YARPRandom::Rand (-noise,noise));
			gaze(2) += (YARPRandom::Rand (-noise,noise));
		}

		// compute the reaching map (planar).
		//
		for (int comp = 0; comp < m_osize; comp++)
		{
			control(comp+1) = 0;
			for (int i = 1; i <= m_isize; i++)
				control(comp+1) += (gaze(i) * m_linear_approx[comp](i));
			control(comp+1) += m_linear_approx[comp](m_isize+1);
		}
	}
	else
	{
		control = 0;
		printf ("YARPPlanarReaching: action_no %d not recognized\n", action_no);
	}
}

void YARPPlanarReaching::SetBasis (int i, const JointPos& head, const CVisDVector& v)
{
	m_r[i] = v;

	//
	// take head and gyro info and copy into the vector.
	ForwardKinHead (head, m_g[i]);

	// store vergence in radians.
	m_vergence[i](1) = (head.j2 - head.j3) * pi / (2 * 43500);
}

void YARPPlanarReaching::GetBasis (int i, CVisDVector& gaze, CVisDVector& a, double& vergence)
{
	gaze = m_g[i];
	a = m_r[i];
	vergence = m_vergence[i](1);
}

