// CanControlDlg.h : header file
//

#if !defined(AFX_CANCONTROLDLG_H__5D1C232C_8C95_4FCB_A3EF_75B1DA2B3ABC__INCLUDED_)
#define AFX_CANCONTROLDLG_H__5D1C232C_8C95_4FCB_A3EF_75B1DA2B3ABC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCanControlDlg dialog

class CCanControlDlg : public CDialog
{
// Construction
public:
	CCanControlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCanControlDlg)
	enum { IDD = IDD_CANCONTROL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCanControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCanControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANCONTROLDLG_H__5D1C232C_8C95_4FCB_A3EF_75B1DA2B3ABC__INCLUDED_)
