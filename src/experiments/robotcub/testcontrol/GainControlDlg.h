#if !defined(AFX_GAINCONTROLDLG_H__B65754CE_C7AF_4CA7_8F68_A33C467F733A__INCLUDED_)
#define AFX_GAINCONTROLDLG_H__B65754CE_C7AF_4CA7_8F68_A33C467F733A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GainControlDlg.h : header file
//

///#include "TestControlDlg.h"
class CTestControlDlg;

/////////////////////////////////////////////////////////////////////////////
// CGainControlDlg dialog

class CGainControlDlg : public CDialog
{
// Construction
public:
	CGainControlDlg(CWnd* pParent = NULL);   // standard constructor
	void EnableInterface (void);
	void DisableInterface (void);
	void UpdateAxisParams (int bus, int axis);
	
	CTestControlDlg *m_parent;

// Dialog Data
	//{{AFX_DATA(CGainControlDlg)
	enum { IDD = IDD_DIALOG_GAIN };
	CButton	m_writeflash_bt;
	CButton	m_readflash_bt;
	CComboBox	m_bus_combo;
	CEdit	m_tlimit_edt;
	CEdit	m_shift_edt;
	CEdit	m_pgain_edt;
	CEdit	m_offset_edt;
	CEdit	m_min_edt;
	CEdit	m_max_edt;
	CEdit	m_ilimit_edt;
	CEdit	m_igain_edt;
	CEdit	m_dgain_edt;
	CComboBox	m_axis_combo;
	CButton	m_update_bt;
	CButton	m_setminmax_bt;
	double	m_dgain;
	double	m_igain;
	double	m_ilimit;
	double	m_max;
	double	m_min;
	double	m_offset;
	double	m_pgain;
	double	m_shift;
	double	m_tlimit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGainControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGainControlDlg)
	afx_msg void OnButtonHide();
	afx_msg void OnButtonUpdate();
	afx_msg void OnButtonSetminmax();
	afx_msg void OnSelendokComboAxis();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelendokComboBus();
	afx_msg void OnButtonReadflash();
	afx_msg void OnButtonWriteflash();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAINCONTROLDLG_H__B65754CE_C7AF_4CA7_8F68_A33C467F733A__INCLUDED_)
