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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

// $Id: YARPTwoDKalmanFilter.h,v 1.1 2004-07-29 13:09:14 babybot Exp $

#if !defined __TWODKALMANFILTERH__
#define __TWODKALMANFILTERH__

#include <yarp/YARPRobotMath.h>

class YARPTwoDKalmanFilter  
{
protected:
	YVector m_theta;
	double m_y;
	YMatrix m_psi;

	YMatrix m_r1;
	double m_r2;

	YMatrix m_p;
	YMatrix m_q;

	YVector m_k;

private:
	// temporary stuff.
	YMatrix psi_t;
	YMatrix num;
	YMatrix partial;
	YMatrix tmp;
	YVector tmpv;

public:
	YARPTwoDKalmanFilter();
	virtual ~YARPTwoDKalmanFilter();

	// R1 is the covariance of the noise (i.e. the system noise)
	// R2 is the variance of the noise in the model (the linear model)
	inline void SetVariance (const YMatrix& R1, const double R2);

	// initial values of P (covariance matrix)
	// initial value of theta (the parameter estimate)
	inline void SetInitialState (const YMatrix& P0, const YVector& T0);

	// Yt is the last measurement, psi_t is the new data point.
	// Linear regression: Yt = psi_t * theta
	inline YVector Kalman (double Yt, const YVector& psi_t);

	inline YVector GetGain (void) const { return m_k; }
};

inline void YARPTwoDKalmanFilter::SetVariance (const YMatrix& R1, const double R2)
{
	m_r1 = R1;
	m_r2 = R2;
}

inline void YARPTwoDKalmanFilter::SetInitialState (const YMatrix& P0, const YVector& T0)
{
	m_theta = T0;
	m_p = P0;
}

inline YVector YARPTwoDKalmanFilter::Kalman (double Yt, const YVector& new_psi)
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
