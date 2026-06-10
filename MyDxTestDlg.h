// MyDxTestDlg.h : header file
//

#if !defined(AFX_MYDXTESTDLG_H__F0FB21C6_641E_4E7C_ADD2_95D7027C88AE__INCLUDED_)
#define AFX_MYDXTESTDLG_H__F0FB21C6_641E_4E7C_ADD2_95D7027C88AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyDirectX.h"
#include "FileFormatRs274X.h"
//#include "atlimage.h"

/////////////////////////////////////////////////////////////////////////////
// CMyDxTestDlg dialog

class CMyDxTestDlg : public CDialog
{
	CDC m_memDC;
	CMetaFileDC m_metaDC;
	HENHMETAFILE m_hMetaFile;

	BOOL m_bDraw;
//	CImage m_Image;
	CBitmap m_Bitmap[2]; 
	int m_nPixel_H, m_nPixel_V;

	CMyDirectX myDx;
	CString m_sPrevGerbPath, m_sDlgOpenGerbPath;

	CFileFormatRs274X* m_pRs274X;

	int FileBrowse(CString& sPath);
	void DrawBk(HDC& hdc);
	void DrawRs274X(HDC& hdc, BOOL bFullRes=FALSE);
	void DrawText(HDC& hdc);

	void SetScrlBarMax(int nMaxH, int nMaxV);
	void SetScrlBar(int nCurH, int nCurV);

	int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) ;
	void Zoom();


// Construction
public:
	CMyDxTestDlg(CWnd* pParent = NULL);	// standard constructor
	~CMyDxTestDlg();

	//void OnDispProgress(WPARAM wPara, LPARAM lPara);
	
	void filllines(HDC& hdc, const CRect& client, int color = -1);

// Dialog Data
	//{{AFX_DATA(CMyDxTestDlg)
	enum { IDD = IDD_MYDXTEST_DIALOG };
	CScrollBar	m_scroll_H;
	CScrollBar	m_scroll_V;
	double	m_dPixelResolution;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDxTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyDxTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEditPixelResolution();
	afx_msg void OnFit();
	afx_msg void OnZoom();
	afx_msg void OnButtonTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDXTESTDLG_H__F0FB21C6_641E_4E7C_ADD2_95D7027C88AE__INCLUDED_)
