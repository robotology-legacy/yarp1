// TwoDKalmanFilter.h: interface for the CTwoDKalmanFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TWODKALMANFILTER_H__7E923E13_3F1A_11D4_AE63_000000000000__INCLUDED_)
#define AFX_TWODKALMANFILTER_H__7E923E13_3F1A_11D4_AE63_000000000000__INCLUDED_

#ifdef WIN32

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#endif

#ifdef __QNX__
#include <YARPSafeNew.h>
#endif

#include <VisMatrix.h>


class CTwoDKalmanFilter  
{
protected:
	CVisDVector m_theta;
	double m_y;
	CVisDMatrix m_psi;

	CVisDMatrix m_r1;
	double m_r2;

	CVisDMatrix m_p;
	CVisDMatrix m_q;

	CVisDVector m_k;

private:
	// temporary stuff.
	CVisDMatrix psi_t;
	CVisDMatrix num;
	CVisDMatrix partial;
	CVisDMatrix tmp;
	CVisDVector tmpv;

public:
	CTwoDKalmanFilter();
	virtual ~CTwoDKalmanFilter();

	// R1 is the covariance of the noise (i.e. the system noise)
	// R2 is the variance of the noise in the model (the linear model)
	inline void SetVariance (const CVisDMatrix& R1, const double R2);

	// initial values of P (covariance matrix)
	// initial value of theta (the parameter estimate)
	inline void SetInitialState (const CVisDMatrix& P0, const CVisDVector& T0);

	// Yt is the last measurement, psi_t is the new data point.
	// Linear regression: Yt = psi_t * theta
	inline CVisDVector Kalman (double Yt, const CVisDVector& psi_t);

	inline CVisDVector GetGain (void) const { return m_k; }
};

inline void CTwoDKalmanFilter::SetVariance (const CVisDMatrix& R1, const double R2)
{
	m_r1 = R1;
	m_r2 = R2;
}

inline void CTwoDKalmanFilter::SetInitialState (const CVisDMatrix& P0, const CVisDVector& T0)
{
	m_theta = T0;
	m_p = P0;
}

inline CVisDVector CTwoDKalmanFilter::Kalman (double Yt, const CVisDVector& new_psi)
{
	// new psi.
	m_psi (1, 1) = new_psi (1);
	m_psi (2, 1) = new_psi (2);
	m_psi (3, 1) = new_psi (3);
	m_psi (4, 1) = new_psi (4);

	// compute new P.
	psi_t = m_psi.Transposed ();

	num = 0;
	double den;

	num = m_p * m_psi * psi_t * m_p;
	partial = psi_t * m_p * m_psi;
	den = m_r2 + partial (1, 1);
	num /= den;

	// compute Q first then update P. 
	m_q = m_p / den;
	m_p += (m_r1 - num);
	
	// compute new gain K.
	tmp = m_q * m_psi;
	m_k (1) = tmp(1, 1);
	m_k (2) = tmp(2, 1);
	m_k (3) = tmp(3, 1);
	m_k (4) = tmp(4, 1);

	// form prediction.
	tmpv = psi_t * m_theta;
	double y_pred = tmpv (1);

	// new theta.
	m_theta += (m_k * (Yt - y_pred));

	return m_theta;
}

#endif // !defined(AFX_TWODKALMANFILTER_H__7E923E13_3F1A_11D4_AE63_000000000000__INCLUDED_)
