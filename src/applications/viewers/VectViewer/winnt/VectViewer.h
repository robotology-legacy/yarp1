// VectViewer.h : main header file for the VECTVIEWER application
//

#if !defined(AFX_VECTVIEWER_H__31E67942_696C_4147_BC09_259E3A33B204__INCLUDED_)
#define AFX_VECTVIEWER_H__31E67942_696C_4147_BC09_259E3A33B204__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVectViewerApp:
// See VectViewer.cpp for the implementation of this class
//

class CVectViewerApp : public CWinApp
{
public:
	CVectViewerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVectViewerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int _posX;
	int _posY;
	int _height;
	int _width;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VECTVIEWER_H__31E67942_696C_4147_BC09_259E3A33B204__INCLUDED_)
