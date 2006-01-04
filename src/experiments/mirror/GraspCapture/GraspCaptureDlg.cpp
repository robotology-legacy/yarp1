// GraspCaptureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GraspCapture.h"
#include "GraspCaptureDlg.h"

#include <yarp/YARPTime.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char* connectScriptName = "C:\\yarp\\src\\experiments\\mirror\\GraspCapture\\graspCaptureConnect.bat";

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
	if (pSysMenu != NULL) {
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()) {
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

	if (!registerPorts()) {
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

	if (IsIconic()) {
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
	} else {
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
	Tracker0Dialog.Create(CLiveTrackerDlg::IDD, this);
	Tracker0Dialog.SetWindowText("LIVE: Tracker #0");
	Tracker1Dialog.Create(CLiveTrackerDlg::IDD, this);
	Tracker1Dialog.SetWindowText("LIVE: Tracker #1");
	Camera0Dialog.Create(CLiveCameraDlg::IDD, this);
	Camera0Dialog.SetWindowText("LIVE: Camera #0");
	Camera1Dialog.Create(CLiveCameraDlg::IDD, this);
	Camera1Dialog.SetWindowText("LIVE: Camera #1");
	MessagesDialog.Create(CMessagesDlg::IDD, this);
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	bLiveTracker0 = false;
	bLiveTracker1 = false;
	bLiveGlove = false;
	bLiveCamera0 = false;
	bLiveCamera1 = false;
	bShowDebugWnd = false;

}

void CGraspCaptureDlg::OnLiveCamera() 
{

	if ( options.useCamera0 ) {
		if ( bLiveCamera0 ) {
			Camera0Dialog.ShowWindow(SW_HIDE);
			bLiveCamera0 = false;
		} else {
			Camera0Dialog.ShowWindow(SW_RESTORE);
			bLiveCamera0 = true;
		}
	}

	if ( options.useCamera1 ) {
		if ( bLiveCamera1 ) {
			Camera1Dialog.ShowWindow(SW_HIDE);
			bLiveCamera1 = false;
		} else {
			Camera1Dialog.ShowWindow(SW_RESTORE);
			bLiveCamera1 = true;
		}
	}

}

void CGraspCaptureDlg::OnLiveGlove() 
{

	if (bLiveGlove) {
		GloveDialog.ShowWindow(SW_HIDE);
		bLiveGlove = false;
	} else {
		GloveDialog.ShowWindow(SW_RESTORE);
		bLiveGlove = true;
	}

}

void CGraspCaptureDlg::OnLiveTracker() 
{

	if ( options.useTracker0 ) {
		if (bLiveTracker0) {
			Tracker0Dialog.ShowWindow(SW_HIDE);
			bLiveTracker0 = false;
		} else {
			Tracker0Dialog.ShowWindow(SW_RESTORE);
			bLiveTracker0 = true;
		}
	}
	
	if ( options.useTracker1 ) {
		if (bLiveTracker1) {
			Tracker1Dialog.ShowWindow(SW_HIDE);
			bLiveTracker1 = false;
		} else {
			Tracker1Dialog.ShowWindow(SW_RESTORE);
			bLiveTracker1 = true;
		}
	}
	
}

void CGraspCaptureDlg::InitMembers()
{

	options.sizeX = 0;
	options.sizeY = 0;
	ACE_OS::strcpy(options.portName, "mirrorGrasp");
	ACE_OS::strcpy(options.netName, "default");
	ACE_OS::strcpy(options.savePath, "d:\\tmp");
	ACE_OS::strcpy(options.prefix, "seq");
	options.useCamera0 = 0;
	options.useCamera1 = 0;
	options.useTracker0 = 0;
	options.useTracker1 = 0;
	options.useDataGlove = 0;
	options.usePresSens = 0;
	options.refreshFrequency = 40;
	nSeq = 0;

}

void CGraspCaptureDlg::OnConnect() 
{

	cmd_outport.Content() = CCMDConnect;
	cmd_outport.Write();
	rep_inport.Read();
	int reply = rep_inport.Content();
	if ( reply != CMD_FAILED) {
		logText += "Server Connected.\r\n";
		options.useDataGlove = reply & HW_DATAGLOVE;
		options.useTracker0 = reply & HW_TRACKER0;
		options.useTracker1 = reply & HW_TRACKER1;
		options.usePresSens = reply & HW_PRESSENS;
		options.useCamera0 = reply & HW_CAMERA0;
		options.useCamera1 = reply & HW_CAMERA1;
		if ( options.useCamera0 || options.useCamera1 ) {
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
		}
		prepareDataStructures();
		saverThread.useCamera0 = options.useCamera0;
		saverThread.useCamera1 = options.useCamera1;
		saverThread.useDataGlove = options.useDataGlove;
		saverThread.usePresSens = options.usePresSens;
		saverThread.useTracker0 = options.useTracker0;
		saverThread.useTracker1 = options.useTracker1;
		saverThread.pImg0 = &(img0);
		saverThread.pImg1 = &(img1);
		saverThread.pData = &(data);
		saverThread.p_data_inport = &(data_inport);
		saverThread.p_img0_inport = &(img0_inport);
		saverThread.p_img1_inport = &(img1_inport);
		if ( options.useCamera0 ) {
			Camera0Dialog.pImage = &img0;
			Camera0Dialog.sizeX = options.sizeX;
			Camera0Dialog.sizeY = options.sizeY;
		}
		if ( options.useCamera1 ) {
			Camera1Dialog.pImage = &img1;
			Camera1Dialog.sizeX = options.sizeX;
			Camera1Dialog.sizeY = options.sizeY;
		}
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_KILL)->EnableWindow(FALSE);
		if( options.useCamera0 || options.useCamera1 ) {
			GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		}
		if(options.useDataGlove || options.usePresSens) {
			GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
		}
		if(options.useTracker0 || options.useTracker1) {
			GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);

		m_timerID = SetTimer(1, options.refreshFrequency, NULL);
		_ASSERT (m_timerID != 0);	
	
	} else {
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
	ACE_OS::sprintf(portName,"/%s/i:img0", options.portName);
	ret = img0_inport.Register(portName, options.netName);
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

	ACE_OS::sprintf(portName,"/%s/i:img1", options.portName);
	ret = img1_inport.Register(portName, options.netName);
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
	
	system(connectScriptName);
	logText += "Connection between ports established.\r\n";
	MessagesDialog.m_MessageBox.SetWindowText(logText);
	return true;

}

void CGraspCaptureDlg::OnDisconnect() 
{

	KillTimer (m_timerID);

	logText += "Sending the Disconnect command... ";
	cmd_outport.Content() = CCMDDisconnect;
	cmd_outport.Write();
	rep_inport.Read();
	int reply = rep_inport.Content();
	if ( reply == CMD_ACK) {
		logText += "done.\r\n";
	} else {
		logText += "failed.\r\n";
		MessageBox("Peripherals disconnection Failed.", "Error.",MB_ICONERROR);
		MessagesDialog.ShowWindow(SW_HIDE);
	}

	MessagesDialog.m_MessageBox.SetWindowText(logText);

	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_KILL)->EnableWindow(TRUE);

}

void CGraspCaptureDlg::OnClose() 
{

	data_inport.Unregister();
	img0_inport.Unregister();
	img1_inport.Unregister();
	rep_inport.Unregister();
	cmd_outport.Unregister();

	CDialog::OnClose();

}

void CGraspCaptureDlg::OnTimer(UINT nIDEvent) 
{	

	cmd_outport.Content() = CCMDGetData;
	cmd_outport.Write();
	rep_inport.Read();
	int reply = rep_inport.Content();
	if ( reply == CMD_ACK) {
		if ( options.useDataGlove || options.useTracker0 || options.useTracker1 || options.usePresSens) {
			if ( data_inport.Read(false) ) {
				data = data_inport.Content();
			}
		}
		if ( options.useCamera0 ) {
			if ( img0_inport.Read(false) ) {
				img0.Refer(img0_inport.Content());
			}
		}
		if ( options.useCamera1 ) {
			if ( img1_inport.Read(false) ) {
				img1.Refer(img1_inport.Content());
			}
		}
	} else {
		MessageBox("Impossible to read data from collector server.", "Error.",MB_ICONERROR);
		m_timerID = NULL;
	}

	// Show (when necessary) the tracker Data
	if (bLiveTracker0) {
		Tracker0Dialog.UpdateState(data.tracker0);
	}
	if (bLiveTracker1) {
		Tracker1Dialog.UpdateState(data.tracker1);
	}

	// Show (when necessary) the glove and pressure data
	if (bLiveGlove) {
		GloveDialog.UpdateState(data.glove, data.pressure);
	}

	// Show (when necessary) the image from cameras
	if ( bLiveCamera0 ) {
		YARPGenericImage *pImg0 = &img0;
		Camera0Dialog.UpdateState(pImg0);
	}
	if ( bLiveCamera1 ) {
		YARPGenericImage *pImg1 = &img1;
		Camera1Dialog.UpdateState(pImg1);
	}

	CDialog::OnTimer(nIDEvent);

}

void CGraspCaptureDlg::OnAcqStart() 
{

	if (bLiveCamera0 || bLiveCamera1) {
		OnLiveCamera();
	}
	if (bLiveGlove) {
		OnLiveGlove();
	}
	if (bLiveTracker0 || bLiveTracker1) {
		OnLiveTracker();
	}

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
	if ( options.useDataGlove || options.useTracker0 || options.useTracker1 || options.usePresSens) {
		ACE_OS::sprintf(fName, "%s.csv",saverThread.prefix);
		saverThread.pFile = fopen(fName,"w");
		if (saverThread.pFile == NULL) {
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
	if ( reply == CMD_ACK) {
		logText += "Saving Thread started.\r\n";
		GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACQ_STOP)->EnableWindow(TRUE);
		//saverThread.
		KillTimer (m_timerID);
		saverThread.Begin();
	} else {
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

	m_timerID = SetTimer(1, options.refreshFrequency, NULL);
	_ASSERT (m_timerID != 0);	
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
	if ( options.useCamera0 || options.useCamera1 )
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
	if (options.useDataGlove || options.usePresSens)
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
	if (options.useTracker0 || options.useTracker1)
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
	img0.Resize (options.sizeX, options.sizeY);
	img1.Resize (options.sizeX, options.sizeY);
}

void CGraspCaptureDlg::cleanDataStructures(void)
{
// Pressure Sensors
	data.pressure.channelA = 0;
	data.pressure.channelB = 0;
	data.pressure.channelC = 0;
	data.pressure.channelD = 0;
// Trackers
	data.tracker0.x = 0.0;
	data.tracker0.y = 0.0;
	data.tracker0.z = 0.0;
	data.tracker0.azimuth = 0.0;
	data.tracker0.elevation = 0.0;
	data.tracker0.roll = 0.0;
	data.tracker1.x = 0.0;
	data.tracker1.y = 0.0;
	data.tracker1.z = 0.0;
	data.tracker1.azimuth = 0.0;
	data.tracker1.elevation = 0.0;
	data.tracker1.roll = 0.0;
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

	img0.Zero();
	img1.Zero();
}

void CGraspCaptureDlg::unregisterPorts()
{

	cmd_outport.Unregister();
	rep_inport.Unregister();
	data_inport.Unregister();
	img0_inport.Unregister();
	img1_inport.Unregister();

}

void CGraspCaptureDlg::OnKill() 
{

	logText += "Sending the KILL command..";
	
	cmd_outport.Content() = CCMDQuit;
	cmd_outport.Write();
	rep_inport.Read();
	int reply = rep_inport.Content();
	if ( reply == CMD_ACK) {
		logText += " Done.";
	} else {
		logText += " Failed.";
		MessageBox("Problems killing the server.", "Error.", MB_ICONERROR);
	}

	MessagesDialog.m_MessageBox.SetWindowText(logText);

	OnOK();

}

void CGraspCaptureDlg::OnOptions() 
{

	BOOL ret;

	setupOptionsDialog();

	ret = OptionsDialog.DoModal();

	if (ret) {
		readFromOptionsDialog();
	}
}

void CGraspCaptureDlg::setupOptionsDialog()
{

	OptionsDialog.m_NetName = options.netName;
	OptionsDialog.m_PortName = options.portName;
	OptionsDialog.m_Prefix = options.prefix;
	OptionsDialog.m_SavePath = options.savePath;
	OptionsDialog.m_RefreshTime = options.refreshFrequency;

}

void CGraspCaptureDlg::readFromOptionsDialog()
{

	ACE_OS::strcpy(options.netName,(LPCSTR)(OptionsDialog.m_NetName) );
	ACE_OS::strcpy(options.portName,(LPCSTR)(OptionsDialog.m_PortName) );
	ACE_OS::strcpy(options.prefix,(LPCSTR)(OptionsDialog.m_Prefix) );
	ACE_OS::strcpy(options.savePath,(LPCSTR)(OptionsDialog.m_SavePath) );
	options.refreshFrequency = OptionsDialog.m_RefreshTime;

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
