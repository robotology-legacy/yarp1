#if !defined(AFX_LIVETRACKERDLG_H__3D2FC82E_6484_49DD_BDC1_E1EE1657820D__INCLUDED_)
#define AFX_LIVETRACKERDLG_H__3D2FC82E_6484_49DD_BDC1_E1EE1657820D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LiveTrackerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLiveTrackerDlg dialog

class CLiveTrackerDlg : public CDialog
{
// Construction
public:
	void UpdateState(TrackerData newData);
	CLiveTrackerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLiveTrackerDlg)
	enum { IDD = IDD_TRACKER };
	float	m_Azimuth;
	float	m_Elevation;
	float	m_Roll;
	float	m_X;
	float	m_Y;
	float	m_Z;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLiveTrackerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	HDRAWDIB m_drawdib;
protected:

	// Generated message map functions
	//{{AFX_MSG(CLiveTrackerDlg)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVETRACKERDLG_H__3D2FC82E_6484_49DD_BDC1_E1EE1657820D__INCLUDED_)
