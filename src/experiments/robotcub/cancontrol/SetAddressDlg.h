#if !defined(AFX_SETADDRESSDLG_H__3354CFD4_D9A0_4005_956D_3FEF160169C9__INCLUDED_)
#define AFX_SETADDRESSDLG_H__3354CFD4_D9A0_4005_956D_3FEF160169C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetAddressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetAddressDlg dialog

class CSetAddressDlg : public CDialog
{
// Construction
public:
	CSetAddressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetAddressDlg)
	enum { IDD = IDD_SETADDR_DIALOG };
	int		m_address;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetAddressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetAddressDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETADDRESSDLG_H__3354CFD4_D9A0_4005_956D_3FEF160169C9__INCLUDED_)
