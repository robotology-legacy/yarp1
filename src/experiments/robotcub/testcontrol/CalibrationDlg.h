#if !defined(AFX_CALIBRATIONDLG_H__1CAA2A52_7EFB_4D9B_B7C2_FE71EBFA4B1F__INCLUDED_)
#define AFX_CALIBRATIONDLG_H__1CAA2A52_7EFB_4D9B_B7C2_FE71EBFA4B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalibrationDlg.h : header file
//

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
	CButton	m_rolltilt_ctrl;
	CButton	m_neckpan_ctrl;
	CButton	m_j4_ctrl;
	CButton	m_j3_ctrl;
	CButton	m_j2_ctrl;
	CButton	m_j1_ctrl;
	//}}AFX_DATA


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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBRATIONDLG_H__1CAA2A52_7EFB_4D9B_B7C2_FE71EBFA4B1F__INCLUDED_)
