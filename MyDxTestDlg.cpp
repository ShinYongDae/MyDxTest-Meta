// MyDxTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyDxTest.h"
#include "MyDxTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDxTestDlg dialog

CMyDxTestDlg::CMyDxTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDxTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDxTestDlg)
	m_dPixelResolution = 10.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pRs274X = NULL;
	m_bDraw = FALSE;
	m_hMetaFile = NULL;
	m_metaHdc = NULL;
}

CMyDxTestDlg::~CMyDxTestDlg()
{
//	m_memDC.DeleteDC();
	
	if(m_pRs274X)
	{
		delete m_pRs274X;
		m_pRs274X = NULL;
	}

	if (m_hMetaFile)
	{
		::DeleteEnhMetaFile(m_hMetaFile);
		m_hMetaFile = NULL;
	}
}

void CMyDxTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDxTestDlg)
	DDX_Control(pDX, IDC_SCROLLBAR_H, m_scroll_H);
	DDX_Control(pDX, IDC_SCROLLBAR_V, m_scroll_V);
	DDX_Text(pDX, IDC_EDIT_PIXEL_RESOLUTION, m_dPixelResolution);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDxTestDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDxTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_PIXEL_RESOLUTION, OnChangeEditPixelResolution)
	ON_BN_CLICKED(IDC_FIT, OnFit)
	ON_BN_CLICKED(IDC_ZOOM, OnZoom)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	//}}AFX_MSG_MAP
	//ON_MESSAGE(WM_DISP_PROCESS, OnDispProgress)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDxTestDlg message handlers
//void CMyDxTestDlg::OnDispProgress(WPARAM wPara, LPARAM lPara)
//{
//	CString sMsg;
//	int nCurr = (int)wPara;
//	int nTot = (int)lPara;
//
//	sMsg.Format("%d / %d", nCurr, nTot);
//	this->SetWindowText(sMsg);
//	GetDlgItem(IDC_STATIC_PROGRESS)->SetWindowText(sMsg);
//}

BOOL CMyDxTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	if(!m_pRs274X)
		m_pRs274X = new CFileFormatRs274X(GetDlgItem(IDC_STATIC_PIC));

	HWND hWnd = GetDlgItem(IDC_STATIC_PIC)->GetSafeHwnd();
	if(!myDx.Init(hWnd))
	{
		AfxMessageBox("Error DirectX Initialize.");
		return FALSE;
	}

	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);

/*
	HDC  hdc = ::GetDC(GetDlgItem(IDC_STATIC_PIC)->m_hWnd);
	CDC* cdc = CDC::FromHandle(hdc);
	m_memDC.CreateCompatibleDC(cdc);

	int nBitmapH, nBitmapV;
 	nBitmapH = 8124;//client.Width() / 0.1;
 	nBitmapV = 8124;//client.Height() / 0.1;
//	nBitmapH = int(1000.0 * 1000.0 / m_dPixelResolution);
//	nBitmapV = int(1000.0 * 1000.0 / m_dPixelResolution);
	m_Bitmap[1].CreateCompatibleBitmap(cdc, client.Width(), client.Height());
	m_Bitmap[0].CreateCompatibleBitmap(cdc, nBitmapH, nBitmapV);

//	m_memDC.PatBlt(0,0, client.Width(), client.Height(), BLACKNESS); // WHITENESS, PATCOPY, PATINVERT, DSTINVERT
*/

	char szDesc[] = "MFC Enhanced MetaFile Application";
	CRect rc(0,0,10000,10000);
	//CMetaFileDC metaDC;

	m_metaDC.CreateEnhanced(NULL,NULL,&rc,szDesc);

	//MM_HIMETRIC 맵핑 모드를 이용하여 그래픽 함수 레코딩.
	m_metaDC.SetMapMode(MM_HIMETRIC);

