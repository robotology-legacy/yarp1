// TestControlDlg.h : header file
//

#if !defined(AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_)
#define AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestControlDlg dialog

class CTestControlDlg : public CDialog
{
// Construction
public:
	CTestControlDlg(CWnd* pParent = NULL);	// standard constructor
	void EnableGUI (void);
	void DisableGUI (void);
	bool _headinitialized;

// Dialog Data
	//{{AFX_DATA(CTestControlDlg)
	enum { IDD = IDD_TESTCONTROL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHelpAbout();
	afx_msg void OnFileExit();
	afx_msg void OnClose();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnInterfaceStart();
	afx_msg void OnInterfaceStop();
	afx_msg void OnUpdateInterfaceStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInterfaceStop(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenconsole();
	afx_msg void OnUpdateFileOpenconsole(CCmdUI* pCmdUI);
	afx_msg void OnFileCloseconsole();
	afx_msg void OnUpdateFileCloseconsole(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_)
