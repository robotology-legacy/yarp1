// HandDemoDlg.h : header file
//

#if !defined(AFX_HANDDEMODLG_H__90A4CD0D_3D9F_4FB4_B207_E24DEC52B052__INCLUDED_)
#define AFX_HANDDEMODLG_H__90A4CD0D_3D9F_4FB4_B207_E24DEC52B052__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HandThread.h"
#include "DisplayDlg.h"
#include "TorquesDlg.h"
#include "FingersDlg.h"

const int __hand_rate = 40;

/////////////////////////////////////////////////////////////////////////////
// CHandDemoDlg dialog

class CHandDemoDlg : public CDialog
{
// Construction
public:
	CHandDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHandDemoDlg)
	enum { IDD = IDD_HANDDEMO_DIALOG };
	int		m_pos1;
	int		m_pos2;
	int		m_pos3;
	int		m_pos4;
	int		m_pos5;
	int		m_pos6;
	double	m_gain_prop1;
	double	m_gain_prop2;
	double	m_gain_prop3;
	double	m_gain_prop4;
	double	m_gain_prop5;
	double	m_gain_prop6;
	int		m_acc1;
	int		m_acc2;
	int		m_acc3;
	int		m_acc4;
	int		m_acc5;
	int		m_acc6;
	int		m_sp1;
	int		m_sp2;
	int		m_sp3;
	int		m_sp5;
	int		m_sp4;
	int		m_sp6;
	double		m_gain_der1;
	double		m_gain_der2;
	double		m_gain_der3;
	double		m_gain_der4;
	double		m_gain_der5;
	double		m_gain_der6;
	double		m_gain_int1;
	double		m_gain_int2;
	double		m_gain_int3;
	double		m_gain_int4;
	double		m_gain_int5;
	double		m_gain_int6;
	int		m_joint;
	int		m_hall;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHandDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	HandThread _hand_thread;
	CDisplayDlg _display_dlg;
	CTorquesDlg	_torques_dlg;
	CFingersDlg	_fingers_dlg;
	UINT	_timer;

	double _errors[6];
	double _ref_pos[6];

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHandDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	afx_msg void OnGo();
	virtual void OnOK();
	afx_msg void OnReset();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStop();
	afx_msg void OnUpdategains();
	afx_msg void OnUpdatespeeds();
	afx_msg void OnUpdatetransmission();
	afx_msg void OnVmove();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	double command[6];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HANDDEMODLG_H__90A4CD0D_3D9F_4FB4_B207_E24DEC52B052__INCLUDED_)