/*
	//노란색 브러시 생성.
	CBrush brush, *pOldBrush;
	brush.CreateSolidBrush(RGB(255,255,0));
	pOldBrush = ((CBrush*)m_metaDC.SelectObject(&brush));
	m_metaDC.Ellipse(0,0,10000,-10000);

	//"Book Antique"글꼴 선택.
	CFont font, *pOldFont;
	font.CreateFont(2000,0,0,0,FW_NORMAL,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Book Antique");
	pOldFont = (CFont*)m_metaDC.SelectObject(&font);

	//텍스트 출력.
	m_metaDC.SetBkMode(TRANSPARENT);
	m_metaDC.SetTextAlign(TA_CENTER|TA_BOTTOM);
	m_metaDC.TextOut(5000,-5000,"Enhanced");
	m_metaDC.SetTextAlign(TA_CENTER|TA_TOP);
	m_metaDC.TextOut(5000,-5000,"MetaFile");

	//DC복원.
	m_metaDC.SelectObject(pOldBrush);
	m_metaDC.SelectObject(pOldFont);
	brush.DeleteObject();
	font.DeleteObject();

	// 메타 파일 DC를 닫고 확장 메타파일의 핸들을 얻어 저장.
	m_hMetaFile = m_metaDC.CloseEnhanced();
*/

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyDxTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDxTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
// 		BITMAP BitMap;
// 		if(!m_Bitmap[0].GetBitmap(&BitMap))
// 			return;

		CDialog::OnPaint();

		CRect client, rcMeta, rcDest, rcPxlClient;
		GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
		int nHt = client.Height();
		int nWd = client.Width();

		// DLU를 픽셀로 변환
		rcPxlClient.CopyRect(client);
		MapDialogRect(&rcPxlClient);

		// 이제 client.Width()와 client.Height()는 실제 픽셀값으로 변경됨
		int pxlClientW = rcPxlClient.Width();
		int pxlClientH = rcPxlClient.Height();

		double dMargin = 0.1;
		FRECTC rt = m_pRs274X->GetFrameExtent();
		double dRtW = (rt.X2 - rt.X1);
		double dRtH = (rt.Y2 - rt.Y1);
		m_nPixel_H = int((rt.X2 - rt.X1) * 1000.0 / m_dPixelResolution);
		m_nPixel_V = int((rt.Y2 - rt.Y1) * 1000.0 / m_dPixelResolution);
// 		m_nPixel_H = int((rt.X2 - rt.X1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
// 		m_nPixel_V = int((rt.Y2 - rt.Y1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
// 		SetScrlBarMax(m_nPixel_H, m_nPixel_V);
// 		SetScrlBar(0,0);

		double dRtoV = (double)dRtH / (double)nHt;
		double dRtoH = (double)dRtW / (double)nWd;
		//double dRtoV = (double)m_nPixel_V / (double)nHt;
		//double dRtoH = (double)m_nPixel_H / (double)nWd;

		double dSize = dRtH;
		if (dRtW < dRtH)
			dSize = dRtW;
		int nSize = 175;// 200;// 165;// (int)dSize;

		//if (dRtoH > dRtoV)
		//	dSize *= dRtoV;
		//else
		//	dSize *= dRtoH;
		//
		//int nSize = (int)dSize * 2 * 5.8;
		//nSize = nWd;

		rcMeta.left = client.left;
		rcMeta.top = client.bottom;
		rcMeta.right = client.left + nSize;// (double)m_nPixel_V * 0.001;
		rcMeta.bottom = client.bottom + nSize;// (double)m_nPixel_H * 0.001;
		//rcMeta.right = client.left + 317;// (double)m_nPixel_V * 0.001;
		//rcMeta.bottom = client.bottom + 317;// (double)m_nPixel_H * 0.001;

/*		//if(dRtoH < dRtoV)
		if (dRtoH > dRtoV)
		{
			rcMeta.left = client.left; 
			rcMeta.top = client.bottom;
			rcMeta.right = client.left + (double)nWd * 0.25; //0.355
			rcMeta.bottom = client.bottom + (double)nHt * 0.25; //0.355
			//rcMeta.left = client.left; 
			//rcMeta.top = client.bottom-nWd*dMargin;
			//rcMeta.right = client.left+int((double)m_nPixel_H/dRtoH); 
			//rcMeta.bottom = client.bottom+int((double)m_nPixel_H/dRtoH)-nWd*dMargin; 
		}
		else
		{
			rcMeta.left = client.left;
			rcMeta.top = client.bottom;
			rcMeta.right = client.left + (double)nWd * 0.25; //0.355
			rcMeta.bottom = client.bottom + (double)nHt * 0.25; //0.355
			//rcMeta.left = client.left; 
			//rcMeta.top = client.bottom-nWd*dMargin;
			//rcMeta.right = client.left+int(m_nPixel_V/dRtoV)/4;
			//rcMeta.bottom = client.bottom+int(m_nPixel_V/dRtoV)/4-nWd*dMargin;
		}
		*/
/*
		rcMeta.left = client.left; rcMeta.top = client.bottom;//(client.bottom+client.top)/2;
		rcMeta.right = client.left+m_nPixel_H; rcMeta.bottom = client.bottom+m_nPixel_V;
//		rcMeta.right = client.right; rcMeta.bottom = client.bottom+client.Height();
*/

		HWND hWnd = GetDlgItem(IDC_STATIC_PIC)->m_hWnd;
		HDC  hdc = ::GetDC(hWnd);

		if(NULL != hdc)
		{
			DrawBk(hdc);
			if(m_bDraw)
			{
				CDC* cdc = CDC::FromHandle(hdc);
				//HENHMETAFILE hMetaFile = (HENHMETAFILE)m_metaDC.GetSafeHdc();

				
				if (m_hMetaFile)
				{
					rcDest.CopyRect(rcMeta);
					//cdc->PlayMetaFile(m_hMetaFile, &client); // 확장 메타파일 플레이.
					cdc->PlayMetaFile(m_hMetaFile, &rcDest); // 확장 메타파일 플레이.
					//cdc->PlayMetaFile(m_hMetaFile, &rcMeta);
				}
			}

			::ReleaseDC(hWnd, hdc);
		}
