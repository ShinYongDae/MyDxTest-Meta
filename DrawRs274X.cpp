// DrawRs274X.cpp : implementation file
//

#include "stdafx.h"
#include "mydxtest.h"
#include "DrawRs274X.h"
#include "ColorTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef PI
	#define PI 3.141592653589793f
#endif

const double DEG2RAD = PI/180.0;
const double RAD2DEG = 180.0/PI;

#define DetGcode 99999


/////////////////////////////////////////////////////////////////////////////
// CDrawRs274X

CDrawRs274X::CDrawRs274X(CFileFormatRs274X* pParent)
{
	m_pParent = pParent;
	m_hDC = pParent->m_hDC;
	m_hParent = pParent->m_hParent;
	m_dPixelResolution = 10.0;
	m_nListLines = 0;
}

CDrawRs274X::~CDrawRs274X()
{
}


BEGIN_MESSAGE_MAP(CDrawRs274X, CWnd)
	//{{AFX_MSG_MAP(CDrawRs274X)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDrawRs274X message handlers

BOOL CDrawRs274X::DrawObject(UINT nObjIndex)	//MyObjEntity
{
//  	m_pParent->m_csDrawing.Lock();
    byte dType;
	float  dX, dY, fRadius;
	FPOINTC fptCenter;

	vector <FPOINTC> vecCoord;

	int pNum = 0, i;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;
	FPOINTC *fptCoord;

	GraphObj gObj = m_pParent->m_pLayerInfo->listObj.at(nObjIndex);
		
    dType = gObj.Type.nType;

	if(m_nPrevAttribute != gObj.Type.nAttr || m_nPrevPolarity != gObj.Type.nPolarity || 
		dType == DRAW_TYPE::POLYGON_SQ || dType == DRAW_TYPE::POLYGON_MQ)
	{
		m_crObjColor = GetObjectColor(gObj);
		SetForeColor(m_crObjColor);
	}

	m_nApertureListIndex = -1;

	float X1, Y1, X2, Y2; int nListNum;
	switch(dType)
	{
	case DRAW_TYPE::LINE:
		fParam = &m_pParent->m_pAptDef->Param.at(0).fParam[0];


		if (m_pParent->m_pAptDef->chStdAptType == _T('C')) // Then		
		{		   
			dX = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
			Key.Param[0] = (dX*dX + dY*dY); //length
			Key.Param[1] = m_pParent->m_pAptDef->Param.at(0).fParam[0];// line width

			if(m_pParent->m_fCanvasPixelResolution > m_pParent->m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID = SearchLineTemplate(Key.key);
				if(nID > -1)
				{
					nID += m_nListLines;//m_glListLines;
					X1 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					MyLineList(X1, Y1, X2, Y2, nID);
				}
				else
				{
					X1 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					DrawLineRound(X1, Y1, X2, Y2, fParam[0]);
				}
			}
			else
 				DrawLineRound(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0]);
		}		
		else if (m_pParent->m_pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0], fParam[1]);
		}		
		else if(m_pParent->m_pAptDef->strAptName.GetLength() > 0) 
		{
			if(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
				MyLineMacro(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, m_pParent->m_pAptDef->minmax);
				MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, gObj.Type.nPolarity);
			}
		}
		break;	
	case DRAW_TYPE::FLASH: // 'Flash : bType[1]
		m_nApertureListIndex = m_pParent->m_pAptDef->nGlListIndex;

		fParam = &m_pParent->m_pAptDef->Param.at(0).fParam[0];

		if(m_nApertureListIndex>0)
		{
			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
 				MyCallApertureList(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,m_nApertureListIndex);
// 				m_pParent->m_csDrawing.Unlock();
				return FALSE;
			}
		}
		
		pNum = m_pParent->m_pAptDef->Param.at(0).fParam.size();
		
		if(m_pParent->m_pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{
			if (pNum == 1)
			{
				fRadius = fParam[0] / 2.0;
				DrawCircle(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if (pNum > 2 && pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
//				SetStencil(FALSE);
				
				if(pNum == 3)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], 0);
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], fParam[3]);
				}	
//				SetStencil(TRUE);
				DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pParent->m_pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pParent->m_pAptDef->chStdAptType == _T('O'))	//    ' Oval
		{
			if(pNum < 3)
			{
				MyOval(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
			
				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyOval (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pParent->m_pAptDef->chStdAptType == _T('P'))	//    ' Polygon
		{
			int nSide = fParam[1];
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]);      
			}
			else 
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
		 
				if (pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius);
				}
				else if (pNum >= 5)
				{
					 MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[3], fParam[4], 0);
				}
//				 SetStencil(TRUE);
				 MyPolygonNSide(nSide, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]); 
//				 glDisable(GL_STENCIL_TEST);
			}
		 }
		else //    ' Userdefined Apperture
		{
			MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
// 			if(m_nApertureListIndex< 1)
// 			{
// 				MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);		
// 			}
// 			else
// 			{
// 				CallList(m_nApertureListIndex);
// 				glPushMatrix();
// 				glTranslatef(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,0);
// 				glCallList(m_nApertureListIndex);
// 				glPopMatrix();	
// 			}
			
		
		}
		break;
	case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ: // bType[5], bType[6] 'PolygonFill with G74, G75
		MyQuadrantPolygon(&gObj,&m_pParent->m_pLayerInfo->listObj.at(nObjIndex+1));
		break;        
	case DRAW_TYPE::SNR:	// bType[7]
		MySnRObjList(gObj, nObjIndex, 0, 0);
		break;
	case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &m_pParent->m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y,
					m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
					m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
					fParam[0], CW);
		break;
	case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &m_pParent->m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, 
						m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
						m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
						fParam[0], CCW);
		break;
	case DRAW_TYPE::TEXT:
		fParam = &m_pParent->m_pAptDef->Param.at(0).fParam[0];
		fptCenter = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt];

		GetCannedText(m_pParent->m_pAptDef->strAptName, fptCenter, fParam[0]*1.2, vecCoord);
		
		m_nApertureListIndex = m_pParent->m_pAptDef->nGlListIndex;
		
		nIndex = m_pParent->m_pAptDef->Param.at(0).nPriNum;

		if(m_nApertureListIndex>0)
		{
			fptCoord = &vecCoord[0];
			for(i = 0 ; i < vecCoord.size(); i++)
			{
				if(nIndex == 98)
					m_pTrUtil->RotatePoint(fptCoord[i], 0.0, 1.0, fptCenter.x, fptCenter.y);
				MyCallApertureList(fptCoord[i].x, fptCoord[i].y,m_nApertureListIndex);
			}
// 			m_pParent->m_csDrawing.Unlock();
			return FALSE;
		}
		break;
	}

//  	m_pParent->m_csDrawing.Unlock();
	return FALSE;
}

/*
void CDrawRs274X::AllocDrawList()
{
// 	m_bDrawFirst=TRUE;
	AllocCamDrawStruct();
	SetDrawResolution(m_fDrawArcResolution);
//	int nAperturecnt = MakeApertureList();
	SetDrawResolution(m_fDrawArcResolution); // 1/10 of Monitor Pixel resolution
	int nLineCnt = MakeLineTemplateList();
 	int nSnRObject = MakeSnRObjectList();
}

void CDrawRs274X::AllocCamDrawStruct()
{
	m_nNumOfAperture = GetNumOfAperture();
// 	if(m_nNumOfAperture>0)
// 		m_nListAperture = glGenLists(m_nNumOfAperture);

	m_nNumOfLineTemplate = GetNumOfLineTemplate();
// 	if(m_nNumOfLineTemplate>0)
// 		m_glListLines=glGenLists(m_nNumOfLineTemplate);     //Line List
	
	m_nNumOfSnRObject = GetNumOfSnR();
// 	if(m_nNumOfSnRObject>0)
// 		m_glListSnR = glGenLists(m_nNumOfSnRObject);

//	m_nGLListIndex = 0;
	m_nListIndex = 0;
}
*/

void CDrawRs274X::DrawPolygonLine(const vector <FPOINTC> &vecPt,int nLineWidth)
{
	if(nLineWidth < 1)		nLineWidth = 1;
	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, nLineWidth, RGB(0, 0, 255));
	CPen* op = cdc->SelectObject(&p);

//	float lineWidth;
	int i, nNumOfVertex;
	nNumOfVertex = vecPt.size();
	if(nNumOfVertex<1)
		return;

	FPOINTC fPosWin, fPosCam;
	fPosCam = vecPt.at(0);
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
	 	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
// 	for( i=1; i<nNumOfVertex; i++)
// 	{
// 		cdc->LineTo(vecPt.at(i).x, vecPt.at(i).y);
// 	}
	CPoint *pts = new CPoint[nNumOfVertex+1];
	pts[0].x = fPosWin.x; pts[0].y = fPosWin.y;
	for( i=1; i<nNumOfVertex; i++)
	{
		fPosCam = vecPt.at(i);
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	cdc->Polygon(pts, nNumOfVertex);
	delete[] pts;

	cdc->SelectObject(op);

//	::ReleaseDC(m_hParent, hDC);

// 	if(nLineWidth != 0)
// 	{
// 		glGetFloatv(GL_LINE_WIDTH,&lineWidth);
// 		glLineWidth(nLineWidth);
// 		glBegin(GL_LINE_LOOP);
// 		for( i=0; i<nNumOfVertex; i++)
// 		{
// 			glVertex2f(vecPt.at(i).x, vecPt.at(i).y);
// 		}
// 		glEnd();
// 		glLineWidth(lineWidth);
// 	}
// 	else
// 	{
// 		glBegin(GL_LINE_LOOP);
// 		for( i=0; i<nNumOfVertex; i++)
// 		{
// 			glVertex2f(vecPt.at(i).x, vecPt.at(i).y);
// 		}
// 		glEnd();
// 	}
}

void CDrawRs274X::GetArcVertices(vector<FPOINTC> &vecPt, const FPOINTC &ptCenter, float radius, float start_angle, float arc_angle, int dir)
{
	float theta, tangetial_factor, radial_factor;
	float x,y,tx,ty; 
	FPOINTC BufPt;

	int ii;
	int num_segments = GetArcSegment(radius,arc_angle*RAD2DEG);

	theta = arc_angle / float(num_segments - 1);//theta is now calculated from the arc angle instead, the - 1 bit comes from the fact that the arc is open

	tangetial_factor = tanf(theta);

	radial_factor = cosf(theta);

	x = radius * cosf(start_angle);//we now start at the start angle
	y = radius * sinf(start_angle); 


	for(ii = 0; ii < num_segments; ii++)
	{ 
		BufPt.x = x + ptCenter.x;
		BufPt.y = y + ptCenter.y;
		vecPt.push_back(BufPt);

		if(dir == CW) //cw direction
		{
			tx = y; 
			ty = -x;
		}
		else // ccw direction
		{
			tx = -y; 
			ty = x;
		}

		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 

		x *= radial_factor; 
		y *= radial_factor; 
	} 
}


void CDrawRs274X::GetArcVertices(vector<FPOINT> &vecPt, const FPOINT &ptCenter, double radius, double start_angle, double arc_angle,int dir)
{
	float theta, tangetial_factor, radial_factor;
	float x,y,tx,ty; 
	FPOINT BufPt;

	int ii;
	int num_segments = GetArcSegment(radius,arc_angle*RAD2DEG);

	theta = arc_angle / double(num_segments - 1);//theta is now calculated from the arc angle instead, the - 1 bit comes from the fact that the arc is open

	tangetial_factor = tanf(theta);

	radial_factor = cosf(theta);

	x = radius * cosf(start_angle);//we now start at the start angle
	y = radius * sinf(start_angle); 


	for(ii = 0; ii < num_segments; ii++)
	{ 
		BufPt.x = x + ptCenter.x;
		BufPt.y = y + ptCenter.y;
		vecPt.push_back(BufPt);

		if(dir == CW) //cw direction
		{
			tx = y; 
			ty = -x;
		}
		else // ccw direction
		{
			tx = -y; 
			ty = x;
		}

		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 

		x *= radial_factor; 
		y *= radial_factor; 
	} 
}

void CDrawRs274X::CallList(int nListIndex)
{

}

