// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5A4AF8F9_850B_4DE7_B7E1_9091CB3EA57A__INCLUDED_)
#define AFX_STDAFX_H__5A4AF8F9_850B_4DE7_B7E1_9091CB3EA57A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400
#define WINVER 0x0400

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#define ACE_HAS_MFC 0

#include <yarp/YARPConfig.h>
#include <yarp/YARPRateThread.h>
#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPRobotHardware.h>

#include <yarp/YARPADCUtils.h>
#include <yarp/YARPEsdDaqDeviceDriver.h>

#include <ace/config.h>
#include <ace/OS.h>

#include <iostream>
#include <math.h>

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPMath.h>
#include <yarp/YARPRobotMath.h>

#include "CanControlParams.h"



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__5A4AF8F9_850B_4DE7_B7E1_9091CB3EA57A__INCLUDED_)
