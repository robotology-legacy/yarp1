// cameraTuneDlg.h : header file
//

#if !defined(AFX_CAMERATUNEDLG_H__FFCF2AD9_CE55_406F_8C2B_FC4528F4F4B7__INCLUDED_)
#define AFX_CAMERATUNEDLG_H__FFCF2AD9_CE55_406F_8C2B_FC4528F4F4B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LiveCameraDlg.h"
#include "LiveEdgesDlg.h"
#include "MessageDlg.h"
#include "OptionsDlg.h"

struct mOptions
{
	int	sizeX;
	int sizeY;
	int nImages;
	int refresh;
	int boardN;
	int thetaSize;
	int rhoSize;
	bool autosave;
	char fileName[256];
};

typedef struct mOptions PgmOptions;

/////////////////////////////////////////////////////////////////////////////
// CCameraTuneDlg dialog

class CCameraTuneDlg : public CDialog
{
// Construction
public:
	
	CCameraTuneDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCameraTuneDlg)
	enum { IDD = IDD_CAMERATUNE_DIALOG };
	CButton	m_StartButton;
	CButton	m_OptionButton;
	CProgressCtrl	m_ProgBar;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCameraTuneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	void setupOptions();
	double _rad2deg(double rad);
	double _normalize0ToPI(double angle);
	void Calibrate();
	void initMembers();
	CLiveCameraDlg	CameraDialog;
	CLiveEdgesDlg	EdgesDialog;
	CMessageDlg		MessageDialog;
	COptionsDlg		OptionsDialog;

	bool bLiveCamera;
	bool bLiveEdges;
	YARPGrabber	grabber;
	YARPImageOf<YarpPixelBGR> img;
	YARPImageOf<YarpPixelBGR> *img_buffer;
	YARPImageOf<YarpPixelMono> susan_img;
	YARPImageOf<YarpPixelMono> img_mono;
	YARPHoughTransform hough_flt;
	UINT m_timerID;
	PgmOptions options;

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCameraTuneDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnShowCamera();
	afx_msg void OnShowEdges();
	afx_msg void OnClose();
	afx_msg void OnStartCamera();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCalibrate();
	afx_msg void OnOptions();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMERATUNEDLG_H__FFCF2AD9_CE55_406F_8C2B_FC4528F4F4B7__INCLUDED_)
