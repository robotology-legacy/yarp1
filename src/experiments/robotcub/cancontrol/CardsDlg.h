#if !defined(AFX_CARDSDLG_H__03E69796_1DB5_4FD7_BAB7_90691E9810CC__INCLUDED_)
#define AFX_CARDSDLG_H__03E69796_1DB5_4FD7_BAB7_90691E9810CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CardsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsDlg dialog

class CCardsDlg : public CDialog
{
// Construction
public:
	CCardsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCardsDlg)
	enum { IDD = IDD_CARDS_DIALOG };
	int		m_cards;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARDSDLG_H__03E69796_1DB5_4FD7_BAB7_90691E9810CC__INCLUDED_)
