// Simple ViewerDlg.h : header file
//

#if !defined(AFX_SIMPLEVIEWERDLG_H__A6D629B4_D4B3_4EA8_BAAC_7F5E92766340__INCLUDED_)
#define AFX_SIMPLEVIEWERDLG_H__A6D629B4_D4B3_4EA8_BAAC_7F5E92766340__INCLUDED_
#include "LogPolarSDK.h"
#include <Vfw.h>
#include <Winsock2.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSimpleViewerDlg dialog

class CSimpleViewerDlg : public CDialog
{
// Construction
public:
	int * m_iRemapLUT;
	HDC m_hMyDC;
	BITMAPINFOHEADER m_biHeader;
	BITMAPINFOHEADER m_biHeaderRem;
	unsigned char * m_ucAcqImage;
	unsigned char * m_ucDisplImage;
	unsigned char * m_ucDisplImageRem;
	void Acquire_From_Camera();
	HDRAWDIB m_hDrawDib;
	HDRAWDIB m_hDrawDibRem;
	void Close_Display();
	void Close_Acquisition();
	bool m_bStop;
	void Send_Frame();
	int Acquire_Frame();
	bool EndedThread;
	int m_iCNumb;
	int m_iPortAddress;
	HANDLE m_hThread;
	CRect m_ClientRect;
	CRect m_rcRepaintArea;
	int Init_Destination();
	Image_Data m_idParam;
	int Init_File();
	bool m_bIsStarted;
	CSimpleViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSimpleViewerDlg)
	enum { IDD = IDD_SIMPLEVIEWER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSimpleViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	friend DWORD WINAPI VisualThread(LPVOID param);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEVIEWERDLG_H__A6D629B4_D4B3_4EA8_BAAC_7F5E92766340__INCLUDED_)
