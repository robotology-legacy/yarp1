// BodyMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BodyMap.h"
#include "BodyMapDlg.h"

#include <yarp/YARPTime.h>
#include <yarp/YARPImages.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBodyMapDlg dialog

CBodyMapDlg::CBodyMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog( CBodyMapDlg::IDD, pParent ),
	  _saverThread( _options, _settings )
{

	//{{AFX_DATA_INIT(CBodyMapDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

}

void CBodyMapDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBodyMapDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CBodyMapDlg, CDialog)
	//{{AFX_MSG_MAP(CBodyMapDlg)
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
// CBodyMapDlg message handlers

BOOL CBodyMapDlg::OnInitDialog()
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
	_GloveDialog.Create(CLiveGloveDlg::IDD, this);
	_Tracker0Dialog.Create(CLiveTrackerDlg::IDD, this);
	_Tracker0Dialog.SetWindowText("LIVE: Tracker #0");
	_Tracker1Dialog.Create(CLiveTrackerDlg::IDD, this);
	_Tracker1Dialog.SetWindowText("LIVE: Tracker #1");
	_Camera0Dialog.Create(CLiveCameraDlg::IDD, this);
	_Camera0Dialog.SetWindowText("LIVE: Camera #0");
	_Camera1Dialog.Create(CLiveCameraDlg::IDD, this);
	_Camera1Dialog.SetWindowText("LIVE: Camera #1");
	// enable windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);

	if ( RegisterAndConnectPorts() == YARP_FAIL ) {
		MessageBox("Could not register or connect ports.", "Fatal error.", MB_ICONERROR);
		exit(YARP_FAIL);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control

}

void CBodyMapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);

}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CBodyMapDlg::OnPaint() 
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
HCURSOR CBodyMapDlg::OnQueryDragIcon()
{

	return (HCURSOR) m_hIcon;

}

void CBodyMapDlg::OnLiveCamera() 
{

	// toggle dialogs
	if ( _options.useCamera0 ) {
		_Camera0Dialog.ShowWindow(!_Camera0Dialog.ShowWindow(SW_SHOWNA));
	}
	if ( _options.useCamera1 ) {
		_Camera1Dialog.ShowWindow(!_Camera1Dialog.ShowWindow(SW_SHOWNA));
	}

}

void CBodyMapDlg::OnLiveGlove() 
{

	// toggle dialog
	_GloveDialog.ShowWindow(!_GloveDialog.ShowWindow(SW_SHOWNA));

}

void CBodyMapDlg::OnLiveTracker() 
{

	// toggle dialogs
	if ( _options.useTracker0 ) {
		_Tracker0Dialog.ShowWindow(!_Tracker0Dialog.ShowWindow(SW_SHOWNA));
	}
	if ( _options.useTracker1 ) {
		_Tracker1Dialog.ShowWindow(!_Tracker1Dialog.ShowWindow(SW_SHOWNA));
	}
	
}

void CBodyMapDlg::OnConnect() 
{

	// send connect command to collector
	_settings._cmd_outport.Content() = CCmdConnect;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdSucceeded ) {
		_settings._cmd_inport.Read();
		int tmpOptions = _settings._cmd_inport.Content();
		// set options accordingly - resize images
		_options.useDataGlove = ((tmpOptions & HardwareUseDataGlove) ? true : false);
		_options.useGazeTracker = ((tmpOptions & HardwareUseGT) ? true : false);
		_options.useTracker0 = ((tmpOptions & HardwareUseTracker0) ? true : false);
		_options.useTracker1 = ((tmpOptions & HardwareUseTracker1) ? true : false);
		_options.usePresSens = ((tmpOptions & HardwareUsePresSens) ? true : false);
		_options.useCamera0 = ((tmpOptions & HardwareUseCamera0) ? true : false);
		_options.useCamera1 = ((tmpOptions & HardwareUseCamera1) ? true : false);
		if ( _options.useCamera0 || _options.useCamera1 ) {
			_settings._cmd_inport.Read();
			_options.sizeX = _settings._cmd_inport.Content();
			_settings._cmd_inport.Read();
			_options.sizeY = _settings._cmd_inport.Content();
			_settings._img0.Resize (_options.sizeX, _options.sizeY);
			_settings._img1.Resize (_options.sizeX, _options.sizeY);
		}
		// configure camera dialogs
		if ( _options.useCamera0 ) {
			_Camera0Dialog.pImage = &_settings._img0;
			_Camera0Dialog.sizeX = _options.sizeX;
			_Camera0Dialog.sizeY = _options.sizeY;
		}
		if ( _options.useCamera1 ) {
			_Camera1Dialog.pImage = &_settings._img1;
			_Camera1Dialog.sizeX = _options.sizeX;
			_Camera1Dialog.sizeY = _options.sizeY;
		}
		// enable the required windows
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
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
		_settings._timerID = SetTimer(1, _options.refreshFrequency, NULL);
		_ASSERT (_settings._timerID != 0);	
	} else {
		MessageBox("Could not connect to mirrorCollector.", "Error.",MB_ICONERROR);
	}	

}

