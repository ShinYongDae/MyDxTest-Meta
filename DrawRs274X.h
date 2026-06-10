#if !defined(AFX_DRAWRS274X_H__3E7B5445_642C_4D33_BD2C_95459C42BBC6__INCLUDED_)
#define AFX_DRAWRS274X_H__3E7B5445_642C_4D33_BD2C_95459C42BBC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawRs274X.h : header file
//

#include "FileFormatRs274X.h"

/////////////////////////////////////////////////////////////////////////////
// CDrawRs274X window

class CDrawRs274X : public CFileFormatRs274X
{
	CFileFormatRs274X* m_pParent;
	//CCriticalSection m_csDrawing;
	double m_dPixelResolution;

	typedef struct tag_SNR_LIST_INDEX {
		int     nObjectIndex;
		int     nListIndex;
		FRECTC	   frtBoundingBox;
	} SNR_LIST_INDEX, *PSNR_LIST_INDEX;

	//Draw Object data structure
	struct DrawListAPerture
	{
		vector <int> nDcode;
		vector <int> nListNum;
	};

	vector <FPOINTC> m_vecVertics;

	int m_nListAperture; // m_glListAperture
	int m_nListIndex; // m_nGLListIndex
	int m_nListLines; // m_glListLines
	int m_nListSnR; // m_glListSnR

	CSize m_sizeWindow;
	UINT16 m_nPrevAttribute;
	UINT16 m_nPrevPolarity;

	CfPoint3D m_fExtentStartPos;
	CfPoint3D m_fExtentEndPos;
	FPOINTC m_fptDrawOffset;

	int m_nLayerID;
	BOOL m_bDisplayBackgroundImg;
	FRECTC m_frtDrawFrameExtent;
	BOOL m_bCheckDraw;
	int m_nApertureListIndex;
	COLORREF m_crObjColor;
	float m_fCanvasPixelResolution, m_fDrawArcResolution;
	float m_fDrawResolution;

	int m_nActionCode;

// 	void AllocDrawList();
// 	void AllocCamDrawStruct();
	
	void CallList(int nListIndex);

	FPOINTC ConvertWindowCoord(FPOINTC& fPoint);
	FPOINTC ConvertResolutionCoord(FPOINTC& fPoint);

	BOOL CheckDrawAllow(UINT nObjIndex);
	BOOL CheckDrawAllow(const FRECTC &FMinMax);
	int  SearchLineTemplate(UINT64 nVal);

	COLORREF GetObjectColor(GraphObj &obj);
	int  GetArcSegment(float fRadius,float fAngle=360.0);
	double GetDistance(CfPoint FirstPoint,CfPoint SecondPoint); // 두점 사이의 거리를 얻는다 
	void GetArcVertices(vector<FPOINT> &vecPt, const FPOINT &ptCenter, double radius, double start_angle, double arc_angle, int dir);
	void GetArcVertices(vector<FPOINTC> &vecPt,const FPOINTC &ptCenter,float radius, float start_angle, float arc_angle, int dir);
	COLORREF GetSnRObjectColor(CFileFormatRs274X::GraphObj &obj);

//	FPOINTC OriginTranslateCoord(vector <FPOINTC> &vecCoord, GraphObj &gObj, FRECTC fExtent);
//	void TranslateCoordbyActionCode(vector <FPOINTC> &vecCoord, GraphObj &gObj, FPOINTC fptCenter);
	
//	void RotateTranslateExtent(FRECTC &Extent, double cV, double sV, double cX, double cY, double dX, double dY);
//	void RotateObject90(vector<FPOINTC> &vecCoord,GraphObj *obj,FPOINTC fC);

	void SetForeColor(COLORREF crColor);
	void SetCanvasPixelResolution();
	void SetStencil(int nMode);
	void SetDrawResolution(float fResolution);
	void SetDrawArcResolution(float fResolution);
	