/*
// 클립보드에 복사 예제 코드
void CMyMetaView::OnEditCopy()
{
	COleDataSource* pOleData;
	tagSTGMEDIUM* data;

	pOleData = new COleDataSource;
	data = new tagSTGMEDIUM;
	data->tymed = TYMED_ENHMF;
	data->hEnhMetaFile = m_hMF;
	pOleData->CacheData(CF_ENHMETAFILE, data);
	pOleData->FlushClipboard();
	pOleData->SetClipboard();

	delete data;
}
*/
// 		COLORREF BkColor = RGB(0,0,0);//GetSysColor(COLOR_3DFACE);
// 		HDC& hdc = myDx.BeginPaint(BkColor);


//		HDC  hdc = ::GetDC(GetDlgItem(IDC_STATIC_PIC)->m_hWnd);

// 		CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
// 		CDC* cdc = CDC::FromHandle(hdc);
// 		CDC m_memDC;
// 		CBitmap m_Bitmap; 
// 		CBitmap* pOldBitmap; 
// 		m_memDC.CreateCompatibleDC(cdc);
// 		m_Bitmap.CreateCompatibleBitmap(cdc, client.Width(), client.Height());
// 		pOldBitmap = (CBitmap*)m_memDC.SelectObject(&m_Bitmap);
// 		m_memDC.PatBlt(0,0, client.Width(), client.Height(), BLACKNESS); // WHITENESS, PATCOPY, PATINVERT, DSTINVERT


// 		if(NULL !=hdc)
// 		{
// 			DrawBk(hdc);
// 			if(m_bDraw)
// 			{
// 				CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
// 				CDC* cdc = CDC::FromHandle(hdc);

/*
				CDC* cdc = CDC::FromHandle(hdc);
				CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
				CBitmap* pOldBitmap; 
//				if(((CButton*)GetDlgItem(IDC_FIT))->GetCheck())
					pOldBitmap = (CBitmap*)m_memDC.SelectObject(&m_Bitmap[0]);
// 				else
// 					pOldBitmap = (CBitmap*)m_memDC.SelectObject(&m_Bitmap[1]);
				//m_memDC.PatBlt(0,0, client.Width(), client.Height(), BLACKNESS); // WHITENESS, PATCOPY, PATINVERT, DSTINVERT

				SCROLLINFO siH, siV;
				m_scroll_H.GetScrollInfo(&siH);
				m_scroll_V.GetScrollInfo(&siV);
 				cdc->BitBlt(-siH.nPos, -siV.nPos, client.Width()+siH.nPos, client.Height()+siV.nPos, &m_memDC, 0, 0, SRCCOPY);
				m_memDC.SelectObject(pOldBitmap); 
*/

/*				SCROLLINFO siH, siV;
				m_scroll_H.GetScrollInfo(&siH);
				m_scroll_V.GetScrollInfo(&siV);

				CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
				CDC* cdc = CDC::FromHandle(hdc);
				Graphics graphics(hdc);
				//Image image(BitMap.bmBits);
				graphics.SetSmoothingMode(SmoothingModeHighQuality); //곡선처리.
				graphics.DrawImage(BitMap.bmBits, -siH.nPos, -siV.nPos, client.Width()+siH.nPos, client.Height()+siV.nPos);
*/

				
// 				int success = StretchBlt(hdc, -siH.nPos, -siV.nPos, client.Width()-siH.nPos, client.Height()-siV.nPos, m_memDC.m_hDC,
//                              0, 0, 8124, 8124, SRCCOPY );

// 				int success = StretchBlt(hdc, -siH.nPos, -siV.nPos, 2*client.Width()-siH.nPos, 2*client.Height()-siV.nPos, m_memDC.m_hDC,
//                              0, 0, client.Width(), client.Height(), SRCCOPY );
// 				success = TransparentBlt(hdc, -siH.nPos, -siV.nPos, 2*client.Width()-siH.nPos, 2*client.Height()-siV.nPos, m_memDC.m_hDC,
//                              0, 0, client.Width(), client.Height(), RGB(0,0,0));

//				m_memDC.SelectObject(pOldBitmap); 


