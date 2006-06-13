#if !defined(AFX_CALIBRATIONDLG_H__1CAA2A52_7EFB_4D9B_B7C2_FE71EBFA4B1F__INCLUDED_)
#define AFX_CALIBRATIONDLG_H__1CAA2A52_7EFB_4D9B_B7C2_FE71EBFA4B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalibrationDlg.h : header file
//

#ifndef MAX_ARM_JNTS
#define MAX_ARM_JNTS 15
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalibrationDlg dialog

class CCalibrationDlg : public CDialog
{
// Construction
public:
	CCalibrationDlg(CWnd* pParent = NULL);   // standard constructor
	void EnableInterface ();
	void DisableInterface ();

// Dialog Data
	//{{AFX_DATA(CCalibrationDlg)
	enum { IDD = IDD_DIALOG_CALIBRATION };
	CButton	m_button_acal;
	CButton	m_button_hcal;
	CButton	m_rolltilt_ctrl;
	CButton	m_neckpan_ctrl;
	CButton	m_j4_ctrl;
	CButton	m_j3_ctrl;
	CButton	m_j2_ctrl;
	CButton	m_j1_ctrl;
	//}}AFX_DATA
	CButton	m_button_aj[MAX_ARM_JNTS];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalibrationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalibrationDlg)
	afx_msg void OnButtonHide();
	afx_msg void OnButtonJ1();
	afx_msg void OnButtonJ2();
	afx_msg void OnButtonJ3();
	afx_msg void OnButtonJ4();
	afx_msg void OnButtonAJ0();
	afx_msg void OnButtonAJ1();
	afx_msg void OnButtonAJ2();
	afx_msg void OnButtonAJ3();
	afx_msg void OnButtonAJ4();
	afx_msg void OnButtonAJ5();
	afx_msg void OnButtonAJ6();
	afx_msg void OnButtonAJ7();
	afx_msg void OnButtonAJ8();
	afx_msg void OnButtonAJ9();
	afx_msg void OnButtonAJ10();
	afx_msg void OnButtonAJ11();
	afx_msg void OnButtonAJ12();
	afx_msg void OnButtonAJ13();
	afx_msg void OnButtonAJ14();
	afx_msg void OnButtonACal();
	afx_msg void OnButtonHCal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBRATIONDLG_H__1CAA2A52_7EFB_4D9B_B7C2_FE71EBFA4B1F__INCLUDED_)