void CBodyMapDlg::OnDisconnect() 
{

	// kill live timer
	KillTimer (_settings._timerID);

	// disconnect collector
	_settings._cmd_outport.Content() = CCmdDisconnect;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdFailed) {
		MessageBox("Could not disconnect from mirrorCollector.", "Error.",MB_ICONERROR);
	}

	// enable/disable required windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);

}

void CBodyMapDlg::OnClose() 
{

	if ( GetDlgItem(IDC_DISCONNECT)->IsWindowEnabled() ) {
		OnDisconnect();
	}
	DisconnectAndUnregisterPorts();
	CDialog::OnClose();

}

void CBodyMapDlg::OnTimer(UINT nIDEvent)
{

	// this is the timer event. each time the timer ticks, we gather data
	// off the collector and update the enabled windows.

	// get data
	_settings._cmd_outport.Content() = CCmdGetData;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdSucceeded ) {
		// update data structures, when needed
		if ( _options.useDataGlove || _options.useGazeTracker ||
		     _options.useTracker0 || _options.useTracker1 || _options.usePresSens ) {
			if ( _settings._data_inport.Read(false) ) {
				_settings._data = _settings._data_inport.Content();
			}
		}
		if ( _options.useCamera0 ) {
			if ( _settings._img0_inport.Read(false) ) {
				_settings._img0.Refer(_settings._img0_inport.Content());
				ShowTrackerXY(_settings._img0);
			}
		}
		if ( _options.useCamera1 ) {
			if ( _settings._img1_inport.Read(false) ) {
				_settings._img1.Refer(_settings._img1_inport.Content());
				ShowTrackerXY(_settings._img1);
			}
		}
	} else {
		MessageBox("Could not read data from mirrorCollector.", "Error.",MB_ICONERROR);
	}

	// update dialogs
	_Tracker0Dialog.UpdateState(_settings._data.tracker0Data);
	_Tracker1Dialog.UpdateState(_settings._data.tracker1Data);
	_GloveDialog.UpdateState(_settings._data.gloveData, _settings._data.pressureData, _settings._data.GTData);
	_Camera0Dialog.UpdateState(&_settings._img0);
	_Camera1Dialog.UpdateState(&_settings._img1);

	CDialog::OnTimer(nIDEvent);

}

void CBodyMapDlg::OnAcqStart()
{

	// disable windows
	GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);

	// have acquisition thread open its own file
	if ( _saverThread.openFile() == YARP_FAIL ) {
		MessageBox("Could not open output file.", "Error.", MB_ICONERROR);
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		return;
	}

	// activate collector's streaming mode
	_settings._cmd_outport.Content() = CCmdStartStreaming;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdSucceeded) {
		GetDlgItem(IDC_ACQ_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACQ_STOP)->EnableWindow(TRUE);
		// during streaming, kill timer
		KillTimer (_settings._timerID);
		// start acquisition thread
		_saverThread.Begin();
	} else {
		MessageBox("Could not start saving thread.", "Error.", MB_ICONERROR);
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	}

}

