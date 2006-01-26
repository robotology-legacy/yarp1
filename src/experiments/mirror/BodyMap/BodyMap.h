// BodyMap.h : main header file for the BodyMap application
//

#if !defined(AFX_BodyMap_H__99B381EA_5725_492E_ACE4_4A9BABC2F469__INCLUDED_)
#define AFX_BodyMap_H__99B381EA_5725_492E_ACE4_4A9BABC2F469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

void FindTrackerXY(YARPImageOf<YarpPixelBGR>&, int*, int*, int*);

/////////////////////////////////////////////////////////////////////////////
// CBodyMapApp:
// See BodyMap.cpp for the implementation of this class
//

class CBodyMapApp : public CWinApp
{
public:
	CBodyMapApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBodyMapApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBodyMapApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BodyMap_H__99B381EA_5725_492E_ACE4_4A9BABC2F469__INCLUDED_)
