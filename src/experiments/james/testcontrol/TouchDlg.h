#if !defined(AFX_TOUCHDLG_H__F70A7AC7_6FF6_42AB_A980_CDB058DB6738__INCLUDED_)
#define AFX_TOUCHDLG_H__F70A7AC7_6FF6_42AB_A980_CDB058DB6738__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TouchDlg.h : header file
//

// max mph analog channels.
const int MAX_CHANNELS = 32;

/////////////////////////////////////////////////////////////////////////////
// CTouchDlg dialog

class CTouchDlg : public CDialog
{
// Construction
public:
	CTouchDlg(CWnd* pParent = NULL);   // standard constructor
	void EnableInterface ();
	void DisableInterface ();
	void UpdateInterface ();
	void SetMask (int newmask);


// Dialog Data
	//{{AFX_DATA(CTouchDlg)
	enum { IDD = IDD_DIALOG_TOUCH };
	//}}AFX_DATA

	int		m_mask;

	int		m_count;
	CEdit	m_edt[MAX_CHANNELS];
	short	m_c[MAX_CHANNELS];
	short   m_tmp[MAX_CHANNELS];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTouchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTouchDlg)
	afx_msg void OnButtonHide();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOUCHDLG_H__F70A7AC7_6FF6_42AB_A980_CDB058DB6738__INCLUDED_)