/*				double dMargin = 0.1;
				FRECTC rt = m_pRs274X->GetFrameExtent();
				int nPixel_H = int((rt.X2 - rt.X1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
				int nPixel_V = int((rt.Y2 - rt.Y1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
				int success = StretchBlt( hdc, 0, 0, client.Width(), client.Height(), m_memDC.m_hDC,
                             0, 0, nPixel_H, nPixel_V, SRCCOPY );
				m_memDC.SelectObject(pOldBitmap); */

// 				cdc->BitBlt(0,0, client.Width(), client.Height(),&m_memDC ,0, 0, SRCCOPY);

//				m_memDC.SelectObject(pOldBitmap); 


//			}
//			::SetBkMode(hdc,TRANSPARENT);

// 			DrawBk(hdc);
//			DrawRs274X(hdc);

// 			HDC hDC = m_memDC.GetSafeHdc();
// 			DrawRs274X(hDC);
// 			cdc->BitBlt(0,0, client.Width(), client.Height(),&m_memDC ,0, 0, SRCCOPY);
// 			m_memDC.SelectObject(pOldBitmap); 
			
			//DrawText(hdc);




/*//
			CRect client; GetClientRect(&client);

			filllines(hdc, client, RGB(128, 128, 128));

			CDC* cdc = CDC::FromHandle(hdc);

			CPen p(PS_DASHDOTDOT, 1, RGB(128, 128, 128));
			CPen* op = cdc->SelectObject(&p);
			
			cdc->MoveTo(30, 87);
			cdc->LineTo(client.Width()-20, client.Height() - 30);

			cdc->SetBkMode(TRANSPARENT);
			cdc->TextOut(20, 20, "double buffering in directx", 27);
			cdc->SelectObject(op);
//*/

//			myDx.EndPaint();

//			::ReleaseDC(GetDlgItem(IDC_STATIC_PIC)->m_hWnd, hdc);
// 		}

// 		m_memDC.DeleteDC(); 

	}
}

void CMyDxTestDlg::DrawBk(HDC& hdc)
{
	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
	CDC* cdc = CDC::FromHandle(hdc);

	cdc->FillSolidRect(client.left,client.top,client.right,client.bottom,RGB(0,0,0));	//GetSysColor(COLOR_3DFACE));
}

void CMyDxTestDlg::DrawRs274X(HDC& hdc, BOOL bFullRes)
{
	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
	if(bFullRes)
		m_pRs274X->SetPixelResolution(m_dPixelResolution);
	else
		m_pRs274X->SetPixelResolution(0.0);
	m_pRs274X->Draw(hdc, client);
}

void CMyDxTestDlg::DrawText(HDC& hdc)
{
	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
	CDC* cdc = CDC::FromHandle(hdc);

//	cdc->SetBkMode(TRANSPARENT);
	//cdc->SetBkMode(OPAQUE);
	//cdc->SetBkColor(RGB(0, 255, 0));
	cdc->SetTextColor(RGB(255, 0, 0));
	cdc->TextOut(20, 20, "double buffering in directx", 27);
}

