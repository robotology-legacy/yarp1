#if !defined(AFX_TESTAXISDLG_H__A1A6E7C8_D318_4DCD_B0A1_615594AB2E23__INCLUDED_)
#define AFX_TESTAXISDLG_H__A1A6E7C8_D318_4DCD_B0A1_615594AB2E23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestAxisDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestAxisDlg dialog

class CTestAxisDlg : public CDialog
{
// Construction
public:
	CTestAxisDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTestAxisDlg)
	enum { IDD = IDD_TEST_DIALOG };
	CEdit	m_velocity_ctrl;
	CEdit	m_stop_ctrl;
	CEdit	m_start_ctrl;
	CEdit	m_repetitions_ctrl;
	CButton	m_stopm_ctrl;
	CButton	m_startm_ctrl;
	int		m_repetitions;
	double	m_startposition;
	double	m_stopposition;
	double	m_speed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestAxisDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_repetition;
	int m_index;

	// Generated message map functions
	//{{AFX_MSG(CTestAxisDlg)
	afx_msg void OnButtonHide();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButtonStartm();
	afx_msg void OnButtonStopm();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTAXISDLG_H__A1A6E7C8_D318_4DCD_B0A1_615594AB2E23__INCLUDED_)
