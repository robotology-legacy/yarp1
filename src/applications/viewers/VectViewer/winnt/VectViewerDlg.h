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
#include <YARPSemaphore.h>
#include <YARPMath.h>
#include <YARPVectorPortContent.h>

#include <math.h>

const int __yDist = 3;
const int __xDist = 3;

const int __dispXPos = 0;
const int __dispYPos = 0;

class CRecv;

class CVectViewerDlg : public CDialog
{
// Construction
public:
	CVectViewerDlg(const char *name, const char *net, int period, int size, int length, int height, CWnd* pParent = NULL);	// standard constructor
	~CVectViewerDlg();

// Dialog Data
	//{{AFX_DATA(CVectViewerDlg)
	enum { IDD = IDD_VECTVIEWER_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void update(double *nVal)
	{
		// _mutex.Wait();
		if (_freeze)
			return;
		
		///////////// compute/apply scale
		if (_aScale) {
			_counter++;
			if (_counter > _xSize)
			{
				// change scale
				for(int i = 0; i < _size; i++)
				{
					if (_max[i] > 0)
						_scale[i] = 0.5*1/_max[i];
					else
						_scale[i] = 0.0;

					_max[i] = 0.0;
				}
				_counter = 0;
				// erase window
				eraseHistory();
				refreshHistory();
			}
		}
		///////////////////////////////////////

		int i = 0;
		for(i = 0; i < _size; i++)
		{
			// keep maximum
			if (fabs(nVal[i])>_max[i])
				_max[i] = fabs(nVal[i]);

			_current[i] = 0.5*_ySize * (1 - _zoom[i] * nVal[i]*_scale[i]);

			if (_current[i] > _ySize)
			{
				_current[i] = _ySize;
				_counter = _xSize;	//force rescaling
			}
			else if (_current[i] < 0)
			{
				_current[i] = 0;
				_counter = _xSize; //force rescaling
			}

			// history !
			// shift
			int j;
			for(j = 0; j < _xSize-1; j++)
				_history[i][j] = _history[i][j+1];
			// keep new val
			_history[i][_xSize-1] = nVal[i];
		}
		
		for (i = 0; i < _size; i++)
		{	
			ScrollAndPaint(_dcMem[i], _current[i], _previous[i]);
			_previous[i] = _current[i];
		}

		// _mutex.Post();
	}

	void eraseHistory()
	{
		int i;
		int j;
		for(i = 0; i < _size; i++)
		{
			for(j = 0; j < _xSize; j++)
			{
				for(int m = 0; m < _ySize; m++)
				_dcMem[i].SetPixel(j, m, RGB(0, 0, 0));
			}
		}
	}
	void refreshHistory()
	{	
		int i;
		int j;
		for(i = 0; i < _size; i++)
		{
			// _dcMem[i] //erase
			double tmp;
			for(j = 0; j < _xSize; j++)
			{
				tmp = _ySize * 0.5 * (1 - _zoom[i] * _history[i][j]*_scale[i]);
				_dcMem[i].SetPixel(j, tmp, RGB(255, 0, 0));
			}
			_previous[i] = tmp;	//store last value
		}
	}

	void ScrollAndPaint(CDC &dc, double actual, double old)
	{
		CRect tmp(0, 0, _xSize, _ySize);
		dc.ScrollDC(-1, 0, tmp, tmp, NULL, NULL);

		// erase the old value and paint the new one
		dc.SetPixel(_xSize-1, old, RGB(0, 0, 0));
		dc.SetPixel(_xSize-1, actual, RGB(255, 0, 0));
	}

	void setScale(double *s)
	{
		memcpy(_scale, s, sizeof(double)*_size);
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
	afx_msg void OnFileSave();
	afx_msg void OnImageFreeze();
	afx_msg void OnUpdateImageFreeze(CCmdUI* pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SaveCurrentFrame(const char *filename);
	
	double *_current;
	double *_previous;

	double **_history;

	CBitmap *_dspWindows;
	CDC		*_dcMem;
	double *_zoom;
	double *_scale;
	double *_max;
	CRect  _displayRect;

	int _nRows;
	int _nColumns;

	UINT _timer;

	CRecv *_pReceiver;

	char _name[256];
	char _network[256];

	int _period;
	int _size;
	int _ySize;
	int _xSize;

	int _counter;

	CMenu _menu;
	BOOL _freeze;

	YARPSemaphore _mutex;

public:
	bool _aScale;
};

class CRecv: public YARPThread
{
public:
	CRecv(CVectViewerDlg *own):
	_inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP)
	{
		_owner = own;
	}
	~CRecv(){}

	virtual void Body(void)
	{
		while(!IsTerminated())
		{
			_inPort.Read();
			_data = _inPort.Content();
			_owner->update(_data.data());
		}
	}

	void Register(char *name = NULL, char *net = NULL)
	{
		_inPort.Register(name, net);
	}

	CVectViewerDlg *_owner;
	YARPInputPortOf<YVector> _inPort;
	YVector _data;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VECTVIEWERDLG_H__01C7084B_343A_41E6_AA1E_66CF15B285F0__INCLUDED_)
