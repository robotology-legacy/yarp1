// Simple Viewer.h : main header file for the SIMPLE VIEWER application
//

#if !defined(AFX_SIMPLEVIEWER_H__3498A76E_D4D0_4418_870D_2BA893A08DA3__INCLUDED_)
#define AFX_SIMPLEVIEWER_H__3498A76E_D4D0_4418_870D_2BA893A08DA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimpleViewerApp:
// See Simple Viewer.cpp for the implementation of this class
//

class CSimpleViewerApp : public CWinApp
{
public:
	CSimpleViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSimpleViewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEVIEWER_H__3498A76E_D4D0_4418_870D_2BA893A08DA3__INCLUDED_)
