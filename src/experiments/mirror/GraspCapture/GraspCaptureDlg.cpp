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
	ON_BN_CLICKED(IDC_KILL, OnKill)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_DEBUG_WND, OnDebugWnd)
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
	InitDlgMembers();
	InitMembers();
	if (!registerPorts())
	{
		MessageBox("Ports Registration Failed.", "Fatal error.",MB_ICONERROR);
		exit(YARP_FAIL);
	}
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
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	bLiveTracker = false;
	bLiveGlove = false;
	bLiveCamera = false;
	bShowDebugWnd = true;
	MessagesDialog.ShowWindow(SW_RESTORE);
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
			m_timerID = SetTimer(321, options.refresh, NULL);
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
	options.sizeX = 0;
	options.sizeY = 0;
	options.useCamera = 0;
	options.useTracker = 0;
	options.useDataGlove = 0;
	options.usePresSens = 0;
	setupOptions();
	nSeq = 0;
}

void CGraspCaptureDlg::OnConnect() 
{
	cmd_outport.Content() = CCMDConnect;
	cmd_outport.Write();
	rep_inport.Read();
	int reply;
	reply = rep_inport.Content();
	if ( reply != CMD_FAILED)
	{
		logText += "Server Connected.\r\n";
		options.useDataGlove = reply & HW_DATAGLOVE;
		options.useTracker = reply & HW_TRACKER;
		options.usePresSens = reply & HW_PRESSENS;
		options.useCamera = reply & HW_CAMERA;
		if (options.useCamera)
		{
			rep_inport.Read();
			options.sizeX = rep_inport.Content();
			rep_inport.Read();
			options.sizeY = rep_inport.Content();
			logText += "Image size is ";
			char dummy[100];
			ACE_OS::sprintf(dummy,"%d",options.sizeX);
			logText += dummy;;
			logText += " X ";
			ACE_OS::sprintf(dummy,"%d",options.sizeY);
			logText += dummy;
			logText += "\r\n";
			MessageBox("Peripherals connected.", "Connection succeded.");
		}
		prepareDataStructures();
		saverThread.useCamera = options.useCamera;
		saverThread.useDataGlove = options.useDataGlove;
		saverThread.usePresSens = options.usePresSens;
		saverThread.useTracker = options.useTracker;
		saverThread.pImg = &(img);
		saverThread.pData = &(data);
		saverThread.p_data_inport = &(data_inport);
		saverThread.p_img_inport = &(img_inport);
		if (options.useCamera)
		{
			CameraDialog.pImage = &img;
			CameraDialog.sizeX = options.sizeX;
			CameraDialog.sizeY = options.sizeY;
		}
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_KILL)->EnableWindow(FALSE);
		if(options.useCamera)
			GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		if(options.useDataGlove || options.usePresSens)
			GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
		if(options.useTracker)
			GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
		GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
	}
	else
	{
		logText += "Connection Failed\r\n.";
		MessageBox("Peripherals connection Failed.", "Error.",MB_ICONERROR);
	}	
	MessagesDialog.m_MessageBox.SetWindowText(logText);
}

bool CGraspCaptureDlg::registerPorts()
{
	char portName[255];
	int ret = 0;
	
	// Data port registration
	ACE_OS::sprintf(portName,"/%s/i:str", options.portName);
	ret = data_inport.Register(portName, options.netName);
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering output port with name ";
		logText += portName;
		logText += ".\r\n";
		return false;
	}
	logText += "Port :";
	logText += portName;
	logText += " registered.\r\n";
	
// Images port registration
	ACE_OS::sprintf(portName,"/%s/i:img", options.portName);
	ret = img_inport.Register(portName, options.netName);
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering output port with name ";
		logText += portName;
		logText += ".\r\n";
		return false;
	}
	logText += "Port :";
	logText += portName;
	logText += " registerd.\r\n";

// Commands port registration
	ACE_OS::sprintf(portName,"/%s/o:int", options.portName);
	ret =cmd_outport.Register(portName, options.netName);
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering output port with name ";
		logText += portName;
		logText += ".\r\n";
		return false;
	}
	logText += "Port :";
	logText += portName;
	logText += " registerd.\r\n";

