// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__64D999AE_2299_4E4D_9FAB_95378D6658B7__INCLUDED_)
#define AFX_STDAFX_H__64D999AE_2299_4E4D_9FAB_95378D6658B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxmt.h>


// VS2008 이하로 컴파일할때는 아래 주석을 해제
#if(_MSC_VER<1500)
	#define UNDER_VS2008
#endif
#ifdef UNDER_VS2008
	#ifndef _In_
		#define _In_
	#endif
	#define CAtlString	CString
#endif

#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__64D999AE_2299_4E4D_9FAB_95378D6658B7__INCLUDED_)