void CMyDxTestDlg::filllines(HDC& hdc, const CRect& client, int color)
{
	static int gap = 80;
	int i;

	CDC* cdc = CDC::FromHandle(hdc);
	CPen p(PS_DASHDOTDOT, 1, color);
	CPen* op = cdc->SelectObject(&p);

	for (i = gap ; i <= client.Height(); i = i+gap)
	{
		cdc->MoveTo(0, i);
		cdc->LineTo(client.Width(), i);
	}

	for (i = 80 ; i <= client.Width(); i = i+80)
	{
		cdc->MoveTo(i, 0);
		cdc->LineTo(i, client.Height());
	}
	cdc->SelectObject(op);
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyDxTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyDxTestDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString sPath;

	if (!FileBrowse(sPath))
		return;

	m_pRs274X->FreeMemory();

	CString sFile = m_pRs274X->Load(sPath);
	if(sFile.IsEmpty())
	{
		AfxMessageBox("Failed Load File.");
		return;
	}

	LPCTSTR lpszData = (LPCTSTR)sFile;
	BOOL bRtn = m_pRs274X->Decoding((char*)lpszData);

	m_pRs274X->CloseDecoding();



 	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
/*
	int nBitmapH = 8124;//client.Width() / 0.1;
 	int nBitmapV = 8124;//client.Height() / 0.1;
	m_dPixelResolution = 10.0;
	UpdateData(FALSE);

//	HDC  hdc = ::GetDC(GetDlgItem(IDC_STATIC_PIC)->m_hWnd);
	CBitmap* pOldBitmap[2]; 
	pOldBitmap[0] = (CBitmap*)m_memDC.SelectObject(&m_Bitmap[0]);
	m_memDC.PatBlt(0, 0, nBitmapH, nBitmapV, BLACKNESS); // WHITENESS, PATCOPY, PATINVERT, DSTINVERT
// 	m_memDC.PatBlt(0,0, client.Width(), client.Height(), BLACKNESS); // WHITENESS, PATCOPY, PATINVERT, DSTINVERT
	HDC hdc = m_memDC.GetSafeHdc();
	DrawRs274X(hdc,TRUE);
	m_memDC.SelectObject(pOldBitmap[0]); 
//	::ReleaseDC(GetDlgItem(IDC_STATIC_PIC)->m_hWnd, hdc);

	double dMargin = 0.1;
	FRECTC rt = m_pRs274X->GetFrameExtent();
	m_nPixel_H = int((rt.X2 - rt.X1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
	m_nPixel_V = int((rt.Y2 - rt.Y1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);

*/

/*
	if(m_nPixel_H > 8124 || m_nPixel_V > 8124)
	{
		int nPixel = (m_nPixel_H > m_nPixel_V) ? m_nPixel_H : m_nPixel_V;
		m_dPixelResolution *= (double)nPixel / 8124.0;
		UpdateData(FALSE);

//		pOldBitmap[1] = (CBitmap*)m_memDC.SelectObject(&m_Bitmap[1]);
		m_memDC.PatBlt(0, 0, nBitmapH, nBitmapV, BLACKNESS); // WHITENESS, PATCOPY, PATINVERT, DSTINVERT
		hdc = m_memDC.GetSafeHdc();
		DrawRs274X(hdc, TRUE);
//		m_memDC.SelectObject(pOldBitmap[1]); 
		rt = m_pRs274X->GetFrameExtent();
		m_nPixel_H = int((rt.X2 - rt.X1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
		m_nPixel_V = int((rt.Y2 - rt.Y1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
	}
*/
// 	m_memDC.SelectObject(pOldBitmap[0]); 

/*
// 	((CButton*)GetDlgItem(IDC_FIT))->SetCheck(FALSE);
//	SetScrlBarMax(0,0);

	((CButton*)GetDlgItem(IDC_ZOOM))->SetCheck(TRUE);
	SetScrlBarMax(m_nPixel_H, m_nPixel_V);
	SetScrlBar(0,0);

// 	SetScrlBarMax(2*client.Width(), 2*client.Height());
// 	SetScrlBar(0,0);

*/
	m_dPixelResolution = 10.0;
	//m_dPixelResolution = 5.0;
	UpdateData(FALSE);

	//HDC m_metaHdc;
	if (!m_hMetaFile)
	{
		m_metaDC.PatBlt(0, 0, 38000, 38000, BLACKNESS); // 33000, 33000 --> 200

		// 확장 메타파일 생성
		//char szDesc[MAX_PATH];
		//StringToChar(_T("Enhanced Metafile Application"), szDesc);
		//CRect rc(0, 0, 10000, 10000);
		//m_metaDC.CreateEnhanced(NULL, NULL, rc, _T("Enhanced Metafile Application"));
		//m_metaDC.CreateEnhanced(NULL, NULL, rc, szDesc);

		// MM_HIMETRIC 맵핑모드를 이용하여 그래픽 함수 레코딩
		//m_metaDC.SetMapMode(MM_HIMETRIC);

		m_metaHdc = m_metaDC.GetSafeHdc();
		DrawRs274X(m_metaHdc, TRUE);
		// 레코딩 완료

		// 메타 파일 DC를 닫고 확장 메타파일의 핸들을 얻어 저장.
		m_hMetaFile = m_metaDC.CloseEnhanced();
	}

// 	double dMargin = 0.1;
// 	FRECTC rt = m_pRs274X->GetFrameExtent();
// 	m_nPixel_H = int((rt.X2 - rt.X1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
// 	m_nPixel_V = int((rt.Y2 - rt.Y1) * (1.0 + dMargin) * 1000.0 / m_dPixelResolution);
// 	SetScrlBarMax(m_nPixel_H, m_nPixel_V);
// 	SetScrlBar(0,0);

	//if (m_metaHdc)
	//{
	//	DrawRs274X(m_metaHdc, TRUE);
	//	// 메타 파일 DC를 닫고 확장 메타파일의 핸들을 얻어 저장.
	//	m_hMetaFile = m_metaDC.CloseEnhanced();
	//}

	m_bDraw = TRUE;
	InvalidateRect(&client);

	// GDI plus.........................................

	HPALETTE hpal;  
	hpal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);  
  
	//Gdiplus::Bitmap을 만든다!!!  
	Bitmap image((HBITMAP)m_Bitmap[0], hpal);  
	int width = image.GetWidth();  
	int height = image.GetHeight();  
  
	/* GDI plus로 저장하기 !! */  
	CLSID jpegClsid;  
	GetEncoderClsid(L"image/jpeg", &jpegClsid);  
	image.Save(L"D:\\test.jpg", &jpegClsid, NULL);  


}

