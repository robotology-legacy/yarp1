// RecursiveLS.cpp: implementation of the YARPRecursiveLS class.
//
//////////////////////////////////////////////////////////////////////

// $Id: YARPRecursiveLS.cpp,v 1.1 2003-11-10 19:08:57 babybot Exp $

#include "YARPRecursiveLS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

YARPRecursiveLS::YARPRecursiveLS(int size, double lambda)
{
	m_size = size;
	m_lambda = lambda;

	m_theta.Resize (m_size);
	m_psi.Resize (m_size, 1);

	m_theta = 0;
	m_y = 0;
	m_psi = 0;

	m_p.Resize (m_size, m_size);
	m_q.Resize (m_size, m_size);
	m_p = 0;
	m_q = 0;

	m_k.Resize (m_size);
	m_k = 0;

	// tmp stuff.
	psi_t.Resize (1, m_size);
	num.Resize (m_size, m_size);
	partial.Resize (1, 1);
	tmp.Resize (m_size, 1);
	tmpv.Resize (1);

	m_steps = 0;
}

YARPRecursiveLS::~YARPRecursiveLS()
{

}