FPOINTC CDrawRs274X::ConvertWindowCoord(FPOINTC& fPoint)
{
	FPOINTC fP, szWin, OriginWin, OriginCam;
	double dMargin = 0.1;
	szWin.x = double(m_pParent->m_sizeWindow.cx) * (1.0-dMargin);
	szWin.y = double(m_pParent->m_sizeWindow.cy) * (1.0-dMargin);
	OriginWin.x = double(m_pParent->m_sizeWindow.cx) * dMargin / 2.0;
	OriginWin.y = double(m_pParent->m_sizeWindow.cy) * dMargin / 2.0;

	FRECTC camRect = m_pParent->m_FrameExtent;//GetFrameExtent();
	OriginCam.x = -1.0 * camRect.X1; // Cam - Left
	OriginCam.y = -1.0 * camRect.Y1; // Cam - Top

	double dWidth = double(camRect.X2 - camRect.X1);
	double dHeight = double(camRect.Y2 - camRect.Y1);
	double dResolutionX = szWin.x / dWidth;
	double dResolutionY = szWin.y / dHeight;
	double dResolution = (dResolutionX > dResolutionY) ? dResolutionY : dResolutionX;
	dResolution *= 1.0; // Zoom In (x1)

	fP.x = (fPoint.x + OriginCam.x) * dResolution + OriginWin.x;
	fP.y = (fPoint.y + OriginCam.y) * dResolution + OriginWin.y;			// Coordinate origin is left-top.
// 	fP.y = (double)m_pParent->m_sizeWindow.cy - (fPoint.y * dResolution + Origin.y);	// Coordinate origin is left-bottom.

	return fP;
}

FPOINTC CDrawRs274X::ConvertResolutionCoord(FPOINTC& fPoint)
{
	FPOINTC fP, szWin, OriginWin, OriginCam;
	double dMargin = 0.1;
	szWin.x = double(m_pParent->m_sizeWindow.cx) * (1.0-dMargin);
	szWin.y = double(m_pParent->m_sizeWindow.cy) * (1.0-dMargin);
	OriginWin.x = double(m_pParent->m_sizeWindow.cx) * dMargin / 2.0;
	OriginWin.y = double(m_pParent->m_sizeWindow.cy) * dMargin / 2.0;

	FRECTC camRect = m_pParent->m_FrameExtent;//GetFrameExtent();
	OriginCam.x = -1.0 * camRect.X1; // Cam - Left
	OriginCam.y = -1.0 * camRect.Y1; // Cam - Top

	fP.x = (fPoint.x + OriginCam.x) * 1000.0 / m_dPixelResolution + OriginWin.x;
	fP.y = (fPoint.y + OriginCam.y) * 1000.0 / m_dPixelResolution + OriginWin.y;			// Coordinate origin is left-top.
// 	fP.y = (double)m_pParent->m_sizeWindow.cy - (fPoint.y * dResolution + Origin.y);	// Coordinate origin is left-bottom.

	return fP;
}

void CDrawRs274X::DrawConvexPolygon(const vector <FPOINTC> &fPoint)
{
//	glBegin(GL_POLYGON);

	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);

	LOGBRUSH lb; 
	lb.lbStyle = PS_SOLID;
	lb.lbColor = RGB(255, 0, 0);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, &lb);
	CPen* op = cdc->SelectObject(&pen);

//	CBrush brushRed(RGB(255, 0, 0));   
// 	CBrush brushRed;   
// 	brushRed.CreateSolidBrush(RGB(255, 0, 0));	// 브러시 생성
// 	CBrush* pOldBrush;
// 	pOldBrush = cdc->SelectObject(&brushRed);	// filled circle

	int i,nNumOfVertex;
	nNumOfVertex = fPoint.size();
	if(nNumOfVertex<1)
		return;

	FPOINTC fPosWin, fPosCam;
	fPosCam = fPoint.at(0);
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
/*
	for( i=1; i<nNumOfVertex; i++)
	{
// 		glVertex2f(fPoint.at(i).x,fPoint.at(i).y);			
		fPosCam = fPoint.at(i);
		fPosWin = ConvertWindowCoord(fPosCam);
		cdc->LineTo(fPosWin.x, fPosWin.y);
	}
*/
//	cdc->BeginPath();
	CPoint *pts = new CPoint[nNumOfVertex+1];
	for( i=0; i<nNumOfVertex; i++)
	{
		fPosCam = fPoint.at(i);
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
//	cdc->EndPath();

// 	int nNumPts;
// 	nNumPts = cdc->GetPath(NULL, NULL, 0);
// 	LPPOINT lpPoints = new POINT[nNumPts];
// 	LPBYTE lpTypes = new BYTE[nNumPts];
// 	nNumPts = cdc->GetPath(lpPoints, lpTypes, nNumPts);

// 	cdc->SetBkMode(OPAQUE);

//	cdc->SetPolyFillMode(WINDING);//ALTERNATE
	cdc->Polygon(pts, nNumOfVertex+1);
	delete[] pts;

// 	cdc->SetBkMode(TRANSPARENT);



//	cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);

//	::ReleaseDC(m_hParent, hDC);

// 	glEnd();
}

void CDrawRs274X::DrawLineRect(float fSx, float fSy, float fEx, float fEy, float fLineWidth, float fLineHeight )
{   
	float ww, hh, a, b;
	FPOINTC pp[6];

	ww = fLineWidth / 2;
	hh = fLineHeight / 2;

	if (fSx != fEx && fSy != fEy)
	{
		if (fEx > fSx)
		{
			if (fEy > fSy)
			{
				pp[0].x = (fSx - ww);
				pp[0].y = (fSy - hh);
				pp[1].x = (fSx - ww);
				pp[1].y = (fSy + hh);
				pp[2].x = (fEx - ww);
				pp[2].y = (fEy + hh);
				pp[3].x = (fEx + ww);
				pp[3].y = (fEy + hh);
				pp[4].x = (fEx + ww);
				pp[4].y = (fEy - hh);
				pp[5].x = (fSx + ww);
				pp[5].y = (fSy - hh);
			}
			else
			{
				pp[0].x = (fSx - ww);
				pp[0].y = (fSy - hh);
				pp[1].x = (fSx - ww);
				pp[1].y = (fSy + hh);
				pp[2].x = (fSx + ww);
				pp[2].y = (fSy + hh);
				pp[3].x = (fEx + ww);
				pp[3].y = (fEy + hh);
				pp[4].x = (fEx + ww);
				pp[4].y = (fEy - hh);
				pp[5].x = (fEx - ww);
				pp[5].y = (fEy - hh);
			}
		}
		else
		{
			if (fSy > fEy)
			{
				pp[0].x = (fEx - ww);
				pp[0].y = (fEy - hh);
				pp[1].x = (fEx - ww);
				pp[1].y = (fEy + hh);
				pp[2].x = (fSx - ww);
				pp[2].y = (fSy + hh);
				pp[3].x = (fSx + ww);
				pp[3].y = (fSy + hh);
				pp[4].x = (fSx + ww);
				pp[4].y = (fSy - hh);
				pp[5].x = (fEx + ww);
				pp[5].y = (fEy - hh);
			}
			else
			{
				pp[0].x = (fEx - ww);
				pp[0].y = (fEy - hh);
				pp[1].x = (fEx - ww);
				pp[1].y = (fEy + hh);
				pp[2].x = (fEx + ww);
				pp[2].y = (fEy + hh);
				pp[3].x = (fSx + ww);
				pp[3].y = (fSy + hh);
				pp[4].x = (fSx + ww);
				pp[4].y = (fSy - hh);
				pp[5].x = (fSx - ww);
				pp[5].y = (fSy - hh);
			}
		}
	
		DrawConvexPolygon(pp,6);
	}
	else
	{
		FPOINTC pp[4];
		if (fSx == fEx && fSy == fEy)
		{
			pp[0].x = (fSx - ww);     //  '좌하
			pp[0].y = (fSy - hh);     //
			pp[1].x = (fSx + ww);     //  '우하
			pp[1].y = (fSy - hh);     //

			pp[2].x = (fEx + ww);     //  '우상
			pp[2].y = (fEy + hh);     //
			pp[3].x = (fEx - ww);     //  '좌상
			pp[3].y = (fEy + hh);     //
		}
		else if (fSx == fEx)
		{
			if (fSy > fEy )//  'b > a가 되게
			{
				a = fEy;
				b = fSy;
			}
			else
			{
				a = fSy;
				b = fEy;
			}
			pp[0].x = (fSx - ww);     //  '좌하
			pp[0].y = (a - hh);      //
			pp[1].x = (fSx + ww);     //  '우하
			pp[1].y = (a - hh);      //

			pp[2].x = (fEx + ww);     //  '우상
			pp[2].y = (b + hh);      //
			pp[3].x = (fEx - ww);    //   '좌상
			pp[3].y = (b + hh);     // 
		}
		else
		{
			if (fSx > fEx)
			{
				a = fEx;
				b = fSx;
			}
			else
			{
				a = fSx;
				b = fEx;
			}
			pp[0].x = (a - ww);      //  '좌하
			pp[0].y = (fSy - hh);     //
			pp[1].x = (b + ww);      //  '우하
			pp[1].y = (fSy - hh);    // 

			pp[2].x = (b + ww);      //  '우상
			pp[2].y = (fEy + hh);     //
			pp[3].x = (a - ww);     //   '좌상
			pp[3].y = (fEy + hh);    // 
		}
		DrawConvexPolygon(pp,4);		
	}
}

void CDrawRs274X::DrawConvexPolygon(const FPOINTC *fPoint,int nNumberOfVertex)
{
	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, 1, RGB(0, 255, 255));
	CPen* op = cdc->SelectObject(&p);

	int i;
	if(nNumberOfVertex<1)
		return;

	FPOINTC fPosWin, fPosCam;
	fPosCam = fPoint[0];
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

	CPoint *pts = new CPoint[nNumberOfVertex+1];
	for( i=0; i<nNumberOfVertex; i++)
	{
		fPosCam = fPoint[i];
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	cdc->Polygon(pts, nNumberOfVertex+1);
	delete[] pts;

	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);

/*
//	glBegin(GL_POLYGON);
	int i;
	for( i=0; i<nNumberOfVertex; i++)
	{
		;
//		glVertex2f(fPoint[i].x, fPoint[i].y);
	}
//	glEnd();
*/
}


