#if !defined(AFX_TORQUESDLG_H__04820123_4F0D_11D7_8DA5_0050BA08FE00__INCLUDED_)
#define AFX_TORQUESDLG_H__04820123_4F0D_11D7_8DA5_0050BA08FE00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TorquesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTorquesDlg dialog

class CTorquesDlg : public CDialog
{
// Construction
public:
	CTorquesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTorquesDlg)
	enum { IDD = IDD_TORQUES };
	//}}AFX_DATA

	double m_t[6];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTorquesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTorquesDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TORQUESDLG_H__04820123_4F0D_11D7_8DA5_0050BA08FE00__INCLUDED_)
