// camviewDlg.h : header file
//

#if !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
#define AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////
#include <YARPDIBConverter.h>
#include <Vfw.h>

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
	bool m_frozen;
	int m_period;

	YARPInputPortOf<YARPGenericImage> m_inport;
	YARPImageOf<YarpPixelBGR> m_img;
	YARPImageOf<YarpPixelBGR> m_flipped;
	YARPDIBConverter m_converter;
	YARPSemaphore m_mutex;

	CRecv (CCamviewDlg *owner = NULL) : YARPThread (), m_mutex(1) 
	{
		m_owner = owner;
		memset (m_name, 0, 512);
		m_x = m_y = -1;
		m_frozen = false;
		m_period = 0;
	}

	~CRecv () {}

	void SetOwner (CCamviewDlg *owner) { m_owner = owner; }
	void SetName (const char * name) { strcpy (m_name, name); }
	inline int GetWidth (void) const { return m_x; }
	inline int GetHeight (void) const { return m_y; }
	inline void SetPeriod (int newvalue) { m_period = newvalue; }
	inline int GetPeriod (void) const { return m_period; }

	unsigned char * AcquireBuffer (void)
	{
		m_mutex.Wait ();
		return (unsigned char *)m_converter.GetBuffer();
	}

	void ReleaseBuffer (void)
	{
		m_mutex.Post();
	}

	virtual void Body (void);

	void Freeze (bool fr)
	{
		m_mutex.Wait();
		m_frozen = fr;
		m_mutex.Post();
	}
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
	bool m_frozen;
	HDRAWDIB m_drawdib;

// Dialog Data
	//{{AFX_DATA(CCamviewDlg)
	enum { IDD = IDD_CAMVIEW_DIALOG };
	CButton	m_ctrl_quit;
	CStatic	m_ctrl_name;
	CString	m_connection_name;
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
	afx_msg void OnFileExit();
	afx_msg void OnHelpAbout();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnImageFreeze();
	afx_msg void OnUpdateImageFreeze(CCmdUI* pCmdUI);
	afx_msg void OnImageNewrate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