BOOL CDrawRs274X::MyMacro(const CDrawRs274X::LPAPERTURE_DEF &pAptDef, float x, float y, byte Pol)
{	
//	LPAPERTURE_DEF pAptDef = (LPAPERTURE_DEF)_pAptDef;

	vector <FPOINTC> vecVertics;
	CDC* cdc;
	CPen p; 
	CPen* op;
	CPoint *pts;
	FPOINTC fPosWin, fPosCam;

	BOOL bOnOff;
	int i, j, kk, nNumOfVertex;
	FPOINTC fpt;
	BOOL bClearDataObject=FALSE;
	float cV, sV, fAng, X1, Y1, X2, Y2, fRadius;

	float *fParam;

	
	APERTURE_PRIMITIVE AptPrim;
	int nParam = pAptDef->Param.size();
	if(m_nApertureListIndex < 1)
		m_pParent->m_pAptDef->nGlListIndex = nParam; // 20170519-syd

// 	glClear(GL_STENCIL_BUFFER_BIT);
// 	glEnable(GL_STENCIL_TEST);

	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	cdc = CDC::FromHandle(hDC);
	p.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	op = cdc->SelectObject(&p);

	for(j = nParam-1 ; j >=0 ; j--)  //nParam
	{
		AptPrim = pAptDef->Param.at(j);
		fParam = &AptPrim.fParam[0];
		
		if(AptPrim.nPriNum != 6 && AptPrim.nPriNum != 7)
		{
			bOnOff = fParam[0];
		}
			
		switch(AptPrim.nPriNum)
		{
		case 1:
//			SetStencil(bOnOff);
			fRadius = fParam[1]/2.0;
			DrawCircle(fParam[2] + x, fParam[3] + y, fRadius);	 //onoff 1 =dark 2=clear
			break;
		case 21:
//			SetStencil(bOnOff);
			MyRectCenter(fParam[3] + x, fParam[4] + y, fParam[1], fParam[2], fParam[5]); //onoff 1 =dark 2=clear
			break;
		case 4:
			if(nNumOfVertex < 1)
				break;

			vecVertics.clear();
// 			m_vecVertics.clear();
			nNumOfVertex = fParam[1] / 2;
			
			kk = 2;
			for(i = 0 ; i < nNumOfVertex ; i++)
			{
				fpt.x = fParam[kk] + x;
				kk++;
				fpt.y = fParam[kk] + y;
				kk++;

				vecVertics.push_back(fpt);
// 				m_vecVertics.push_back(fpt);
			}
// 			glStencilFunc(GL_NEVER,0,1);
// 			glStencilOp(GL_INVERT,GL_INVERT,GL_INVERT);
// 			glBegin(GL_TRIANGLE_FAN);
// 			for(i=0; i<nNumOfVertex; i++)
// 			{
// 				glVertex2f(m_vecVertics.at(i).x,m_vecVertics.at(i).y);
// 			}
// 			glEnd();		

			if(nNumOfVertex > 0)
			{
				fPosCam = vecVertics.at(0);
// 				fPosCam = m_vecVertics.at(0);
				if(m_dPixelResolution)
					fPosWin = ConvertResolutionCoord(fPosCam);
				else
					fPosWin = ConvertWindowCoord(fPosCam);
				cdc->MoveTo(fPosWin.x, fPosWin.y);

				pts = new CPoint[nNumOfVertex+1];
				for( i=0; i<nNumOfVertex; i++)
				{
					fPosCam = vecVertics.at(i);
// 					fPosCam = m_vecVertics.at(i);
					if(m_dPixelResolution)
						fPosWin = ConvertResolutionCoord(fPosCam);
					else
 						fPosWin = ConvertWindowCoord(fPosCam);
					pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
				}
				pts[i].x = pts[0].x; pts[i].y = pts[0].y;
				cdc->Polygon(pts, nNumOfVertex);
				delete[] pts;
			}
			break;
		case 5:
// 			glDisable(GL_STENCIL_TEST);
 			MyPolygonNSide(fParam[1], fParam[2] + x, fParam[3] + y, fParam[4], fParam[5]);
			break;
		case 6:
// 			glDisable(GL_STENCIL_TEST);
 			MyMoire(fParam[0] + x, fParam[1] + y, fParam[2], fParam[3], fParam[4], fParam[5], fParam[6], fParam[7], fParam[8]);
			break;  //need
		case 7:
// 			glDisable(GL_STENCIL_TEST);
 			MyThermal(fParam[0] + x, fParam[1] + y, fParam[2], fParam[3], fParam[5], 4, fParam[4], 0);	
			break;
		case 2:
		case 20:
// 			SetStencil(bOnOff);
			fAng = fParam[6];
			X1 = fParam[2];
			Y1 = fParam[3];
			X2 = fParam[4];
			Y2 = fParam[5];
			if(fAng != 0)
			{
				cV = cos(fAng * DEG2RAD);
				sV = sin(fAng * DEG2RAD);
				
				X1 = fParam[2] * cV - fParam[3] * sV;
				Y1 = fParam[2] * sV + fParam[3] * cV;
				X2 = fParam[4] * cV - fParam[5] * sV;
				Y2 = fParam[4] * sV + fParam[5] * cV;
			}
 			MyLine(X1 + x, Y1 + y, X2 + x, Y2 + y, fParam[1]);
			break;
		case 22:   //need
 			//SetStencil(bOnOff);
 			MyRectLowerLeft(fParam[3] + x, fParam[4] + y, fParam[1], fParam[2], fParam[5]);
			break;
		}
	}

	for(j = 0 ; j < nParam ; j++)  //nParam
	{
		AptPrim = pAptDef->Param.at(j);
		fParam = &AptPrim.fParam[0];
		switch(AptPrim.nPriNum)
		{
		case 4:
 			vecVertics.clear();
//  		m_vecVertics.clear();
			nNumOfVertex = fParam[1] / 2;
			
			kk = 2;
			for(i = 0 ; i < nNumOfVertex ; i++)
			{
				fpt.x = fParam[kk] + x;
				kk++;
				fpt.y = fParam[kk] + y;
				kk++;

 				vecVertics.push_back(fpt);
//				m_vecVertics.push_back(fpt);
			}
// 			glStencilFunc(GL_EQUAL,1,1);
// 			glStencilOp(GL_ZERO,GL_ZERO,GL_ZERO);
// 			glBegin(GL_TRIANGLE_FAN);
// 			for(i=0; i<nNumOfVertex; i++)
// 			{
// 				glVertex2f(m_vecVertics.at(i).x,m_vecVertics.at(i).y);
// 			}
// 			glEnd();
// 
			if(nNumOfVertex > 0)
			{
				fPosCam = vecVertics.at(0);
// 				fPosCam = m_vecVertics.at(0);
				if(m_dPixelResolution)
					fPosWin = ConvertResolutionCoord(fPosCam);
				else
					fPosWin = ConvertWindowCoord(fPosCam);
				cdc->MoveTo(fPosWin.x, fPosWin.y);

				pts = new CPoint[nNumOfVertex+1];
				for( i=0; i<nNumOfVertex; i++)
				{
					fPosCam = vecVertics.at(i);
// 					fPosCam = m_vecVertics.at(i);
					if(m_dPixelResolution)
						fPosWin = ConvertResolutionCoord(fPosCam);
					else
 						fPosWin = ConvertWindowCoord(fPosCam);
					pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
				}
				pts[i].x = pts[0].x; pts[i].y = pts[0].y;
				cdc->Polygon(pts, nNumOfVertex+1);
				delete[] pts;
			}
			break;
		}
	}
// 	glDisable(GL_STENCIL_TEST);
	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);

	return FALSE;
}

BOOL CDrawRs274X::MyRectLowerLeft(float fSx, float fSy, float fLineWidth, float fLineHeight, float fAngle)
{
    FPOINTC cp;
	FPOINTC pp[4];
	float XX, YY, cV, sV;

    if (fAngle)
	{
        cV = cos(fAngle * DEG2RAD);
        sV = sin(fAngle * DEG2RAD);
        
        cp.x = fSx + fLineWidth / 2;
        cp.y = fSy + fLineHeight / 2;
    
        XX = -fLineWidth / 2;
        YY = -fLineHeight / 2;
        pp[0].x = (XX * cV) - (YY * sV) + cp.x;
        pp[0].y = (XX * sV) + (YY * cV) + cp.y;
//        '우하
        XX = fLineWidth / 2;
        YY = -fLineHeight / 2;
        pp[1].x = (XX * cV) - (YY * sV) + cp.x;
        pp[1].y = (XX * sV) + (YY * cV) + cp.y;
//        '우상
        XX = fLineWidth / 2;
        YY = fLineHeight / 2;
        pp[2].x = (XX * cV) - (YY * sV) + cp.x;
        pp[2].y = (XX * sV) + (YY * cV) + cp.y;
//        '좌상
        XX = -fLineWidth / 2;
        YY = fLineHeight / 2;
        pp[3].x = (XX * cV) - (YY * sV) + cp.x;
        pp[3].y = (XX * sV) + (YY * cV) + cp.y;
        
	}
    else
	{
        pp[0].x = fSx;           
        pp[0].y = fSy;           
        pp[1].x = fSx + fLineWidth;       
        pp[1].y = fSy;           
        
        pp[2].x = fSx + fLineWidth;       
        pp[2].y = fSy + fLineHeight;       
        pp[3].x = fSx;           
        pp[3].y = fSy + fLineHeight;       
    }

	DrawQuad(pp[0].x,pp[0].y,pp[1].x,pp[1].y,pp[2].x,pp[2].y,pp[3].x,pp[3].y,TRUE);	
	
	return FALSE;
}

BOOL CDrawRs274X::MyLine(float fSx, float fSy, float fEx, float fEy, float fLineWidth)
{
    double fLength, fHeight, fWidth, fTx, fTy;
	FPOINTC fp1,fp2,fp3,fp4;

    fLength = sqrt((fEx - fSx) * (fEx - fSx) + (fEy - fSy) * (fEy - fSy));
    
    if (fLength < 0.000001)
	{
        fp1.x =fSx - fLineWidth / 2.0;
        fp1.y =fSy - fLineWidth / 2.0;
        fp2.x =fSx - fLineWidth / 2.0;
        fp2.y =fSy + fLineWidth / 2.0;
		fp3.x =fSx + fLineWidth / 2.0;
        fp3.y =fSy + fLineWidth / 2.0;
		fp4.x =fSx + fLineWidth / 2.0;
        fp4.y =fSy - fLineWidth / 2.0;
    }    
    else
	{
        fHeight = fEy - fSy;
        fWidth = fEx - fSx;

        fTy = fLineWidth / 2.0 * fWidth / fLength;
        fTx = fLineWidth / 2.0 * fHeight / fLength;
            
        fp1.x =fSx + fTx;
        fp1.y =fSy - fTy;
        fp2.x =fEx + fTx;
        fp2.y =fEy - fTy;
        fp3.x =fEx - fTx;
        fp3.y =fEy + fTy;
        fp4.x =fSx - fTx;
        fp4.y =fSy + fTy;
    }

	DrawQuad(fp1.x,fp1.y,fp2.x,fp2.y,fp3.x,fp3.y,fp4.x,fp4.y,TRUE);
	
	return FALSE;
}

BOOL CDrawRs274X::MyThermal(float x, float y, float Dmo, float Dmi, float Ang, int num,  float gap, float angle)
{
    float   Ro, Ri, Theta1, Theta2, t, cV1, cV2, sV1, sV2;
	FPOINTC pp[9];
    int i;
  	FPOINTC fpt = FPOINTC(x, y);  

//	if(m_nApertureListIndex==-1)
	{
		Ro = Dmo / 2;
		Ri = Dmi / 2;
		t = gap / 2;
		Ang = Ang * DEG2RAD;

		Theta1 = atan(t / sqrt(Ri * Ri - t * t));
		cV1 = cos(Ang + Theta1);
		sV1 = sin(Ang + Theta1);

		Theta2 = atan(t / sqrt(Ro * Ro - t * t));
		cV2 = cos(Ang + Theta2);
		sV2 = sin(Ang + Theta2);

		pp[0].x = x + Ri * cV1;
		pp[0].y = y + Ri * sV1;
		pp[1].x = x + Ro * cV2;
		pp[1].y = y + Ro * sV2;

		pp[2].x = -DetGcode;  //'CCW
		pp[2].y = -DetGcode;

		cV1 = cos(Ang + PI / 2 - Theta1);
		sV1 = sin(Ang + PI / 2 - Theta1);
		cV2 = cos(Ang + PI / 2 - Theta2);
		sV2 = sin(Ang + PI / 2 - Theta2);

		pp[3].x = x + Ro * cV2;
		pp[3].y = y + Ro * sV2;
		pp[4].x = x; //     'Center X
		pp[4].y = y; //     'Center Y

		pp[5].x = x + Ri * cV1;
		pp[5].y = y + Ri * sV1;

		pp[6].x = DetGcode;
		pp[6].y = DetGcode;
		pp[7] = pp[0];

		pp[8] = pp[4];    
	
		MyPolygon(pp, 9);

		for (i=1 ; i<num ; i++)
		{
			m_pTrUtil->RotatePoint90(pp[0],fpt);
			m_pTrUtil->RotatePoint90(pp[1],fpt);
			m_pTrUtil->RotatePoint90(pp[3],fpt);
			m_pTrUtil->RotatePoint90(pp[5],fpt);
			pp[7] = pp[0];

			MyPolygon(pp, 9);
		}
	}
// 	else
// 	{
// 		CallList(m_nApertureListIndex);
// 		glPushMatrix();
// 		glTranslatef(x,y,0.0);
// 		glCallList(m_nApertureListIndex);
// 		glPopMatrix();
// 	}  

	return FALSE;
}

BOOL CDrawRs274X::MyPolygon(const FPOINTC *fCoord, int nVertex)
{
	int i, npp, nDir;
	double	fRadius, fStartAngle, fEndAngle, fArcAngle;
	FPOINTC pPre, pPos, pIJ, fPt;
	vector <FPOINTC> vecVertics;

	vecVertics.clear();
// 	m_vecVertics.clear();

    npp = nVertex - 1;

	pPre = fCoord[0]; 
	vecVertics.push_back(pPre);
// 	m_vecVertics.push_back(pPre);

    for (i=1 ; i<=npp ; i++)
	{
		fPt = fCoord[i];
        if (fabs(fPt.x) == DetGcode && fabs(fPt.y) == DetGcode)
		{
            if (fPt.x > 0)	//+DetGCode
				nDir = 1;
            else
				nDir = -1;

            i++;
			pPos = fCoord[i];
            i++;
			pIJ = fCoord[i];

            fRadius = sqrt((pPos.x - pIJ.x) * (pPos.x - pIJ.x) + (pPos.y - pIJ.y) * (pPos.y - pIJ.y));

			fStartAngle = atan2(pPre.y - pIJ.y,pPre.x - pIJ.x);
			fEndAngle = atan2(pPos.y - pIJ.y,pPos.x - pIJ.x);

			if(nDir == -1)
			{
				if(fEndAngle>fStartAngle)
					fArcAngle = fEndAngle-fStartAngle;
				else
					fArcAngle = 2*PI-fStartAngle+fEndAngle;
			}
			else
			{
				if(fEndAngle>fStartAngle)
					fArcAngle = 2*PI-fEndAngle+fStartAngle;
				else
					fArcAngle = fStartAngle-fEndAngle;
			}
			
			if(fArcAngle == 0)
				fArcAngle = 2*PI;

			GetArcVertices(vecVertics,pIJ,fRadius,fStartAngle,fArcAngle,nDir);
// 			GetArcVertices(m_vecVertics,pIJ,fRadius,fStartAngle,fArcAngle,nDir);
			
			fPt = vecVertics.at(vecVertics.size() - 1);
// 			fPt = m_vecVertics.at(m_vecVertics.size() - 1);

			if(fPt.x != pPos.x ||  fPt.y != pPos.y)
				vecVertics.push_back(pPos);
// 				m_vecVertics.push_back(pPos);

        }
        else
		{
            pPos = fCoord[i];

			vecVertics.push_back(pPos);
// 			m_vecVertics.push_back(pPos);
        }
		pPre = pPos;
    }

	DrawConcavePolygon(vecVertics);
// 	DrawConcavePolygon(m_vecVertics);

	return FALSE;
}

