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
#include "SaverThread.h"

//////////////////////////////////////
typedef struct GraspCaptureOptionsStruct {
	GraspCaptureOptionsStruct() {
		sizeX = sizeY = 0;
		ACE_OS::strcpy(portName, "mirrorGrasp");
		ACE_OS::strcpy(netName, "default");
		ACE_OS::strcpy(imgNetName, "Net1");
		ACE_OS::strcpy(savePath, "d:\\tmp");
		ACE_OS::strcpy(prefix, "seq");
		useCamera0 = useCamera1 = useTracker0 = useTracker1 =
		useGazeTracker = useDataGlove = usePresSens = false;
		refreshFrequency = 40;
	};
	char connectScriptName[255];
	int	sizeX;
	int sizeY;
	char portName[255];
	char netName[255];
	char imgNetName[255];
	char savePath[255];
	char prefix[255];
	bool useCamera0;
	bool useCamera1;
	bool useTracker0;
	bool useTracker1;
	bool useGazeTracker;
	bool useDataGlove;
	bool usePresSens;
	int refreshFrequency;
} GraspCaptureOptions;

/////////////////////////////////////////////////////////////////////////////
// CGraspCaptureDlg dialog

class CGraspCaptureDlg : public CDialog
{

	// Construction
public:

	bool registerPorts(void);
	void unregisterPorts(void);

	CGraspCaptureDlg(CWnd* pParent = NULL);	// standard constructor

	// Dialog Data
	//{{AFX_DATA(CGraspCaptureDlg)
	enum { IDD = IDD_GRASPCAPTURE_DIALOG };
	CString	m_prefixEdit;
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
	afx_msg void OnDebugWnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	// dialogs
	CLiveGloveDlg	GloveDialog;
	CLiveTrackerDlg Tracker0Dialog;
	CLiveTrackerDlg Tracker1Dialog;
	CLiveCameraDlg	Camera0Dialog;
	CLiveCameraDlg	Camera1Dialog;

	// data coming from the collector
	CollectorNumericalData _data;
	CollectorImage         _img0;
	CollectorImage         _img1;

	// the streaming thread
	CSaverThread saverThread;
	int nSeq;

	// program options
	GraspCaptureOptions _options;

	// communication ports: data
	YARPInputPortOf<CollectorNumericalData> _data_inport;
	YARPInputPortOf<YARPGenericImage>       _img0_inport;
	YARPInputPortOf<YARPGenericImage>       _img1_inport;
	// communication ports: commands
	YARPInputPortOf<int>  _cmd_inport;
	YARPOutputPortOf<int> _cmd_outport;

	// timer for live acquisition
	UINT m_timerID;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRASPCAPTUREDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_)