// Response port registration
	ACE_OS::sprintf(portName,"/%s/i:int", options.portName);
	ret = rep_inport.Register(portName, options.netName);
	if (ret != YARP_OK)
	{
		logText += "\r\nFATAL ERROR: problems registering output port with name ";
		logText += portName;
		logText += ".\r\n";
		return false;
	}
	logText += "Port :";
	logText += portName;
	logText += " registerd.\r\n";
	
	system("connect.bat");
	logText += "Connection between ports established.\r\n";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	return true;
}

void CGraspCaptureDlg::OnDisconnect() 
{
	logText += "Sending the Disconnect command..";
	cmd_outport.Content() = CCMDDisconnect;
	cmd_outport.Write();
	rep_inport.Read();
	int reply;
	reply = rep_inport.Content();
	if ( reply == CMD_ACK)
	{
		logText += " Done.\r\n";
	}	
	else
	{
		logText += " Failed.\r\n";
		MessageBox("Peripherals disconnection Failed.", "Error.",MB_ICONERROR);
		MessagesDialog.ShowWindow(SW_HIDE);
	}
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_KILL)->EnableWindow(TRUE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
}

void CGraspCaptureDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
// Timer Killing
	if (m_timerID != NULL)
		KillTimer (m_timerID);
// Disconnection 
// Port releasing
	data_inport.Unregister();
	img_inport.Unregister();
	rep_inport.Unregister();
	cmd_outport.Unregister();

	CDialog::OnClose();
}

void CGraspCaptureDlg::OnTimer(UINT nIDEvent) 
{	
	cmd_outport.Content() = CCMDGetData;
	cmd_outport.Write();
	rep_inport.Read();
	int reply;
	reply = rep_inport.Content();
	if ( reply == CMD_ACK)
	{
		if ( options.useDataGlove || options.useTracker || options.usePresSens)
		{
			data_inport.Read();
			data = data_inport.Content();
		}
		if (options.useCamera)
		{
			img_inport.Read();
			img.Refer(img_inport.Content());
		}
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
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_QUIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_OPTIONS)->EnableWindow(FALSE);

	CFileFind finder;

	
	char fName[255];
	
	saverThread.pFile = NULL;
	ACE_OS::sprintf(saverThread.prefix, "%s\\%s%03d",options.savePath, options.prefix, nSeq);
	logText += "Saving data to the path ";
	logText += options.savePath;
	logText += " with prefix ";
	logText += saverThread.prefix;
	logText += "\r\n";
	if ( options.useDataGlove || options.useTracker || options.usePresSens)
	{
		
		ACE_OS::sprintf(fName, "%s.csv",saverThread.prefix);
		saverThread.pFile = fopen(fName,"w");
		if (saverThread.pFile == NULL)
		{
			logText += "ERROR: Impossible to open output file.\r\n";
			MessagesDialog.m_MessageBox.SetWindowText(logText);
			MessageBox("Impossible to open output file.", "Error.", MB_ICONERROR);
			GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
			GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
			GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
			GetDlgItem(IDC_QUIT)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);
			return;
		}
		saverThread.writeHeaderToFile();
		logText += "Saving data to file ";
		logText += fName;
		logText += "\r\n";
	}

	cleanDataStructures();

	cmd_outport.Content() = CCMDStartStreaming;
	cmd_outport.Write();
	rep_inport.Read();
	int reply;
	reply = rep_inport.Content();
	if ( reply == CMD_ACK)
	{
		logText += "Saving Thread started.\r\n";
		GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACQ_STOP)->EnableWindow(TRUE);
		//saverThread.
		saverThread.Begin();
	}
	else
	{
		logText += "ERROR: Failed to start Thread.\r\n";
		MessageBox("Impossible to start saving thread.", "Error.", MB_ICONERROR);
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_QUIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);
	}
	MessagesDialog.m_MessageBox.SetWindowText(logText);
}