BOOL CDrawRs274X::MyLineMacro(float fSx, float fSy, float fEx, float fEy, FRECTC fExtent)
{
	float fWidth, fHeight, fTheta;
	FPOINTC fptCoord;
	vector <FPOINTC> vecVertics;

	fWidth = (fExtent.X2 - fExtent.X1) / 2.0;
    fHeight = (fExtent.Y2 - fExtent.Y1) / 2.0;
    
 	vecVertics.clear();
//	m_vecVertics.clear();

    if(fSx != fEx && fSy != fEy)
	{
        fTheta = atan2(fabs(fEy - fSy), fabs(fEx - fSx));
        fWidth = fWidth * cos(fTheta);
        fHeight = fHeight * sin(fTheta);
    }
            
    if(fSx == fEx)
	{
        fptCoord.x = fSx - fWidth;
        fptCoord.y = fSy;
		vecVertics.push_back(fptCoord);
// 		m_vecVertics.push_back(fptCoord);
        fptCoord.x = fSx + fWidth;
        fptCoord.y = fSy;
		vecVertics.push_back(fptCoord);
// 		m_vecVertics.push_back(fptCoord);
        fptCoord.x = fEx + fWidth;
        fptCoord.y = fEy;
		vecVertics.push_back(fptCoord);
// 		m_vecVertics.push_back(fptCoord);
        fptCoord.x = fEx - fWidth;
        fptCoord.y = fEy;
		vecVertics.push_back(fptCoord);
// 		m_vecVertics.push_back(fptCoord);
	}
    else if(fEx > fSx)
	{
        if(fSy == fEy)
		{
			fptCoord.x = fSx;
            fptCoord.y = fSy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fSx;
            fptCoord.y = fSy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx;
            fptCoord.y = fEy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx;
            fptCoord.y = fEy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
		}
        else if(fEy > fSy)
		{
            fptCoord.x = fSx + fWidth;
            fptCoord.y = fSy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fSx - fWidth;
            fptCoord.y = fSy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx - fWidth;
            fptCoord.y = fEy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx + fWidth;
            fptCoord.y = fEy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
		}
        else
		{
            fptCoord.x = fSx - fWidth;
            fptCoord.y = fSy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fSx + fWidth;
            fptCoord.y = fSy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx + fWidth;
            fptCoord.y = fEy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx - fWidth;
            fptCoord.y = fEy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
        }
	}
    else
	{
        if(fSy == fEy)
		{
            fptCoord.x = fSx;
            fptCoord.y = fSy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fSx;
            fptCoord.y = fSy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx;
            fptCoord.y = fEy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx;
            fptCoord.y = fEy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
		}
        else if(fEy > fSy)
		{
            fptCoord.x = fSx + fWidth;
            fptCoord.y = fSy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fSx - fWidth;
            fptCoord.y = fSy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx - fWidth;
            fptCoord.y = fEy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx + fWidth;
            fptCoord.y = fEy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
		}
        else
		{
            fptCoord.x = fSx - fWidth;
            fptCoord.y = fSy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fSx + fWidth;
            fptCoord.y = fSy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx + fWidth;
            fptCoord.y = fEy - fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
            fptCoord.x = fEx - fWidth;
            fptCoord.y = fEy + fHeight;
			vecVertics.push_back(fptCoord);
// 			m_vecVertics.push_back(fptCoord);
        }
    }

	DrawConcavePolygon(vecVertics);
// 	DrawConcavePolygon(m_vecVertics);
	
	return FALSE;
}

int CDrawRs274X::GetArcSegment(float fRadius, float fAngle)
{
	if(fAngle<0)
		fAngle=-fAngle;
	
	double fOptSeg = 100*sqrt(fRadius);
	fOptSeg *= (fAngle/360.0);
	
	double fResSeg = (2.0*PI*fRadius) / m_pParent->m_fDrawResolution;
	fResSeg *= (fAngle/360.0);
	
	double fMinSeg = max(2.0, fAngle/60.0);
	double fSeg = max(fMinSeg, min(fOptSeg, fResSeg));
	int nSeg = ceil(fSeg);

	return nSeg;
}

void CDrawRs274X::DrawCircle(float cx, float cy, float r, int fill) 
{ 
	HDC  hDC = m_hDC;
// 	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, 1, RGB(255, 255, 0));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushRed(RGB(255, 255, 0));   
	CBrush* pOldBrush;

	if(fill)	// filled circle
		pOldBrush = cdc->SelectObject(&brushRed);
	// hollow circle since the circle is a closed curve, this is a loop now

	int i;
	int num_segments = GetArcSegment(r);
 	float circle_length = (2.0 * PI * r);
	float theta = 2.0 * PI / float(num_segments); //theta is now calculated from the arc angle instead, the - 1 bit comes from the fact that the arc is open
	float tangetial_factor = tanf(theta);
	float radial_factor = cosf(theta);
	float x = r;
	float y = 0;
	float tx , ty;

	FPOINTC fPosWin, fPosCam;
	fPosCam.x = x + cx;
	fPosCam.y = y + cy;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

//	cdc->BeginPath();
	CPoint *pts = new CPoint[num_segments+1];
	pts[0].x = fPosWin.x; pts[0].y = fPosWin.y;
	
	for(i = 1; i < num_segments; i++)
	{ 
		tx = -y; 
		ty = x; 
		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 
		x *= radial_factor; 
		y *= radial_factor; 

		fPosCam.x = x + cx;
		fPosCam.y = y + cy;
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	} 
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
//	cdc->EndPath();

// 	int nNumPts = cdc->GetPath(NULL, NULL, 0);
// 	if(nNumPts > 0)
// 	{
// 		LPPOINT lpPoints = new POINT[nNumPts];
// 		LPBYTE lpTypes = new BYTE[nNumPts];
// 		nNumPts = cdc->GetPath(lpPoints, lpTypes, nNumPts);
// 	}
	
	if(fill)	// filled circle
		cdc->SetPolyFillMode(WINDING);//ALTERNATE
	cdc->Polygon(pts, num_segments+1);
	delete[] pts;

	if(fill)	// filled circle
		cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);


/*
// 	if(fill)
// 		glBegin(GL_POLYGON); // filled circle
// 	else
// 		glBegin(GL_LINE_LOOP);// hollow circle since the circle is a closed curve, this is a loop now

	for(ii = 0; ii < num_segments; ii++)
	{ 
//		glVertex3f(x + cx, y + cy,0.0f);

		tx = -y; 
		ty = x; 


		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 

		x *= radial_factor; 
		y *= radial_factor; 
	} 
//	glEnd(); 
*/
}

void CDrawRs274X::DrawDonut(float fCx, float fCy, float fDmo, float fDmi)
{ 
	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, 1, RGB(0, 255, 255));
	CPen* op = cdc->SelectObject(&p);

	float w = (fDmo - fDmi) / 2;
	int segments=GetArcSegment(fDmo/2);
	float theta = 2 * PI / float(segments); 
	float tangetial_factor = tanf(theta); 
	float radial_factor = 1 - cosf(theta); 
	float x1 = fCx + (fDmo/2); 
	float y1 = fCy;
	float x2 = fCx + (fDmi/2); 
	float y2 = fCy; 
	float tx,ty,rx,ry;

	int i;

	cdc->SetROP2(R2_WHITE);

	FPOINTC fPosWin, fPosCam;
	fPosCam.x = x2;
	fPosCam.y = y2;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

	CPoint *pts = new CPoint[segments+2];
	pts[0].x = fPosWin.x; pts[0].y = fPosWin.y;
	for(i=1; i<segments+1; i++)
	{
		tx = -(y2 - fCy); 
		ty = x2 - fCx; 
		x2 += tx * tangetial_factor; 
		y2 += ty * tangetial_factor; 
		rx = fCx - x2; 
		ry = fCy - y2; 
		x2 += rx * radial_factor; 
		y2 += ry * radial_factor; 

		fPosCam.x = x2;
		fPosCam.y = y2;
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	cdc->Polygon(pts, segments+2);

	cdc->SetROP2(R2_NOT);

	fPosCam.x = x1;
	fPosCam.y = y1;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
	pts[0].x = fPosWin.x; pts[0].y = fPosWin.y;
	for(i=1; i<segments+1; i++)
	{
		tx = -(y1 - fCy); 
		ty = x1 - fCx; 
		x1 += tx * tangetial_factor; 
		y1 += ty * tangetial_factor; 
		rx = fCx - x1; 
		ry = fCy - y1; 
		x1 += rx * radial_factor; 
		y1 += ry * radial_factor; 

		fPosCam.x = x1;
		fPosCam.y = y1;
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	cdc->Polygon(pts, segments+2);

	cdc->SetROP2(R2_NOP);
	
	fPosCam.x = x2;
	fPosCam.y = y2;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
	pts[0].x = fPosWin.x; pts[0].y = fPosWin.y;
	for(i=1; i<segments+1; i++)
	{
		tx = -(y2 - fCy); 
		ty = x2 - fCx; 
		x2 += tx * tangetial_factor; 
		y2 += ty * tangetial_factor; 
		rx = fCx - x2; 
		ry = fCy - y2; 
		x2 += rx * radial_factor; 
		y2 += ry * radial_factor; 

		fPosCam.x = x2;
		fPosCam.y = y2;
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	cdc->Polygon(pts, segments+2);

	delete[] pts;

	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);

/*
//	glEnable(GL_STENCIL_TEST);
//	SetStencil(FALSE);

//		glBegin(GL_POLYGON);
	for(i=0; i<segments+1; i++)
	{
// 		glVertex2f(x2, y2);
		tx = -(y2 - fCy); 
		ty = x2 - fCx; 
		x2 += tx * tangetial_factor; 
		y2 += ty * tangetial_factor; 
		rx = fCx - x2; 
		ry = fCy - y2; 
		x2 += rx * radial_factor; 
		y2 += ry * radial_factor; 
	}
//		glEnd();
//	SetStencil(TRUE);
//		glBegin(GL_POLYGON);
	for(i=0; i<segments+1; i++)
	{
// 		glVertex2f(x1, y1);
		tx = -(y1 - fCy); 
		ty = x1 - fCx; 
		x1 += tx * tangetial_factor; 
		y1 += ty * tangetial_factor; 
		rx = fCx - x1; 
		ry = fCy - y1; 
		x1 += rx * radial_factor; 
		y1 += ry * radial_factor; 
	}
// 		glEnd();
// 	ClearStencil(TRUE);
// 	glDisable(GL_STENCIL_TEST);
// 	glBegin(GL_LINE_LOOP);
	for(i=0; i<segments+1; i++)
	{
// 		glVertex2f(x2, y2);
		tx = -(y2 - fCy); 
		ty = x2 - fCx; 
		x2 += tx * tangetial_factor; 
		y2 += ty * tangetial_factor; 
		rx = fCx - x2; 
		ry = fCy - y2; 
		x2 += rx * radial_factor; 
		y2 += ry * radial_factor; 
	}
// 	glEnd();
*/	
}

void CDrawRs274X::SetStencil(BOOL bStencil)
{
	if(bStencil==STENCIL_DARK)
	{
// 		glStencilFunc(GL_EQUAL,0,15);
// 		glStencilOp(GL_ZERO,GL_ZERO,GL_ZERO);
	}
	else if(bStencil ==STENCIL_CLEAR)   //clear
	{
// 		glStencilFunc(GL_NEVER,1,1);
// 		glStencilOp(GL_INCR,GL_INCR,GL_INCR);	
	}
}

BOOL CDrawRs274X::MyRectCenter(float fSx, float fSy, float fLineWidth, float fLineHeight, float fAngle)
{
	FPOINTC pp[4];
	float cV, sV, X1, Y1, X2, Y2;

	float LineWidth;

	LineWidth=fLineWidth;

    fLineWidth = fLineWidth / 2;
    fLineHeight = fLineHeight / 2;
	
    if (fAngle != 0)
	{
        cV = cos(fAngle * DEG2RAD);
        sV = sin(fAngle * DEG2RAD);
        X1 = (fSx - fLineWidth * cV);
        Y1 = (fSy - fLineWidth * sV);
        X2 = (fSx + fLineWidth * cV);
        Y2 = (fSy + fLineWidth * sV);
        cV = fLineHeight * cV;
        sV = fLineHeight * sV;
	}
	
	if(fAngle != 0)
	{
		DrawQuad((X1 - sV),(Y1 + cV),(X1 + sV),(Y1 - cV),(X2 + sV),(Y2 - cV),(X2 - sV),(Y2 + cV),TRUE);		
	}
	else
	{
		DrawRect(CfPoint3D((fSx - fLineWidth),(fSy - fLineHeight),0.0),CfPoint3D((fSx + fLineWidth),(fSy + fLineHeight),0.0));
	}

	return FALSE;	
}

void CDrawRs274X::DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int fill)
{	
	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, 1, RGB(0, 0, 255));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushBlue(RGB(0, 0, 255));   
	CBrush* pOldBrush;

	if(fill)	// filled circle
		pOldBrush = cdc->SelectObject(&brushBlue);

	int i;
	FPOINTC fPosWin[4], fPosCam[4];
	CPoint *pts = new CPoint[5];

	fPosCam[0].x = x1;	fPosCam[0].y = y1;
	fPosCam[1].x = x2;	fPosCam[1].y = y2;
	fPosCam[2].x = x3;	fPosCam[2].y = y3;
	fPosCam[3].x = x4;	fPosCam[3].y = y4;

	for( i=0; i<4; i++)
	{
		if(m_dPixelResolution)
			fPosWin[i] = ConvertResolutionCoord(fPosCam[i]);
		else
 			fPosWin[i] = ConvertWindowCoord(fPosCam[i]);
		pts[i].x = fPosWin[i].x; pts[i].y = fPosWin[i].y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	
	cdc->MoveTo(pts[0].x, pts[0].y);
	cdc->Polygon(pts, 5);
	delete[] pts;

	if(fill)	// filled circle
		cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);

