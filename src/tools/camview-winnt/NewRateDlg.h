#if !defined(AFX_NEWRATEDLG_H__9E373195_DD90_4828_BF40_32BD2A55F037__INCLUDED_)
#define AFX_NEWRATEDLG_H__9E373195_DD90_4828_BF40_32BD2A55F037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewRateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewRateDlg dialog

class CNewRateDlg : public CDialog
{
// Construction
public:
	CNewRateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewRateDlg)
	enum { IDD = IDD_NEWRATE_DIALOG };
	int		m_period;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewRateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewRateDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWRATEDLG_H__9E373195_DD90_4828_BF40_32BD2A55F037__INCLUDED_)
