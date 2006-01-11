// TestControlDlg.h : header file
//

#if !defined(AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_)
#define AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GainControlDlg.h"
#include "CalibrationDlg.h"
#include "SeqDlg.h"
#include "TouchDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTestControlDlg dialog


// since it's a constant anyway.
#define MAX_ARM_JNTS 15
#define MAX_HEAD_JNTS 8


class CTestControlDlg : public CDialog
{
// Construction
public:
	CTestControlDlg(CWnd* pParent = NULL);	// standard constructor
	void EnableGUI (void);
	void DisableGUI (void);
	void AllocHeadArrays (int nj);
	void FreeHeadArrays (void);
	void AllocArmArrays (int nj);
	void FreeArmArrays (void);

	// variables.
	bool _headinitialized;			// whether the head control device is initialized.
	bool _arminitialized;			// same for the arm/hand.
	bool _touchinitialized;

	bool _headrunning;				// whether the PID controller is ON.
	bool _armrunning;
	bool _touchrunning;

	double *_headstore[N_POSTURES];  
	double *_headstorev[N_POSTURES];  

	double *_armstore[N_POSTURES];  
	double *_armstorev[N_POSTURES];  

	double *_headjointstore;		// temporary arrays.
	double *_headlastreached;
	double *_armjointstore;
	double *_armlastreached;
	char _buffer[512];				// general purpose buffer.

	CGainControlDlg _gaincontroldlg;
	CCalibrationDlg _calibrationdlg;
	CSeqDlg			_sequencedlg;
	CTouchDlg		_touchdlg;

	// my hack!
	CEdit	m_sa_ctrl[MAX_ARM_JNTS];
	CEdit	m_va_ctrl[MAX_ARM_JNTS];
	CEdit	m_pa_ctrl[MAX_ARM_JNTS];
	double	m_pa[MAX_ARM_JNTS];
	double	m_va[MAX_ARM_JNTS];

	CEdit	m_s_ctrl[MAX_HEAD_JNTS];
	CEdit	m_v_ctrl[MAX_HEAD_JNTS];
	CEdit	m_p_ctrl[MAX_HEAD_JNTS];
	double	m_p[MAX_HEAD_JNTS];
	double  m_v[MAX_HEAD_JNTS];

// Dialog Data
	//{{AFX_DATA(CTestControlDlg)
	enum { IDD = IDD_TESTCONTROL_DIALOG };
	CComboBox	m_entry_all_ctrl;
	CButton	m_goall_ctrl;
	CComboBox	m_entry_ctrl_arm;
	CButton	m_storecurrent_ctrl_arm;
	CButton	m_store_ctrl_arm;
	CButton	m_stop_ctrl_arm;
	CButton	m_run_ctrl_arm;
	CButton	m_idle_ctrl_arm;
	CButton	m_go_ctrl_arm;
	CButton	m_0encoders_ctrl_arm;
	CButton	m_0encoders_ctrl;
	CButton	m_calibratehead_ctrl;
	CButton	m_storecurrent_ctrl;
	CButton	m_go_ctrl;
	CComboBox	m_entry_ctrl;
	CButton	m_store_ctrl;
	CButton	m_stop_ctrl;
	CButton	m_run_ctrl;
	CButton	m_idle_ctrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHelpAbout();
	afx_msg void OnFileExit();
	afx_msg void OnClose();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnInterfaceStart();
	afx_msg void OnInterfaceStop();
	afx_msg void OnUpdateInterfaceStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInterfaceStop(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenconsole();
	afx_msg void OnUpdateFileOpenconsole(CCmdUI* pCmdUI);
	afx_msg void OnFileCloseconsole();
	afx_msg void OnUpdateFileCloseconsole(CCmdUI* pCmdUI);
	afx_msg void OnInterfaceShowgain();
	afx_msg void OnInterfaceHidegain();
	afx_msg void OnButtonRun();
	afx_msg void OnButtonIdle();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonGo();
	afx_msg void OnButtonStore();
	afx_msg void OnSelendokComboEntry();
	afx_msg void OnButtonStoreCurrent();
	afx_msg void OnFileLoadpostures();
	afx_msg void OnUpdateFileLoadpostures(CCmdUI* pCmdUI);
	afx_msg void OnFileSavepostures();
	afx_msg void OnUpdateFileSavepostures(CCmdUI* pCmdUI);
	afx_msg void OnButtonCalibratehead();
	afx_msg void OnButton0encoders();
	afx_msg void OnButton0encodersArm();
	afx_msg void OnButtonGoArm();
	afx_msg void OnButtonIdleArm();
	afx_msg void OnButtonRunArm();
	afx_msg void OnButtonStopArm();
	afx_msg void OnButtonStoreArm();
	afx_msg void OnButtonStoreCurrentArm();
	afx_msg void OnSelendokComboEntryArm();
	afx_msg void OnButtonAll();
	afx_msg void OnSelendokComboEntryAll();
	afx_msg void OnPosturesSetsequence();
	afx_msg void OnUpdatePosturesSetsequence(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadsequence();
	afx_msg void OnFileSavesequence();
	afx_msg void OnUpdateFileLoadsequence(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSavesequence(CCmdUI* pCmdUI);
	afx_msg void OnInterfaceHidehalleffect();
	afx_msg void OnUpdateInterfaceHidehalleffect(CCmdUI* pCmdUI);
	afx_msg void OnInterfaceShowhalleffect();
	afx_msg void OnUpdateInterfaceShowhalleffect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInterfaceHidegain(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInterfaceShowgain(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTCONTROLDLG_H__D7CA13D6_D930_46CA_886A_A410FDCB5806__INCLUDED_)
