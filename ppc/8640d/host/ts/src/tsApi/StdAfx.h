// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6CF8B44B_EA02_4BBD_809A_2C3D33B9549E__INCLUDED_)
#define AFX_STDAFX_H__6CF8B44B_EA02_4BBD_809A_2C3D33B9549E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#ifdef WIN32
#pragma comment(lib, "wsock32")
#endif

#include <windows.h>         // MFC core and standard components

#include <stdio.h>
#include <iostream>

#include "winsock2.h"
#pragma warning(disable:4786)

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6CF8B44B_EA02_4BBD_809A_2C3D33B9549E__INCLUDED_)