// 	if(fill)
// 		glBegin(GL_QUADS); // filled quad
// 	else
// 		glBegin(GL_LINE_LOOP); // hollow quad
// 
// 	glVertex2f(x1,y1);
// 	glVertex2f(x2,y2);
// 	glVertex2f(x3,y3);
// 	glVertex2f(x4,y4);
// 	glEnd();
}

void CDrawRs274X::DrawRect(const CfPoint3D &fptStart,const CfPoint3D &fptEnd,int width, int fill)
{
	if(width < 1)		width = 1;
	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, width, RGB(0, 0, 255));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushBlue(RGB(255, 0, 0));   
	CBrush* pOldBrush;

	if(fill)	// filled circle
		pOldBrush = cdc->SelectObject(&brushBlue);

	int i;
	FPOINTC fPosWin[4], fPosCam[4];
	CPoint *pts = new CPoint[5];

	fPosCam[0].x = fptStart.x;	fPosCam[0].y = fptStart.y;
	fPosCam[1].x = fptStart.x;	fPosCam[1].y = fptEnd.y;
	fPosCam[2].x = fptEnd.x;	fPosCam[2].y = fptEnd.y;
	fPosCam[3].x = fptEnd.x;	fPosCam[3].y = fptStart.y;

	for( i=0; i<4; i++)
	{
		if(m_dPixelResolution)
			fPosWin[i] = ConvertResolutionCoord(fPosCam[i]);
		else
 			fPosWin[i] = ConvertWindowCoord(fPosCam[i]);
		pts[i].x = fPosWin[i].x; pts[i].y = fPosWin[i].y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	
	cdc->MoveTo(pts[0].x, pts[0].y);
	cdc->Polygon(pts, 5);
	delete[] pts;

	if(fill)	// filled circle
		cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);

// 	float lineWidth;
// 	if(fill)
// 	{
// 		glBegin(GL_QUADS); // filled quad
// 
// 		glVertex3f(fptStart.x,fptStart.y,fptStart.z);
// 		glVertex3f(fptStart.x,fptEnd.y,fptStart.z);
// 		glVertex3f(fptEnd.x,fptEnd.y,fptStart.z);
// 		glVertex3f(fptEnd.x,fptStart.y,fptStart.z);
// 
// 		glEnd();
// 	}
// 	else
// 	{
// 		if(width != 0)
// 		{
// 			glGetFloatv(GL_LINE_WIDTH,&lineWidth);
// 
// 			glLineWidth(width);
// 			
// 			glBegin(GL_LINE_LOOP); // hollow quad
// 			glVertex3f(fptStart.x,fptStart.y,fptStart.z);
// 			glVertex3f(fptStart.x,fptEnd.y,fptStart.z);
// 			glVertex3f(fptEnd.x,fptEnd.y,fptStart.z);
// 			glVertex3f(fptEnd.x,fptStart.y,fptStart.z);
// 			glEnd();
// 
// 			glLineWidth(lineWidth);
// 		}
// 		else
// 		{
// 			glBegin(GL_LINE_LOOP); // hollow quad
// 			glVertex3f(fptStart.x,fptStart.y,fptStart.z);
// 			glVertex3f(fptStart.x,fptEnd.y,fptStart.z);
// 			glVertex3f(fptEnd.x,fptEnd.y,fptStart.z);
// 			glVertex3f(fptEnd.x,fptStart.y,fptStart.z);
// 			glEnd();
// 		}
// 	}
}

BOOL CDrawRs274X::MyOval(float fCx, float fCy, float fWidth, float fHeight)
{
    float  X1, Y1, X2, Y2, d, ww;
    
    if (fWidth > fHeight )
	{
        ww = fHeight;
        d = (fWidth - ww) / 2;
        X1 = fCx - d;
        Y1 = fCy;
        X2 = fCx + d;
        Y2 = fCy;
	}
    else
	{
        ww = fWidth;
        d = (fHeight - ww) / 2;
        X1 = fCx;
        Y1 = fCy - d;
        X2 = fCx;
        Y2 = fCy + d;
    }

	DrawLineRound(X1, Y1, X2, Y2, ww);

	return FALSE;
}

// an n-sided regular polygon
BOOL CDrawRs274X::MyPolygonNSide(int n, float cX, float cY, float d, float ang)   
{
	float cV, sV, sTheta, cTheta, XX, YY, R, X1, Y1, Theta;
	int i;
	vector <FPOINTC> vecVertics;

	FPOINTC fpt;
    if (n < 3) return FALSE;

	vecVertics.clear();
// 	m_vecVertics.clear();

    cV = cos(ang * DEG2RAD);
    sV = sin(ang * DEG2RAD);
    R = d / 2;
    
    Theta = 2 * PI / n;
    X1 = R;
    Y1 = 0;
    
    for (i=0 ; i<n ; i++)
	{
        sTheta = sin(i * Theta);
        cTheta = cos(i * Theta);
        XX = X1 * cTheta - Y1 * sTheta;
        YY = X1 * sTheta + Y1 * cTheta;          
            
        fpt.x =(XX * cV - YY * sV) + cX;
        fpt.y =(XX * sV + YY * cV) + cY;
		vecVertics.push_back(fpt);
// 		m_vecVertics.push_back(fpt);
    }

	DrawConcavePolygon(vecVertics);
// 	DrawConcavePolygon(m_vecVertics);

/*
	if(m_nApertureListIndex==-1)
	{
		DrawConcavePolygon(m_vecVertics);
	}
	else
	{
// 		glPushMatrix();
// 		glTranslatef(cX,cY,0);
// 		glCallList(m_nApertureListIndex);
// 		glPopMatrix();
	}
*/
	return FALSE;
}

BOOL CDrawRs274X::MyQuadrantPolygon(CDrawRs274X::GraphObj *obj, CDrawRs274X::GraphObj *NextObj)
{	
	int i, npp, nDir;
	double	fRadius, fStartAngle, fEndAngle, fArcAngle;
	FPOINTC pPre, pPos, pIJ, fPt;

	vector <FPOINTC> vecVertics;
	vecVertics.clear();
//	m_vecVertics.clear();

  npp = NextObj->nStPnt -obj->nStPnt  - 1;	
	
	pPre = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt];

    if (npp == 1) //     'Circular Area Fill
	{
        fPt = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+1];    //' Center Point
        fRadius = sqrt((pPre.x - fPt.x) * (pPre.x - fPt.x) + (pPre.y - fPt.y) * (pPre.y - fPt.y));
		DrawCircle(fPt.x, fPt.y, fRadius);
	}  
    else if (npp >= 2)
	{  
        for (i=1 ; i<=npp ; i++)
		{
			fPt = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
            if (fabs(fPt.x) == DetGcode && fabs(fPt.y) == DetGcode)
			{
                if (fPt.x > 0)	//+DetGCode
					nDir = 1;
                else
					nDir = -1;

	            i++;
                pPos = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
                                
                i++;
                pIJ = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
				pIJ.x += pPre.x;
				pIJ.y += pPre.y;
                                
                fRadius = sqrt((pPos.x - pIJ.x) * (pPos.x - pIJ.x) + (pPos.y - pIJ.y) * (pPos.y - pIJ.y));

				fStartAngle = atan2(pPre.y - pIJ.y,pPre.x - pIJ.x);
				fEndAngle = atan2(pPos.y - pIJ.y,pPos.x - pIJ.x);

				if(nDir == -1)
				{
					if(fEndAngle>fStartAngle)
						fArcAngle = fEndAngle-fStartAngle;
					else
						fArcAngle = 2*PI-fStartAngle+fEndAngle;
				}
				else
				{
					if(fEndAngle>fStartAngle)
						fArcAngle = 2*PI-fEndAngle+fStartAngle;
					else
						fArcAngle = fStartAngle-fEndAngle;
				}
				
				if(fArcAngle == 0)
					fArcAngle = 2*PI;

// 				GetArcVertices(m_vecVertics,pIJ,fRadius,fStartAngle,fArcAngle,nDir);
// 				fPt = m_vecVertics.at(m_vecVertics.size() - 1);
// 				if(fPt.x != pPos.x ||  fPt.y != pPos.y)
// 					m_vecVertics.push_back(pPos);
				GetArcVertices(vecVertics,pIJ,fRadius,fStartAngle,fArcAngle,nDir);
				fPt = vecVertics.at(vecVertics.size() - 1);
				if(fPt.x != pPos.x ||  fPt.y != pPos.y)
					vecVertics.push_back(pPos);
				

            }
            else
			{
                pPos = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
// 				m_vecVertics.push_back(pPos);
 				vecVertics.push_back(pPos);
           }

            pPre = pPos;
        }


// 		DrawConcavePolygon(m_vecVertics);
 		DrawConcavePolygon(vecVertics);
   }

	return FALSE;
}