void CMyDxTestDlg::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

int CMyDxTestDlg::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)  
{  
    UINT num = 0;  
    UINT size = 0;  
    ImageCodecInfo * pImageCodecInfo = NULL;  
    GetImageEncodersSize(&num, &size);  
    if(size == 0)  
        return -1;  
  
    pImageCodecInfo = (ImageCodecInfo *)(malloc(size));  
    if(pImageCodecInfo == NULL)  
        return -1;  
  
    GetImageEncoders(num, size, pImageCodecInfo);  
    for(UINT j = 0 ; j < num; ++j)  
    {  
        if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)  
        {  
            *pClsid = pImageCodecInfo[j].Clsid;  
            free(pImageCodecInfo);  
            return j;  
        }  
  
    }  
    free(pImageCodecInfo);  
    return -1;  
}  



int CMyDxTestDlg::FileBrowse(CString& sPath)
{
	sPath = "";

	/* Load from file */
	CString FilePath;
	CFileFind finder;
	CString SrchPath, filePos,strTitleMsg,strErrMsg;
	
	CWaitCursor mCursor;
	
	CString DirPath[10];
	char strPrevDir[MAX_PATH];
	DWORD dwLength;
	CString strWorkDir;
	BOOL bResult;
	
	CString strMcNum;
	int nAoiMachineNum = 0;
	
	filePos = m_sPrevGerbPath + "\\*.*";
	if(!finder.FindFile(filePos))
	{
		dwLength = GetCurrentDirectory(MAX_PATH, strPrevDir);
		if(!dwLength)
		{
			AfxMessageBox(_T("현재의 폴더이름을 읽기에 실패하였습니다. "));
			return FALSE;
		}

		filePos = strPrevDir;
	}
// 	if (!finder.FindFile(filePos))
// 	{
// 		strErrMsg.Format("파일이 없습니다.\r\n%s", filePos);
// 		AfxMessageBox(strErrMsg);
// 		filePos = "C:\\*.*";
// 	}
// 	else
// 	{
// 		dwLength = GetCurrentDirectory(MAX_PATH, strPrevDir);
// 		if(!dwLength)
// 		{
// 			AfxMessageBox(_T("현재의 폴더이름을 읽기에 실패하였습니다. "));
// 			return FALSE;
// 		}
// 
// 		filePos = strPrevDir;
// 	}
	
	// File Open Filter 
	static char BASED_CODE szFilter[] = "Gerber Files (*.gbr;*.m00)|*.gbr;*.m00|All Files (*.*)|*.*||";
	
	// CFileDialog 
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter,NULL);
	
	// Win2k Style FileDialog Box
	dlg.m_ofn.lStructSize = sizeof (OPENFILENAME) + 12; // +12를 Win2k Style로 다이얼로그 박스가 Open됨.
	
	// Open Directory
	strWorkDir = filePos;	
	dlg.m_ofn.lpstrInitialDir = strWorkDir;
	
	bResult = 0;
	
	// Dialog Open
	if(dlg.DoModal() == IDOK)
	{
		sPath = m_sDlgOpenGerbPath = FilePath = dlg.GetPathName();
		
		char *pStr=FilePath.GetBuffer(MAX_PATH);		
		
		FilePath.ReleaseBuffer();
		bResult = TRUE;

		CString sFileName;
		int nPos = FilePath.ReverseFind('\\');
		sFileName = FilePath.Right(FilePath.GetLength()-nPos-1);
		GetDlgItem(IDC_STATIC_PATH)->SetWindowText(sFileName);
		m_sPrevGerbPath = FilePath.Left(nPos); 
	}
	else
	{
		bResult = FALSE;
		GetDlgItem(IDC_STATIC_PATH)->SetWindowText("");
	}
	
	
	return bResult;	
}



void CMyDxTestDlg::SetScrlBarMax(int nMaxH, int nMaxV) // [pixel]
{
	SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;

    m_scroll_H.GetScrollInfo(&si);
    si.nMin = 0;
    si.nPage = 1;	
	si.nPos = 0;

    si.nMax = nMaxH;
	m_scroll_H.SetScrollInfo(&si);
    m_scroll_H.SetScrollPos(si.nPos, TRUE);

    m_scroll_V.GetScrollInfo(&si);
    si.nMin = 0;
    si.nPage = 1;	
	si.nPos = 0;

    si.nMax = nMaxV;
	m_scroll_V.SetScrollInfo(&si);
    m_scroll_V.SetScrollPos(si.nPos, TRUE);
}

