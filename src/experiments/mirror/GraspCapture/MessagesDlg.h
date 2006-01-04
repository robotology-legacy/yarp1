#if !defined(AFX_MESSAGESDLG_H__A20DC43A_FF6D_4A01_A1CF_8DF2718A49CB__INCLUDED_)
#define AFX_MESSAGESDLG_H__A20DC43A_FF6D_4A01_A1CF_8DF2718A49CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessagesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessagesDlg dialog

class CMessagesDlg : public CDialog
{
// Construction
public:
	CMessagesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMessagesDlg)
	enum { IDD = IDD_MESSAGES };
	CEdit	m_MessageBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessagesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessagesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFrequency();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGESDLG_H__A20DC43A_FF6D_4A01_A1CF_8DF2718A49CB__INCLUDED_)