BOOL CDrawRs274X::MySnRObjList(CDrawRs274X::GraphObj &gObj,int nObjIndex, float ddX, float ddY)
{
	int nStObj, nEdObj, nXRepeat, nYRepeat;
	float dX, dY;
	FRECTC  cBoundRect;
	FRECTC  frtObjExtent, fExtent;
	FRECTC frtCurrentExtent;

	BOOL bFind = FALSE, bFoundPickingObj;
	SNR_LIST_INDEX stSnrListIndex;
	int i,j, k,kk, nCoord;
	int nListIndex;

	GraphObj obj;
	vector <FPOINTC> vecObjCoord;
	FPOINTC fptOffset, fptCenter;
	dX = 0;
	dY = 0;

	frtCurrentExtent = m_pParent->m_FrameExtent;//GetFrameExtent();

	nStObj = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
	nEdObj = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
	
	nXRepeat = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x;
	nYRepeat = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;


	if(gObj.dCode > 0 && m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
	{
		nListIndex = gObj.dCode;

		for(i = 0 ; i < nXRepeat ; i++)
		{
			for(j = 0 ; j < nYRepeat ; j++)
			{
				if(i !=0 || j != 0)
				{
					dX = i*m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x + ddX;
					dY = j*m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y + ddY;
				
					cBoundRect.X1=gObj.Extent.X1+dX;
					cBoundRect.Y1=gObj.Extent.Y1+dY;
					cBoundRect.X2=gObj.Extent.X2+dX;
					cBoundRect.Y2=gObj.Extent.Y2+dY;

					if(CheckDrawAllow(cBoundRect))
						CallList(nListIndex);
// 					{			
// 						glPushMatrix();
// 						glTranslatef(dX,dY,0.0f);
// 						glCallList(nListIndex);
// 						glPopMatrix();					
// 					}
// 					else
// 					{
// 						int ttt=0;
// 					}
				}
			}
		}
	}
	else
	{
		for(i = 0 ; i < nXRepeat ; i++)
		{
			for(j = 0 ; j < nYRepeat ; j++)
			{
				dX = i*m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x + ddX;
				dY = j*m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y + ddY;
				
				if(i != 0 || j != 0)
				{
					for(int k= nStObj; k<= nEdObj; k++)
					{
						cBoundRect.X1=gObj.Extent.X1+dX;
						cBoundRect.Y1=gObj.Extent.Y1+dY;
						cBoundRect.X2=gObj.Extent.X2+dX;
						cBoundRect.Y2=gObj.Extent.Y2+dY;

 						if(CheckDrawAllow(cBoundRect))
						{
							MyObjEntityShift(k, dX, dY);
						}
					}
				}
			}
		}
	}

	return bFind;
}

BOOL CDrawRs274X::MyObjEntityShift(int k, float dX, float dY)
{
	float dX1, dY1;
	float fRadius;

	int pNum = 0;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;

	GraphObj gObj = m_pParent->m_pLayerInfo->listObj.at(k);

	if(gObj.Type.nAttr == DRAW_TYPE::NONE)
		return FALSE;

// 	if(m_bFindLineObjPickingMode)
// 	{
// 		if(gObj.Type.nType != DRAW_TYPE::LINE)
// 			return FALSE;
// 	}
	

	byte dType = gObj.Type.nType; 

	LPAPERTURE_DEF pAptDef;
	m_pParent->m_pLayerInfo->mapAptList.Lookup(gObj.dCode, pAptDef);


	if(m_nPrevAttribute != gObj.Type.nAttr || m_nPrevPolarity != gObj.Type.nPolarity)
	{
		m_crObjColor = GetSnRObjectColor(gObj);
		SetForeColor(m_crObjColor);
	}

    switch(dType)
	{
	case DRAW_TYPE::LINE:
		fParam = &pAptDef->Param.at(0).fParam[0];
	
        if (pAptDef->chStdAptType == _T('C')) // Then
		{
			dX1 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY1 = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;

			Key.Param[0] = (dX1*dX1 + dY1*dY1); // thickness
			Key.Param[1] = pAptDef->Param.at(0).fParam[0];// line width

			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID =SearchLineTemplate(Key.key);
				if(nID > 0)
				{
					nID+=m_nListLines;//m_glListLines;										
					MyLineList(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, nID);
				}
				else
					DrawLineRound(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, fParam[0]);
			}
			else
				DrawLineRound(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, fParam[0]);
		}
		else if (pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, fParam[0], fParam[1]);
		}
		else if(m_pParent->m_pAptDef->strAptName.GetLength() > 0)
		{
			if(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, gObj.Type.nPolarity);
				MyLineMacro(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, m_pParent->m_pAptDef->minmax);
				MyMacro(m_pParent->m_pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, gObj.Type.nPolarity);
			}
		}
		break;
	
    case DRAW_TYPE::FLASH:
		fParam = &pAptDef->Param.at(0).fParam[0];
		
		pNum = pAptDef->Param.at(0).fParam.size();

		m_nApertureListIndex = pAptDef->nGlListIndex;
		if(m_nApertureListIndex>0)
		{
			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				MyCallApertureList(m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,m_nApertureListIndex);
				return FALSE;
			}
		}

		if(pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{

			if (pNum == 1)
			{				
				fRadius = fParam[0] / 2.0;
				DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1]);
			}
			else if (pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//SetStencil(FALSE);
			
				if(pNum == 3)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[1], fParam[2], 0);
				}
				else if(pNum == 4)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[1], fParam[2], fParam[3]);
				}
				//SetStencil(TRUE);
				DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,fRadius); 
				//glDisable(GL_STENCIL_TEST);
			}
		}
		else if(pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[2], fParam[3], 0);
				}
				//SetStencil(TRUE);
				MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1], 0);
				//glDisable(GL_STENCIL_TEST);
			}
		}
		else if(pAptDef->chStdAptType == _T('O')) //    ' Oval
		{
			if(pNum < 3)
			{
				MyOval (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
						DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1], 0);
				}
				//SetStencil(TRUE);
				MyOval (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1]);
				//glDisable(GL_STENCIL_TEST);
			}				
		}
		else if(pAptDef->chStdAptType == _T('P'))
		{
			int nSide = fParam[1];
			
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[2]);      
			}
			else
			{
				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//m_GLDraw.SetStencil(FALSE);

				if(pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius);
				}
				else if (pNum >= 5)
				{
					MyRectCenter (m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[3], fParam[4], 0);
				}
				//SetStencil(TRUE);
				MyPolygonNSide(nSide, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[2]);  
				//glDisable(GL_STENCIL_TEST);
			}
		}
		else    //    ' Userdefined Apperture
		{ 
			MyMacro(pAptDef, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, gObj.Type.nPolarity);
        }
		break;
    case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ:
		MyQuadrantPolygonShift(&gObj,&m_pParent->m_pLayerInfo->listObj.at(k+1), dX, dY);
		break;
    case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &pAptDef->Param.at(0).fParam[0];

		MyArc ( m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, 
					m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, 
					 m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y+dY, 
					fParam[0], CW);
		break;
    case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &pAptDef->Param.at(0).fParam[0];

		MyArc ( m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, 
					m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, 
					 m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x+dX,  m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y+dY, 
					fParam[0], CCW);
		break;
	}

	return FALSE;
}

BOOL CDrawRs274X::MyQuadrantPolygonShift(CDrawRs274X::GraphObj *obj, CDrawRs274X::GraphObj *Nextobj, float dX, float dY)
{
	int i, npp, nDir;
	double	fRadius, fStartAngle, fEndAngle, fArcAngle;
	FPOINTC pPre, pPos, pIJ, fPt;
	vector <FPOINTC> vecVertics;

	vecVertics.clear();
// 	m_vecVertics.clear();

    npp = Nextobj->nStPnt -obj->nStPnt - 1;

	pPre = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt];
	pPre.x += dX;
	pPre.y += dY;

    if (npp == 1) //     'Circular Area Fill
	{
        fPt = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+1];    //' Center Point
        fRadius = sqrt((pPre.x - fPt.x) * (pPre.x - fPt.x) + (pPre.y - fPt.y) * (pPre.y - fPt.y));
		DrawCircle(fPt.x, fPt.y, fRadius);
	}  
    else if (npp >= 2)
	{
        for (i = 1 ; i<= npp ; i++)
		{
			fPt = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
            if (fabs(fPt.x) == DetGcode && fabs(fPt.y) == DetGcode )
			{
                if (fPt.x > 0)	//+DetGCode
					nDir = 1;
                else
					nDir = -1;

	            i++;
				pPos = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
                pPos.x += dX;
                pPos.y += dY;
                                
                i++;
                pIJ = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
				pIJ.x += pPre.x;
				pIJ.y += pPre.y;
                                
                fRadius = sqrt((pPos.x - pIJ.x) * (pPos.x - pIJ.x) + (pPos.y - pIJ.y) * (pPos.y - pIJ.y));

				fStartAngle = atan2(pPre.y - pIJ.y,pPre.x - pIJ.x);
				fEndAngle = atan2(pPos.y - pIJ.y,pPos.x - pIJ.x);

				if(nDir == -1)
				{
					if(fEndAngle>fStartAngle)
						fArcAngle = fEndAngle-fStartAngle;
					else
						fArcAngle = 2*PI-fStartAngle+fEndAngle;
				}
				else
				{
					if(fEndAngle>fStartAngle)
						fArcAngle = 2*PI-fEndAngle+fStartAngle;
					else
						fArcAngle = fStartAngle-fEndAngle;
				}
				
				if(fArcAngle == 0)
					fArcAngle = 2*PI;

				GetArcVertices(vecVertics,pIJ,fRadius,fStartAngle,fArcAngle,nDir);
// 				GetArcVertices(m_vecVertics,pIJ,fRadius,fStartAngle,fArcAngle,nDir);
				
				fPt = vecVertics.at(vecVertics.size() - 1);
// 				fPt = m_vecVertics.at(m_vecVertics.size() - 1);

				if(fPt.x != pPos.x ||  fPt.y != pPos.y)
					vecVertics.push_back(pPos);				
// 					m_vecVertics.push_back(pPos);				

            }
            else
			{
				pPos = m_pParent->m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
                pPos.x += dX;
                pPos.y += dY;
                
				vecVertics.push_back(pPos);
//				m_vecVertics.push_back(pPos);
           }
            pPre = pPos;
        }

		DrawConcavePolygon(vecVertics);
// 		DrawConcavePolygon(m_vecVertics);
	 } 
	 return FALSE;
}

COLORREF CDrawRs274X::GetSnRObjectColor(CDrawRs274X::GraphObj &obj)
{
	COLORREF crColor;
	m_nPrevAttribute = obj.Type.nAttr;
	m_nPrevPolarity = obj.Type.nPolarity;

	m_pParent->m_pLayerInfo->mapObjColor.Lookup((DRAW_TYPE::ATTRIBUTE)obj.Type.nAttr, crColor);
	if (m_pParent->m_pLayerInfo->bPolarity == LAYER_TYPE::POSITIVE)
	{
		if (obj.Type.nPolarity == DRAW_TYPE::POSITIVE)
			return GREY99;
		else
			return BLACK;    
	}
	else
	{        
		if (obj.Type.nPolarity == DRAW_TYPE::POSITIVE)	
			return BLACK;    
		else
			return GREY99;
	}
	
	return BLACK;
}

BOOL CDrawRs274X::CheckDrawAllow(const FRECTC &fObjExtent)
{
	const double fMargin = 0.5;
	BOOL nDrawShow=1;


	if(m_fExtentStartPos.x-fMargin>fObjExtent.X2 || m_fExtentEndPos.x+fMargin<fObjExtent.X1 ||  
		m_fExtentStartPos.y-fMargin>fObjExtent.Y2 || m_fExtentEndPos.y+fMargin<fObjExtent.Y1)
	{
		nDrawShow=0;
	}

	return  nDrawShow;
}

double CDrawRs274X::GetDistance(CfPoint FirstPoint, CfPoint SecondPoint)
{
	return sqrt(pow(SecondPoint.x-FirstPoint.x,2)+pow(SecondPoint.y-FirstPoint.y,2));
}

BOOL CDrawRs274X::MyArc(float fSx, float fSy, float fEx, float fEy, float fCX, float fCY, float fWidth, int nDir)
{
	float fRadius = GetDistance(CfPoint(fSx,fSy),CfPoint(fCX,fCY));

	if(fSx == fEx && fSy == fEy && (fSx != fCX || fSy != fCY))
	{
		float fDiameter = fRadius * 2.0;

		DrawDonut(fCX, fCY, fDiameter + fWidth, fDiameter - fWidth);
	}
	else
	{
		// Gcd: CW=2, CCW=3
		float fStartAngle = atan2(fSy-fCY,fSx-fCX);
		float fEndAngle = atan2(fEy-fCY,fEx-fCX);
		float fArcAngle;
		if(nDir == CCW)
		{
			if(fEndAngle>fStartAngle)
			{
				fArcAngle = fEndAngle-fStartAngle;
			}
			else
			{
				fArcAngle = 2*PI-fStartAngle+fEndAngle;
			}
		}
		else
		{
			if(fEndAngle>fStartAngle)
			{
				fArcAngle = 2*PI-fEndAngle+fStartAngle;
			}
			else
			{
				fArcAngle = fStartAngle-fEndAngle;
			}
		}

		DrawArcRound(fCX,fCY,fRadius,fStartAngle,fArcAngle,fWidth,nDir);

	}
	return FALSE;
}

