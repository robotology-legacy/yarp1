// camview.h : main header file for the CAMVIEW application
//

#if !defined(AFX_CAMVIEW_H__754999A4_631C_4C12_B65C_A6FECC79FBB7__INCLUDED_)
#define AFX_CAMVIEW_H__754999A4_631C_4C12_B65C_A6FECC79FBB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCamviewApp:
// See camview.cpp for the implementation of this class
//

class CCamviewApp : public CWinApp
{
public:
	CCamviewApp();
	CString m_portname;
	CString m_netname;
	CString m_out_portname;
	CString m_out_netname;
	int m_period;
	int m_x;
	int m_y;
	bool m_lp;
	bool m_fov;
	bool m_enable_output;
	bool m_horiz_flip;
	int m_width;
	int m_height;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCamviewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCamviewApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMVIEW_H__754999A4_631C_4C12_B65C_A6FECC79FBB7__INCLUDED_)
