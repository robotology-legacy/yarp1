// camviewDlg.h : header file
//

#if !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
#define AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////
#include <YARPDIBConverter.h>


/////////////////////////////////////////////////////////////////////////////
// CCamviewDlg dialog

class CCamviewDlg;

///
///
class CRecv : public YARPThread 
{
public:
	CCamviewDlg *m_owner;
	char m_name[512];
	int m_x;
	int m_y;

	YARPInputPortOf<YARPGenericImage> m_inport;
	YARPImageOf<YarpPixelBGR> m_img;
	YARPDIBConverter m_converter;
	YARPSemaphore m_mutex;

	CRecv (CCamviewDlg *owner = NULL) : YARPThread (), m_mutex(1) 
	{
		m_owner = owner;
		memset (m_name, 0, 512);
		m_x = m_y = -1;
	}

	~CRecv () {}

	void SetOwner (CCamviewDlg *owner) { m_owner = owner; }
	void SetName (const char * name) { strcpy (m_name, name); }

	unsigned char * AcquireBuffer (void)
	{
		m_mutex.Wait();
		return (unsigned char *)m_converter.GetBuffer();
	}

	void ReleaseBuffer (void)
	{
		m_mutex.Post();
	}

	virtual void Body (void);
};

///
///
///

class CCamviewDlg : public CDialog
{
// Construction
public:
	CCamviewDlg(CWnd* pParent = NULL);	// standard constructor
	CRecv m_receiver;

// Dialog Data
	//{{AFX_DATA(CCamviewDlg)
	enum { IDD = IDD_CAMVIEW_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCamviewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCamviewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnClose();
	afx_msg void OnQuit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
