#if !defined(AFX_LIVECAMERADLG_H__26ADB3DB_7670_4FCE_90EF_5628162969CA__INCLUDED_)
#define AFX_LIVECAMERADLG_H__26ADB3DB_7670_4FCE_90EF_5628162969CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LiveCameraDlg.h : header file
//

#include "DIBConverter.h"

/////////////////////////////////////////////////////////////////////////////
// CLiveCameraDlg dialog

class CLiveCameraDlg : public CDialog
{
// Construction
public:
	CLiveCameraDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLiveCameraDlg)
	enum { IDD = IDD_CAMERA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLiveCameraDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLiveCameraDlg)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int sizeY;
	int sizeX;
	void UpdateState(YARPGenericImage *img);
	void CopyToScreen(HDRAWDIB hDD, HDC hDC, unsigned char *img, int destX, int destY, double zoomX, double zoomY);
	YARPDIBConverter m_converter;
	HDRAWDIB m_drawdib;
	YARPGenericImage *pImage;
	CRect m_rect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVECAMERADLG_H__26ADB3DB_7670_4FCE_90EF_5628162969CA__INCLUDED_)
