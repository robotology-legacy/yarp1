#if !defined(AFX_SEQDLG_H__B70661E9_D016_4E63_90CE_32BC9AD9E8C9__INCLUDED_)
#define AFX_SEQDLG_H__B70661E9_D016_4E63_90CE_32BC9AD9E8C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SeqDlg.h : header file
//

#include "testcontroldlg.h"

#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>

/////////////////////////////////////////////////////////////////////////////
// CSeqDlg dialog

const int SEQUENCE_LEN = 20;

class CSeqDlg : public CDialog, public YARPThread
{
// Construction
public:
	CSeqDlg(CWnd* pParent = NULL);   // standard constructor
	CTestControlDlg *px;

// Dialog Data
	//{{AFX_DATA(CSeqDlg)
	enum { IDD = IDD_DIALOG_SEQUENCE };
	//}}AFX_DATA
	int		m_s[SEQUENCE_LEN];
	int		m_delay[SEQUENCE_LEN];
	void EnableInterface (void);
	void DisableInterface (void);

	void Body (void);
	void SaveSequence (FILE *fp);
	void LoadSequence (FILE *fp);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeqDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSeqDlg)
	afx_msg void OnButtonHide();
	afx_msg void OnButtonRun();
	afx_msg void OnButtonLoop();
	afx_msg void OnButtonStopl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEQDLG_H__B70661E9_D016_4E63_90CE_32BC9AD9E8C9__INCLUDED_)
