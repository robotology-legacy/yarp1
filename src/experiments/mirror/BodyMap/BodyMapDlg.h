// BodyMapDlg.h : header file
//

#if !defined(AFX_BodyMapDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_)
#define AFX_BodyMapDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BodyMap.h"
#include "LiveCameraDlg.h"
#include "LiveGloveDlg.h"
#include "LiveTrackerDlg.h"

#include "learning.h"

/////////////////////////////////////////////////////////////////////////////
// CBodyMapDlg dialog

class CBodyMapDlg : public CDialog {

public:

	int RegisterAndConnectPorts(void);
	void DisconnectAndUnregisterPorts(void);
	void ShowTrackerXY(YARPImageOf<YarpPixelBGR>&, int, int);
	void ShowExpectedTrackerXY(YARPImageOf<YarpPixelBGR>&, int, int);
	void FindTrackerXY(YARPImageOf<YarpPixelBGR>&, int*, int*);

	CBodyMapDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	//{{AFX_DATA(CBodyMapDlg)
	enum { IDD = IDD_BODYMAP_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBodyMapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


protected:

	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBodyMapDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLiveCamera();
	afx_msg void OnLiveGlove();
	afx_msg void OnLiveTracker();
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAcqStart();
	afx_msg void OnAcqStop();
	afx_msg void OnDebugWnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	// dialogs
	CLiveGloveDlg	_GloveDialog;
	CLiveTrackerDlg _Tracker0Dialog;
	CLiveTrackerDlg _Tracker1Dialog;
	CLiveCameraDlg	_Camera0Dialog;
	CLiveCameraDlg	_Camera1Dialog;

	// program options
	BodyMapOptions _options;

	// program settings
	BodyMapSettings _settings;

	// learning modules
	BodyMapLearningBlock _learningBlock;
	// true whilst we are gathering samples
	bool _acquiringSamples;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BodyMapDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_)
