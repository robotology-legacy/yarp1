// GraspCaptureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GraspCapture.h"
#include "GraspCaptureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

YARPInputPortOf<MNumData> _data_inport;
YARPInputPortOf<YARPGenericImage> _img_inport;
YARPInputPortOf<int> _rep_inport;
YARPOutputPortOf<MCommands> _cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
int _sizeX;
int _sizeY;
char _path[255];
int _nSeq;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraspCaptureDlg dialog

CGraspCaptureDlg::CGraspCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGraspCaptureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGraspCaptureDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGraspCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraspCaptureDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGraspCaptureDlg, CDialog)
	//{{AFX_MSG_MAP(CGraspCaptureDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LIVE_CAMERA, OnLiveCamera)
	ON_BN_CLICKED(IDC_LIVE_GLOVE, OnLiveGlove)
	ON_BN_CLICKED(IDC_LIVE_TRACKER, OnLiveTracker)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ACQ_START, OnAcqStart)
	ON_BN_CLICKED(IDC_ACQ_STOP, OnAcqStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraspCaptureDlg message handlers

BOOL CGraspCaptureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	InitMembers();
	InitDlgMembers();
	bool ret = false;
	ret = SetupPorts();
	if (ret)
		MessageBox("Ports Registration Successed.", "All done.", MB_ICONINFORMATION);
	else
	{
		MessageBox("Ports Registration Failed.", "Fatal error.",MB_ICONERROR);
		exit(YARP_FAIL);
	}
	MessagesDialog.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGraspCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGraspCaptureDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGraspCaptureDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CGraspCaptureDlg::InitDlgMembers()
{
	GloveDialog.Create(CLiveGloveDlg::IDD, this);
	TrackerDialog.Create(CLiveTrackerDlg::IDD, this);
	CameraDialog.Create(CLiveCameraDlg::IDD, this);
	MessagesDialog.Create(CMessagesDlg::IDD, this);
	bLiveTracker = false;
	bLiveGlove = false;
	bLiveCamera = false;
	m_timerID = NULL;
}

void CGraspCaptureDlg::OnLiveCamera() 
{
	if (bLiveCamera)
	{
		CameraDialog.ShowWindow(SW_HIDE);
		bLiveCamera = false;
	}
	else
	{
		if (m_timerID == NULL)
		{
			m_timerID = SetTimer(321, 100, NULL);
			_ASSERT (m_timerID != 0);
		}
		CameraDialog.ShowWindow(SW_RESTORE);
		bLiveCamera = true;
	}
	if (!bLiveTracker && !bLiveGlove && !bLiveCamera && (m_timerID != NULL) ) 
	{
		KillTimer (m_timerID);
		m_timerID = NULL;
	}
}

void CGraspCaptureDlg::OnLiveGlove() 
{
	if (bLiveGlove)
	{
		GloveDialog.ShowWindow(SW_HIDE);
		bLiveGlove = false;
	}
	else
	{
		if (m_timerID == NULL)
		{
			m_timerID = SetTimer(321, 100, NULL);
			_ASSERT (m_timerID != 0);
		}
		GloveDialog.ShowWindow(SW_RESTORE);
		bLiveGlove = true;
	}
	if (!bLiveTracker && !bLiveGlove && !bLiveCamera && (m_timerID != NULL) ) 
	{
		KillTimer (m_timerID);
		m_timerID = NULL;
	}	
}

void CGraspCaptureDlg::OnLiveTracker() 
{
	if (bLiveTracker)
	{
		TrackerDialog.ShowWindow(SW_HIDE);
		bLiveTracker = false;
	}
	else
	{
		if (m_timerID == NULL)
		{
			m_timerID = SetTimer(321, 100, NULL);
			_ASSERT (m_timerID != 0);
		}
		TrackerDialog.ShowWindow(SW_RESTORE);
		bLiveTracker = true;
	}
	
	if (!bLiveTracker && !bLiveGlove && !bLiveCamera && (m_timerID != NULL) ) 
	{
		KillTimer (m_timerID);
		m_timerID = NULL;
	}

}

void CGraspCaptureDlg::InitMembers()
{
	_sizeX = 0;
	_sizeY = 0;
	reply = -1;
	outfile = NULL;
	_nSeq = 0;
}

void CGraspCaptureDlg::OnConnect() 
{
	CString logText;
	MessagesDialog.ShowWindow(SW_RESTORE);
	logText = "Connecting..\r\nOpening Output file 'data.txt'...";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	outfile = fopen("data.txt","w");
	if (outfile == NULL)
	{
		logText += "\r\nFATAL ERROR: impossible to open output file 'data.txt'.";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		MessageBox("Peripherals connection Failed.", "Error.",MB_ICONERROR);	
	}
	else 
	{
		logText += " Done.";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		logText += "\r\nSending the Connect command to server...";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		_cmd_outport.Content() = CCMDConnect;
		_cmd_outport.Write();
		_rep_inport.Read();
		reply = _rep_inport.Content();
		if ( reply == CMD_ACK)
		{
			_rep_inport.Read();
			_sizeX = _rep_inport.Content();
			_rep_inport.Read();
			_sizeY = _rep_inport.Content();
			img.Resize (_sizeX, _sizeY);
			logText += " Done.\r\nPeripherals Connected.";
			CString logText2;
			logText2.Format("\r\nImage Size is x = %d y = %d",_sizeX, _sizeY); 
			logText += logText2;
			MessagesDialog.m_MessageBox.SetWindowText(logText);
			MessageBox("Peripherals connection Successed.", "All done.", MB_ICONINFORMATION);
			MessagesDialog.ShowWindow(SW_HIDE);
		}
		else
		{
			logText += " Failed.";
			MessagesDialog.m_MessageBox.SetWindowText(logText);
			MessageBox("Peripherals connection Failed.", "Error.",MB_ICONERROR);
			MessagesDialog.ShowWindow(SW_HIDE);	
		}	
	}
}

bool CGraspCaptureDlg::SetupPorts()
{
	CString logText;
	MessagesDialog.ShowWindow(SW_RESTORE);
	// Data port registration
	logText = "Registering Data port (input)...";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	int ret = 0;	
	ret = _data_inport.Register("/test/i:str","default");
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering output port with name '/test/i:str' (see above).\r\n";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		return false;
	}
	
	logText += " Done.\r\n";
	MessagesDialog.m_MessageBox.SetWindowText(logText);

// Images port registration
	logText += "Registering Images port (input)...";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	ret = 0;	
	ret = _img_inport.Register("/test/i:img","default");
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering output port with name '/test/i:img' (see above).\r\n";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		return false;
	}
	
	logText += " Done.\r\n";
	MessagesDialog.m_MessageBox.SetWindowText(logText);