void CMyDxTestDlg::SetScrlBar(int nCurH, int nCurV)
{
	SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;

    m_scroll_H.GetScrollInfo(&si);

	si.nPos = nCurH;
	m_scroll_H.SetScrollInfo(&si);
    m_scroll_H.SetScrollPos(si.nPos, TRUE);

    m_scroll_V.GetScrollInfo(&si);

	si.nPos = nCurV;
	m_scroll_V.SetScrollInfo(&si);
    m_scroll_V.SetScrollPos(si.nPos, TRUE);
}

void CMyDxTestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

// 	int nStep=1000.0/m_dPixelResolution; // every 1mm
	int nStep=1; // [pixel]
	int nCurPos;

	if(m_scroll_H.m_hWnd == pScrollBar->m_hWnd)
	{
		SCROLLINFO si =
		{
			sizeof(SCROLLINFO),
				SIF_ALL | SIF_DISABLENOSCROLL,
		};

		m_scroll_H.GetScrollInfo(&si);

		switch (nSBCode)
		{
		case SB_BOTTOM:         // scroll to bottom
			nCurPos = 0;
			break;
		case SB_TOP:            // scroll to top
			nCurPos = si.nMax;
			break;
		case SB_PAGEDOWN:       // scroll one page down
			nCurPos = si.nPos + nStep;
			if(nCurPos < 0)
				nCurPos = 0;
			break;
		case SB_PAGEUP:         // scroll one page up
			nCurPos = si.nPos - nStep;
			if(nCurPos > si.nMax)
				nCurPos = si.nMax;
			break;
		case SB_LINEDOWN:       // scroll one line up
			nCurPos = si.nPos + nStep;
			if(nCurPos < 0)
				nCurPos = 0;
			break;
		case SB_LINEUP:         // scroll one line up
			nCurPos = si.nPos - nStep;
			if(nCurPos > si.nMax)
				nCurPos = si.nMax;
			break;
		case SB_THUMBTRACK:     // drag scroll box 
			nCurPos = nPos;
			break;
		case SB_THUMBPOSITION:  // scroll to the absolute 
			nCurPos = nPos;
			break;
		case SB_ENDSCROLL:      // end scroll
			//nCurPos = nPos;
			return;
		default:
			return;
		}

		si.nPos = nCurPos;

		if (si.nPos < 0)
			si.nPos = 0;
		if (si.nPos > si.nMax)
			si.nPos = si.nMax;
		m_scroll_H.SetScrollInfo(&si);
	}

	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
	InvalidateRect(&client);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMyDxTestDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

//	int nStep=1000.0/m_dPixelResolution; // every 1mm
	int nStep=1; // [pixel]
	int nCurPos;

	if(m_scroll_V.m_hWnd == pScrollBar->m_hWnd)
	{
		SCROLLINFO si =
		{
			sizeof(SCROLLINFO),
				SIF_ALL | SIF_DISABLENOSCROLL,
		};

		m_scroll_V.GetScrollInfo(&si);

		switch (nSBCode)
		{
		case SB_BOTTOM:         // scroll to bottom
			nCurPos = 0;
			break;
		case SB_TOP:            // scroll to top
			nCurPos = si.nMax;
			break;
		case SB_PAGEDOWN:       // scroll one page down
			nCurPos = si.nPos + nStep;
			if(nCurPos < 0)
				nCurPos = 0;
			break;
		case SB_PAGEUP:         // scroll one page up
			nCurPos = si.nPos - nStep;
			if(nCurPos > si.nMax)
				nCurPos = si.nMax;
			break;
		case SB_LINEDOWN:       // scroll one line up
			nCurPos = si.nPos + nStep;
			if(nCurPos < 0)
				nCurPos = 0;
			break;
		case SB_LINEUP:         // scroll one line up
			nCurPos = si.nPos - nStep;
			if(nCurPos > si.nMax)
				nCurPos = si.nMax;
			break;
		case SB_THUMBTRACK:     // drag scroll box 
			nCurPos = nPos;
			break;
		case SB_THUMBPOSITION:  // scroll to the absolute 
			nCurPos = nPos;
			break;
		case SB_ENDSCROLL:      // end scroll
			//nCurPos = nPos;
			return;
		default:
			return;
		}

		si.nPos = nCurPos;

		if (si.nPos < 0)
			si.nPos = 0;
		if (si.nPos > si.nMax)
			si.nPos = si.nMax;
		m_scroll_V.SetScrollInfo(&si);
	}

	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
	InvalidateRect(&client);
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMyDxTestDlg::OnChangeEditPixelResolution() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unles's you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CMyDxTestDlg::OnFit() 
{
	// TODO: Add your control notification handler code here
	SetScrlBarMax(0, 0);

	((CButton*)GetDlgItem(IDC_ZOOM))->SetCheck(FALSE);

	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
	InvalidateRect(&client);
}

