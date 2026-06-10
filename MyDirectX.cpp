// MyDirectX.cpp : implementation file
//

#include "stdafx.h"
#include "MyDxTest.h"
#include "MyDirectX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDirectX
// template <class TBEGINPAINT>
// CMyDirectX<TBEGINPAINT>::CMyDirectX()
CMyDirectX::CMyDirectX()
{
}

// template <class TBEGINPAINT>
// CMyDirectX<TBEGINPAINT>::~CMyDirectX()
CMyDirectX::~CMyDirectX()
{
}

/*
BEGIN_MESSAGE_MAP(CMyDirectX, CWnd)
	//{{AFX_MSG_MAP(CMyDirectX)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/

/////////////////////////////////////////////////////////////////////////////
// CMyDirectX message handlers

/*
	set-up the primary surface its cooperation level and the 
	clipper, then initialise the color conversion structures
*/

// template <class TBEGINPAINT>
// BOOL CMyDirectX<TBEGINPAINT>::Init(HWND hWin)
BOOL CMyDirectX::Init(HWND hWin)
{
	if(NULL==hWin) return FALSE;

	m_hWin = hWin;
	
	// create DirectDrawObject
	HRESULT hr = ::DirectDrawCreate(NULL, &m_DirectDraw.p,NULL);
	if (FAILED(hr)) return false;
	hr = m_DirectDraw->SetCooperativeLevel(m_hWin,DDSCL_NORMAL);
	if (FAILED(hr)) return false;
	// create primary surface
	CDDSURFACEDESC dds(DDSD_CAPS, DDSCAPS_PRIMARYSURFACE);
	hr = m_DirectDraw->CreateSurface(&dds,&m_PrimarySurface.p, NULL);
	if (FAILED(hr)) return false;
	// attach clipper to window all blits will 
	// then be clipped to the primary surface
	hr = m_DirectDraw->CreateClipper(0, &m_Clipper.p, NULL);
	if (FAILED(hr)) return false;
	hr = m_Clipper->SetHWnd(0, m_hWin);
	if (FAILED(hr)) return false;
	// attach clipper to primary surface
	hr = m_PrimarySurface->SetClipper(m_Clipper);
	if (FAILED(hr)) return false;

	// work out the pixel format for colour conversions
	CDDPIXELFORMAT pf(m_PrimarySurface.p);
	if(pf.IsValid())
	{
		if(pf.InRGBMode())
		{
			DWORD tst = pf.GetBitCount();
			if(15 == tst) // 16 bit 555
			{
				std::auto_ptr<ClrConverter> rgbconverter(new CRGBTo16BIT555);
				rgbconverter_ = rgbconverter;
			}
			if(16 == tst) // 16 bit 565
			{
				std::auto_ptr<ClrConverter> rgbconverter(new CRGBTo16BIT565);
				rgbconverter_ = rgbconverter;
			}
			if(24 == tst) // 24 bit
			{
				std::auto_ptr<ClrConverter> rgbconverter(new CRGB24BIT);
				rgbconverter_ = rgbconverter;
			}
			if(32 == tst) // 32 bit
			{
				std::auto_ptr<ClrConverter> rgbconverter(new CRGB32BIT);
				rgbconverter_ = rgbconverter;
			}
		}
		else if(pf.InPaletteMode())
		{
			std::auto_ptr<ClrConverter> rgbconverter(new CRGB8BIT(m_PrimarySurface));
			rgbconverter_ = rgbconverter;
		}
		else
		{
			ATLTRACE("u r living on borrowed time - please retire now\n");
		}
	}

	return TRUE;
}

BOOL CMyDirectX::IsValid()
{
	return (m_PrimarySurface.p != NULL) ? TRUE : FALSE;
}

IDirectDrawSurface* CMyDirectX::CreateSecondarySurface(const int Width, const int Height)
{
	if(NULL!=m_PrimarySurface.p)
	{
		IDirectDrawSurface* ss =NULL;
		CDDSURFACEDESC dds(DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH, DDSCAPS_OFFSCREENPLAIN, Width, Height);
		HRESULT hr = m_DirectDraw->CreateSurface(&dds,&ss, NULL);
		if(SUCCEEDED(hr))
			return ss;
	}
	return NULL;
}

