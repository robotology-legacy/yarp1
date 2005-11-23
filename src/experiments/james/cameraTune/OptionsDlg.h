#if !defined(AFX_OPTIONSDLG_H__B219DB75_CE21_4C4F_89D9_461B87BC8B5F__INCLUDED_)
#define AFX_OPTIONSDLG_H__B219DB75_CE21_4C4F_89D9_461B87BC8B5F__INCLUDED_

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
	BOOL	m_AutoSave;
	CString	m_FileName;
	int		m_BoardN;
	int		m_SizeX;
	int		m_SizeY;
	int		m_Rho;
	int		m_Theta;
	int		m_RefreshTime;
	int		m_BufferSize;
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__B219DB75_CE21_4C4F_89D9_461B87BC8B5F__INCLUDED_)
