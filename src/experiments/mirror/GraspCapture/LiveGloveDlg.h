#if !defined(AFX_LIVEGLOVEDLG_H__27547B42_99B7_43E0_B8D1_F08C0AC24AB3__INCLUDED_)
#define AFX_LIVEGLOVEDLG_H__27547B42_99B7_43E0_B8D1_F08C0AC24AB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LiveGloveDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLiveGloveDlg dialog

class CLiveGloveDlg : public CDialog
{
// Construction
public:
	void UpdateState(DataGloveData newGlove_d, PresSensData newPres_d, GazeTrackerData newGT_d);
	CLiveGloveDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLiveGloveDlg)
	enum { IDD = IDD_DATAGLOVE };
	int		m_Pres1;
	int		m_Pres2;
	int		m_Pres3;
	int		m_Pres4;
	int		m_abdTI;
	int		m_abdIM;
	int		m_abdMR;
	int		m_abdRP;
	int		m_finTI;
	int		m_finTM;
	int		m_finTO;
	int		m_finII;
	int		m_finIM;
	int		m_finIO;
	int		m_finMI;
	int		m_finMM;
	int		m_finMO;
	int		m_finRI;
	int		m_finRM;
	int		m_finRO;
	int		m_finPI;
	int		m_finPM;
	int		m_finPO;
	int		m_palmArch;
	int		m_wristPitch;
	int		m_wristYaw;
	int		m_PupDiam;
	double	m_pupX;
	double	m_pupY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLiveGloveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLiveGloveDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIVEGLOVEDLG_H__27547B42_99B7_43E0_B8D1_F08C0AC24AB3__INCLUDED_)
