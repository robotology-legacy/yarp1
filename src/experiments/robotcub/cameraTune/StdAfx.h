// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7787337D_1E57_45C3_AC35_DE78B87E420F__INCLUDED_)
#define AFX_STDAFX_H__7787337D_1E57_45C3_AC35_DE78B87E420F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//=============================================================================
// YARP Includes
//=============================================================================
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPImage.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPRobotHardware.h>
#include "YARPSusanFilter.h"
#include "YARPHoughTransform.h"

//=============================================================================
// System Includes
//=============================================================================
#include <Vfw.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7787337D_1E57_45C3_AC35_DE78B87E420F__INCLUDED_)
