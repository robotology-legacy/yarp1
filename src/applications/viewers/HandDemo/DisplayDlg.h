#if !defined(AFX_DISPLAYDLG_H__D9300415_3E00_4358_A9CE_7ED329C0BE73__INCLUDED_)
#define AFX_DISPLAYDLG_H__D9300415_3E00_4358_A9CE_7ED329C0BE73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisplayDlg dialog

class CDisplayDlg : public CDialog
{
// Construction
public:
	CDisplayDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisplayDlg)
	enum { IDD = IDD_JOINTS };
	BOOL	m_inmotion1;
	BOOL	m_inmotion2;
	BOOL	m_inmotion3;
	BOOL	m_inmotion4;
	BOOL	m_inmotion5;
	BOOL	m_inmotion6;
	int		m_input;
	//}}AFX_DATA

	double	m_pos[6];
	double	m_sp[6];
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisplayDlg)
	afx_msg void OnHide();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYDLG_H__D9300415_3E00_4358_A9CE_7ED329C0BE73__INCLUDED_)