	BOOL MyMacro(const CFileFormatRs274X::LPAPERTURE_DEF &pAptDef, float x, float y, byte Pol);
	void MyLineList(float X1, float Y1, float X2, float Y2,int nListNum);
	BOOL MyLineMacro(float fSx, float fSy, float fEx, float fEy, FRECTC fExtent);
	BOOL MyRectCenter(float fSx, float fSy, float fLineWidth, float fLineHeight, float fAngle);
	BOOL MyOval(float fCx, float fCy, float fWidth, float fHeight);
	BOOL MyPolygonNSide(int n, float cX, float cY, float d, float Ang);
	BOOL MyQuadrantPolygon(CFileFormatRs274X::GraphObj *obj, CFileFormatRs274X::GraphObj *NextObj);
	BOOL MySnRObjList(CFileFormatRs274X::GraphObj &gObj, int nObjIndex, float ddX, float ddY);
	BOOL MyArc(float fSx, float fSy, float fEx, float fEy, float fCX, float fCY, float fWidth, int nDir);
	void MyCallApertureList(float fx, float fy,int nListNum);
	BOOL MyMoire(float x, float y, float fOutDiameter, float fRingWidth, float fGap, int nCircle, float fLineWidth, float fLineLegth, float Ang);
	BOOL MyObjEntityShift(int k, float dX, float dY);
	BOOL MyQuadrantPolygonShift(CFileFormatRs274X::GraphObj *obj, CFileFormatRs274X::GraphObj *Nextobj, float dX, float dY);
	void MyMakeLineList(float fLength, float w);
	BOOL MyThermal(float x, float y, float Dmo, float Dmi, float Ang, int num,  float gap, float Angle);
	BOOL MyPolygon(const FPOINTC *fCoord, int nVertex);
	BOOL MyLine(float fSx, float fSy, float fEx, float fEy, float fLineWidth);
	BOOL MyRectLowerLeft(float fSx, float fSy, float fLineWidth, float fLineHeight, float fAngle);

	void DrawRect(const CfRect &rect,int width = 0, int fill=TRUE);
	void DrawLineRound(float fSx, float fSy, float fEx, float fEy, float fLineWidth, BOOL bFill = TRUE);
	void DrawLineRect(float fSx, float fSy, float fEx, float fEy, float fLineWidth, float fLineHeight );
	void DrawCircle(float cx, float cy, float r, int fill=TRUE);
	void DrawDonut(float fCx, float fCy, float fDmo, float fDmi);
	void DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int fill=TRUE);
	void DrawRect(const CfPoint3D &fptStart,const CfPoint3D &fptEnd,int width=0, int fill=TRUE);
	void DrawArcRound(float cx, float cy, float r, float start_angle, float arc_angle, float width,int dir,int fill=TRUE);
	void DrawPolygonLine(const vector <FPOINTC> &vecPt,int nLineWidth = 0);
	void DrawConvexPolygon(const FPOINTC *fPoint,int nNumberOfVertex);
	void DrawConvexPolygon(const vector <FPOINTC> &fPoint);
	void DrawConcavePolygon(const vector <FPOINTC> &fPoint);

//	void DispProgress(int nCurr, int nTot);
	void GetCannedText(CString strCannedText, FPOINTC fptStart, float fDistance, vector <FPOINTC> &vecCoord);


// Construction
public:
	CDrawRs274X(CFileFormatRs274X* pParent=NULL);

// Attributes
public:
	HWND m_hParent;
	HDC  m_hDC;

// Operations
public:
	BOOL DrawObject(UINT nObjIndex);

	int CallApertureList();
	int CallLineTemplateList();
	int CallSnRObjectList();

	void SetPixelResolution(double dPixelResolution);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawRs274X)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDrawRs274X();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDrawRs274X)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWRS274X_H__3E7B5445_642C_4D33_BD2C_95459C42BBC6__INCLUDED_)
