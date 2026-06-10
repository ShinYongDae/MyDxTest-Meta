// MyDxTest.h : main header file for the MYDXTEST application
//

#if !defined(AFX_MYDXTEST_H__98726052_66EA_448F_B713_A343A9CA30EE__INCLUDED_)
#define AFX_MYDXTEST_H__98726052_66EA_448F_B713_A343A9CA30EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyDxTestApp:
// See MyDxTest.cpp for the implementation of this class
//

#include "gdiplus.h"
#pragma comment(lib,"gdiplus.lib")

using namespace Gdiplus;


class CMyDxTestApp : public CWinApp
{
	ULONG_PTR m_gdiplusToken; // 변수 하나 추가

public:
	CMyDxTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDxTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyDxTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDXTEST_H__98726052_66EA_448F_B713_A343A9CA30EE__INCLUDED_)
