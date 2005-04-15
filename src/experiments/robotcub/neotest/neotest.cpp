// neotest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "icsnVC40.h"		// include the neoVI header information

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];		// The title bar text


// neoVI specific stuff ////////////////////////////////////////////////////////////////////////
int hObject = 0;							// holds a handle to the neoVI object
unsigned long mNumberOfMessagesRead=0;		// holds the number of messages read
unsigned long mNumberOfErrorsRead=0;		// holds the number of messages read
bool m_bPortOpen=false;						// flag which indicates if the port is open or not
icsSpyMessage stMessages[19999];			// holds the received messages
//////////////////////////////////////////////////////////////////////////////////////////

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_NEOTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_NEOTEST);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_NEOTEST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_NEOTEST;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// this function displays the current messages to the main window
// it is called when we get a paint request
void PaintWindow(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int lCount;
	TCHAR szOut[200]; //
	long iCurrentX =0;
	long iCurrentY=0;
	INT cy,cx;
	RECT rect,rectCli;
	double dTime;
	int iTime;

	// get the device context
	hdc = BeginPaint(hWnd, &ps);
	
	// clear the back ground by drawing a rectangle
	GetClientRect(hWnd, &rectCli);	
	Rectangle(hdc, rectCli.left,rectCli.top, rectCli.right, rectCli.bottom);

	// Get the height and length of the string
	DrawText (hdc, TEXT ("XX"),  -1, &rect, 
				  DT_CALCRECT | DT_CENTER | DT_SINGLELINE);
	// determine the height of the characters
	cy = rect.bottom - rect.top + 3;

	// report how many messages and errors were read
	wsprintf (szOut,TEXT("Messages Read: %d    Errors Read: %d"),mNumberOfMessagesRead,mNumberOfErrorsRead);
	ExtTextOut(hdc,0,0,0,0,szOut, _tcslen(szOut),0); // write our string to the display

	// draw each message to the window
	for (lCount=0;lCount <mNumberOfMessagesRead;lCount++)
	{
		// Calculate the time for this message
		dTime = ((double) stMessages[lCount].TimeHardware2) * NEOVI_TIMESTAMP_2 + 
							((double) stMessages[lCount].TimeHardware) * NEOVI_TIMESTAMP_1;
		dTime *= 1000;				// convert to milli-seconds
		iTime = (int) dTime;		// conever to an integer

		iCurrentY += cy;	// increment the position
		// is the current message a CAN message
		if (stMessages[lCount].Protocol == SPY_PROTOCOL_CAN)
		{
			// copy a can style message
			wsprintf (szOut,TEXT("Time (ms) %d Message %d Network %d ArbID %X Data Length: %d b1: %X b2: %X b3: %X b4: %X b5: %X b6: %X b7: %X b8: %X") 
				,iTime,lCount,stMessages[lCount].NetworkID,stMessages[lCount].ArbIDOrHeader,
				stMessages[lCount].NumberBytesData,
				stMessages[lCount].Data[0],
				stMessages[lCount].Data[1],
				stMessages[lCount].Data[2],
				stMessages[lCount].Data[3],
				stMessages[lCount].Data[4],
				stMessages[lCount].Data[5],
				stMessages[lCount].Data[6],
				stMessages[lCount].Data[7]);
		}
		else
		{
				wsprintf (szOut,TEXT("Time (ms) %d Message %d Network %d Data Length: %d b1: %X b2: %X b3: %X b4: %X b5: %X b6: %X b7: %X b8: %X b9: %X b10: %X b11: %X") 
				,iTime,lCount,stMessages[lCount].NetworkID,
				stMessages[lCount].NumberBytesData + stMessages[lCount].NumberBytesHeader,
				((icsSpyMessageJ1850 *) stMessages)[lCount].Header[0], // type cast to a icsSpyMessageJ1850 type message to get to the header
				((icsSpyMessageJ1850 *) stMessages)[lCount].Header[1],
				((icsSpyMessageJ1850 *) stMessages)[lCount].Header[2],
				stMessages[lCount].Data[0],
				stMessages[lCount].Data[1],
				stMessages[lCount].Data[2],
				stMessages[lCount].Data[3],
				stMessages[lCount].Data[4],
				stMessages[lCount].Data[5],
				stMessages[lCount].Data[6],
				stMessages[lCount].Data[7]);

		}
		ExtTextOut(hdc,iCurrentX,iCurrentY,0,0,szOut, _tcslen(szOut),0); // write our string to the display
	}

	// call the endpaint routine
	EndPaint(hWnd, &ps);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	TCHAR szOut[200]; //	
	unsigned char bNetworkID[16];		// holds the network ids for openport
	unsigned char bSCPIDs[255];		// holds the network ids for openport
	int lNumberOfErrors;				// holds the number of errors in the error quene
	int lNumberOfMessages;				// holds the number of messages when reading messages
	unsigned long lResult;				// holds the return value of neoVI calls
	unsigned long lCount;				// general purpose counter variable
	icsSpyMessage stMsg;				// message for transmiting CAN frames
	icsSpyMessageJ1850 stJMsg;			// message for transmiting ISO and J1850 frames
	int iErrors[599];
	TCHAR szDescriptionShort[255];
	TCHAR szDescriptionLong[255];
	int lMaxLengthShort,lMaxLengthLong,lErrorSeverity,lRestartNeeded;
	int iDeviceTypes[255];
	int iComPort[255];
	int iSerialNum[255];
	int iOpenedStates[255];
	int iDeviceNumbers[255];
	int iNumDevices;
	unsigned char bConfigBytes[1024];
	int iNumConfigBytes=1024;
	int iOldCNF1;
	int iOldCNF2;
	int iOldCNF3;
	
	switch (message) 
	{
		case WM_TIMER:
			// Read the messages every timer event (1000 ms)
			if (m_bPortOpen) // only if the port is open
			{
				// call icsneoGetMessages to read out the messages
				lResult = icsneoGetMessages(hObject,stMessages,&lNumberOfMessages,&lNumberOfErrors);
				if (lResult != 0)
				{
					// a successful read
					mNumberOfErrorsRead = lNumberOfErrors;
					mNumberOfMessagesRead = lNumberOfMessages;	// store the number of messages in the current buffer
					InvalidateRect(hWnd, NULL, FALSE);			// force a repaint to display the messages
				}
			}
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_NEOVI_GETERRORS:
					// Read the errors from the DLL
					lResult = icsneoGetErrorMessages(hObject,iErrors,&lNumberOfErrors);
					if (lResult == 0)
						MessageBox(hWnd,TEXT("Problem Reading errors"),TEXT("neoVI Example"),0);
					// dump the neoVI errors
					if (lNumberOfErrors > 0)
					{
						for (lCount=0;lCount <lNumberOfErrors;lCount++)
						{
							wsprintf(szOut,TEXT("Error %d  - "),iErrors[lCount]);
							OutputDebugString(szOut);
							icsneoGetErrorInfo(iErrors[lCount],
								szDescriptionShort,szDescriptionLong,
								&lMaxLengthShort,&lMaxLengthLong,
								&lErrorSeverity,
								&lRestartNeeded);
							OutputDebugString(szDescriptionShort);
							OutputDebugString(TEXT("\n"));
						}
					}
					else
						OutputDebugString(TEXT("No Errors to report\n"));
					break;
				case IDM_NEOVI_TXHSCAN:
					// the transmit HScan menu item
					if (m_bPortOpen) // only if the port is opened
					{
						stMsg.ArbIDOrHeader = 0xFF;	// arbritration ID
						stMsg.NumberBytesData = 4;	// number of data bytes
						stMsg.Data[0] = 0x22;	/// Data elements
						stMsg.Data[1] = 0x52;
						stMsg.Data[2] = 0x12;
						stMsg.Data[3] = 0x28;
						stMsg.StatusBitField = 0;	// not extended or remote frame
						stMsg.StatusBitField2 = 0;
						// transmit the message
						lResult = icsneoTxMessages(hObject,&stMsg,NETID_HSCAN,1); 
						if (lResult == 0) // check for errors
							MessageBox(hWnd,TEXT("Problem Transmitting Messages"),TEXT("neoVI Example"),0);
					}
					else // must have port open to transmit
						MessageBox(hWnd,TEXT("Port Not Open"),TEXT("neoVI Example"),0);

					break;
				case IDM_NEOVI_TXMSCAN:
					if (m_bPortOpen) 
					{
						stMsg.ArbIDOrHeader = 0xFF;
						stMsg.NumberBytesData = 4;
						stMsg.Data[0] = 0x22;
						stMsg.Data[1] = 0x52;
						stMsg.Data[2] = 0x12;
						stMsg.Data[3] = 0x28;
						stMsg.StatusBitField = 0;
						stMsg.StatusBitField2 = 0;
						lResult = icsneoTxMessages(hObject,&stMsg,NETID_MSCAN,1);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Transmitting Messages"),TEXT("neoVI Example"),0);
					}
					else
						MessageBox(hWnd,TEXT("Port Not Open"),TEXT("neoVI Example"),0);

					break;
				case IDM_NEOVI_TXSWCAN:
					if (m_bPortOpen) 
					{
						stMsg.ArbIDOrHeader = 0xFF;
						stMsg.NumberBytesData = 4;
						stMsg.Data[0] = 0x22;
						stMsg.Data[1] = 0x52;
						stMsg.Data[2] = 0x12;
						stMsg.Data[3] = 0x28;
						stMsg.StatusBitField = 0;
						stMsg.StatusBitField2 = 0;
						lResult = icsneoTxMessages(hObject,&stMsg,NETID_SWCAN,1);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Transmitting Messages"),TEXT("neoVI Example"),0);
					}
					else
						MessageBox(hWnd,TEXT("Port Not Open"),TEXT("neoVI Example"),0);
					break;
				case IDM_NEOVI_TXJ1850VPW:
					if (m_bPortOpen) 
					{
						// Use the J1850 structure
						stJMsg.NumberBytesHeader = 3;  // J1850 structure uses a three byte header
						stJMsg.Header[0] = 0xFF;
						stJMsg.Header[1] = 0x03;
						stJMsg.Header[2] = 0x40;
						stJMsg.NumberBytesData = 4; 
						stJMsg.Data[0] = 0x22;
						stJMsg.Data[1] = 0x52;
						stJMsg.Data[2] = 0x12;
						stJMsg.Data[3] = 0x28;
						stMsg.StatusBitField = 0;
						stMsg.StatusBitField2 = 0;
						// transmit the message and cast the message structure to the proper type
						lResult = icsneoTxMessages(hObject,(icsSpyMessage * ) &stJMsg,NETID_JVPW,1);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Transmitting Messages"),TEXT("neoVI Example"),0);
					}
					else
						MessageBox(hWnd,TEXT("Port Not Open"),TEXT("neoVI Example"),0);
					break;
				case IDM_NEOVI_TXISO:
					if (m_bPortOpen) 
					{
						
						stJMsg.NumberBytesHeader = 3;
						stJMsg.Header[0] = 0xFF;
						stJMsg.Header[1] = 0x03;
						stJMsg.Header[2] = 0x40;
						stJMsg.NumberBytesData = 4;
						stJMsg.Data[0] = 0x22;
						stJMsg.Data[1] = 0x52;
						stJMsg.Data[2] = 0x12;
						stJMsg.Data[3] = 0x28;
						stMsg.StatusBitField = 0;
						stMsg.StatusBitField2 = 0;
						lResult = icsneoTxMessages(hObject,(icsSpyMessage * ) &stJMsg,NETID_ISO,1);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Transmitting Messages"),TEXT("neoVI Example"),0);
					}
					else
						MessageBox(hWnd,TEXT("Port Not Open"),TEXT("neoVI Example"),0);
					break;
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
					// Cloes the port if opened and free the memory
					if (m_bPortOpen) icsneoClosePort(hObject,&lNumberOfErrors);
					icsneoFreeObject(hObject);
					
					DestroyWindow(hWnd);
					break;
				case IDM_NEOVI_STOPREADING:
					KillTimer(hWnd,1);		// kill the timer
					break;
				case IDM_NEOVI_READMESSAGES:
					// open the first neoVI on USB
					SetTimer(hWnd,1,1000,0);
					break;
				case IDM_OPEN_RS232:
					// initialize the networkid array
					for (lCount=0;lCount<16;lCount++)
						bNetworkID[lCount] = lCount;
					// open the first neoVI on USB
					if (!m_bPortOpen) // only if not already opend 
					{
						lResult = icsneoOpenPort(3 ,NEOVI_COMMTYPE_RS232,  INTREPIDCS_DRIVER_STANDARD,bNetworkID,bSCPIDs, &hObject);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Opening Port"),TEXT("neoVI Example"),0);
						else
						{
							m_bPortOpen =true;
							MessageBox(hWnd,TEXT("Port Opened Successfully"),TEXT("neoVI Example"),0);
						}
					}
					break;
				case IDM_NEOVI_OPENPORT1:
					// initialize the networkid array
					for (lCount=0;lCount<16;lCount++)
						bNetworkID[lCount] = lCount;
					// open the first neoVI on USB
					if (!m_bPortOpen) // only if not already opend 
					{
						lResult = icsneoOpenPort(1 ,NEOVI_COMMTYPE_USB_BULK,  INTREPIDCS_DRIVER_STANDARD,bNetworkID,bSCPIDs, &hObject);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Opening Port"),TEXT("neoVI Example"),0);
						else
						{
							m_bPortOpen =true;
							MessageBox(hWnd,TEXT("Port Opened Successfully"),TEXT("neoVI Example"),0);
						}
					}
					break;
				case IDM_NEOVI_CLOSEPORT1:
					// close the port
					lResult = icsneoClosePort(hObject, &lNumberOfErrors);
					if (lResult == 0)
						MessageBox(hWnd,TEXT("Problem Closing Port"),TEXT("neoVI Example"),0);
					else
					{
						m_bPortOpen =false;
						MessageBox(hWnd,TEXT("Port Closed Successfully"),TEXT("neoVI Example"),0);
					}
					break;
				case IDM_NEOVI_SETHSCANBAUDTO250K:
					//unsigned char bConfigBytes[1024];
					//int iNumConfigBytes=1024;

					if (m_bPortOpen) 
					{
						lResult = icsneoGetConfiguration(hObject, bConfigBytes, &iNumConfigBytes);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Reading Configuration"),TEXT("neoVI Example"),0);
						else
						{
							
							 iOldCNF1=bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF1];
							 iOldCNF2=bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF2];
							 iOldCNF3=bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF3];
							
							 // 250 K for Value CAN 500k for neoVI (neoVI and valuecan use different CAN controller rates)
							 bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF1] = 0x03;
							 bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF2] = 0xB8;
							 bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF3] = 0x05;

							 lResult = icsneoSendConfiguration(hObject, bConfigBytes, iNumConfigBytes);
							 
							 if (lResult == 0)
								 MessageBox(hWnd,TEXT("Problem Updating Configuration"),TEXT("neoVI Example"),0);
							 else
							 {
								 wsprintf(szOut,TEXT("Old Values: HSCAN CNF1 =  %x  HSCAN CNF2 =  %x  HSCAN CNF3 =  %x \n\nNew Values HSCAN CNF1 =  %x  HSCAN CNF2 =  %x  HSCAN CNF3 =  %x  "),
													iOldCNF1,
													iOldCNF2,
													iOldCNF3,
													bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF1],
													bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF2],
													bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF3]);
								 MessageBox(hWnd,szOut,TEXT("neoVI Example"),0);
							 }


					
						}
							
					}
					else
						MessageBox(hWnd,TEXT("Port Not Open"),TEXT("neoVI Example"),0);

					break;
				case IDM_NEOVI_SETHSCANBAUDTO500K:
					//unsigned char bConfigBytes[1024];
					//int iNumConfigBytes=1024;
					
					if (m_bPortOpen) 
					{
						lResult = icsneoGetConfiguration(hObject, bConfigBytes, &iNumConfigBytes);
						if (lResult == 0)
							MessageBox(hWnd,TEXT("Problem Reading Configuration"),TEXT("neoVI Example"),0);
						else
						{
							
							iOldCNF1=bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF1];
							iOldCNF2=bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF2];
							iOldCNF3=bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF3];
							
							// 500 K for Value CAN 1000k for neoVI (neoVI and valuecan use different CAN controller rates)
							bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF1] = 0x01;
							bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF2] = 0xB8;
							bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF3] = 0x05;
							
							lResult = icsneoSendConfiguration(hObject, bConfigBytes, iNumConfigBytes);
							
							if (lResult == 0)
								MessageBox(hWnd,TEXT("Problem Updating Configuration"),TEXT("neoVI Example"),0);
							else
							{
								wsprintf(szOut,TEXT("Old Values: HSCAN CNF1 =  %x  HSCAN CNF2 =  %x  HSCAN CNF3 =  %x \n\nNew Values HSCAN CNF1 =  %x  HSCAN CNF2 =  %x  HSCAN CNF3 =  %x  "),
									iOldCNF1,
									iOldCNF2,
									iOldCNF3,
									bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF1],
									bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF2],
									bConfigBytes[NEO_CFG_MPIC_HS_CAN_CNF3]);
								MessageBox(hWnd,szOut,TEXT("neoVI Example"),0);
							}
							
							
							
						}
						
					}
					else
						MessageBox(hWnd,TEXT("Port Not Open"),TEXT("neoVI Example"),0);
					
					break;
				case IDM_NEOVI_OPENPORTSERIAL:
					if (icsneoFindAllCOMDevices(INTREPIDCS_DRIVER_STANDARD, 0,1,1,iDeviceTypes,iComPort,iSerialNum,&iNumDevices))
					{
                        if (iNumDevices > 0)
                        {
						
							lResult = icsneoOpenPortEx(iComPort[0] ,NEOVI_COMMTYPE_RS232,  
													INTREPIDCS_DRIVER_STANDARD,0,57600,1,bNetworkID, &hObject);
							if (lResult == 0)
								MessageBox(hWnd,TEXT("Problem Opening Port"),TEXT("neoVI Example"),0);
							else
							{
								m_bPortOpen =true;
								MessageBox(hWnd,TEXT("Port Opened Successfully"),TEXT("neoVI Example"),0);
							}

								
                        }
						else
							MessageBox(hWnd,TEXT("Problem Opening Port"),TEXT("neoVI Example"),0);
					}
					else
						MessageBox(hWnd,TEXT("Problem Opening Port"),TEXT("neoVI Example"),0);
					break;
				case IDM_NEOVI_OPENPORT_FIRSTUSB:
					//FindAllUSBDevices(false,iDeviceNumbers,iSerialNum,iOpenedStates,iNumDevices);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			PaintWindow(hWnd);
			//hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			//RECT rt;
			//GetClientRect(hWnd, &rt);
			//DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			//EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
