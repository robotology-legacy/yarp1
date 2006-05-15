// TestControl.h : main header file for the TESTCONTROL application
//

#if !defined(AFX_TESTCONTROL_H__CE26C8B5_3A60_4AAC_9C62_2582F948E479__INCLUDED_)
#define AFX_TESTCONTROL_H__CE26C8B5_3A60_4AAC_9C62_2582F948E479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestControlApp:
// See TestControl.cpp for the implementation of this class
//

class CTestControlApp : public CWinApp
{
public:
	CTestControlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestControlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestControlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCONTROL_H__CE26C8B5_3A60_4AAC_9C62_2582F948E479__INCLUDED_)
