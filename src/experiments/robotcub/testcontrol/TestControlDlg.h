// TestControlDlg.h : header file
//

#if !defined(AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_)
#define AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GainControlDlg.h"
#include "CalibrationDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTestControlDlg dialog

class CTestControlDlg : public CDialog
{
// Construction
public:
	CTestControlDlg(CWnd* pParent = NULL);	// standard constructor
	void EnableGUI (void);
	void DisableGUI (void);
	void AllocArrays (int nj);
	void FreeArrays (void);

	// variables.
	bool _headinitialized;			// whether the head control device is initialized.
	int  _headjoints;				// number of joints.
	
	double *_headstore[N_POSTURES];  
	double *_headstorev[N_POSTURES];  

	double *_headjointstore;		// temporary array.
	double *_headlastreached;		// temporary array.
	char _buffer[512];				// general purpose buffer.

	CGainControlDlg _gaincontroldlg;
	CCalibrationDlg _calibrationdlg;

// Dialog Data
	//{{AFX_DATA(CTestControlDlg)
	enum { IDD = IDD_TESTCONTROL_DIALOG };
	CButton	m_0encoders_ctrl;
	CButton	m_calibratehead_ctrl;
	CButton	m_storecurrent_ctrl;
	CEdit	m_s8_ctrl;
	CEdit	m_s7_ctrl;
	CEdit	m_s6_ctrl;
	CEdit	m_s5_ctrl;
	CEdit	m_s4_ctrl;
	CEdit	m_s3_ctrl;
	CEdit	m_s2_ctrl;
	CEdit	m_s1_ctrl;
	CEdit	m_v8_ctrl;
	CEdit	m_v7_ctrl;
	CEdit	m_v6_ctrl;
	CEdit	m_v5_ctrl;
	CEdit	m_v4_ctrl;
	CEdit	m_v3_ctrl;
	CEdit	m_v2_ctrl;
	CEdit	m_v1_ctrl;
	CEdit	m_p8_ctrl;
	CEdit	m_p7_ctrl;
	CEdit	m_p6_ctrl;
	CEdit	m_p5_ctrl;
	CEdit	m_p4_ctrl;
	CEdit	m_p3_ctrl;
	CEdit	m_p2_ctrl;
	CEdit	m_p1_ctrl;
	CButton	m_go_ctrl;
	CComboBox	m_entry_ctrl;
	CButton	m_store_ctrl;
	CButton	m_stop_ctrl;
	CButton	m_run_ctrl;
	CButton	m_idle_ctrl;
	double	m_p1;
	double	m_p2;
	double	m_p3;
	double	m_p4;
	double	m_p5;
	double	m_p6;
	double	m_p7;
	double	m_p8;
	double	m_v1;
	double	m_v2;
	double	m_v3;
	double	m_v4;
	double	m_v5;
	double	m_v6;
	double	m_v7;
	double	m_v8;
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
	afx_msg void OnInterfaceShowgain();
	afx_msg void OnInterfaceHidegain();
	afx_msg void OnButtonRun();
	afx_msg void OnButtonIdle();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonGo();
	afx_msg void OnButtonStore();
	afx_msg void OnSelendokComboEntry();
	afx_msg void OnButtonStoreCurrent();
	afx_msg void OnFileLoadpostures();
	afx_msg void OnUpdateFileLoadpostures(CCmdUI* pCmdUI);
	afx_msg void OnFileSavepostures();
	afx_msg void OnUpdateFileSavepostures(CCmdUI* pCmdUI);
	afx_msg void OnButtonCalibratehead();
	afx_msg void OnButton0encoders();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_)
