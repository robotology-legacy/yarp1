// BodyMap.h : main header file for the BodyMap application
//

#if !defined(AFX_BodyMap_H__99B381EA_5725_492E_ACE4_4A9BABC2F469__INCLUDED_)
#define AFX_BodyMap_H__99B381EA_5725_492E_ACE4_4A9BABC2F469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// ----------- general functions, used by all classes of the application
void FindTrackerXY(YARPImageOf<YarpPixelBGR>&, int*, int*, int*);

// ----------- program options, coming from the collector
typedef struct BodyMapOptionsStruct {
	BodyMapOptionsStruct() {
		useCamera0 = useCamera1 = false;
		sizeX = sizeY = 0;
		useTracker0 = useTracker1 = useGazeTracker = useDataGlove = usePresSens = false;
		refreshFrequency = 40;
	};
	bool useCamera0, useCamera1;
	int	sizeX;
	int sizeY;
	bool useTracker0, useTracker1, useGazeTracker, useDataGlove, usePresSens;
	int refreshFrequency;
} BodyMapOptions;

// ----------- program settings
typedef struct BodyMapSettingsStruct {
	BodyMapSettingsStruct() :
	  _data_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
	  _img0_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
	  _img1_inport (YARPInputPort::DEFAULT_BUFFERS, YARP_TCP),
	  _cmd_inport (YARPInputPort::NO_BUFFERS, YARP_TCP),
	  _cmd_outport (YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP),
	  _sequenceNumber(0), _timerID(0)
	{
		ACE_OS::strcpy(BodyMapPortName, "BodyMap");
		ACE_OS::strcpy(MirrorCollectorPortName, "mirrorCollector");
		ACE_OS::strcpy(netName, "default");
		ACE_OS::strcpy(savePath, "d:\\tmp");
		ACE_OS::strcpy(saveFilenamePrefix, "BodyMapSequence");
	};
	// strings related to ports and save-file
	char BodyMapPortName[255];
	char MirrorCollectorPortName[255];
	char netName[255];
	char savePath[255];
	char saveFilenamePrefix[255];
	// data coming from the collector
	CollectorNumericalData _data;
	CollectorImage         _img0;
	CollectorImage         _img1;
	// the saved sequence order number
	int _sequenceNumber;
	// communication ports: data
	YARPInputPortOf<CollectorNumericalData> _data_inport;
	YARPInputPortOf<YARPGenericImage>       _img0_inport;
	YARPInputPortOf<YARPGenericImage>       _img1_inport;
	// communication ports: commands
	YARPInputPortOf<int>  _cmd_inport;
	YARPOutputPortOf<int> _cmd_outport;
	// ID of live acquisition timer
	UINT _timerID;
} BodyMapSettings;

/////////////////////////////////////////////////////////////////////////////
// CBodyMapApp:
// See BodyMap.cpp for the implementation of this class
//

class CBodyMapApp : public CWinApp
{
public:
	CBodyMapApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBodyMapApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBodyMapApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BodyMap_H__99B381EA_5725_492E_ACE4_4A9BABC2F469__INCLUDED_)
