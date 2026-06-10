#if !defined(AFX_MYDIRECTX_H__2DD1DD98_6FF6_4EA0_9339_6A2DDF1E624F__INCLUDED_)
#define AFX_MYDIRECTX_H__2DD1DD98_6FF6_4EA0_9339_6A2DDF1E624F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDirectX.h : header file
//

#include "MyDxDef.h"

/////////////////////////////////////////////////////////////////////////////
// CMyDirectX window


// template <class TBEGINPAINT = TDoBeginPaint>
// class CMyDirectX : public TBEGINPAINT
class CMyDirectX : public CWnd
{
	CComPtr<IDirectDraw> m_DirectDraw;
	CComPtr<IDirectDrawSurface> m_PrimarySurface, m_SecondarySurface;
	CComPtr<IDirectDrawClipper> m_Clipper;

	HWND m_hWin;
	std::auto_ptr<ClrConverter> rgbconverter_;

	HDC m_CurrentHdc;
	XRect m_CurrentRect;
//	TBEGINPAINT m_tBP;

	BOOL IsValid();
	IDirectDrawSurface* CreateSecondarySurface(const int Width, const int Height);
	void fillclr(IDirectDrawSurface* surface, COLORREF BackFillColor);
	BOOL ClientToScreen(LPRECT lpRect);
	BOOL SetPixel(int x, int y, UCHAR* pb, DDSURFACEDESC& ddsd, int Clr);
	IDirectDrawPalette* Build256Palette();


// Construction
public:
	CMyDirectX();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDirectX)
	//}}AFX_VIRTUAL

// Implementation
public:
//	virtual ~CMyDirectX();
// 	template <class TBEGINPAINT = TDoBeginPaint>
 	~CMyDirectX();

	BOOL Init(HWND hWin);
	HDC& BeginPaint(COLORREF BackFillColor = 0);
	BOOL EndPaint();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyDirectX)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
};


// typedef CMyDirectX<TDoBeginPaint> CMyDirectX_BP;
// // use this one if you call your BeginPaint/EndPaint or use CDC
// typedef CMyDirectX<TDontBeginPaint> CMyDirectX_NBP;


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDIRECTX_H__2DD1DD98_6FF6_4EA0_9339_6A2DDF1E624F__INCLUDED_)
