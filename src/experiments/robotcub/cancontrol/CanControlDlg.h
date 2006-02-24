// CanControlDlg.h : header file
//

#if !defined(AFX_CANCONTROLDLG_H__5D1C232C_8C95_4FCB_A3EF_75B1DA2B3ABC__INCLUDED_)
#define AFX_CANCONTROLDLG_H__5D1C232C_8C95_4FCB_A3EF_75B1DA2B3ABC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCanControlDlg dialog

#include "CanControlParams.h"
#include "TestAxisDlg.h"

class CCanControlDlg : public CDialog
{
// Construction
public:
	CCanControlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCanControlDlg)
	enum { IDD = IDD_CANCONTROL_DIALOG };
	CButton	m_remove_filter_ctrl;
	CEdit	m_msg_filter_ctrl;
	CButton	m_filter_ctrl;
	CButton	m_spy_ctrl;
	CStatic	m_status_ctrl;
	CButton	m_flash_read_ctrl;
	CEdit	m_max_ctrl;
	CEdit	m_min_ctrl;
	CButton	m_setminmax_ctrl;
	CButton	m_prepare_ctrl;
	CButton	m_stop_ctrl;
	CEdit	m_desired_acceleration_ctrl;
	CComboBox	m_mode_ctrl;
	CEdit	m_desired_speed_ctrl;
	CEdit	m_desired_position_ctrl;
	CButton	m_go_ctrl;
	CButton	m_pwm_enable_ctrl;
	CButton	m_pwm_disable_ctrl;
	CButton	m_enable_ctrl;
	CButton	m_disable_ctrl;
	CButton	m_reset_position_ctrl;
	CEdit	m_current_position_ctrl;
	CButton	m_flash_ctrl;
	CEdit	m_tlimit_ctrl;
	CEdit	m_shift_ctrl;
	CEdit	m_offset_ctrl;
	CEdit	m_ilimit_ctrl;
	CEdit	m_igain_ctrl;
	CEdit	m_dgain_ctrl;
	CEdit	m_pgain_ctrl;
	CButton	m_update_ctrl;
	CComboBox	m_axis_ctrl;
	int		m_axis;
	double	m_pgain;
	double	m_dgain;
	double	m_igain;
	double	m_ilimit;
	double	m_offset;
	double	m_shift;
	double	m_tlimit;
	double	m_desired_position;
	double	m_desired_speed;
	double	m_desired_acceleration;
	double	m_min_position;
	double	m_max_position;
	int		m_msg_filter;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCanControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

public:
/// my stuff.
#ifndef __ESD_DRIVER
	YARPValueCanDeviceDriver	m_driver;
	ValueCanOpenParameters		m_params;
#else
	YARPEsdCanDeviceDriver		m_driver;
	EsdCanOpenParameters		m_params;
#endif

	int							m_njoints;
	unsigned char				m_destinations[CANBUS_MAXCARDS];
	double						m_current_limits[CANBUS_MAXCARDS * 2];
	bool						m_driverok;

	void ActivateGUI ();
	void DeactivateGUI ();
	void UpdateAxisParams (int axis);

	char						m_buffer[256];
	double						m_vmove[CANBUS_MAXCARDS*2];
	double						m_current_displayed_position;

	CTestAxisDlg				m_testaxisdlg;

protected:
	// Generated message map functions
	//{{AFX_MSG(CCanControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHelpAbout();
	afx_msg void OnClose();
	afx_msg void OnFileExit();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnDriverRun();
	afx_msg void OnDriverKill();
	afx_msg void OnParametersAddressofcards();
	afx_msg void OnParametersNumberofcards();
	afx_msg void OnSelendokComboAxis();
	afx_msg void OnUpdateDriverKill(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDriverRun(CCmdUI* pCmdUI);
	afx_msg void OnDriverSendaddress();
	afx_msg void OnUpdateDriverSendaddress(CCmdUI* pCmdUI);
	afx_msg void OnButtonUpdate();
	afx_msg void OnFlash();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonReset();
	afx_msg void OnButtonDisable();
	afx_msg void OnButtonEnable();
	afx_msg void OnButtonPwmDis();
	afx_msg void OnButtonPwmEn();
	afx_msg void OnButtonGo();
	afx_msg void OnSelendokComboMode();
	afx_msg void OnButtonStop();
	afx_msg void OnUpdateParametersAddressofcards(CCmdUI* pCmdUI);
	afx_msg void OnUpdateParametersNumberofcards(CCmdUI* pCmdUI);
	afx_msg void OnButtonPrepare();
	afx_msg void OnButtonSetminmax();
	afx_msg void OnFlashRead();
	afx_msg void OnButtonSpy();
	afx_msg void OnFileOpenconsole();
	afx_msg void OnUpdateFileOpenconsole(CCmdUI* pCmdUI);
	afx_msg void OnFileCloseconsole();
	afx_msg void OnUpdateFileCloseconsole(CCmdUI* pCmdUI);
	afx_msg void OnButtonFilter();
	afx_msg void OnButtonRemoveFilter();
	afx_msg void OnParametersTestaxis();
	afx_msg void OnUpdateParametersTestaxis(CCmdUI* pCmdUI);
	afx_msg void OnFileLoadconfiguration();
	afx_msg void OnFileSaveconfiguration();
	afx_msg void OnUpdateFileLoadconfiguration(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveconfiguration(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANCONTROLDLG_H__5D1C232C_8C95_4FCB_A3EF_75B1DA2B3ABC__INCLUDED_)
