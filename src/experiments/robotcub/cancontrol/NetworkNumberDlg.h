#if !defined(AFX_NETWORKNUMBERDLG_H__9139E366_D94C_480F_BCB8_0AC80107CFB7__INCLUDED_)
#define AFX_NETWORKNUMBERDLG_H__9139E366_D94C_480F_BCB8_0AC80107CFB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetworkNumberDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkNumberDlg dialog

class CNetworkNumberDlg : public CDialog
{
// Construction
public:
	CNetworkNumberDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNetworkNumberDlg)
	enum { IDD = IDD_DIALOG_NETNUM };
	int		m_busno;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetworkNumberDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetworkNumberDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETWORKNUMBERDLG_H__9139E366_D94C_480F_BCB8_0AC80107CFB7__INCLUDED_)
