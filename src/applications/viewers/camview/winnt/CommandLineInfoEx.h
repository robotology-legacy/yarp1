// CommandLineInfoEx.h: interface for the CCommandLineInfoEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDLINEINFOEX_H__8EC8AA2D_2E94_43C2_A8B0_10BE866D161A__INCLUDED_)
#define AFX_COMMANDLINEINFOEX_H__8EC8AA2D_2E94_43C2_A8B0_10BE866D161A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////
// Microsoft Systems Journal -- October 1999
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0, runs on Windows 98 and probably Windows NT too.
//

// Improved CCommandLineInfo parses arbitrary switches.
// Use instead of CCommandLineInfo:
//
//    CCommandLineInfoEx cmdinfo;
//    ParseCommandLine(cmdinfo); // (from app object)
//
// After parsing, you can call GetOption to get the value of any switch. Eg:
//
//    if (cmdinfo.GetOption("nologo")) {
//       // handle it
//    }
//
// to get the value of a string option, type
//
//    CString filename;
//    if (cmdinfo.GetOption("f")) {
//       // now filename is string following -f option
//    }
//

class CCommandLineInfoEx : public CCommandLineInfo 
{
public:
	BOOL GetOption(LPCTSTR option, CString& val);
	BOOL GetOption(LPCTSTR option) { return GetOption(option, CString()); }

protected:
	CMapStringToString m_options; // hash of options
	CString  m_sLastOption;       // last option encountered
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
};

#endif // !defined(AFX_COMMANDLINEINFOEX_H__8EC8AA2D_2E94_43C2_A8B0_10BE866D161A__INCLUDED_)
