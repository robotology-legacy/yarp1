// Generic viewer for vectors
//
// June 2003, nat

#if !defined(AFX_VECTVIEWERDLG_H__01C7084B_343A_41E6_AA1E_66CF15B285F0__INCLUDED_)
#define AFX_VECTVIEWERDLG_H__01C7084B_343A_41E6_AA1E_66CF15B285F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVectViewerDlg dialog

#include <conf/YARPConfig.h>
#include <YARPThread.h>
#include <YARPPort.h>

const int __length = 3;
const int __displayPeriod = 100;

const int __xSize = 300;
const int __ySize = 100;
 
const int __yDist = 3;
const int __xDist = 3;

const int __dispXPos = 0;
const int __dispYPos = 0;

class CRecv;

class CVectViewerDlg : public CDialog
{
// Construction
public:
	CVectViewerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVectViewerDlg)
	enum { IDD = IDD_VECTVIEWER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void update(double *nVal)
	{
		// do I need to lock "_current" ?

		int i = 0;
		for(i = 0; i < __length; i++)
			_current[i] = __ySize * (0.5 + _zoom[i] * nVal[i]);

	}

	void ScrollAndPaint(CDC &dc, double actual, double old)
	{
		CRect tmp(0, 0, __xSize, __ySize);
		dc.ScrollDC(-1, 0, tmp, tmp, NULL, NULL);

		// erase the old value and paint the new one
		dc.SetPixel(__xSize-1, old, RGB(0, 0, 0));
		dc.SetPixel(__xSize-1, actual, RGB(255, 0, 0));
	}

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVectViewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	double _current[__length];
	double _previous[__length];

	CBitmap *_dspWindows;
	CDC		*_dcMem;
	double *_zoom;
	CRect  _displayRect;

	int _nRows;
	int _nColumns;

	UINT _timer;

	CRecv *_pReceiver;

};

class CRecv: public YARPThread
{
public:
	CRecv(CVectViewerDlg *own):
	_inPort(YARP_UDP)
	{
		_owner = own;
	}
	~CRecv(){}

	virtual void Body(void)
	{
	//	_inPort.Register("/force/i:1");

		while(true)
		{
			_inPort.Read();
			memcpy(_current, _inPort.Content(), __length*sizeof(double));
			_owner->update(_current);
		}
	}

	void Register(char *name = NULL)
	{
		_inPort.Register("/force/i:1");
	}

	CVectViewerDlg *_owner;
	YARPInputPortOf<double [__length]> _inPort;
	double _current[__length];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VECTVIEWERDLG_H__01C7084B_343A_41E6_AA1E_66CF15B285F0__INCLUDED_)
