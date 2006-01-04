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
#include "SaverThread.h"
#include "OptionsDlg.h"


//////////////////////////////////////
struct GraspOptions
{
	int	sizeX;
	int sizeY;
	char portName[255];
	char netName[255];
	char savePath[255];
	char prefix[255];
	int useCamera0;
	int useCamera1;
	int useTracker0;
	int useTracker1;
	int useDataGlove;
	int usePresSens;
	int refreshFrequency;
};

typedef struct GraspOptions PgmOptions;

/////////////////////////////////////////////////////////////////////////////
// CGraspCaptureDlg dialog

class CGraspCaptureDlg : public CDialog
{
// Construction
public:
	void setupOptionsDialog();
	void readFromOptionsDialog();
	bool registerPorts(void);
	void InitMembers();
	void InitDlgMembers();
	void prepareDataStructures(void);
	void cleanDataStructures(void);
	void unregisterPorts(void);
	CGraspCaptureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CGraspCaptureDlg)
	enum { IDD = IDD_GRASPCAPTURE_DIALOG };
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
	afx_msg void OnKill();
	afx_msg void OnOptions();
	afx_msg void OnDebugWnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CLiveGloveDlg	GloveDialog;
	CLiveTrackerDlg Tracker0Dialog;
	CLiveTrackerDlg Tracker1Dialog;
	CLiveCameraDlg	Camera0Dialog;
	CLiveCameraDlg	Camera1Dialog;
	CMessagesDlg MessagesDialog;
	COptionsDlg OptionsDialog;
	bool bLiveTracker0;
	bool bLiveTracker1;
	bool bLiveGlove;
	bool bLiveCamera0;
	bool bLiveCamera1;
	bool bShowDebugWnd;
	UINT m_timerID;
	
	MNumData data;
	YARPImageOf<YarpPixelBGR> img0;
	YARPImageOf<YarpPixelBGR> img1;
	CSaverThread saverThread;
	int nSeq;
	PgmOptions options;
	YARPInputPortOf<MNumData> data_inport;
	YARPInputPortOf<YARPGenericImage> img0_inport;
	YARPInputPortOf<YARPGenericImage> img1_inport;
	YARPInputPortOf<int> rep_inport;
	YARPOutputPortOf<MCommands> cmd_outport;
	CString logText;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRASPCAPTUREDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_)
