// CommandLineInfoEx.cpp: implementation of the CCommandLineInfoEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "camview.h"
#include "CommandLineInfoEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////
// Microsoft Systems Journal -- October 1999
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0, runs on Windows 98 and probably Windows NT 
//

// Parse a command line parameter/token. Just add it to the table.
// 
void CCommandLineInfoEx::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag) 
	{
		// this is a "flag" (begins with / or -)
		m_options[pszParam] = "TRUE";    // default value is "TRUE"
		m_sLastOption = pszParam;        // save in case other value specified   
	} 
	else 
	if (!m_sLastOption.IsEmpty()) 
	{
		// last token was option: set value
		m_options[m_sLastOption] = pszParam;
		m_sLastOption.Empty(); // clear
	}   // Call base class so MFC can see this param/token.

	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}

BOOL CCommandLineInfoEx::GetOption(LPCTSTR option, CString& val)
{
   return m_options.Lookup(option, val);
}

