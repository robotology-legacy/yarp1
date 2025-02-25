// camviewDlg.h : header file
// Generic viewer for vectors
//
// June 2003, nat
// July 2004, use argc and argv + YARPParseParameters::parse

#if !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
#define AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////
#include "DIBConverter.h"
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPBottleContent.h>
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
	char m_netname[512];
	char m_out_netname[512];
	int m_x;
	int m_y;
	bool m_frozen;
	bool m_logp;
	bool m_fov;
	bool m_normal;
	bool m_horiz_flip;
	int m_period;
	double m_est_interval;
	CRect m_rect;

	YARPInputPortOf<YARPGenericImage> m_inport;
	YARPGenericImage m_img;
	YARPGenericImage m_flipped;
	YARPImageOf<YarpPixelBGR> m_remapped;
	YARPImageOf<YarpPixelBGR> m_colored;

	YARPLogpolar m_mapper;

	YARPDIBConverter m_converter;
	YARPSemaphore m_mutex;

	CRecv (CCamviewDlg *owner = NULL) : YARPThread (), m_inport(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST), m_mutex(1)
	{
		m_owner = owner;
		memset (m_name, 0, 512);
		sprintf (m_netname, "default");
		sprintf(m_out_netname, "default");
		
		m_x = m_y = 256;
		m_frozen = false;
		m_period = 0;
		m_est_interval = 0;
		m_logp = false;
		m_fov = false;
		m_horiz_flip = false;

		m_rect.left = -1;
		m_rect.top = -1;
		m_rect.right = -1;
		m_rect.bottom = -1;
	}

	~CRecv () {}

	inline void SetOwner (CCamviewDlg *owner) { m_owner = owner; }
	inline void SetName (const char * name) { strcpy (m_name, name); }
	inline int GetWidth (void) const { return m_x; }
	inline int GetHeight (void) const { return m_y; }
	inline void SetPeriod (int newvalue) { m_period = newvalue; }
	inline int GetPeriod (void) const { return m_period; }
	inline void AssumeLogpolar (void) { m_logp = true; }
	inline void AssumeDisplayFovea (void) { m_fov = true; }
	inline void AssumeHorizFlip (void) { m_horiz_flip = true; }
	inline bool TablesOk (void) const { return m_mapper.TablesOk(); }
	inline void SetNetworkName (const char * net) { strcpy (m_netname, net); }
	inline void SetNetworkNameOut(const char * net) { strcpy (m_out_netname, net); }

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

	void SaveCurrentFrame (const char *filename);

	double GetEstimatedInterval (void)
	{
		m_mutex.Wait();
		double ret = m_est_interval;
		m_mutex.Post();
		return ret;
	}

	void SetPaintRectangle (int l, int t, int r, int b)
	{
		m_mutex.Wait();
		m_rect.left = l;
		m_rect.top = t;
		m_rect.right = r;
		m_rect.bottom = b;
		m_mutex.Post();
	}

	void TryClosePort () { m_inport.Unregister(); }
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

	bool m_enable_output;
	YARPOutputPortOf<YARPBottle> *m_outPort;
	YARPBottle m_outBottle;
	CString	m_output_connection;

	double m_zx;
	double m_zy;
	int m_x;
	int m_y;
	int m_fx;
	int m_fy;

// Dialog Data
	//{{AFX_DATA(CCamviewDlg)
	enum { IDD = IDD_CAMVIEW_DIALOG };
	CStatic	m_ctrl_name;
	CString	m_connection_name;
	int		m_image_x;
	int		m_image_y;
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
	afx_msg void OnFileSaveimage();
	afx_msg void OnUpdateFileSaveimage(CCmdUI* pCmdUI);
	afx_msg void OnImageShowinterval();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMVIEWDLG_H__23BF774D_13B8_425B_8BA1_05E121A6000C__INCLUDED_)