void CMyDxTestDlg::OnZoom() 
{
	// TODO: Add your control notification handler code here
	SetScrlBarMax(m_nPixel_H, m_nPixel_V);
	SetScrlBar(0,0);

	((CButton*)GetDlgItem(IDC_FIT))->SetCheck(FALSE);

	CRect client; GetDlgItem(IDC_STATIC_PIC)->GetClientRect(&client);
	InvalidateRect(&client);	
}

void CMyDxTestDlg::OnButtonTest() 
{
	// TODO: Add your control notification handler code here
	Zoom();
}

void CMyDxTestDlg::Zoom() 
{
/*	int i, j, num_C;
	BITMAP BitMap;
	if(!m_Bitmap[0].GetBitmap(&BitMap))
		return;

	if(BitMap.bmBitsPixel == 24)
		num_C = 3;
	else if(BitMap.bmBitsPixel == 8)
		num_C = 1;

	int nHeight = (int)BitMap.bmHeight;
	int nWidth = (int)BitMap.bmWidth;
	int nSize = nHeight * nWidth;

	double **tempInputImageR, **tempInputImageG, **tempInputImageB;
	double **tempOutputImageR, **tempOutputImageG, **tempOutputImageB;

	double dZoomRate = 0.5;

	double dReWidth = (double)nWidth * dZoomRate;
	double dReHeight = (double)nHeight * dZoomRate;
	double dReSize = dReWidth * dReHeight;
	int nReSize = int(dReSize);

	unsigned char *pOutputImage = new unsigned char[nReSize*3];
	tempInputImageR = Image2DMem(nHeight, nWidth);
	tempInputImageG = Image2DMem(nHeight, nWidth);
	tempInputImageB = Image2DMem(nHeight, nWidth);

	tempOutputImageR = Image2DMem(dReHeight, dReWidth);
	tempOutputImageG = Image2DMem(dReHeight, dReWidth);
	tempOutputImageB = Image2DMem(dReHeight, dReWidth);

	for(i=0; i<nHeight; i++)
	{
		for(j=0; j<nWidth; j++)
		{
			tempInputImageB[i][j] = BitMap.bmBits[(i*nWidth*3 + j*num_C + 0)];
			tempInputImageG[i][j] = BitMap.bmBits[(i*nWidth*3 + j*num_C + 1)];
			tempInputImageR[i][j] = BitMap.bmBits[(i*nWidth*3 + j*num_C + 2)];
		}
	}

	for(i=0; i<nReSize*3; i++)
		pOutputImage[i] = 255;

	for(i=0; i<int(dReHeight); i++)
	{
		for(j=0; j<int(dReWidth); j++)
		{
			tempOutputImageB[i][j] = tempInputImageB[(int)((double)i/dZoomRate)][(int)(j/dZoomRate)];
			tempOutputImageG[i][j] = tempInputImageB[(int)((double)i/dZoomRate)][(int)(j/dZoomRate)];
			tempOutputImageR[i][j] = tempInputImageB[(int)((double)i/dZoomRate)][(int)(j/dZoomRate)];
		}
	}

	delete tempInputImageB, tempInputImageG, tempInputImageR;
	delete tempOutputImageB, tempOutputImageG, tempOutputImageR;
*/
/*
	// Bitmap 축소의 핵심부분
	for(i=0; i<int(dReHeight); i++)
	{
		for(j=0; j<int(dReWidth); j++)
		{
			sumR = 0.0; sumG = 0.0; SumB = 0.0;
			for(q=0; q<int(dZoomRate); q++)
			{
				for(r=0; r<int(dZoomRate); r++)
				{
					sumR += tempInputImageB[(int)((double)i/dZoomRate) + q][(int)(j/dZoomRate) + r];
					sumG += tempInputImageB[(int)((double)i/dZoomRate) + q][(int)(j/dZoomRate) + r];
					sumB += tempInputImageB[(int)((double)i/dZoomRate) + q][(int)(j/dZoomRate) + r];
				}
			}
			tempOutputImageB[i][j] = (sumB * (dZoomRate*dZoomRate));
			tempOutputImageG[i][j] = (sumG * (dZoomRate*dZoomRate));
			tempOutputImageR[i][j] = (sumR * (dZoomRate*dZoomRate));

			pOutputImage[i*dReWidth*3 = j*num_C + 0] = (unsigned char)(sumB * (dZoomRate*dZoomRate));
			pOutputImage[i*dReWidth*3 = j*num_C + 1] = (unsigned char)(sumG * (dZoomRate*dZoomRate));
			pOutputImage[i*dReWidth*3 = j*num_C + 2] = (unsigned char)(sumR * (dZoomRate*dZoomRate));
		}
	}
*/
}