// RecursiveLS.h: interface for the CRecursiveLS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECURSIVELS_H__FFFD9033_4555_11D4_AE76_000000000000__INCLUDED_)
#define AFX_RECURSIVELS_H__FFFD9033_4555_11D4_AE76_000000000000__INCLUDED_

#ifdef WIN32

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#endif

#ifdef __QNX__
#include <YARPSafeNew.h>
#endif

#include <VisMatrix.h>

// this is general enough if data vector are
// inserted one row at a time.
// 
class CRecursiveLS  
{
protected:
	CVisDVector m_theta;
	double m_y;
	CVisDMatrix m_psi;

	double m_lambda;

	CVisDMatrix m_p;
	CVisDMatrix m_q;

	CVisDVector m_k;

	int m_size;
	int m_steps;

private:
	// temporary stuff.
	CVisDMatrix psi_t;
	CVisDMatrix num;
	CVisDMatrix partial;
	CVisDMatrix tmp;
	CVisDVector tmpv;

public:
	CRecursiveLS(int size, double lambda);
	virtual ~CRecursiveLS();

	// initial values of P (covariance matrix)
	// initial value of theta (the parameter estimate)
	inline void SetInitialState (const CVisDMatrix& P0, const CVisDVector& T0);

	// Yt is the last measurement, psi_t is the new data point.
	// Linear regression: Yt = psi_t * theta
	inline CVisDVector Estimate (double Yt, const CVisDVector& psi_t);

	inline CVisDVector GetGain (void) const { return m_k; }
	inline int GetSteps (void) const { return m_steps; }
	inline void SetSteps (int steps) { m_steps = steps; }
	inline CVisDMatrix GetP (void) const { return m_p; }

	inline void Reset (void);
};

inline void CRecursiveLS::SetInitialState (const CVisDMatrix& P0, const CVisDVector& T0)
{
	m_theta = T0;
	m_p = P0;
}

inline CVisDVector CRecursiveLS::Estimate (double Yt, const CVisDVector& new_psi)
{
	// new psi.
	for (int i = 1; i <= m_size; i++)
	{
		m_psi (i, 1) = new_psi (i);
	}

	// compute new P.
	psi_t = m_psi.Transposed ();

	num = 0;
	double den;

	num = m_p * m_psi * psi_t * m_p;
	partial = psi_t * m_p * m_psi;
	den = m_lambda + partial (1, 1);
	num /= den;

	// compute Q first then update P. 
	m_q = m_p / den;
	m_p -= num;
	m_p /= m_lambda;

	// compute new gain K.
	tmp = m_q * m_psi;

	for (i = 1; i <= m_size; i++)
	{
		m_k (i) = tmp(i, 1);
	}

	// form prediction.
	tmpv = psi_t * m_theta;
	double y_pred = tmpv (1);

	// new theta.
	m_theta += (m_k * (Yt - y_pred));

	m_steps ++;

	return m_theta;
}

inline void CRecursiveLS::Reset (void)
{
	m_theta = 0;
	m_y = 0;
	m_psi = 0;

	m_p = 0;
	m_q = 0;

	m_k = 0;

	m_steps = 0;
}
#endif // !defined(AFX_RECURSIVELS_H__FFFD9033_4555_11D4_AE76_000000000000__INCLUDED_)
