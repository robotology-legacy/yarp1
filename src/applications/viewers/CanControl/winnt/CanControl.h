// CanControl.h : main header file for the CANCONTROL application
//

#if !defined(AFX_CANCONTROL_H__818E0F22_508B_4A3B_A079_CDDF7DD79794__INCLUDED_)
#define AFX_CANCONTROL_H__818E0F22_508B_4A3B_A079_CDDF7DD79794__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCanControlApp:
// See CanControl.cpp for the implementation of this class
//

class CCanControlApp : public CWinApp
{
public:
	CCanControlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCanControlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCanControlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANCONTROL_H__818E0F22_508B_4A3B_A079_CDDF7DD79794__INCLUDED_)
