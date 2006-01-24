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
#include "SaverThread.h"

//////////////////////////////////////
typedef struct BodyMapOptionsStruct {
	BodyMapOptionsStruct() {
		sizeX = sizeY = 0;
		ACE_OS::strcpy(BodyMapPortName, "BodyMap");
		ACE_OS::strcpy(MirrorCollectorPortName, "mirrorCollector");
		ACE_OS::strcpy(netName, "default");
		ACE_OS::strcpy(savePath, "d:\\tmp");
		ACE_OS::strcpy(prefix, "seq");
		useCamera0 = useCamera1 = useTracker0 = useTracker1 =
		useGazeTracker = useDataGlove = usePresSens = false;
		refreshFrequency = 40;
	};
	int	sizeX;
	int sizeY;
	char BodyMapPortName[255];
	char MirrorCollectorPortName[255];
	char netName[255];
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
} BodyMapOptions;

/////////////////////////////////////////////////////////////////////////////
// CBodyMapDlg dialog

class CBodyMapDlg : public CDialog
{

	// Construction
public:

	int RegisterAndConnectPorts(void);
	void DisconnectAndUnregisterPorts(void);
	void ShowTrackerXY(YARPImageOf<YarpPixelBGR>&);
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


	// Implementation
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
	BodyMapOptions _options;

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

#endif // !defined(AFX_BodyMapDLG_H__1C4B76BC_BE02_4C31_BDB6_5A7AB990D4B0__INCLUDED_)
