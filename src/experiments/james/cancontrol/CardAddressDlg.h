#if !defined(AFX_CARDADDRESSDLG_H__ADA8C80E_960B_426C_A5CF_694F98327853__INCLUDED_)
#define AFX_CARDADDRESSDLG_H__ADA8C80E_960B_426C_A5CF_694F98327853__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CardAddressDlg.h : header file
//

#include "CanControlParams.h"

/////////////////////////////////////////////////////////////////////////////
// CCardAddressDlg dialog

class CCardAddressDlg : public CDialog
{
// Construction
public:
	CCardAddressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCardAddressDlg)
	enum { IDD = IDD_ADDRESS_DIALOG };
	//}}AFX_DATA

	int		m_address[CANBUS_MAXCARDS];
	BOOL	m_disabled[CANBUS_MAXCARDS];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardAddressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardAddressDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARDADDRESSDLG_H__ADA8C80E_960B_426C_A5CF_694F98327853__INCLUDED_)
