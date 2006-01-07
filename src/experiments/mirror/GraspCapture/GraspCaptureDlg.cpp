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
	: CDialog(CGraspCaptureDlg::IDD, pParent),
	  _data_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
	  _img0_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
	  _img1_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
	  _cmd_inport (YARPInputPort::NO_BUFFERS, YARP_TCP),
	  _cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP)
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

	// create and set up dialogs
	GloveDialog.Create(CLiveGloveDlg::IDD, this);
	Tracker0Dialog.Create(CLiveTrackerDlg::IDD, this);
	Tracker0Dialog.SetWindowText("LIVE: Tracker #0");
	Tracker1Dialog.Create(CLiveTrackerDlg::IDD, this);
	Tracker1Dialog.SetWindowText("LIVE: Tracker #1");
	Camera0Dialog.Create(CLiveCameraDlg::IDD, this);
	Camera0Dialog.SetWindowText("LIVE: Camera #0");
	Camera1Dialog.Create(CLiveCameraDlg::IDD, this);
	Camera1Dialog.SetWindowText("LIVE: Camera #1");
	// enable windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);

	nSeq = 0;

	if ( ! registerPorts() ) {
		MessageBox("Ports Registration Failed.", "Fatal error.",MB_ICONERROR);
		exit(YARP_FAIL);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control

}

void CGraspCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
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

void CGraspCaptureDlg::OnLiveCamera() 
{

	// toggle dialogs
	Camera0Dialog.ShowWindow(!Camera0Dialog.ShowWindow(SW_SHOWNA));
	Camera1Dialog.ShowWindow(!Camera1Dialog.ShowWindow(SW_SHOWNA));

}

void CGraspCaptureDlg::OnLiveGlove() 
{

	// toggle dialog
	GloveDialog.ShowWindow(!GloveDialog.ShowWindow(SW_SHOWNA));

}

void CGraspCaptureDlg::OnLiveTracker() 
{

	// toggle dialogs
	Tracker0Dialog.ShowWindow(!Tracker0Dialog.ShowWindow(SW_SHOWNA));
	Tracker1Dialog.ShowWindow(!Tracker1Dialog.ShowWindow(SW_SHOWNA));
	
}

void CGraspCaptureDlg::OnConnect() 
{

	// send connect command to collector
	_cmd_outport.Content() = CCmdConnect;
	_cmd_outport.Write(true);
	_cmd_inport.Read();
	if ( _cmd_inport.Content() == CCmdSucceeded ) {
		_cmd_inport.Read();
		int tmpOptions = _cmd_inport.Content();
		// set options accordingly - resize images
		_options.useDataGlove = ((tmpOptions & HardwareUseDataGlove) ? true : false);
		_options.useGazeTracker = ((tmpOptions & HardwareUseGT) ? true : false);
		_options.useTracker0  = ((tmpOptions & HardwareUseTracker0) ? true : false);
		_options.useTracker1  = ((tmpOptions & HardwareUseTracker1) ? true : false);
		_options.usePresSens  = ((tmpOptions & HardwareUsePresSens) ? true : false);
		_options.useCamera0   = ((tmpOptions & HardwareUseCamera0) ? true : false);
		_options.useCamera1   = ((tmpOptions & HardwareUseCamera1) ? true : false);
		if ( _options.useCamera0 || _options.useCamera1 ) {
			_cmd_inport.Read();
			_options.sizeX = _cmd_inport.Content();
			_cmd_inport.Read();
			_options.sizeY = _cmd_inport.Content();
			_img0.Resize (_options.sizeX, _options.sizeY);
			_img1.Resize (_options.sizeX, _options.sizeY);
		}
		// configure saver thread
		saverThread.useCamera0 = _options.useCamera0;
		saverThread.useCamera1 = _options.useCamera1;
		saverThread.useGazeTracker = _options.useGazeTracker;
		saverThread.useDataGlove = _options.useDataGlove;
		saverThread.usePresSens = _options.usePresSens;
		saverThread.useTracker0 = _options.useTracker0;
		saverThread.useTracker1 = _options.useTracker1;
		saverThread.pImg0 = &_img0;
		saverThread.pImg1 = &_img1;
		saverThread.pData = &_data;
		saverThread.p_data_inport = &_data_inport;
		saverThread.p_img0_inport = &_img0_inport;
		saverThread.p_img1_inport = &_img1_inport;
		// configure camera dialogs
		if ( _options.useCamera0 ) {
			Camera0Dialog.pImage = &_img0;
			Camera0Dialog.sizeX = _options.sizeX;
			Camera0Dialog.sizeY = _options.sizeY;
		}
		if ( _options.useCamera1 ) {
			Camera1Dialog.pImage = &_img1;
			Camera1Dialog.sizeX = _options.sizeX;
			Camera1Dialog.sizeY = _options.sizeY;
		}
		// enable the required windows
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_KILL)->EnableWindow(FALSE);
		if( _options.useCamera0 || _options.useCamera1 ) {
			GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		}
		if(_options.useDataGlove || _options.useGazeTracker || _options.usePresSens) {
			GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
		}
		if(_options.useTracker0 || _options.useTracker1) {
			GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
		// start live timer
		m_timerID = SetTimer(1, _options.refreshFrequency, NULL);
		_ASSERT (m_timerID != 0);	
	} else {
		MessageBox("Peripherals connection Failed.", "Error.",MB_ICONERROR);
	}	

}