void CDrawRs274X::DrawArcRound(float cx, float cy, float r, float start_angle, float arc_angle, float width, int dir, int fill) 
{
	float halfWidth = width/2.0;
	FPOINTC bufPt;
	vector <FPOINTC> vecVertics;

	vecVertics.clear();
// 	m_vecVertics.clear();
	if(dir == CW)
	{
		GetArcVertices(vecVertics,FPOINTC(cx,cy),r-halfWidth,start_angle,arc_angle,CW);
// 		GetArcVertices(m_vecVertics,FPOINTC(cx,cy),r-halfWidth,start_angle,arc_angle,CW);
		bufPt.x = cx+r*cos(start_angle-arc_angle);
		bufPt.y = cy+r*sin(start_angle-arc_angle);
		if(start_angle-arc_angle -PI < 0)
		{
			GetArcVertices(vecVertics,bufPt,halfWidth,start_angle-arc_angle+PI,PI,CCW);
// 			GetArcVertices(m_vecVertics,bufPt,halfWidth,start_angle-arc_angle+PI,PI,CCW);
		}
		else
		{
			GetArcVertices(vecVertics,bufPt,halfWidth,start_angle-arc_angle -PI,PI,CCW);
// 			GetArcVertices(m_vecVertics,bufPt,halfWidth,start_angle-arc_angle -PI,PI,CCW);
		}
		if(start_angle-arc_angle<0)
		{
			GetArcVertices(vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle-arc_angle+2.0*PI,arc_angle,CCW);
// 			GetArcVertices(m_vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle-arc_angle+2.0*PI,arc_angle,CCW);
		}
		else
		{
			GetArcVertices(vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle-arc_angle,arc_angle,CCW);
// 			GetArcVertices(m_vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle-arc_angle,arc_angle,CCW);
		}
		
		
		bufPt.x = cx+r*cos(start_angle);
		bufPt.y = cy+r*sin(start_angle);
		GetArcVertices(vecVertics,bufPt,halfWidth,start_angle,PI,CCW);
// 		GetArcVertices(m_vecVertics,bufPt,halfWidth,start_angle,PI,CCW);
	}
	else if(dir == CCW)
	{
		GetArcVertices(vecVertics,FPOINTC(cx,cy),r-halfWidth,start_angle,arc_angle,CCW);
// 		GetArcVertices(m_vecVertics,FPOINTC(cx,cy),r-halfWidth,start_angle,arc_angle,CCW);
		bufPt.x = cx+r*cos(start_angle+arc_angle);
		bufPt.y = cy+r*sin(start_angle+arc_angle);
		if(start_angle+arc_angle +PI> 2.0*PI)
		{
			GetArcVertices(vecVertics,bufPt,halfWidth,start_angle+arc_angle-PI,PI,CW);
// 			GetArcVertices(m_vecVertics,bufPt,halfWidth,start_angle+arc_angle-PI,PI,CW);
		}
		else
		{
			GetArcVertices(vecVertics,bufPt,halfWidth,start_angle+arc_angle+PI,PI,CW);
// 			GetArcVertices(m_vecVertics,bufPt,halfWidth,start_angle+arc_angle+PI,PI,CW);
		}
		if(start_angle+arc_angle>2.0*PI)
		{
			GetArcVertices(vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle+arc_angle-2.0*PI,arc_angle,CW);
// 			GetArcVertices(m_vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle+arc_angle-2.0*PI,arc_angle,CW);
		}
		else
		{
			GetArcVertices(vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle+arc_angle,arc_angle,CW);
// 			GetArcVertices(m_vecVertics,FPOINTC(cx,cy),r+halfWidth,start_angle+arc_angle,arc_angle,CW);
		}
		
		
		bufPt.x = cx+r*cos(start_angle);
		bufPt.y = cy+r*sin(start_angle);
		GetArcVertices(vecVertics,bufPt,halfWidth,start_angle,PI,CW);		
// 		GetArcVertices(m_vecVertics,bufPt,halfWidth,start_angle,PI,CW);		
	}


	if(fill)
	{
		DrawConcavePolygon(vecVertics);
// 		DrawConcavePolygon(m_vecVertics);
	}
	else
	{
		DrawPolygonLine(vecVertics);
// 		DrawPolygonLine(m_vecVertics);
	}

	return;
}

void CDrawRs274X::DrawConcavePolygon(const vector <FPOINTC> &fPoint)
{
//	m_csDrawing.Lock();
	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, 1, RGB(255, 0, 255));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushBlue(RGB(255, 0, 0));   
	CBrush* pOldBrush = cdc->SelectObject(&brushBlue);
	cdc->SetROP2(R2_XORPEN);

	int i,nNumOfVertex;
	nNumOfVertex = fPoint.size();
	if(nNumOfVertex<1)
		return;

	FPOINTC fPosWin, fPosCam;
	fPosCam = fPoint.at(0);
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

	CPoint *pts = new CPoint[nNumOfVertex+1];
	for( i=0; i<nNumOfVertex; i++)
	{
		fPosCam = fPoint.at(i);
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
			fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
	cdc->Polygon(pts, nNumOfVertex+1);

	cdc->SelectObject(pOldBrush);

// 	CBrush brushBlue2(RGB(0, 0, 0));   
// 	CBrush* pOldBrush2 = cdc->SelectObject(&brushBlue2);
// 
// 	fPosCam = fPoint.at(0);
// 	fPosWin = ConvertWindowCoord(fPosCam);
// 	cdc->MoveTo(fPosWin.x, fPosWin.y);
// 	for( i=0; i<nNumOfVertex; i++)
// 	{
// 		fPosCam = fPoint.at(i);
// 		fPosWin = ConvertWindowCoord(fPosCam);
// 		//cdc->LineTo(fPosWin.x, fPosWin.y);
// 		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
// 	}
// 	pts[i].x = pts[0].x; pts[i].y = pts[0].y;
// 	cdc->Polygon(pts, nNumOfVertex+1);

	delete[] pts;

//	cdc->SelectObject(pOldBrush2);
	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);

//	m_csDrawing.Unlock();

// 	glEnable(GL_STENCIL_TEST);
// 
// 
// 	glStencilFunc(GL_NEVER,0,1);
// 	glStencilOp(GL_INVERT,GL_INVERT,GL_INVERT);
// 	glBegin(GL_TRIANGLE_FAN);
// 	int i,nNumOfVertex;
// 	nNumOfVertex = fPoint.size();
// 	for( i=0; i<nNumOfVertex; i++)
// 	{
//		glVertex2f(fPoint.at(i).x,fPoint.at(i).y);
// 	}
//	glEnd();
	

// 	glStencilFunc(GL_EQUAL,1,1);
// 	glStencilOp(GL_ZERO,GL_ZERO,GL_ZERO);
// 
// 	glBegin(GL_TRIANGLE_FAN);
// 	for( i=0; i<nNumOfVertex; i++)
// 	{
// 		glVertex2f(fPoint.at(i).x,fPoint.at(i).y);
// 	}
// 	glEnd();
// 
// 	glDisable(GL_STENCIL_TEST);
}

void CDrawRs274X::MyCallApertureList(float fx, float fy,int nListNum)
{
	CallList(nListNum);
// 	glPushMatrix();
// 	glTranslatef(fx,fy,0);
// 	glCallList(nListNum);
// 	if(!glIsList(nListNum))
// 		AfxMessageBox(_T("list error"));
// 	glPopMatrix();
}


BOOL CDrawRs274X::MyMoire(float x, float y, float fOutDiameter, float fRingWidth, float fGap, int nCircle, float fLineWidth, float fLineLegth, float Ang)
{
    float X1, X2, Y1, Y2, cV, sV, XX;
	float fInDia;

    for (int i=0 ; i<nCircle ; i++)
	{
		fInDia =  (fOutDiameter - 2.0 * fRingWidth);
		DrawDonut(x, y, fOutDiameter, fInDia);
		fOutDiameter = fOutDiameter - (fRingWidth + fGap) * 2;		
    }
    
    X1 = 0;
    Y1 = -0.5 * fLineLegth;
    X2 = 0;
    Y2 = 0.5 * fLineLegth;
    if (Ang != 0)
	{
        cV = cos(Ang * DEG2RAD);
        sV = sin(Ang * DEG2RAD);
        XX = X1 * cV - Y1 * sV;
        Y1 = X1 * sV + Y1 * cV;
        X1 = XX;
        XX = X2 * cV - Y2 * sV;
        Y2 = X2 * sV + Y2 * cV;
        X2 = XX;
    }
	DrawLineRound (X1+x, Y1+y, X2+x,Y2+y, fLineWidth);
  
    X1 = -0.5 * fLineLegth;
    Y1 = 0;
    X2 = 0.5 * fLineLegth;
    Y2 = 0;
    if (Ang != 0)
	{
        cV = cos(Ang * DEG2RAD);
        sV = sin(Ang * DEG2RAD);
        XX = X1 * cV - Y1 * sV;
        Y1 = X1 * sV + Y1 * cV;
        X1 = XX;
        XX = X2 * cV - Y2 * sV;
        Y2 = X2 * sV + Y2 * cV;
        X2 = XX;
    }

	DrawLineRound (X1+x, Y1+y, X2+x, Y2+y, fLineWidth);
	return FALSE;
}

void CDrawRs274X::GetCannedText(CString strCannedText, FPOINTC fptStart, float fDistance, vector <FPOINTC> &vecCoord)
{
	vector<FPOINTC> vecBuf;
	int i, j, nLen;

#ifdef UNICODE
	char    pText[256] = {0,}; 
	nLen = WideCharToMultiByte( CP_ACP, 0, strCannedText, -1, NULL, 0, NULL, NULL );   
	WideCharToMultiByte( CP_ACP, 0, strCannedText, -1, pText, nLen, NULL, NULL ); 	
#else
	char *pText = strCannedText.GetBuffer(0);
	nLen = strCannedText.GetLength();
#endif
	for(i=0;i<nLen;i++)
	{
		if(GetDrillPoints(pText[i], fptStart, vecBuf, fDistance))
		{
			for(j=0;j<vecBuf.size();j++)
			{
				vecCoord.push_back(vecBuf[j]);
			}
			fptStart.x+=5.0*fDistance;
		}
	}
}

void CDrawRs274X::DrawLineRound(float fSx, float fSy, float fEx, float fEy, float fLineWidth,BOOL bFill)
{
	vector <FPOINTC> vecVertics;
	FPOINTC bufPt;

	vecVertics.clear();
// 	m_vecVertics.clear();

	bufPt.x = fSx;
	bufPt.y = fSy;
	float fLineAngle = atan2(fSy - fEy,fSx - fEx);
	float fBufAngle;
	float fRadius = fLineWidth/2.0;
	fBufAngle = fLineAngle -PI/2.0;
	if(fBufAngle<0)
		fBufAngle += 2.0*PI;

	GetArcVertices(vecVertics,bufPt,fRadius,fBufAngle,PI,CCW);
// 	GetArcVertices(m_vecVertics,bufPt,fRadius,fBufAngle,PI,CCW);
	bufPt.x = fEx;
	bufPt.y = fEy;
	fBufAngle += PI;
	if(fBufAngle>2.0*PI)
		fBufAngle -= 2.0*PI;

 	GetArcVertices(vecVertics,bufPt,fRadius,fBufAngle,PI,CCW);
//  GetArcVertices(m_vecVertics,bufPt,fRadius,fBufAngle,PI,CCW);
	if(bFill)
	{
		DrawConvexPolygon(vecVertics);
// 		DrawConvexPolygon(m_vecVertics);
	}
	else
	{
		DrawPolygonLine(vecVertics);
// 		DrawPolygonLine(m_vecVertics);
	}
}