void CGraspCaptureDlg::OnAcqStop() 
{
	cmd_outport.Content() = CCMDStopStreaming;
	cmd_outport.Write();
	saverThread.End();

	if (saverThread.pFile != NULL)
		fclose(saverThread.pFile);
	int reply;
	reply = rep_inport.Content();
	if ( reply != CMD_ACK)
		MessageBox("Problems stopping saving thread.", "Error.", MB_ICONERROR);
	nSeq++;
	GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_QUIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);
	if (options.useCamera)
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
	if (options.useDataGlove || options.usePresSens)
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
	if (options.useTracker)
		GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
	logText += "Saving Thread stopped.\r\n";
	logText += "Sequence n.";
	char dummy[100];
	ACE_OS::sprintf(dummy,"%d",nSeq);
	logText += dummy;
	logText += " saved (";
	ACE_OS::sprintf(dummy,"%d",saverThread.getSavedFramesN());
	logText += dummy;
	logText += " frames).\r\n";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
}



void CGraspCaptureDlg::prepareDataStructures(void)
{
	img.Resize (options.sizeX, options.sizeY);
}

void CGraspCaptureDlg::cleanDataStructures(void)
{
// Pressure Sensors
	data.pressure.channelA = 0;
	data.pressure.channelB = 0;
	data.pressure.channelC = 0;
	data.pressure.channelD = 0;
// Tracker
	data.tracker.x = 0.0;
	data.tracker.y = 0.0;
	data.tracker.z = 0.0;
	data.tracker.azimuth = 0.0;
	data.tracker.elevation = 0.0;
	data.tracker.roll = 0.0;
// DataGlove
	data.glove.thumb[0] = 0;	// inner
	data.glove.thumb[1] = 0;	// middle
	data.glove.thumb[2] = 0;	// outer
	data.glove.index[0] = 0;	// inner
	data.glove.index[1] = 0;	// middle
	data.glove.index[2] = 0;	// outer
	data.glove.middle[0] = 0;	// inner
	data.glove.middle[1] = 0;	// middle
	data.glove.middle[2] = 0;	// outer
	data.glove.ring[0] = 0;	// inner
	data.glove.ring[1] = 0;	// middle
	data.glove.ring[2] = 0;	// outer
	data.glove.pinkie[0] = 0;	// inner
	data.glove.pinkie[1] = 0;	// middle
	data.glove.pinkie[2] = 0;	// outer
	data.glove.abduction[0] = 0; // thumb-index
	data.glove.abduction[1] = 0; // index-middle
	data.glove.abduction[2] = 0; // middle-ring
	data.glove.abduction[3] = 0; // ring-pinkie
	data.glove.abduction[4] = 0; // palm
	data.glove.palmArch = 0;
	data.glove.wrist[0] = 0; // pitch
	data.glove.wrist[1] = 0; // yaw

	img.Zero();
}

void CGraspCaptureDlg::unregisterPorts()
{
	cmd_outport.Unregister();
	rep_inport.Unregister();
	data_inport.Unregister();
	img_inport.Unregister();
}

void CGraspCaptureDlg::OnKill() 
{
	logText += "Sending the KILL command..";
	
	cmd_outport.Content() = CCMDQuit;
	cmd_outport.Write();
	rep_inport.Read();
	int reply;
	reply = rep_inport.Content();
	if ( reply == CMD_ACK)
	{
		logText += " Done.";
	}	
	else
	{
		logText += " Failed.";
		MessageBox("Problems killing the server.", "Error.", MB_ICONERROR);
	}
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	OnOK();
}

void CGraspCaptureDlg::OnOptions() 
{
	BOOL ret;
	ret = OptionsDialog.DoModal();
	// controllare il valore di ritorno;
	if (ret)
		setupOptions();
}

void CGraspCaptureDlg::setupOptions()
{
	ACE_OS::strcpy(options.netName,(LPCSTR)(OptionsDialog.m_NetName) );
	ACE_OS::strcpy(options.portName,(LPCSTR)(OptionsDialog.m_PortName) );
	ACE_OS::strcpy(options.prefix,(LPCSTR)(OptionsDialog.m_Prefix) );
	ACE_OS::strcpy(options.savePath,(LPCSTR)(OptionsDialog.m_SavePath) );
	options.refresh = OptionsDialog.m_RefreshTime;
}

void CGraspCaptureDlg::OnDebugWnd() 
{
	if 	(bShowDebugWnd)
	{
		bShowDebugWnd = false;
		MessagesDialog.ShowWindow(SW_HIDE);
	}
	else
	{
		bShowDebugWnd = true;
		MessagesDialog.ShowWindow(SW_RESTORE);
	}	
}
