#if !defined(AFX_OPTIONSDLG_H__C6CC0185_C532_4F39_8D4F_E1E4818AC7D6__INCLUDED_)
#define AFX_OPTIONSDLG_H__C6CC0185_C532_4F39_8D4F_E1E4818AC7D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS };
	CString	m_NetName;
	CString	m_PortName;
	CString	m_Prefix;
	int		m_RefreshTime;
	CString	m_SavePath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	afx_msg void OnDefault();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnSetPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__C6CC0185_C532_4F39_8D4F_E1E4818AC7D6__INCLUDED_)