int	CDrawRs274X::CallApertureList() 
{
	int nKey;
	LPAPERTURE_DEF pAptDef;
	DrawListAPerture listAP; 
	POSITION pos = NULL;
	int nInc=0;
	vector <FPOINTC> vecVertics;
	float *fParam;

	LPLAYER_INFORM_RS274X pLayerInfo;

	int nNumOfLayer = GetNumOfLayer();
	
	for(int k = 0 ; k < nNumOfLayer ; k++)
	{
		pLayerInfo = m_ArLayerInform.GetAt(k);
		pos = pLayerInfo->mapAptList.GetStartPosition();

		while(pos != NULL)
		{
			pLayerInfo->mapAptList.GetNextAssoc(pos, nKey, pAptDef);
// 			glNewList(m_nListAperture+nInc,GL_COMPILE);
			int pNum = pAptDef->Param.at(0).fParam.size();

			fParam = &pAptDef->Param.at(0).fParam[0];

			vecVertics.clear();
// 			m_vecVertics.clear();
// 
			if(pAptDef->chStdAptType == _T('C'))	//    ' Circle
			{
				if (pNum == 1)
				{
					DrawCircle(0, 0, fParam[0]*0.5);
				}
				else if (pNum == 2)
				{
					DrawDonut(0, 0, fParam[0], fParam[1]);
				}
				else if(pNum > 2 && pNum < 5)
				{
// 					glClear(GL_STENCIL_BUFFER_BIT);
// 					glEnable(GL_STENCIL_TEST);
// 					SetStencil(FALSE);	

					if (pNum == 3)
					{						
						MyRectCenter(0, 0, fParam[1], fParam[2], 0);
					}
					else if (pNum == 4)
					{
						MyRectCenter( 0, 0, fParam[1], fParam[2], fParam[3]);
					}
// 					SetStencil(TRUE);
					DrawCircle(0, 0, fParam[0]*0.5);
// 					glDisable(GL_STENCIL_TEST);
				}
			}
			else if(pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
			{
				if(pNum < 3)
				{
					MyRectCenter(0, 0, fParam[0], fParam[1], 0);
				}
				else if(pNum < 5)
				{
// 					glClear(GL_STENCIL_BUFFER_BIT);
// 					glEnable(GL_STENCIL_TEST);
// 					SetStencil(FALSE);				

					if (pNum == 3)
					{
						DrawCircle(0, 0, fParam[2]*0.5);
					}
					else if (pNum == 4)
					{
						MyRectCenter(0, 0, fParam[2], fParam[3], 0);
					}
// 					SetStencil(TRUE);
					MyRectCenter(0, 0, fParam[0], fParam[1], 0);
// 					glDisable(GL_STENCIL_TEST);
				}
			}
			else if(pAptDef->chStdAptType == _T('O'))	//    ' Oval
			{
				if(pNum < 3)
				{
					MyOval(0, 0, fParam[0], fParam[1]);
				}
				else if(pNum < 5)
				{
// 					glClear(GL_STENCIL_BUFFER_BIT);
// 					glEnable(GL_STENCIL_TEST);

// 					SetStencil(FALSE);

					if (pNum == 3)
					{
						DrawCircle(0, 0, fParam[2]*0.5);
					}
					else if (pNum == 4)
					{
						MyRectCenter(0, 0, fParam[2], fParam[3], 0);
					}

// 					SetStencil(TRUE);
					MyOval(0, 0, fParam[0], fParam[1]);
// 					glDisable(GL_STENCIL_TEST);
				}
			}
			else if(pAptDef->chStdAptType == _T('P'))	//    ' Polygon
			{
				int nSide = fParam[1];
				if(pNum < 4)
				{
					MyPolygonNSide(nSide, 0, 0, fParam[0], fParam[2]);
				}
				else
				{
// 					glClear(GL_STENCIL_BUFFER_BIT);
// 					glEnable(GL_STENCIL_TEST);
// 					SetStencil(FALSE);
				
					if (pNum == 4)
					{
						DrawCircle(0, 0, fParam[3] * 0.5);
					}
					else if (pNum >= 5)
					{
						MyRectCenter( 0, 0, fParam[3], fParam[4], 0);
					}
// 					SetStencil(TRUE);
					MyPolygonNSide(nSide, 0, 0, fParam[0], fParam[2]);
// 					glDisable(GL_STENCIL_TEST);
				}
			}
			else //    ' Userdefined Apperture
			{
				MyMacro(pAptDef,0,0,0);
			}	

// 			glEndList();
			listAP.nDcode.push_back(nKey);
			listAP.nListNum.push_back(m_nListAperture+nInc);
			
			pAptDef->nGlListIndex = m_nListAperture+nInc;
			pLayerInfo->mapAptList.SetAt(nKey, pAptDef);

			nInc++;
		}

		listAP.nDcode.clear();
		vector <int>(listAP.nDcode.begin(),listAP.nDcode.end()).swap(listAP.nDcode);
		listAP.nListNum.clear();
		vector <int>(listAP.nListNum.begin(),listAP.nListNum.end()).swap(listAP.nListNum);
	}

	return nInc;
}


int CDrawRs274X::CallLineTemplateList()
{
	TEMPLATE_KEY Key;
	int i,nTemplate,nNumOfLineTemplate;
	float fLength;
	float fThickness;

	nNumOfLineTemplate = 0;
	nTemplate = 0;

	int nNumOfLayer = GetNumOfLayer();

	nNumOfLineTemplate = m_pParent->m_vecLineTemplate.size();
	for(i = 0 ; i < nNumOfLineTemplate ; i++) 
	{
		Key.key = m_pParent->m_vecLineTemplate.at(i);

		fLength = sqrt(Key.Param[0]); // length
		fThickness = Key.Param[1]; // line width

// 		glNewList(m_nListLines+nTemplate,GL_COMPILE);
		MyMakeLineList(fLength,fThickness);
// 		glEndList();
		nTemplate++;
	}
	return nTemplate;
}



int CDrawRs274X::CallSnRObjectList() 
{
	int i, k, n, nLayer, nInc = 0, nMCode;
	int nObjIndex, nNumOfObj, nGLError;
	int nStObj, nEdObj, nXRepeat, nYRepeat, nCoord;
	float dX, dY, ddX, ddY;
	FPOINTC fptOffset, fptCenter;
	FRECTC frtCurrentExtent;
	vector <FPOINTC> vecObjCoord;
	GraphObj gObj, obj;	

	int nNumOfLayer = GetNumOfLayer();
	
	for(nLayer = 0 ; nLayer < nNumOfLayer ; nLayer++)
	{ 
		m_nLayerID = nLayer;
		m_pParent->m_pLayerInfo = m_ArLayerInform.GetAt(nLayer);
		 
		if (m_pParent->m_pLayerInfo->bView)
		{			
			nNumOfObj = m_pParent->m_pLayerInfo->listObj.size()-1;

			m_nPrevAttribute = 16;
			m_nPrevPolarity = 3;
			BOOL bMakeNewGL = TRUE;
			for (nObjIndex = 0 ; nObjIndex < nNumOfObj ; nObjIndex++) 
			{
				gObj = m_pParent->m_pLayerInfo->listObj.at(nObjIndex);
				if(gObj.Type.nType == DRAW_TYPE::SNR )
				{
					frtCurrentExtent = m_pParent->m_FrameExtent;//GetFrameExtent();

					nStObj = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
					nEdObj = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					
					nXRepeat = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x;
					nYRepeat = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
				
					dX = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x;
					dY = m_pParent->m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y;

// 					glNewList(m_nListSnR+nInc, GL_COMPILE);
// 					nGLError = glGetError();
// 					switch(nGLError)
// 					{
// 					case GL_INVALID_VALUE:
// 						AfxMessageBox(_T("OpenGL new list error:GL_INVALID_VALUE"));
// 						bMakeNewGL = FALSE;
// 						break;
// 					case GL_INVALID_ENUM:
// 						AfxMessageBox(_T("OpenGL new list error:GL_INVALID_ENUM"));
// 						bMakeNewGL = FALSE;
// 						break;
// 					case GL_INVALID_OPERATION:
// 						AfxMessageBox(_T("OpenGL new list error:GL_INVALID_OPERATION"));
// 						bMakeNewGL = FALSE;
// 						break;
// 					case GL_OUT_OF_MEMORY:
// 						AfxMessageBox(_T("OpenGL new list error:GL_OUT_OF_MEMORY"));
// 						bMakeNewGL = FALSE;
// 						break;
// 					}
					
					if(!bMakeNewGL)
						break;

					for(k= nStObj; k<=nEdObj; k++)
						MyObjEntityShift (k, 0, 0);

// 					glEndList();//jun
					m_pParent->m_pLayerInfo->listObj.at(nObjIndex).dCode = m_nListSnR+nInc;
					nInc++;
				}
			}
			if(!bMakeNewGL)
				break;
		}
	}
	return nInc;
}


void CDrawRs274X::MyMakeLineList(float fLength, float w)
{
	HDC  hDC = m_hDC;
// 	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* op = cdc->SelectObject(&p);


	float fRadius = w/2.0;
 	float fArcLenth = PI * fRadius;
	CfPoint fbuf;
	CfPoint fStart,fEnd;
	int i;
	float tx; 
	float ty;

	fStart=CfPoint(-fLength/2,0);
	fEnd=CfPoint(fLength/2,0);

	int num_segments = GetArcSegment(fRadius,180.0f);

	float theta =PI / float(num_segments); //theta is now calculated from the arc angle instead, the - 1 bit comes from the fact that the arc is open
	float tangetial_factor = tanf(theta);
	float radial_factor = cosf(theta);
	float fRight_x = 0.0;
	float fRight_y = -fRadius;
	
// 	glBegin(GL_POLYGON); // filled circle
	CPoint *pts = new CPoint[num_segments+1];


	FPOINTC fPosWin, fPosCam;
	fPosCam.x = fRight_x + fEnd.x;
	fPosCam.y = fRight_y + fEnd.y;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
	pts[0].x = fPosWin.x; pts[0].y = fPosWin.y;
	for(i = 1; i < num_segments; i++)
	{ 
// 		glVertex2f(fRight_x + fEnd.x, fRight_y + fEnd.y);

		tx = -fRight_y; 
		ty = fRight_x; 
		fRight_x += tx * tangetial_factor; 
		fRight_y += ty * tangetial_factor; 
		fRight_x *= radial_factor; 
		fRight_y *= radial_factor; 

		fPosCam.x = fRight_x;
		fPosCam.y = fRight_y;
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	} 

	fRight_x = 0.0;
	fRight_y = fRadius;
// 	glVertex2f(fRight_x+fEnd.x, fRight_y + fEnd.y);
	fPosCam.x = fRight_x+fEnd.x;
	fPosCam.y = fRight_y + fEnd.y;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
		fPosWin = ConvertWindowCoord(fPosCam);
	pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;

	cdc->Polygon(pts, num_segments+1);

	float fLeft_x = 0.0;
	float fLeft_y = fRadius;

	fPosCam.x = fLeft_x + fStart.x;
	fPosCam.y = fLeft_y + fStart.y;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
	pts[0].x = fPosWin.x; pts[0].y = fPosWin.y;
	for(i = 1; i < num_segments; i++)
	{ 
// 		glVertex2f(fLeft_x + fStart.x, fLeft_y + fStart.y);

		tx = -fLeft_y; 
		ty = fLeft_x; 
		fLeft_x += tx * tangetial_factor; 
		fLeft_y += ty * tangetial_factor; 
		fLeft_x *= radial_factor; 
		fLeft_y *= radial_factor; 

		fPosCam.x = fLeft_x;
		fPosCam.y = fLeft_y;
		if(m_dPixelResolution)
			fPosWin = ConvertResolutionCoord(fPosCam);
		else
 			fPosWin = ConvertWindowCoord(fPosCam);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	} 

	fLeft_x = 0.0;
	fLeft_y = -fRadius;
// 	glVertex2f(fLeft_x+fStart.x, fLeft_y + fStart.y);
// 	glEnd(); 
	fPosCam.x = fLeft_x+fStart.x;
	fPosCam.y = fLeft_y + fStart.y;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;

	cdc->Polygon(pts, num_segments+1);

	delete[] pts;

	cdc->SelectObject(op);
}

COLORREF CDrawRs274X::GetObjectColor(GraphObj &obj)
{
	COLORREF crColor;

	m_nPrevAttribute = obj.Type.nAttr;
	m_nPrevPolarity = obj.Type.nPolarity;

	m_pParent->m_pLayerInfo->mapObjColor.Lookup((DRAW_TYPE::ATTRIBUTE)obj.Type.nAttr, crColor);
	if (m_pParent->m_pLayerInfo->bPolarity == LAYER_TYPE::POSITIVE)
	{
		if (obj.Type.nPolarity == DRAW_TYPE::POSITIVE)
			return GREY99;
		else
			return BLACK;    
	}
	else
	{        
		if (obj.Type.nPolarity == DRAW_TYPE::POSITIVE)	
			return BLACK;    
		else
			return GREY99;
	}
	
	return BLACK;
}

void CDrawRs274X::SetForeColor(COLORREF crColor)
{
// 	float R,G,B;
// 
// 	R=(GetRValue(crColor) / 255.0f);
// 	G=(GetGValue(crColor) / 255.0f);
// 	B=(GetBValue(crColor) / 255.0f);
// 	float currentColor[4];
// 	glGetFloatv(GL_CURRENT_COLOR,currentColor);
// 	if(currentColor[0]!= R || currentColor[1]!= G || currentColor[2]!= B )
// 		glColor3f(R,G,B);
}

int CDrawRs274X::SearchLineTemplate(UINT64 nVal)
{
	vector <UINT64>::iterator iter;
	iter = std::lower_bound(m_pParent->m_vecLineTemplate.begin(), m_pParent->m_vecLineTemplate.end(),nVal);
	
	if(iter != m_pParent->m_vecLineTemplate.end() && *iter == nVal)
		return(iter - m_pParent->m_vecLineTemplate.begin());

	return -1;
}

void CDrawRs274X::MyLineList(float X1, float Y1, float X2, float Y2,int nListNum)
{
//	m_csDrawing.Lock();
	HDC  hDC = m_hDC;
//	HDC  hDC = ::GetDC(m_hParent);
	CDC* cdc = CDC::FromHandle(hDC);
	CPen p(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* op = cdc->SelectObject(&p);

	int i;
	FPOINTC fPosWin, fPosCam;
	fPosCam.x = X1; fPosCam.y = Y1;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
	fPosCam.x = X2; fPosCam.y = Y2;
	if(m_dPixelResolution)
		fPosWin = ConvertResolutionCoord(fPosCam);
	else
 		fPosWin = ConvertWindowCoord(fPosCam);
	cdc->LineTo(fPosWin.x, fPosWin.y);

	cdc->SelectObject(op);
//	::ReleaseDC(m_hParent, hDC);

//	m_csDrawing.Unlock();


/*	float Xc,Yc;
	
	Xc=((X2+X1)/2);	
	Yc=((Y1+Y2)/2);

	float fAngleRad = atan2(Y1-Y2,X1-X2);
	float fRotate = RAD2DEG*fAngleRad;
*/
// 	glPushMatrix();
// 	glTranslatef(Xc,Yc,0);
// 	glRotatef(fRotate,0.0f, 0.0f, 1.0f);
// 	
// 	glCallList(nListNum);
// 	glPopMatrix();

	m_nListLines++;
}


void CDrawRs274X::SetPixelResolution(double dPixelResolution)
{
	m_dPixelResolution = dPixelResolution;
}
