// Register.h : main header file for the REGISTER application
//

#if !defined(AFX_REGISTER_H__91F2650B_1A3B_4F5F_8A9F_7DCA9FE1579D__INCLUDED_)
#define AFX_REGISTER_H__91F2650B_1A3B_4F5F_8A9F_7DCA9FE1579D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRegisterApp:
// See Register.cpp for the implementation of this class
//

class CRegisterApp : public CWinApp
{
public:
	CRegisterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterApp)
	public:
	virtual BOOL InitInstance();

	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRegisterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTER_H__91F2650B_1A3B_4F5F_8A9F_7DCA9FE1579D__INCLUDED_)