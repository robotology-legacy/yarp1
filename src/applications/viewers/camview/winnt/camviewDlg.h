// camviewDlg.h : header file
//

#if !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
#define AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCamviewDlg dialog

class CCamviewDlg : public CDialog
{
// Construction
public:
	CCamviewDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCamviewDlg)
	enum { IDD = IDD_CAMVIEW_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCamviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCamviewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
