// HandDemo.h : main header file for the HANDDEMO application
//

#if !defined(AFX_HANDDEMO_H__8A2D44A0_D2DB_4A6B_8386_CC44B47A4FE5__INCLUDED_)
#define AFX_HANDDEMO_H__8A2D44A0_D2DB_4A6B_8386_CC44B47A4FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHandDemoApp:
// See HandDemo.cpp for the implementation of this class
//

class CHandDemoApp : public CWinApp
{
public:
	CHandDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHandDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHandDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HANDDEMO_H__8A2D44A0_D2DB_4A6B_8386_CC44B47A4FE5__INCLUDED_)