void CMyDirectX::fillclr(IDirectDrawSurface* surface, COLORREF BackFillColor)
{
	DDBLTFX bltfx; memset(&bltfx, 0,sizeof(DDBLTFX)); bltfx.dwSize = sizeof(DDBLTFX);
	// convert the rgb given into what ever pixel format we may have
	bltfx.dwFillColor = rgbconverter_->Convert(GetRValue(BackFillColor),
											   GetGValue(BackFillColor),
											   GetBValue(BackFillColor));
	HRESULT hr = surface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT,&bltfx);
}

BOOL CMyDirectX::ClientToScreen(LPRECT lpRect)
{
	if(!::ClientToScreen(m_hWin, (LPPOINT)lpRect))
		return FALSE;
	return ::ClientToScreen(m_hWin, ((LPPOINT)lpRect)+1);
}

BOOL CMyDirectX::SetPixel(int x, int y, UCHAR* pb, DDSURFACEDESC& ddsd, int Clr)
{
	*((USHORT *)(pb + x*2 + y*ddsd.lPitch)) = Clr;
	return true;
}

IDirectDrawPalette* CMyDirectX::Build256Palette()
{
	IDirectDrawPalette* ddp = 0;
	PALETTEENTRY pal[256];
	int i;

	// setup static windows entries
	for(i =0; i<10 ;i++)
	{
		//the first 10
		pal[i].peFlags = PC_EXPLICIT;
		pal[i].peRed = i;
		pal[i].peGreen = pal[i].peBlue = 0;

		// the last ten
		pal[i+246].peFlags = PC_EXPLICIT;
		pal[i+246].peRed = i+246;
		pal[i+246].peGreen = pal[i].peBlue = 0;
	}

	// default everything else	
	for(i=10; i<246 ;i++)
	{
		pal[i].peFlags = PC_NOCOLLAPSE;
		pal[i].peRed = pal[i].peGreen = pal[i].peBlue = 27;
	}

	HRESULT hr =m_DirectDraw->CreatePalette(DDPCAPS_8BIT, pal, &ddp, NULL);
	return ddp;
}

/*
	if needed create a new secondary surface
	and get the DC off it and return it
*/
// template <class TBEGINPAINT>
// HDC& CMyDirectX<TBEGINPAINT>::BeginPaint(COLORREF BackFillColor)
HDC& CMyDirectX::BeginPaint(COLORREF BackFillColor)
{
	m_CurrentHdc = NULL_HDC;
	if(!IsValid()) return m_CurrentHdc;

//	TBEGINPAINT::BeginPaint(m_hWin);

	XRect client; ::GetClientRect(m_hWin, &client); ClientToScreen(&client);

	// only create new secondary surface if the window has changed size
	if(m_CurrentRect != client) 
	{
		m_SecondarySurface = CreateSecondarySurface(client.Width(), client.Height());
		if(NULL==m_SecondarySurface.p) return m_CurrentHdc;
		m_CurrentRect = client;
	}
	// fill the secondary surface 
	fillclr(m_SecondarySurface, BackFillColor);
	
	// GetDC will _lock_ the surface for us
	m_SecondarySurface->GetDC(&m_CurrentHdc);
	return m_CurrentHdc;
}


/*
	simply blit the secondary surface onto the primary
	one we can't use flip as we are in windowed mode 
*/
// template <class TBEGINPAINT>
// BOOL CMyDirectX<TBEGINPAINT>::EndPaint()
BOOL CMyDirectX::EndPaint()
{
	if(NULL_HDC!=m_CurrentHdc)
	{
		// ReleaseDC will unlock the surface
		m_SecondarySurface->ReleaseDC(m_CurrentHdc);

		// copy all of the secondary surface onto the primary
		HRESULT hr = m_PrimarySurface->Blt(&m_CurrentRect, // destination
					  m_SecondarySurface, NULL,  // Source surface, Source rectangle NULL = entire surface
					  DDBLT_WAIT, NULL);
		if(SUCCEEDED(hr))
		{
//			TBEGINPAINT::EndPaint(m_hWin);
			return TRUE;
		}
	}
	return FALSE;
}