bool CGraspCaptureDlg::registerPorts()
{

	char portName[255];
	
	// Data port registration
	ACE_OS::sprintf(portName,"/%s/i:str", _options.portName);
	if ( _data_inport.Register(portName, _options.netName) != YARP_OK ) {
		return false;
	}
	
	// Images port registration
	ACE_OS::sprintf(portName,"/%s/i:img0", _options.portName);
	if ( _img0_inport.Register(portName, _options.netName) != YARP_OK ) {
		return false;
	}

	ACE_OS::sprintf(portName,"/%s/i:img1", _options.portName);
	if ( _img1_inport.Register(portName, _options.netName) != YARP_OK ) {
		return false;
	}

	// Commands outport registration
	ACE_OS::sprintf(portName,"/%s/o:int", _options.portName);
	if ( _cmd_outport.Register(portName, _options.netName) != YARP_OK ) {
		return false;
	}

	// Commands inport registration
	ACE_OS::sprintf(portName,"/%s/i:int", _options.portName);
	if ( _cmd_inport.Register(portName, _options.netName) != YARP_OK ) {
		return false;
	}

	// connect ports
	system(connectScriptName);

	return true;

}

void CGraspCaptureDlg::unregisterPorts()
{

	// unregister ports
	_cmd_outport.Unregister();
	_cmd_inport.Unregister();
	_data_inport.Unregister();
	_img0_inport.Unregister();
	_img1_inport.Unregister();

}

void CGraspCaptureDlg::OnDisconnect() 
{

	// kill live timer
	KillTimer (m_timerID);

	// disconnect collector
	_cmd_outport.Content() = CCmdDisconnect;
	_cmd_outport.Write();
	_cmd_inport.Read();
	if ( _cmd_inport.Content() == CCmdFailed) {
		MessageBox("Peripherals disconnection Failed.", "Error.",MB_ICONERROR);
	}

	// enable/disable required windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_KILL)->EnableWindow(TRUE);

}

void CGraspCaptureDlg::OnOptions() 
{

	// fill the _options dialog with the current options
	OptionsDialog.m_NetName = _options.netName;
	OptionsDialog.m_PortName = _options.portName;
	OptionsDialog.m_Prefix = _options.prefix;
	OptionsDialog.m_SavePath = _options.savePath;
	OptionsDialog.m_RefreshTime = _options.refreshFrequency;

	// if the user wishes so, set new options values
	if ( OptionsDialog.DoModal() ) {
		ACE_OS::strcpy(_options.netName,(LPCSTR)(OptionsDialog.m_NetName) );
		ACE_OS::strcpy(_options.portName,(LPCSTR)(OptionsDialog.m_PortName) );
		ACE_OS::strcpy(_options.prefix,(LPCSTR)(OptionsDialog.m_Prefix) );
		ACE_OS::strcpy(_options.savePath,(LPCSTR)(OptionsDialog.m_SavePath) );
		_options.refreshFrequency = OptionsDialog.m_RefreshTime;
	}

}

void CGraspCaptureDlg::OnClose() 
{

	// unregister ports
	_data_inport.Unregister();
	_img0_inport.Unregister();
	_img1_inport.Unregister();
	_cmd_inport.Unregister();
	_cmd_outport.Unregister();

	CDialog::OnClose();

}

void CGraspCaptureDlg::OnTimer(UINT nIDEvent) 
{	

	// this is the timer event. each time the timer ticks, we gather data
	// off the collector and update the enabled windows.

	// get data
	_cmd_outport.Content() = CCmdGetData;
	_cmd_outport.Write();
	_cmd_inport.Read();
	if ( _cmd_inport.Content() == CCmdSucceeded ) {
		// if anything new is available, update data structures
		if ( _data_inport.Read(false) ) {
			_data = _data_inport.Content();
		}
		if ( _img0_inport.Read(false) ) {
			_img0.Refer(_img0_inport.Content());
		}
		if ( _img1_inport.Read(false) ) {
			_img1.Refer(_img1_inport.Content());
		}
	} else {
		MessageBox("Impossible to read data from collector server.", "Error.",MB_ICONERROR);
		m_timerID = NULL;
	}

	// update dialogs
	Tracker0Dialog.UpdateState(_data.tracker0Data);
	Tracker1Dialog.UpdateState(_data.tracker1Data);
	GloveDialog.UpdateState(_data.gloveData, _data.pressureData, _data.GTData);
	Camera0Dialog.UpdateState(&_img0);
	Camera1Dialog.UpdateState(&_img1);

	CDialog::OnTimer(nIDEvent);

}

