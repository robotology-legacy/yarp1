// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7DF70FD0_7DFF_40A5_82C5_E2D356186ACF__INCLUDED_)
#define AFX_STDAFX_H__7DF70FD0_7DFF_40A5_82C5_E2D356186ACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400
#define WINVER 0x0400

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define ACE_HAS_MFC 0

#include <yarp/YARPConfig.h>
#include <yarp/YARPRobotHardware.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPScheduler.h>

/* select the device driver to use */
#ifndef __ESD_DRIVER
#define __ESD_DRIVER
#endif

#ifndef __ESD_DRIVER
#include <yarp/YARPValueCanDeviceDriver.h>
#else
#include <yarp/YARPEsdCanDeviceDriver.h>
#endif

#include <yarp/YARPControlBoardUtils.h>
#include <yarp/YARPConfigFile.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7DF70FD0_7DFF_40A5_82C5_E2D356186ACF__INCLUDED_)
