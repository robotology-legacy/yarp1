//
// YARPPlanarReaching.h
//
//

#ifndef __YARPPlanarReachingh__
#define __YARPPlanarReachingh__

#include <VisMatrix.h>

#include "CogGaze.h"

//
//
//
class YARPPlanarReaching
{
protected:
	enum { N_ACTIONS = 4 };

	int m_isize;
	int m_osize;
	int m_bases;

	CogGaze m_gaze;
	bool m_calibrated;

	CVisDVector *m_linear_approx;
	CVisDVector m_linear_vergence;

	CVisDVector m_gaze_shift[N_ACTIONS];
	CVisDVector m_position_shift[N_ACTIONS];
	CVisDVector m_pushing_values[N_ACTIONS];

	double m_alpha, m_beta, m_gamma;

	CVisDVector *m_v;			// basis vector.
	CVisDVector *m_g;			// gaze vectors.
	CVisDVector *m_vergence;	// vergence.
	CVisDVector *m_r;			// reaching vectors.

	CVisDVector m_sideposition;	// intermediate position for reaching.

public:
	YARPPlanarReaching (int inputsize, int outputsize, int numbases);
	~YARPPlanarReaching (); 

	void PRV (const CVisDVector& v);
	void BuildTemporaryVectors (void);
	void LoadCalibration (const char *filename);
	void StoreCalibration (const char *filename);

	void SetSideposition (const CVisDVector& v) { m_sideposition = v; }
	CVisDVector& GetSideposition (void) { return m_sideposition; }

	void SetBasis (int i, const JointPos& head, const CVisDVector& v);
	void GetBasis (int i, CVisDVector& gaze, CVisDVector& a, double& vergence);
	CVisDVector& GetPushingValue (int i) { return m_pushing_values[i]; }

	void ForwardKinHead (const JointPos& head, CVisDVector& out);
	void ComputeVergence (const CVisDVector& gaze, double& expected);
	void ComputeFixationPoint (const CVisDVector& gaze, double& c1, double& c2);
	void ComputeReaching (const JointPos& head, int action_no, CVisDVector& control, double noise = 0.0);
	void Uncalibrate (void) { m_calibrated = false; }
};


#endif