void CGraspCaptureDlg::OnAcqStart() 
{

	// disable windows
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_QUIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_OPTIONS)->EnableWindow(FALSE);

	CFileFind finder;
	char fName[255];
	
	saverThread.pFile = NULL;
	ACE_OS::sprintf(saverThread.prefix, "%s\\%s%03d",_options.savePath, _options.prefix, nSeq);
	if ( _options.useDataGlove || _options.useGazeTracker ||
		 _options.useTracker0 || _options.useTracker1 || _options.usePresSens) {
		// if necessary, open the data output file
		ACE_OS::sprintf(fName, "%s.csv",saverThread.prefix);
		saverThread.pFile = fopen(fName,"w");
		if (saverThread.pFile == NULL) {
			MessageBox("Impossible to open output file.", "Error.", MB_ICONERROR);
			GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
			GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
			GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
			GetDlgItem(IDC_QUIT)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);
			return;
		}
		saverThread.writeHeaderToFile();
	}

	cleanDataStructures();

	// activate collector's streaming mode
	_cmd_outport.Content() = CCmdStartStreaming;
	_cmd_outport.Write();
	_cmd_inport.Read();
	if ( _cmd_inport.Content() == CCmdSucceeded) {
		GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACQ_STOP)->EnableWindow(TRUE);
		// during streaming, kill timer
		KillTimer (m_timerID);
		// start acquisition thread
		saverThread.Begin();
	} else {
		MessageBox("Impossible to start saving thread.", "Error.", MB_ICONERROR);
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_QUIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);
	}

}

void CGraspCaptureDlg::OnAcqStop() 
{

	// stop collector's streaming mode
	_cmd_outport.Content() = CCmdStopStreaming;
	_cmd_outport.Write();
	if ( _cmd_inport.Content() != CCmdSucceeded) {
		MessageBox("Problems stopping saving thread.", "Error.", MB_ICONERROR);
	}

	// stop saving stream
	saverThread.End();
	// restart timer
	m_timerID = SetTimer(1, _options.refreshFrequency, NULL);
	_ASSERT (m_timerID != 0);	

	// then close the data file
	if (saverThread.pFile != NULL) {
		fclose(saverThread.pFile);
	}

	nSeq++;

	// enable windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_QUIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTIONS)->EnableWindow(TRUE);
	if ( _options.useCamera0 || _options.useCamera1 )
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
	if (_options.useDataGlove || _options.useGazeTracker || _options.usePresSens)
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
	if (_options.useTracker0 || _options.useTracker1)
		GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);

}

void CGraspCaptureDlg::cleanDataStructures(void)
{

	// Pressure Sensors
	_data.pressureData.channelA = 0;
	_data.pressureData.channelB = 0;
	_data.pressureData.channelC = 0;
	_data.pressureData.channelD = 0;

	// Trackers
	_data.tracker0Data.x = 0.0;
	_data.tracker0Data.y = 0.0;
	_data.tracker0Data.z = 0.0;
	_data.tracker0Data.azimuth = 0.0;
	_data.tracker0Data.elevation = 0.0;
	_data.tracker0Data.roll = 0.0;
	_data.tracker1Data.x = 0.0;
	_data.tracker1Data.y = 0.0;
	_data.tracker1Data.z = 0.0;
	_data.tracker1Data.azimuth = 0.0;
	_data.tracker1Data.elevation = 0.0;
	_data.tracker1Data.roll = 0.0;

	// GazeTracker
	_data.GTData.pupilDiam = 0;
	_data.GTData.pupilX = 0;
	_data.GTData.pupilY = 0;

	// DataGlove
	_data.gloveData.thumb[0] = 0;	// inner
	_data.gloveData.thumb[1] = 0;	// middle
	_data.gloveData.thumb[2] = 0;	// outer
	_data.gloveData.index[0] = 0;	// inner
	_data.gloveData.index[1] = 0;	// middle
	_data.gloveData.index[2] = 0;	// outer
	_data.gloveData.middle[0] = 0;	// inner
	_data.gloveData.middle[1] = 0;	// middle
	_data.gloveData.middle[2] = 0;	// outer
	_data.gloveData.ring[0] = 0;	// inner
	_data.gloveData.ring[1] = 0;	// middle
	_data.gloveData.ring[2] = 0;	// outer
	_data.gloveData.pinkie[0] = 0;	// inner
	_data.gloveData.pinkie[1] = 0;	// middle
	_data.gloveData.pinkie[2] = 0;	// outer
	_data.gloveData.abduction[0] = 0; // thumb-index
	_data.gloveData.abduction[1] = 0; // index-middle
	_data.gloveData.abduction[2] = 0; // middle-ring
	_data.gloveData.abduction[3] = 0; // ring-pinkie
	_data.gloveData.abduction[4] = 0; // palm
	_data.gloveData.palmArch = 0;
	_data.gloveData.wrist[0] = 0; // pitch
	_data.gloveData.wrist[1] = 0; // yaw

	_img0.Zero();
	_img1.Zero();
}

void CGraspCaptureDlg::OnKill() 
{

	// kill collector
	_cmd_outport.Content() = CCmdQuit;
	_cmd_outport.Write();
	_cmd_inport.Read();
	if ( _cmd_inport.Content() == CCmdFailed ) {
		MessageBox("Problems killing the server.", "Error.", MB_ICONERROR);
	}

	OnOK();

}
