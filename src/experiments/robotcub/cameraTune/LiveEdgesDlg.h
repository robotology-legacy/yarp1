#if !defined(AFX_LIVEEDGESDLG_H__D4423737_3B75_4D69_80A8_B656CBF6B76F__INCLUDED_)
#define AFX_LIVEEDGESDLG_H__D4423737_3B75_4D69_80A8_B656CBF6B76F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LiveEdgesDlg.h : header file
//

#include "DIBConverter.h"

/////////////////////////////////////////////////////////////////////////////
// CLiveEdgesDlg dialog

class CLiveEdgesDlg : public CDialog
{
// Construction
public:
	void UpdateState(YARPImageOf<YarpPixelBGR> *p_img);
	CLiveEdgesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLiveEdgesDlg)
	enum { IDD = IDD_EDGES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLiveEdgesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLiveEdgesDlg)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void CopyToScreen(HDRAWDIB hDD, HDC hDC, unsigned char *img, int destX, int destY, double zoomX, double zoomY);
	int sizeY;
	int sizeX;
	YARPDIBConverter m_converter;
	HDRAWDIB m_drawdib;
	YARPGenericImage *pImage;
	YARPSusanFilter flt;
	YARPImageOf<YarpPixelMono> edgeImg;
	CRect m_rect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVEEDGESDLG_H__D4423737_3B75_4D69_80A8_B656CBF6B76F__INCLUDED_)