// Commands port registration
	logText += "Registering Commands port (output)...";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	ret = 0;	
	ret = _cmd_outport.Register("/test/o:int","default");
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering input port with name '/test/o:int' (see above).";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		return false;
	}
	logText += " Done.\r\n";
	MessagesDialog.m_MessageBox.SetWindowText(logText);

// Response port registration
	logText += "Registering Response port (input)...";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	ret = 0;	
	ret = _rep_inport.Register("/test/i:int", "default");
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering output port with name '/test/i:int' (see above).";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		return false;
	}
	logText += " Done.\r\n";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	return true;
}

void CGraspCaptureDlg::OnDisconnect() 
{
	CString logText;
	MessagesDialog.ShowWindow(SW_RESTORE);
	logText = "Closing data file..";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	fclose(outfile);
	logText += " Done.\r\nSending the Kill command..";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	_cmd_outport.Content() = CCMDQuit;
	_cmd_outport.Write();
	_rep_inport.Read();
	reply = _rep_inport.Content();
	if ( reply == CMD_ACK)
	{
		logText += " Done.";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		MessageBox("Peripherals disconnection Successed.", "All done.", MB_ICONINFORMATION);
			
	}	
	else
	{
		logText += " Failed.";
		MessagesDialog.m_MessageBox.SetWindowText(logText);
		MessageBox("Peripherals disconnection Failed.", "Error.",MB_ICONERROR);
	}
	MessagesDialog.ShowWindow(SW_HIDE);
}

void CGraspCaptureDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
// Timer Killing
	if (m_timerID != NULL)
		KillTimer (m_timerID);
// Disconnection 
// Port releasing
	_data_inport.Unregister();
	_img_inport.Unregister();
	_rep_inport.Unregister();
	_cmd_outport.Unregister();

	CDialog::OnClose();
}

void CGraspCaptureDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
// Request for data
	_cmd_outport.Content() = CCMDGetData;
	_cmd_outport.Write();
	_rep_inport.Read();
	reply = _rep_inport.Content();
	if ( reply == CMD_ACK)
	{
		_data_inport.Read();
		_img_inport.Read();
		data = _data_inport.Content();
		img.Refer(_img_inport.Content());
	}	
	else
	{
		MessageBox("Impossible to read data from collector server.", "Error.",MB_ICONERROR);
		KillTimer (m_timerID);
		m_timerID = NULL;
	}
// Show (when necessary) the tracker Data
	if (bLiveTracker)
	{
		TrackerDialog.UpdateState(data.tracker);
	}
// Show (when necessary) the glove and pressure data
	if (bLiveGlove)
	{
		GloveDialog.UpdateState(data.glove, data.pressure);
	}

// Show (when necessary) the image from camera
	if (bLiveCamera)
	{
		YARPGenericImage *pImg = &img;
		CameraDialog.UpdateState(pImg);
	}


	CDialog::OnTimer(nIDEvent);
}

void CGraspCaptureDlg::OnAcqStart() 
{
	if (bLiveCamera)
		OnLiveCamera();
	if (bLiveGlove)
		OnLiveGlove();
	if (bLiveTracker)
		OnLiveTracker();

	CFileFind finder;

	BOOL bFound;
	do 
	{
		_nSeq++;
		sprintf(_path,"D:\\Mirror\\sequences\\seq%03d", _nSeq);
		bFound = finder.FindFile(_path);
	} while (bFound);
	
	int Ret;
	Ret = _tmkdir(_path);
	if (Ret != 0)
	{
		MessageBox("Impossible to create destination directory.", "Error.", MB_ICONERROR);
		return;
	}
	//cout << endl << "Starting streaming mode..";
	_cmd_outport.Content() = CCMDStartStreaming;
	_cmd_outport.Write();
	_rep_inport.Read();
	reply = _rep_inport.Content();
	if ( reply == CMD_ACK)
	{
		saverThread.Begin();
		//cout << " Done." << endl;
	}
	else
		MessageBox("Impossible to start saving thread.", "Error.", MB_ICONERROR);	
}

void CGraspCaptureDlg::OnAcqStop() 
{
	//cout << endl << "Stopping streaming mode..";
	_cmd_outport.Content() = CCMDStopStreaming;
	_cmd_outport.Write();
	if ( reply == CMD_ACK)
	{
		saverThread.End();
		//cout << " Done." << endl;
	}
	else
		MessageBox("Impossible to stop saving thread.", "Error.", MB_ICONERROR);
	
}
