#if !defined(AFX_FINGERSDLG_H__7CBA4A41_76FB_47D0_9FA0_744101FD2A1C__INCLUDED_)
#define AFX_FINGERSDLG_H__7CBA4A41_76FB_47D0_9FA0_744101FD2A1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FingersDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFingersDlg dialog

class CFingersDlg : public CDialog
{
// Construction
public:
	CFingersDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFingersDlg)
	enum { IDD = IDD_FINGERSDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_pos[15];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFingersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFingersDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINGERSDLG_H__7CBA4A41_76FB_47D0_9FA0_744101FD2A1C__INCLUDED_)