void CBodyMapDlg::OnAcqStop() 
{

	// stop saving stream. HAVE to do this before stopping the
	// remote stream, otherwise OUR stream will dead lock 
	_saverThread.End();

	// stop collector's streaming mode
	_settings._cmd_outport.Content() = CCmdStopStreaming;
	_settings._cmd_outport.Write(true);
	_settings._cmd_inport.Read();
	if ( _settings._cmd_inport.Content() == CCmdFailed ) {
		MessageBox("Could not stop saving thread.", "Error.", MB_ICONERROR);
	}

	// restart timer
	_settings._timerID = SetTimer(1, _options.refreshFrequency, NULL);
	_ASSERT (_settings._timerID != 0);	

	// close the acquisition thread file
	_saverThread.closeFile();

	// increment senqeuce number
	++_settings._sequenceNumber;

	// enable windows
	GetDlgItem(IDC_ACQ_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_ACQ_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	if ( _options.useCamera0 || _options.useCamera1 )
		GetDlgItem(IDC_LIVE_CAMERA)->EnableWindow(TRUE);
	if (_options.useDataGlove || _options.useGazeTracker || _options.usePresSens)
		GetDlgItem(IDC_LIVE_GLOVE)->EnableWindow(TRUE);
	if (_options.useTracker0 || _options.useTracker1)
		GetDlgItem(IDC_LIVE_TRACKER)->EnableWindow(TRUE);

}

// ---------------- procedures which are not associated with MFC objects

int CBodyMapDlg::RegisterAndConnectPorts()
{

	char tmpBMPortName[255];
	char tmpMCPortName[255];

	// ----------- data
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:str", _settings.BodyMapPortName);
	if ( _settings._data_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:str", _settings.MirrorCollectorPortName);
	if ( _settings._data_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	// ----------- images
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:img0", _settings.BodyMapPortName);
	if ( _settings._img0_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img0", _settings.MirrorCollectorPortName);
	if ( _settings._img0_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:img1", _settings.BodyMapPortName);
	if ( _settings._img1_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img1", _settings.MirrorCollectorPortName);
	if ( _settings._img1_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	// ----------- commands
	ACE_OS::sprintf(tmpBMPortName,"/%s/o:int", _settings.BodyMapPortName);
	if ( _settings._cmd_outport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/i:int", _settings.MirrorCollectorPortName);
	if ( _settings._cmd_outport.Connect(tmpBMPortName, tmpMCPortName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpBMPortName,"/%s/i:int", _settings.BodyMapPortName);
	if ( _settings._cmd_inport.Register(tmpBMPortName, _settings.netName) != YARP_OK ) {
		return YARP_FAIL;
	}
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:int", _settings.MirrorCollectorPortName);
	if ( _settings._cmd_inport.Connect(tmpMCPortName, tmpBMPortName) != YARP_OK ) {
		return YARP_FAIL;
	}

	return YARP_OK;

}

void CBodyMapDlg::DisconnectAndUnregisterPorts()
{

	char tmpBMPortName[255];
	char tmpMCPortName[255];

	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:int", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:int", _settings.MirrorCollectorPortName);
	_settings._cmd_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._cmd_inport.Unregister();

	ACE_OS::sprintf(tmpBMPortName,"/%s/o:int", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"!/%s/i:int", _settings.MirrorCollectorPortName);
	_settings._cmd_outport.Connect(tmpBMPortName, tmpMCPortName);
	_settings._cmd_outport.Unregister();
	
	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:img1", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img1", _settings.MirrorCollectorPortName);
	_settings._img1_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._img1_inport.Unregister();

	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:img0", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:img0", _settings.MirrorCollectorPortName);
	_settings._img0_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._img0_inport.Unregister();

	ACE_OS::sprintf(tmpBMPortName,"!/%s/i:str", _settings.BodyMapPortName);
	ACE_OS::sprintf(tmpMCPortName,"/%s/o:str", _settings.MirrorCollectorPortName);
	_settings._data_inport.Connect(tmpMCPortName, tmpBMPortName);
	_settings._data_inport.Unregister();

}

void CBodyMapDlg::ShowTrackerXY(YARPImageOf<YarpPixelBGR>& img)
{

	YarpPixelBGR tmpBGRWhitePixel(255,255,255);

	int x, y, w;

	FindTrackerXY(img, &x, &y, &w);
	
	if ( x>0 && y>0 ) {
		YARPSimpleOperations::DrawCross<YarpPixelBGR>(img, x, y, tmpBGRWhitePixel, 5);
	}

}
