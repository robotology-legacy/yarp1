// TwoDKalmanFilter.cpp: implementation of the YARPTwoDKalmanFilter class.
//
//////////////////////////////////////////////////////////////////////
 
#include "YARPTwoDKalmanFilter.h"

// $Id: YARPTwoDKalmanFilter.cpp,v 1.1 2003-06-27 13:17:41 babybot Exp $

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPTwoDKalmanFilter::YARPTwoDKalmanFilter()
{
	m_theta.Resize (4);
	m_psi.Resize (4, 1);

	m_theta = 0;
	m_y = 0;
	m_psi = 0;

	m_r1.Resize (4, 4);
	m_r1 = 0;
	m_r2 = 0;

	m_p.Resize (4, 4);
	m_q.Resize (4, 4);
	m_p = 0;
	m_q = 0;

	m_k.Resize (4);
	m_k = 0;

	// tmp stuff.
	psi_t.Resize (1, 4);
	num.Resize (4, 4);
	partial.Resize (1, 1);
	tmp.Resize (4, 1);
	tmpv.Resize (1);
}

YARPTwoDKalmanFilter::~YARPTwoDKalmanFilter()
{

}
