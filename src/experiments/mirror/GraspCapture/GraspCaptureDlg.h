// GraspCaptureDlg.h : header file
//

#if !defined(AFX_GRASPCAPTUREDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_)
#define AFX_GRASPCAPTUREDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GraspCapture.h"
#include "LiveCameraDlg.h"
#include "LiveGloveDlg.h"
#include "LiveTrackerDlg.h"
#include "MessagesDlg.h"
//#include "SaverThread.h"


//////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CGraspCaptureDlg dialog

class CGraspCaptureDlg : public CDialog
{
// Construction
public:
	bool SetupPorts(void);
	void InitMembers();
	void InitDlgMembers();
	CGraspCaptureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGraspCaptureDlg)
	enum { IDD = IDD_GRASPCAPTURE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraspCaptureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CGraspCaptureDlg)
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CLiveGloveDlg	GloveDialog;
	CLiveTrackerDlg TrackerDialog;
	CLiveCameraDlg	CameraDialog;
	CMessagesDlg MessagesDialog;
	bool bLiveTracker;
	bool bLiveGlove;
	bool bLiveCamera;
	UINT m_timerID;

	int reply;
	MNumData data;
	YARPImageOf<YarpPixelBGR> img;
	FILE *outfile;
	CSaverThread saverThread;
	char fileName[255];

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRASPCAPTUREDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_)
