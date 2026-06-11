// FileFormatRs274X.cpp : implementation file
//

#include "stdafx.h"
#include "MyDxTest.h"
#include "ColorTable.h"
#include "FileFormatRs274X.h"
#include "DrawRs274X.h"
#include <iterator>

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
// CFileFormatRs274X

CFileFormatRs274X::CFileFormatRs274X(CWnd* pParent)
{
	m_pParent = pParent;
	m_hParent = pParent->GetSafeHwnd();

	int i;
	for(i=0; i<4; i++)
	{
		m_bAliveThreadDrawing[i] = FALSE;
		m_pThreadDrawing[i] = NULL;
	}

	m_pTrUtil= new CTransformUtil;
	InitVal();

	m_dDetZero = 0.000001;
	m_nMaxLineTemplate = 1000000;
	m_vecLineTemplate.reserve(m_nMaxLineTemplate);

	m_fCanvasPixelResolution = 1.0;
	m_fDrawResolution = m_fCanvasPixelResolution;
	m_nIRAngle = 0; // Image Rotation angle

	m_bEnableDraw = FALSE;
}

CFileFormatRs274X::~CFileFormatRs274X()
{
	int i;
	m_bEnableDraw = FALSE;

	for(i=0; i<4; i++)
	{
		if(m_pThreadDrawing[i])
		{
			if(m_bAliveThreadDrawing[i])
			{
				m_pThreadDrawing[i]->Stop();
				while(m_bAliveThreadDrawing[i])
					Sleep(100);
			}
			delete m_pThreadDrawing[i];
			m_pThreadDrawing[i] = NULL;
		}
	}
	
	FreeMemory();

	if(m_pTrUtil)
	{
		delete m_pTrUtil;
	}
}


BEGIN_MESSAGE_MAP(CFileFormatRs274X, CWnd)
	//{{AFX_MSG_MAP(CFileFormatRs274X)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileFormatRs274X message handlers
UINT CFileFormatRs274X::ThreadDrawing_0( LPVOID lpContext )
{
	CFileFormatRs274X* pThread = reinterpret_cast< CFileFormatRs274X* >( lpContext );

// 	DWORD dwShutdownEventCheckPeriod = 10; // thread shutdown event check period
// 	while( WAIT_OBJECT_0 != WaitForSingleObject( pThread->m_pThreadDrawing[0]->GetShutdownEvent(), dwShutdownEventCheckPeriod) )
// 	{
// 	}
	//pThread->m_bAliveThreadDrawing[0] = TRUE;
	pThread->Drawing_0();
	pThread->m_bAliveThreadDrawing[0] = FALSE;
	TRACE("End : ThreadDrawing_0\n");
	return 0;
}

UINT CFileFormatRs274X::ThreadDrawing_1( LPVOID lpContext )
{
	CFileFormatRs274X* pThread = reinterpret_cast< CFileFormatRs274X* >( lpContext );

// 	DWORD dwShutdownEventCheckPeriod = 10; // thread shutdown event check period
// 	while( WAIT_OBJECT_0 != WaitForSingleObject( pThread->m_pThreadDrawing[0]->GetShutdownEvent(), dwShutdownEventCheckPeriod) )
// 	{
// 	}
	//pThread->m_bAliveThreadDrawing[1] = TRUE;
	pThread->Drawing_1();
	pThread->m_bAliveThreadDrawing[1] = FALSE;
	TRACE("End : ThreadDrawing_1\n");
	return 0;
}

UINT CFileFormatRs274X::ThreadDrawing_2( LPVOID lpContext )
{
	CFileFormatRs274X* pThread = reinterpret_cast< CFileFormatRs274X* >( lpContext );

// 	DWORD dwShutdownEventCheckPeriod = 10; // thread shutdown event check period
// 	while( WAIT_OBJECT_0 != WaitForSingleObject( pThread->m_pThreadDrawing[0]->GetShutdownEvent(), dwShutdownEventCheckPeriod) )
// 	{
// 	}

	//pThread->m_bAliveThreadDrawing[2] = TRUE;
	pThread->Drawing_2();
	pThread->m_bAliveThreadDrawing[2] = FALSE;
	TRACE("End : ThreadDrawing_2\n");
	return 0;
}

UINT CFileFormatRs274X::ThreadDrawing_3( LPVOID lpContext )
{
	CFileFormatRs274X* pThread = reinterpret_cast< CFileFormatRs274X* >( lpContext );

// 	DWORD dwShutdownEventCheckPeriod = 10; // thread shutdown event check period
// 	while( WAIT_OBJECT_0 != WaitForSingleObject( pThread->m_pThreadDrawing[0]->GetShutdownEvent(), dwShutdownEventCheckPeriod) )
// 	{
// 	}

	//pThread->m_bAliveThreadDrawing[3] = TRUE;
	pThread->Drawing_3();
	pThread->m_bAliveThreadDrawing[3] = FALSE;
	TRACE("End : ThreadDrawing_3\n");
	return 0;
}

void CFileFormatRs274X::Drawing_0()
{
	int nObjIndex;
	int nNumOfObj, nSt, nDevide;
	GraphObj gObj;
	nNumOfObj = m_pLayerInfo->listObj.size();
 	//nNumOfObj = m_pLayerInfo->listObj.size()-1;
	nDevide = int(double(nNumOfObj) / 4.0);
	nSt = nDevide * 0;
//	nNumOfObj = nSt+nDevide;
//	nNumOfObj = n NumOfObj / 2;

// 	m_nPrevAttribute = 16;
// 	m_nPrevPolarity = 3;
	CDrawRs274X m_DrawRs274X(this);
	m_DrawRs274X.SetPixelResolution(m_dPixelResolution);

	for (nObjIndex = nSt ; nObjIndex < nNumOfObj ; nObjIndex++) 
	{
		DispProgress(nObjIndex, nNumOfObj);
		
		gObj = m_pLayerInfo->listObj.at(nObjIndex);

		if(m_bCheckDraw && gObj.Type.nType != DRAW_TYPE::SNR )
		{
			if(gObj.Type.nAttr != DRAW_TYPE::NONE)
			{
				if(CheckDrawAllow(nObjIndex))
				{
					if(gObj.dCode > 0)
					{
						m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
					}
					m_DrawRs274X.DrawObject(nObjIndex);
					//DrawObject_0(nObjIndex);//MyObjEntity(nObjIndex,bCheck);
				}
			}
		}
		else
		{
			if(gObj.dCode > 0)
			{
				m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
			}
			m_DrawRs274X.DrawObject(nObjIndex);
			//DrawObject_0(nObjIndex);//MyObjEntity(nObjIndex, bCheck);
		}
	}

//	m_DrawRs274X.CallApertureList();
//	m_DrawRs274X.CallLineTemplateList();
//	m_DrawRs274X.CallSnRObjectList();

}

void CFileFormatRs274X::Drawing_1()
{
	int nObjIndex;
	int nNumOfObj, nSt, nDevide;
	GraphObj gObj;
	nNumOfObj = m_pLayerInfo->listObj.size();
	nDevide = int(double(nNumOfObj) / 4.0);
 	//nNumOfObj = m_pLayerInfo->listObj.size()-1;
	nSt = nDevide * 1;
// 	nNumOfObj = nNumOfObj;
	nNumOfObj = nSt+nDevide;

// 	m_nPrevAttribute = 16;
// 	m_nPrevPolarity = 3;

	CDrawRs274X m_DrawRs274X(this);

	for (nObjIndex = nSt ; nObjIndex < nNumOfObj ; nObjIndex++) 
	{
//		DispProgress(nObjIndex, nNumOfObj);
		
		gObj = m_pLayerInfo->listObj.at(nObjIndex);

		if(m_bCheckDraw && gObj.Type.nType != DRAW_TYPE::SNR )
		{
			if(gObj.Type.nAttr != DRAW_TYPE::NONE)
			{
				if(CheckDrawAllow(nObjIndex))
				{
					if(gObj.dCode > 0)
					{
						m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
					}
					m_DrawRs274X.DrawObject(nObjIndex);
					//DrawObject_1(nObjIndex);//MyObjEntity(nObjIndex,bCheck);
				}
			}
		}
		else
		{
			if(gObj.dCode > 0)
			{
				m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
			}
			m_DrawRs274X.DrawObject(nObjIndex);
			//DrawObject_1(nObjIndex);//MyObjEntity(nObjIndex, bCheck);
		}
	}

}

void CFileFormatRs274X::Drawing_2()
{
	int nObjIndex;
	int nNumOfObj, nSt, nDevide;
	GraphObj gObj;
	nNumOfObj = m_pLayerInfo->listObj.size();
 	//nNumOfObj = m_pLayerInfo->listObj.size()-1;
	nDevide = int(double(nNumOfObj) / 4.0);
	nSt = nDevide * 2;
	nNumOfObj = nSt+nDevide;

// 	m_nPrevAttribute = 16;
// 	m_nPrevPolarity = 3;

	CDrawRs274X m_DrawRs274X(this);

	for (nObjIndex = nSt ; nObjIndex < nNumOfObj ; nObjIndex++) 
	{
//		DispProgress(nObjIndex, nNumOfObj);
		
		gObj = m_pLayerInfo->listObj.at(nObjIndex);

		if(m_bCheckDraw && gObj.Type.nType != DRAW_TYPE::SNR )
		{
			if(gObj.Type.nAttr != DRAW_TYPE::NONE)
			{
				if(CheckDrawAllow(nObjIndex))
				{
					if(gObj.dCode > 0)
					{
						m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
					}
					m_DrawRs274X.DrawObject(nObjIndex);
					//DrawObject_2(nObjIndex);//MyObjEntity(nObjIndex,bCheck);
				}
			}
		}
		else
		{
			if(gObj.dCode > 0)
			{
				m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
			}
			m_DrawRs274X.DrawObject(nObjIndex);
			//DrawObject_2(nObjIndex);//MyObjEntity(nObjIndex, bCheck);
		}
	}

}

void CFileFormatRs274X::Drawing_3()
{
	int nObjIndex;
	int nNumOfObj, nSt, nDevide;
	GraphObj gObj;
	nNumOfObj = m_pLayerInfo->listObj.size();
 	//nNumOfObj = m_pLayerInfo->listObj.size()-1;
	nDevide = int(double(nNumOfObj) / 4.0);
	nSt = nDevide * 3;
	//nNumOfObj = nSt+nDevide;

// 	m_nPrevAttribute = 16;
// 	m_nPrevPolarity = 3;

	CDrawRs274X m_DrawRs274X(this);

	for (nObjIndex = nSt ; nObjIndex < nNumOfObj ; nObjIndex++) 
	{
//		DispProgress(nObjIndex, nNumOfObj);
		
		gObj = m_pLayerInfo->listObj.at(nObjIndex);

		if(m_bCheckDraw && gObj.Type.nType != DRAW_TYPE::SNR )
		{
			if(gObj.Type.nAttr != DRAW_TYPE::NONE)
			{
				if(CheckDrawAllow(nObjIndex))
				{
					if(gObj.dCode > 0)
					{
						m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
					}
					m_DrawRs274X.DrawObject(nObjIndex);
					//DrawObject_3(nObjIndex);//MyObjEntity(nObjIndex,bCheck);
				}
			}
		}
		else
		{
			if(gObj.dCode > 0)
			{
				m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
			}
			m_DrawRs274X.DrawObject(nObjIndex);
			//DrawObject_3(nObjIndex);//MyObjEntity(nObjIndex, bCheck);
		}
	}

}

void CFileFormatRs274X::InitVal()
{
	float fMaxData = pow(2.0,31.0);
	m_MaxFrameExtent.X1 = fMaxData;
    m_MaxFrameExtent.X2 = -fMaxData;
    m_MaxFrameExtent.Y1 = fMaxData;
    m_MaxFrameExtent.Y2 = -fMaxData;

	m_nNumOfSnRObject = 0;
	m_nNumOfLineObject = 0;
	m_nNumOfLineTemplate = 0;

	// <Drawing>
	m_crObjColor = GREEN;
	m_bDisplayBackgroundImg = TRUE;
	m_bCheckDraw = FALSE;
	m_fExtentStartPos = CfPoint3D(0, 0, 0);
	m_fExtentEndPos = CfPoint3D(0, 0, 0);
}

void CFileFormatRs274X::FreeMemory()
{
	LPLAYER_INFORM_RS274X pLayerInfo;
	LPAPERTURE_DEF pAptDef;
	int i, k, nKey;

	POSITION pos = NULL;

	int nLayerSize = m_ArLayerInform.GetSize();
	for(i = 0; i < nLayerSize; i++)
	{
		pLayerInfo = m_ArLayerInform.GetAt(i);
		if(pLayerInfo != NULL)
		{
			pLayerInfo->vecObjCoord.clear();
			vector <FPOINTC>(pLayerInfo->vecObjCoord.begin(), pLayerInfo->vecObjCoord.end()).swap(pLayerInfo->vecObjCoord);
			pLayerInfo->listObj.clear();
			vector <GraphObj>(pLayerInfo->listObj.begin(), pLayerInfo->listObj.end()).swap(pLayerInfo->listObj);
		
			int nNumOfAperture = pLayerInfo->mapAptList.GetCount();
			if(nNumOfAperture>0)
			{
				pos = pLayerInfo->mapAptList.GetStartPosition();
				while(pos != NULL)
				{
					pLayerInfo->mapAptList.GetNextAssoc(pos, nKey, pAptDef);
					if(pAptDef != NULL)
					{
						int nNumOfParam = pAptDef->Param.size();
						for(k = 0 ; k < nNumOfParam; k++)
						{
							pAptDef->Param.at(k).fParam.clear();
							vector <float>(pAptDef->Param.at(k).fParam.begin(), pAptDef->Param.at(k).fParam.end()).swap(pAptDef->Param.at(k).fParam);
						}
						pAptDef->Param.clear();
						vector <APERTURE_PRIMITIVE>(pAptDef->Param.begin(), pAptDef->Param.end()).swap(pAptDef->Param);

						delete pAptDef;
						pAptDef = NULL;
					}
				}
			}
			
			pLayerInfo->mapAptList.RemoveAll();
			pLayerInfo->mapObjColor.RemoveAll();
			delete pLayerInfo;
		}
	}
	m_ArLayerInform.RemoveAll();
	m_nListLines = 0;
}

void CFileFormatRs274X::ResetData()
{
	int i, k, nKey;

	//Initialize Storage Variable
	LPLAYER_INFORM_RS274X pLayerInfo;
	LPAPERTURE_DEF pAptDef;

	CString strMsg;	
	POSITION pos = NULL;

	int nLayerSize = m_ArLayerInform.GetSize();
	for(i = 0; i < nLayerSize; i++)
	{
		pLayerInfo = m_ArLayerInform.GetAt(i);
		if(pLayerInfo != NULL)
		{
			pLayerInfo->vecObjCoord.clear();
			pLayerInfo->listObj.clear();
		
			int nNumOfAperture = pLayerInfo->mapAptList.GetCount();
			if(nNumOfAperture>0)
			{
				pos = pLayerInfo->mapAptList.GetStartPosition();
				while(pos != NULL)
				{
					pLayerInfo->mapAptList.GetNextAssoc(pos, nKey, pAptDef);
					if(pAptDef != NULL)
					{
						int nNumOfParam = pAptDef->Param.size();
						for(k = 0 ; k < nNumOfParam; k++)
							pAptDef->Param.at(k).fParam.clear();
						pAptDef->Param.clear();
						delete pAptDef;
						pAptDef = NULL;
					}
				}
			}
			
			pLayerInfo->mapAptList.RemoveAll();
		
			pLayerInfo->mapObjColor.RemoveAll();
		
			delete pLayerInfo;
		}
	}

	m_ArLayerInform.RemoveAll();
	m_ArLayerStruct.RemoveAll();

	m_vecLineTemplate.clear();
// 	m_vecObjExtent.clear();
// 	m_vecObjCoord.clear();

	//Initialize member variable

// 	m_nMaxValueOfAptCode = 0;
// 	m_dwReleaseTime = m_dwPreParsingTime = m_dwParsingTime = 0;
// 	m_nDrawOrder = 0; 
	m_nNumOfAperture = 0;
	m_nNumOfSnRObject = 0;

	m_nNumOfObject = 0;
	m_nNumOfLineObject = 0;
	m_nNumOfLineTemplate = 0;

	// Maximun number of graphic object
// 	m_nNumOfMaxObj = 25000000;

	// must be initialize
	m_fLayerScaleX = 1.0;
	m_fLayerScaleY = 1.0;

	float fMaxData = pow(2.0,31.0);
	m_MaxFrameExtent.X1 = fMaxData;
    m_MaxFrameExtent.X2 = -fMaxData;
    m_MaxFrameExtent.Y1 = fMaxData;
    m_MaxFrameExtent.Y2 = -fMaxData;

	m_dDetZero = 0.000001;
// 	m_fFiducialDrillSize = 1.5;
// 	m_nDrillSuppress = 0;
// 	m_nFormatDrill = 4;
// 	m_nDrillUnit = LAYER_TYPE::INCH;

// 	m_bFindLineObjPickingMode = FALSE;

	m_nListLines = 0;
}


void CFileFormatRs274X::ResetObjectData(GraphObj &gObj)
{
	gObj.dCode = 0;
	gObj.Type.nType = 0;
	gObj.Type.nAttr = 0;
	gObj.Type.nPolarity = 0;
	gObj.Type.nOrient = 0;
	gObj.Type.nReserve1 = 0;
	gObj.Type.nReserve2 = 0;
	gObj.Type.nParentid = 0;
	gObj.nValue = 0;
	gObj.dCode = 0;
	gObj.dAttrListCode = 0;
	gObj.nStPnt = 0;
	gObj.Extent = m_MaxFrameExtent;
}

void CFileFormatRs274X::ClearObjectData(GraphObj &gObj)
{
	gObj.dCode = 0;
	gObj.Type.nType = 0;
	gObj.Type.nAttr = 0;
	gObj.Type.nPolarity = 0;
	gObj.Type.nOrient = 0;
// 	gObj.Type.nReserve1 = 0;
// 	gObj.Type.nReserve2 = 0;
// 	gObj.Type.nParentid = 0;
// 	gObj.nValue = 0;
// 	gObj.dCode = 0;
// 	gObj.dAttrListCode = 0;
// 	gObj.nStPnt = 0;
	gObj.Extent = m_MaxFrameExtent;
}

CString CFileFormatRs274X::Load(CString sPath)
{
	char FileD[MAX_PATH];
	char *FileData;
	CString DsipMsg="", sData="";
 	int nFileSize, nRSize, i;	//, nSizeTemp	
	FILE *fp;

	m_sFilePath = "";

	CString sFile="", sTempPath=sPath;
	int nPos = sTempPath.ReverseFind('\\');
	if (nPos != -1)
	{
		sFile = sTempPath.Right(sTempPath.GetLength()-nPos-1);
		sTempPath.Delete(nPos, sPath.GetLength() - nPos);
	}

	double dRatio=0.0;
	CString sVal;
// 	CDlgProgress dlg;
// 	sVal.Format("On loading [%s]", sFile);
// 	dlg.Create(sVal);

	//파일을 불러옴. 
	strcpy(FileD, sPath);

	if((fp = fopen((LPCTSTR)FileD, "r")) != NULL)
	{
		fseek(fp, 0, SEEK_END);
		nFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// Allocate space for a path name
		FileData = (char*)calloc(nFileSize, sizeof(char));	
		
		nRSize = 0;
		for( i=0; i < nFileSize ; i++ )
		{
			if( feof( fp ) != 0 )
				break;

// 			dRatio = double(i+1)/double(nFileSize) * 100.0 ;
// 			if(!(int(dRatio)%10))
// 				dlg.SetPos(int(dRatio));

			FileData[i] = fgetc(fp);

			if(FileData[i] == '\n')
			{
				if(FileData[i-1] != '\r')
				{
					nFileSize++;
					FileData = (char*)realloc(FileData, nFileSize);
					FileData[i] = '\r';
					i++;
					nRSize++;
					FileData[i] = '\n';
				}
			}

			nRSize++;
		}

		sData.Format("%s", FileData);

		fclose(fp);
		free( FileData );
	}
	else
	{
		DsipMsg.Format("파일이 존재하지 않습니다.\r\n%s", sPath);
		AfxMessageBox(DsipMsg);
	}

// 	dlg.DestroyWindow();

	m_sFilePath = sPath;

	return sData;
}


int CFileFormatRs274X::GetNumOfLayer()
{
	m_nNumOfLayer = m_ArLayerInform.GetSize();
	return m_nNumOfLayer;
}

int	CFileFormatRs274X::GetNumOfAperture()
{
	return m_nNumOfAperture;
}

int	CFileFormatRs274X::GetNumOfLineTemplate()
{
	return m_nNumOfLineTemplate;
}

int	CFileFormatRs274X::GetNumOfSnR()
{
	return m_nNumOfSnRObject;
}

void CFileFormatRs274X::SetFrameExtent(FRECTC fRect)
{
	m_FrameExtent = fRect;
}

FRECTC CFileFormatRs274X::GetFrameExtent()
{
	// frame extent by gerber file
	return 	m_FrameExtent;
}

FRECTC CFileFormatRs274X::GetLayerExtent(int nLayer)
{
	return m_ArLayerInform[nLayer]->Extent;
}

void CFileFormatRs274X::SetDrawArcResolution(float fResolution)
{
	m_fDrawArcResolution = fResolution;
}


void CFileFormatRs274X::CloseDecoding()
{
	int nLayer;

	m_nNumOfLayer = m_ArLayerInform.GetSize();

	////////////////////////////////////////////////////////////////////////
	FRECTC fRect=GetLayerExtent(0);
	for(nLayer=1; nLayer <GetNumOfLayer() ; nLayer++)
	{
		FRECTC rect = GetLayerExtent(nLayer);
		m_pTrUtil->AdjustExtentByRect(fRect, rect); 
	}
	SetFrameExtent(fRect);
	SetDrawArcResolution(0.005);
	
	AllocDrawList();

	m_bEnableDraw = TRUE;
}

// int CFileFormatRs274X::MakeLineTemplateList()
// {
// 	TEMPLATE_KEY Key;
// 	int i,nTemplate,nNumOfLineTemplate;
// 	float fLength;
// 	float fThickness;
// 
// 	nNumOfLineTemplate = 0;
// 	nTemplate = 0;
// 
// 	int nNumOfLayer = GetNumOfLayer();
// 
// 	nNumOfLineTemplate = m_vecLineTemplate.size();
// 	for(i = 0 ; i < nNumOfLineTemplate ; i++) 
// 	{
// 		Key.key = m_vecLineTemplate.at(i);
// 
// 		fLength = sqrt(Key.Param[0]); // length
// 		fThickness = Key.Param[1]; // line width
// 
// // 		glNewList(m_nListLines+nTemplate,GL_COMPILE);
// 		MyMakeLineList(fLength,fThickness);
// // 		glEndList();
// 		nTemplate++;
// 	}
// 	return nTemplate;
// }

// int	CFileFormatRs274X::MakeApertureList() 
// {
// 	int nKey;
// 	LPAPERTURE_DEF pAptDef;
// 	DrawListAPerture listAP; 
// 	POSITION pos = NULL;
// 	int nInc=0;
// 	vector <FPOINTC> vecVertics;
// 
// 	float *fParam;
// 
// 	LPLAYER_INFORM_RS274X pLayerInfo;
// 
// 	int nNumOfLayer = GetNumOfLayer();
// 	
// 	for(int k = 0 ; k < nNumOfLayer ; k++)
// 	{ 
// 		pLayerInfo = m_ArLayerInform.GetAt(k);
// 		pos = pLayerInfo->mapAptList.GetStartPosition();
// 
// 		while(pos != NULL)
// 		{
// 			pLayerInfo->mapAptList.GetNextAssoc(pos, nKey, pAptDef);
// // 			glNewList(m_nListAperture+nInc,GL_COMPILE);
// 			int pNum = pAptDef->Param.at(0).fParam.size();
// 
// 			fParam = &pAptDef->Param.at(0).fParam[0];
// 
// 			vecVertics.clear();
// // 			m_vecVertics.clear();
// 
// 			if(pAptDef->chStdAptType == _T('C'))	//    ' Circle
// 			{
// 				if (pNum == 1)
// 				{
// 					DrawCircle(0, 0, fParam[0]*0.5);
// 				}
// 				else if (pNum == 2)
// 				{
// 					DrawDonut(0, 0, fParam[0], fParam[1]);
// 				}
// 				else if(pNum > 2 && pNum < 5)
// 				{
// // 					glClear(GL_STENCIL_BUFFER_BIT);
// // 					glEnable(GL_STENCIL_TEST);
// // 					SetStencil(FALSE);	
// 
// 					if (pNum == 3)
// 					{						
// 						MyRectCenter(0, 0, fParam[1], fParam[2], 0);
// 					}
// 					else if (pNum == 4)
// 					{
// 						MyRectCenter( 0, 0, fParam[1], fParam[2], fParam[3]);
// 					}
// // 					SetStencil(TRUE);
// 					DrawCircle(0, 0, fParam[0]*0.5);
// // 					glDisable(GL_STENCIL_TEST);
// 				}
// 			}
// 			else if(pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
// 			{
// 				if(pNum < 3)
// 				{
// 					MyRectCenter(0, 0, fParam[0], fParam[1], 0);
// 				}
// 				else if(pNum < 5)
// 				{
// // 					glClear(GL_STENCIL_BUFFER_BIT);
// // 					glEnable(GL_STENCIL_TEST);
// // 					SetStencil(FALSE);				
// 
// 					if (pNum == 3)
// 					{
// 						DrawCircle(0, 0, fParam[2]*0.5);
// 					}
// 					else if (pNum == 4)
// 					{
// 						MyRectCenter(0, 0, fParam[2], fParam[3], 0);
// 					}
// // 					SetStencil(TRUE);
// 					MyRectCenter(0, 0, fParam[0], fParam[1], 0);
// // 					glDisable(GL_STENCIL_TEST);
// 				}
// 			}
// 			else if(pAptDef->chStdAptType == _T('O'))	//    ' Oval
// 			{
// 				if(pNum < 3)
// 				{
// 					MyOval(0, 0, fParam[0], fParam[1]);
// 				}
// 				else if(pNum < 5)
// 				{
// // 					glClear(GL_STENCIL_BUFFER_BIT);
// // 					glEnable(GL_STENCIL_TEST);
// 
// // 					SetStencil(FALSE);
// 
// 					if (pNum == 3)
// 					{
// 						DrawCircle(0, 0, fParam[2]*0.5);
// 					}
// 					else if (pNum == 4)
// 					{
// 						MyRectCenter(0, 0, fParam[2], fParam[3], 0);
// 					}
// 
// // 					SetStencil(TRUE);
// 					MyOval(0, 0, fParam[0], fParam[1]);
// // 					glDisable(GL_STENCIL_TEST);
// 				}
// 			}
// 			else if(pAptDef->chStdAptType == _T('P'))	//    ' Polygon
// 			{
// 				int nSide = fParam[1];
// 				if(pNum < 4)
// 				{
// 					MyPolygonNSide(nSide, 0, 0, fParam[0], fParam[2]);
// 				}
// 				else
// 				{
// // 					glClear(GL_STENCIL_BUFFER_BIT);
// // 					glEnable(GL_STENCIL_TEST);
// // 					SetStencil(FALSE);
// 				
// 					if (pNum == 4)
// 					{
// 						DrawCircle(0, 0, fParam[3] * 0.5);
// 					}
// 					else if (pNum >= 5)
// 					{
// 						MyRectCenter( 0, 0, fParam[3], fParam[4], 0);
// 					}
// // 					SetStencil(TRUE);
// 					MyPolygonNSide(nSide, 0, 0, fParam[0], fParam[2]);
// // 					glDisable(GL_STENCIL_TEST);
// 				}
// 			}
// 			else //    ' Userdefined Apperture
// 			{
// 				MyMacro(pAptDef,0,0,0);
// 			}	
// 
// // 			glEndList();
// 			listAP.nDcode.push_back(nKey);
// 			listAP.nListNum.push_back(m_nListAperture+nInc);
// 			
// 			pAptDef->nGlListIndex = m_nListAperture+nInc;
// 			pLayerInfo->mapAptList.SetAt(nKey, pAptDef);
// 
// 			nInc++;
// 		}
// 
// 		listAP.nDcode.clear();
// 		vector <int>(listAP.nDcode.begin(),listAP.nDcode.end()).swap(listAP.nDcode);
// 		listAP.nListNum.clear();
// 		vector <int>(listAP.nListNum.begin(),listAP.nListNum.end()).swap(listAP.nListNum);
// 	}
// 	return nInc;
// }

// int	CFileFormatRs274X::MakeSnRObjectList() 
// {
// 	int i, k, n, nLayer, nInc = 0, nMCode;
// 	int nObjIndex, nNumOfObj, nGLError;
// 	int nStObj, nEdObj, nXRepeat, nYRepeat, nCoord;
// 	float dX, dY, ddX, ddY;
// 	FPOINTC fptOffset, fptCenter;
// 	FRECTC frtCurrentExtent;
// 	vector <FPOINTC> vecObjCoord;
// 	GraphObj gObj, obj;	
// 
// 	int nNumOfLayer = GetNumOfLayer();
// 	
// 	for(nLayer = 0 ; nLayer < nNumOfLayer ; nLayer++)
// 	{ 
// 		m_nLayerID = nLayer;
// 		m_pLayerInfo = m_ArLayerInform.GetAt(nLayer);
// 		 
// 		if (m_pLayerInfo->bView)
// 		{			
// 			nNumOfObj = m_pLayerInfo->listObj.size()-1;
// 
// 			m_nPrevAttribute = 16;
// 			m_nPrevPolarity = 3;
// 			BOOL bMakeNewGL = TRUE;
// 			for (nObjIndex = 0 ; nObjIndex < nNumOfObj ; nObjIndex++) 
// 			{
// 				gObj = m_pLayerInfo->listObj.at(nObjIndex);
// 				if(gObj.Type.nType == DRAW_TYPE::SNR )
// 				{
// 					frtCurrentExtent = GetFrameExtent();
// 
// 					nStObj = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
// 					nEdObj = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
// 					
// 					nXRepeat = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x;
// 					nYRepeat = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
// 				
// 					dX = m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x;
// 					dY = m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y;
// 
// // 					glNewList(m_nListSnR+nInc, GL_COMPILE);
// // 					nGLError = glGetError();
// // 					switch(nGLError)
// // 					{
// // 					case GL_INVALID_VALUE:
// // 						AfxMessageBox(_T("OpenGL new list error:GL_INVALID_VALUE"));
// // 						bMakeNewGL = FALSE;
// // 						break;
// // 					case GL_INVALID_ENUM:
// // 						AfxMessageBox(_T("OpenGL new list error:GL_INVALID_ENUM"));
// // 						bMakeNewGL = FALSE;
// // 						break;
// // 					case GL_INVALID_OPERATION:
// // 						AfxMessageBox(_T("OpenGL new list error:GL_INVALID_OPERATION"));
// // 						bMakeNewGL = FALSE;
// // 						break;
// // 					case GL_OUT_OF_MEMORY:
// // 						AfxMessageBox(_T("OpenGL new list error:GL_OUT_OF_MEMORY"));
// // 						bMakeNewGL = FALSE;
// // 						break;
// // 					}
// 					
// 					if(!bMakeNewGL)
// 						break;
// 
// 					for(k= nStObj; k<=nEdObj; k++)
// 						MyObjEntityShift (k, 0, 0);
// 
// // 					glEndList();//jun
// 					m_pLayerInfo->listObj.at(nObjIndex).dCode = m_nListSnR+nInc;
// 					nInc++;
// 				}
// 			}
// 			if(!bMakeNewGL)
// 				break;
// 		}
// 	}
// 	return nInc;
// }


// void CFileFormatRs274X::TranslateCoordbyActionCode(vector <FPOINTC> &vecCoord, GraphObj &gObj, FPOINTC fptCenter)
// {
// 	int i, nNumOfCoord;
// 	float cV, sV, XX, YY;
// 
// 	ldiv_t Rem = ldiv(m_nActionCode, 10);
// 
// 	nNumOfCoord = vecCoord.size();
// 	if(Rem.quot > 0)
// 	{
// 		for( i = 0 ; i < nNumOfCoord; i++)
// 		{
// 			switch(Rem.quot)
// 			{
// 			case TRANSFORM_DATA::MIRROR_V:
// 				vecCoord.at(i).x = -(vecCoord.at(i).x - fptCenter.x) + fptCenter.x;
// 				break;
// 			case TRANSFORM_DATA::MIRROR_H:
// 				vecCoord.at(i).y = -(vecCoord.at(i).y - fptCenter.y) + fptCenter.y;
// 				break;
// 			case TRANSFORM_DATA::ROTATE_180:
// 				m_pTrUtil->RotatePoint180(vecCoord.at(i), fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CCW:
// 				m_pTrUtil->RotatePoint90(vecCoord.at(i), fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CW:
// 				m_pTrUtil->RotatePoint270(vecCoord.at(i), fptCenter);
// 				break;
// 			}
// 		}
// 
// 		switch(Rem.quot)
// 		{
// 		case TRANSFORM_DATA::MIRROR_V:
// 			gObj.Extent.X1 = -(gObj.Extent.X1 - fptCenter.x) + fptCenter.x;
// 			gObj.Extent.X2 = -(gObj.Extent.X2 - fptCenter.x) + fptCenter.x;
// 			if(gObj.Extent.X1 > gObj.Extent.X2)
// 			{
// 				XX = gObj.Extent.X1;
// 				gObj.Extent.X1 = gObj.Extent.X2;
// 				gObj.Extent.X2 = XX; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::MIRROR_H:
// 			gObj.Extent.Y1 = -(gObj.Extent.Y1 - fptCenter.y) + fptCenter.y;
// 			gObj.Extent.Y2 = -(gObj.Extent.Y2 - fptCenter.y) + fptCenter.y;
// 			if(gObj.Extent.Y1 > gObj.Extent.Y2)
// 			{
// 				YY = gObj.Extent.Y1;
// 				gObj.Extent.Y1 = gObj.Extent.Y2;
// 				gObj.Extent.Y2 = YY; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::ROTATE_180:
// 			m_pTrUtil->RotateRect180(gObj.Extent, fptCenter);
// 			m_pTrUtil->RotatePoint180(fptCenter, fptCenter);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CCW:
// 			m_pTrUtil->RotateRect90(gObj.Extent, fptCenter);
// 			m_pTrUtil->RotatePoint90(fptCenter, fptCenter);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CW:
// 			m_pTrUtil->RotateRect270(gObj.Extent, fptCenter);
// 			m_pTrUtil->RotatePoint270(fptCenter, fptCenter);
// 			break;
// 		}
// 	}
// 
// 	if(Rem.rem > 0)
// 	{
// 		for( i = 0 ; i < nNumOfCoord; i++)
// 		{
// 			switch(Rem.rem)
// 			{
// 			case TRANSFORM_DATA::MIRROR_V:
// 				vecCoord.at(i).x = -(vecCoord.at(i).x - fptCenter.x) + fptCenter.x;
// 				break;
// 			case TRANSFORM_DATA::MIRROR_H:
// 				vecCoord.at(i).y = -(vecCoord.at(i).y - fptCenter.y) + fptCenter.y;
// 				break;
// 			case TRANSFORM_DATA::ROTATE_180:
// 				m_pTrUtil->RotatePoint180(vecCoord.at(i), fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CCW:
// 				m_pTrUtil->RotatePoint90(vecCoord.at(i), fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CW:
// 				m_pTrUtil->RotatePoint270(vecCoord.at(i), fptCenter);
// 				break;
// 			}
// 		}
// 		
// 		switch(Rem.rem)
// 		{
// 		case TRANSFORM_DATA::MIRROR_V:
// 			gObj.Extent.X1 = -(gObj.Extent.X1 - fptCenter.x) + fptCenter.x;
// 			gObj.Extent.X2 = -(gObj.Extent.X2 - fptCenter.x) + fptCenter.x;
// 			if(gObj.Extent.X1 > gObj.Extent.X2)
// 			{
// 				XX = gObj.Extent.X1;
// 				gObj.Extent.X1 = gObj.Extent.X2;
// 				gObj.Extent.X2 = XX; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::MIRROR_H:
// 			gObj.Extent.Y1 = -(gObj.Extent.Y1 - fptCenter.y) + fptCenter.y;
// 			gObj.Extent.Y2 = -(gObj.Extent.Y2 - fptCenter.y) + fptCenter.y;
// 			if(gObj.Extent.Y1 > gObj.Extent.Y2)
// 			{
// 				YY = gObj.Extent.Y1;
// 				gObj.Extent.Y1 = gObj.Extent.Y2;
// 				gObj.Extent.Y2 = YY; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::ROTATE_180:
// 			m_pTrUtil->cVsV(180, &cV, &sV);
// 			RotateTranslateExtent(gObj.Extent, cV, sV, fptCenter.x, fptCenter.y, 0, 0);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CCW:
// 			m_pTrUtil->cVsV(90, &cV, &sV);							// CCW Rotate
// 			RotateTranslateExtent(gObj.Extent, cV, sV, fptCenter.x, fptCenter.y, 0, 0);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CW:
// 			m_pTrUtil->cVsV(-90, &cV, &sV);							// CW Rotate
// 			RotateTranslateExtent(gObj.Extent, cV, sV, fptCenter.x, fptCenter.y, 0, 0);	
// 			break;
// 		}
// 	}
// }

// void CFileFormatRs274X::RotateTranslateExtent(FRECTC &Extent, double cV, double sV, double cX, double cY, double dX, double dY)
// {
//     double X1, Y1, X2, Y2;
// 	FRECTC rRect;
// 
//     X1 = ((Extent.X1 - cX) * cV) - ((Extent.Y1 - cY) * sV) + cX + dX;
//     Y1 = ((Extent.X1 - cX) * sV) + ((Extent.Y1 - cY) * cV) + cY + dY;
//     X2 = ((Extent.X2 - cX) * cV) - ((Extent.Y2 - cY) * sV) + cX + dX;
//     Y2 = ((Extent.X2 - cX) * sV) + ((Extent.Y2 - cY) * cV) + cY + dY;
//     
//     if (X1 < X2)
// 	{
// 		rRect.X1 = X1;
//         rRect.X2 = X2;
// 	}
//     else
// 	{
//         rRect.X1 = X2;
//         rRect.X2 = X1;
//     }
//     
//     if (Y1 < Y2)
// 	{
// 		rRect.Y1 = Y1;
//         rRect.Y2 = Y2;
// 	}
//     else
// 	{
// 		rRect.Y1 = Y2;
//         rRect.Y2 = Y1;
//     }
// 	Extent = rRect;   
// }

// void CFileFormatRs274X::RotateObject90(vector<FPOINTC> &vecCoord,GraphObj *obj,FPOINTC fC)
// {
// 	byte dType;
// 	FPOINTC p;
// 	int k, nSize;
// //	, double ang, double cV, double sV
// 	nSize = vecCoord.size();
//     dType = obj->Type.nType;
//     
// 	
// 	if(dType == DRAW_TYPE::LINE) //' Line: m_pTrUtil->m_bType[0]============================================================>
// 	{
// 		m_pTrUtil->RotatePoint90(vecCoord[0], fC);
// 		m_pTrUtil->RotatePoint90(vecCoord[1], fC);
// 		
// 	}
//     else if (dType == DRAW_TYPE::FLASH) // 'Flash : m_pTrUtil->m_bType[1]
// 	{
// 		//RotateTranslatePoint(vecCoord[0], cV, sV, cX,cY, 0, 0);
// 		m_pTrUtil->RotatePoint90(vecCoord[0], fC);
//     }    
//     else if (dType == DRAW_TYPE::POLYGON_SQ || dType == DRAW_TYPE::POLYGON_MQ) // m_pTrUtil->m_bType[5], m_pTrUtil->m_bType[6] 'PolygonFill with G74, G75
// 	{
//         m_pTrUtil->RotatePoint90(vecCoord[0], fC);
// 		for (k=1 ; k < nSize ; k++)
// 		{
// 			if (fabs(vecCoord[k].x) == DetGcode && fabs(vecCoord[k].y) == DetGcode)
// 			{
//                 k++;
// 				m_pTrUtil->RotatePoint90(vecCoord[k], fC);
//                 
//                 k++;
// 				m_pTrUtil->RotatePoint90(vecCoord[k], fC);
// 				
// 			}
//             else
// 			{
// 				m_pTrUtil->RotatePoint90(vecCoord[k], fC);
//             }
//         }
// 	}
//     else if (dType == DRAW_TYPE::ARC_CW || dType == DRAW_TYPE::ARC_CCW) // dType == TYPE_CW
// 	{
// 		m_pTrUtil->RotatePoint90(vecCoord[0], fC);
// 		m_pTrUtil->RotatePoint90(vecCoord[1], fC);
// 		m_pTrUtil->RotatePoint90(vecCoord[2], fC);
// 	}
//     else if (dType == DRAW_TYPE::SNR) 
// 	{
//         double XX = vecCoord[obj->nStPnt+2].x;
//         vecCoord[2].x =  -vecCoord[2].y;
//         vecCoord[2].y = XX;
// 
// 		XX = vecCoord[1].x;
// 		vecCoord[1].x = vecCoord[1].y;
// 		vecCoord[1].y = XX;
//     }    
//                 
//     m_pTrUtil->RotateRect90(obj->Extent, fC);
// }

// FPOINTC CFileFormatRs274X::OriginTranslateCoord(vector <FPOINTC> &vecCoord, GraphObj &gObj, FRECTC fExtent)
// {
// 	int i, nNumOfCoord;
// 	float XX, YY;
// 	FPOINTC fptCenter;
// 
// 	ldiv_t Rem = ldiv(m_pTrUtil->m_nActionCode, 10);
// 
// 	fptCenter.x = (fExtent.X1 + fExtent.X2) / 2.0;
// 	fptCenter.y = (fExtent.Y1 + fExtent.Y2) / 2.0;
// 
// 	nNumOfCoord = vecCoord.size();
// 	if(Rem.rem > 0)
// 	{
// 		for( i = 0 ; i < nNumOfCoord; i++)
// 		{
// 			switch(Rem.rem)
// 			{
// 			case TRANSFORM_DATA::MIRROR_V:
// 				vecCoord.at(i).x = -(vecCoord.at(i).x - fptCenter.x) + fptCenter.x;
// 				break;
// 			case TRANSFORM_DATA::MIRROR_H:
// 				vecCoord.at(i).y = -(vecCoord.at(i).y - fptCenter.y) + fptCenter.y;
// 				break;
// 			case TRANSFORM_DATA::ROTATE_180:
// 				m_pTrUtil->RotatePoint180(vecCoord.at(i),fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CCW:
// 				m_pTrUtil->RotatePoint270(vecCoord.at(i),fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CW:
// 				m_pTrUtil->RotatePoint90(vecCoord.at(i),fptCenter);
// 
// 				break;
// 			}
// 		}
// 
// 		switch(Rem.rem)
// 		{
// 		case TRANSFORM_DATA::MIRROR_V:
// 			gObj.Extent.X1 = -(gObj.Extent.X1 - fptCenter.x) + fptCenter.x;
// 			gObj.Extent.X2 = -(gObj.Extent.X2 - fptCenter.x) + fptCenter.x;
// 			if(gObj.Extent.X1 > gObj.Extent.X2)
// 			{
// 				XX = gObj.Extent.X1;
// 				gObj.Extent.X1 = gObj.Extent.X2;
// 				gObj.Extent.X2 = XX; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::MIRROR_H:
// 			gObj.Extent.Y1 = -(gObj.Extent.Y1 - fptCenter.y) + fptCenter.y;
// 			gObj.Extent.Y2 = -(gObj.Extent.Y2 - fptCenter.y) + fptCenter.y;
// 			if(gObj.Extent.Y1 > gObj.Extent.Y2)
// 			{
// 				YY = gObj.Extent.Y1;
// 				gObj.Extent.Y1 = gObj.Extent.Y2;
// 				gObj.Extent.Y2 = YY; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::ROTATE_180:
// 			m_pTrUtil->RotateRect180(gObj.Extent,fptCenter);
// 			m_pTrUtil->RotatePoint180(fptCenter,fptCenter);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CCW:
// 			m_pTrUtil->RotateRect270(gObj.Extent,fptCenter);
// 			m_pTrUtil->RotatePoint270(fptCenter,fptCenter);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CW:
// 			m_pTrUtil->RotateRect270(gObj.Extent,fptCenter);
// 			m_pTrUtil->RotatePoint90(fptCenter,fptCenter);
// 			break;
// 		}
// 	}
// 
// 	if(Rem.quot > 0)
// 	{
// 		for( i = 0 ; i < nNumOfCoord; i++)
// 		{
// 			switch(Rem.quot)
// 			{
// 			case TRANSFORM_DATA::MIRROR_V:
// 				vecCoord.at(i).x = -(vecCoord.at(i).x - fptCenter.x) + fptCenter.x;
// 				break;
// 			case TRANSFORM_DATA::MIRROR_H:
// 				vecCoord.at(i).y = -(vecCoord.at(i).y - fptCenter.y) + fptCenter.y;
// 				break;
// 			case TRANSFORM_DATA::ROTATE_180:
// 				m_pTrUtil->RotatePoint180(vecCoord.at(i), fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CCW:
// 				m_pTrUtil->RotatePoint270(vecCoord.at(i), fptCenter);
// 				break;
// 			case TRANSFORM_DATA::ROTATE_CW:
// 				m_pTrUtil->RotatePoint90(vecCoord.at(i), fptCenter);
// 				break;
// 			}
// 		}
// 
// 		switch(Rem.quot)
// 		{
// 			case TRANSFORM_DATA::MIRROR_V:
// 			gObj.Extent.X1 = -(gObj.Extent.X1 - fptCenter.x) + fptCenter.x;
// 			gObj.Extent.X2 = -(gObj.Extent.X2 - fptCenter.x) + fptCenter.x;
// 			if(gObj.Extent.X1 > gObj.Extent.X2)
// 			{
// 				XX = gObj.Extent.X1;
// 				gObj.Extent.X1 = gObj.Extent.X2;
// 				gObj.Extent.X2 = XX; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::MIRROR_H:
// 			gObj.Extent.Y1 = -(gObj.Extent.Y1 - fptCenter.y) + fptCenter.y;
// 			gObj.Extent.Y2 = -(gObj.Extent.Y2 - fptCenter.y) + fptCenter.y;
// 			if(gObj.Extent.Y1 > gObj.Extent.Y2)
// 			{
// 				YY = gObj.Extent.Y1;
// 				gObj.Extent.Y1 = gObj.Extent.Y2;
// 				gObj.Extent.Y2 = YY; 
// 			}
// 			break;
// 		case TRANSFORM_DATA::ROTATE_180:
// 			m_pTrUtil->RotateRect180(gObj.Extent, fptCenter);
// 			m_pTrUtil->RotatePoint180(fptCenter, fptCenter);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CCW:
// 			m_pTrUtil->RotateRect270(gObj.Extent, fptCenter);
// 			m_pTrUtil->RotatePoint270(fptCenter, fptCenter);
// 			break;
// 		case TRANSFORM_DATA::ROTATE_CW:
// 			m_pTrUtil->RotateRect90(gObj.Extent, fptCenter);
// 			m_pTrUtil->RotatePoint90(fptCenter, fptCenter);
// 			break;
// 		}
// 	}
// 
// 	return fptCenter;
// }
void CFileFormatRs274X::SetDrawResolution(float fResolution)
{
	m_fDrawResolution = fResolution;
}

BOOL CFileFormatRs274X::DoDecoding(char* pFile, char* pLine)
{
	CString strErrMsg;
	
	while(!m_bLastFrame)
	{
		DecodingParsingFlag();

		if(DecodingIgnore(&pFile, &pLine))
			continue;
		if(DecodingCoordinate(&pFile))
			continue;
		if(DecodingFunction(&pFile))
			continue;
		if(DecodingParameter(&pFile))
			continue;

		strErrMsg.Format(_T("RS274X parameter block parsing error: syntax error"));
		AfxMessageBox(strErrMsg);
		m_bLastFrame = TRUE;
	}

	return TRUE;
}

BOOL CFileFormatRs274X::Decoding(char* pFile)
{
	CString strErrMsg;
	char* pLine = pFile;
	int nIdx=0;

	m_bEnableDraw = FALSE;

	m_pLayerInfo = new LAYER_INFORM_RS274X;
	m_nLayer = 0;
// 	m_nNumOfObject = m_ArLayerStruct.GetAt(m_nLayer).nObject;
//	m_nNumOfCoord = m_ArLayerStruct.GetAt(m_nLayer).nCoord;
//	m_pLayerInfo->listObj.reserve(m_nNumOfObject+1); // n*36byte
	m_pLayerInfo->nFormat = CAM_DATA::RS274X;
	m_pLayerInfo->strFilePath = m_sFilePath;


	m_nNumOfDataBlock= 0;
	m_nNumOfTextLine = 1;
	m_nNumOfObjCoord = 0;
	m_nStartMirrorObj = 0;
	m_nEndMirrorObj = 0;

	m_fptPrevPos.x = 0;
	m_fptPrevPos.y = 0;

	m_vecTemp.reserve(m_nMaxLineTemplate);

	m_bParsingXYFlag = FALSE;
	m_bParsingIJFlag = FALSE;
	m_bParsingGCodeFlag = FALSE;
	m_bParsingdCodeFlag = FALSE;
	m_bPolygonFlag = FALSE;
	m_bImplicitFlag = FALSE;

	m_nObj = 0;
	m_nPrevDCode = -1;
	m_nIRAngle = 0; // Image Rotation angle

	m_bLastFrame = FALSE;

	ResetData();
	ResetObjectData(m_gObj);
	InitMaxLayerExtent();

	DoDecoding(pFile, pLine);
// 	while(!m_bLastFrame)
// 	{
// 		DecodingParsingFlag();
// 
// 		if(DecodingIgnore(&pFile, &pLine))
// 			continue;
// 		if(DecodingCoordinate(&pFile))
// 			continue;
// 		if(DecodingFunction(&pFile))
// 			continue;
// 		if(DecodingParameter(&pFile))
// 			continue;
// 
// 		strErrMsg.Format(_T("RS274X parameter block parsing error: syntax error"));
// 		AfxMessageBox(strErrMsg);
// 		m_bLastFrame = TRUE;
// 	}


	m_bParsingXYFlag = FALSE;
	m_bParsingIJFlag = FALSE;
	m_bParsingGCodeFlag = FALSE;
	m_bParsingdCodeFlag = FALSE;

	//// Add Dummy Object
	//m_gObj.Type.nType = DRAW_TYPE::FLASH;
	//m_gObj.Type.nAttr = DRAW_TYPE::NONE;
	//m_gObj.nStPnt = m_nNumOfObjCoord;
	//
	//m_pLayerInfo->listObj.push_back(m_gObj);
	//ClearObjectData(m_gObj);

	m_nNumOfLineTemplate = m_vecLineTemplate.size();
	if(m_nNumOfLineTemplate>0)
	{
		sort(m_vecLineTemplate.begin(), m_vecLineTemplate.end());
		m_vecTemp.clear();
		std::unique_copy(m_vecLineTemplate.begin(),m_vecLineTemplate.end(),std::back_inserter(m_vecTemp));
		(m_vecLineTemplate).swap(m_vecTemp);
// 		i = m_vecLineTemplate.size();
// 		i = m_vecLineTemplate.capacity();
		m_nNumOfLineTemplate = m_vecLineTemplate.size();

	}
	m_vecTemp.clear();
	vector <UINT64>(m_vecTemp.begin(), m_vecTemp.end()).swap(m_vecTemp);

	m_mapAMMacro.RemoveAll();


    m_pLayerInfo->Extent = GetMaxLayerExtent();//rectBound;
    m_pLayerInfo->bUnit = m_Unit;
	m_pLayerInfo->nLayerGroupID = m_nLayerGroupID;

	if(m_nIRAngle != 0)
	{
		double cX, cY;

		cX = (m_pLayerInfo->Extent.X1 + m_pLayerInfo->Extent.X2)/2.0;
		cY = (m_pLayerInfo->Extent.X1 + m_pLayerInfo->Extent.X2)/2.0;

		ApplyIROptionGRB(m_pLayerInfo, 0, 0, -m_nIRAngle);
	}
	
	m_ArLayerInform.Add(m_pLayerInfo);

	return TRUE;
}

void CFileFormatRs274X::DecodingParsingFlag()
{
	DRAW_TYPE::TYPE dType;
	TEMPLATE_KEY Key;
	float dX, dY;
	float fWidthOfLine;
	LINE_INFO sttLineInfo;


	if(!m_bParsingXYFlag && !m_bParsingIJFlag && !m_bPolygonFlag)
		return;

	if(m_bPolygonFlag)
	{
		if (m_gObj.nStPnt == m_nNumOfObjCoord )
		{
			m_nPrevDCode = m_nDCode;
			if (m_nDCode != 2)
			{
				m_pLayerInfo->vecObjCoord.push_back(m_fptPrevPos);
				m_nNumOfObjCoord++;
			}
		}

		if(m_bParsingIJFlag)
		{
			FPOINTC fpt;
			if(m_nGCode == 2)
			{
				fpt.x = DetGcode;
				fpt.y = DetGcode;
				dType = DRAW_TYPE::ARC_CW;
			}
			else
			{
				fpt.x = -DetGcode;
				fpt.y = -DetGcode;
				dType = DRAW_TYPE::ARC_CCW;
			}
			m_pLayerInfo->vecObjCoord.push_back(fpt);
			m_nNumOfObjCoord++;
			m_pLayerInfo->vecObjCoord.push_back(m_fptCoord);

			m_nNumOfObjCoord++;
			m_pLayerInfo->vecObjCoord.push_back(m_fptOpt);
			m_nNumOfObjCoord++;
        
			fpt.x = m_fptPrevPos.x + m_fptOpt.x;
			fpt.y = m_fptPrevPos.y + m_fptOpt.y;

			FRECTC MnMxTemp = MinMaxArcInPolygonGRB(dType, m_fptPrevPos, m_fptCoord, fpt);
			m_pTrUtil->AdjustExtentByRect(m_gObj.Extent, MnMxTemp);
			SetMaxLayerExtent(m_gObj.Extent);

		}
		else
		{
			m_pTrUtil->AdjustExtentByPoint(m_gObj.Extent, m_fptCoord);
			SetMaxLayerExtent(m_gObj.Extent);
			m_pLayerInfo->vecObjCoord.push_back(m_fptCoord);
			m_nNumOfObjCoord++;
		}
	}
	else
	{
		if(m_bParsingdCodeFlag)
		{
			if (!(m_nDCode == m_nPrevDCode && m_nADCode == m_nPrevADCode && m_fptCoord.x == m_fptPrevPos.x && m_fptCoord.y == m_fptPrevPos.y))
			{
				m_nPrevADCode = m_nADCode;
				switch(m_nDCode)//   'Draw Mode
				{
				case 1:
					m_pLayerInfo->vecObjCoord.push_back(m_fptCoord);
		
					m_nNumOfObjCoord++;
					m_pLayerInfo->vecObjCoord.push_back(m_fptPrevPos);
		
					m_nNumOfObjCoord++;
					m_gObj.dCode = m_nADCode;

					if(m_bParsingGCodeFlag)
					{
						if(m_nGCode > 2)
							m_gObj.Type.nType = DRAW_TYPE::ARC_CCW;
						else
							m_gObj.Type.nType = DRAW_TYPE::ARC_CW;
                
						m_fptOpt.x += m_fptPrevPos.x;
						m_fptOpt.y += m_fptPrevPos.y;
						m_pLayerInfo->vecObjCoord.push_back(m_fptOpt);
						m_nNumOfObjCoord++;
						m_gObj.Extent = MinMaxArcGRB((DRAW_TYPE::TYPE)m_gObj.Type.nType, m_fptPrevPos, m_fptCoord, m_fptOpt, m_CurMinMax);
					}
					else
					{
						if(m_bParsingIJFlag)
						{
							if(m_nGCode > 2)
								m_gObj.Type.nType = DRAW_TYPE::ARC_CCW;
							else
								m_gObj.Type.nType = DRAW_TYPE::ARC_CW;

							m_fptOpt.x += m_fptPrevPos.x;
							m_fptOpt.y += m_fptPrevPos.y;
																	
							m_pLayerInfo->vecObjCoord.push_back(m_fptOpt);
							m_nNumOfObjCoord++;
							m_gObj.Extent = MinMaxArcGRB((DRAW_TYPE::TYPE)m_gObj.Type.nType, m_fptPrevPos, m_fptCoord, m_fptOpt, m_CurMinMax);

						}
						else
						{
							// Linear interpolation
							if (m_fptCoord.x == m_fptPrevPos.x && m_fptCoord.y == m_fptPrevPos.y) //   ' Flash로 변환
								m_gObj.Type.nType = DRAW_TYPE::FLASH; 
							else
							{
								m_gObj.Type.nType = DRAW_TYPE::LINE;

								m_pLayerInfo->mapAptList.Lookup(m_gObj.dCode, m_rAptDef);
								if(m_rAptDef->chStdAptType == _T('C'))
								{
									m_nNumOfLineObject++; // count up line object
									m_nNumOfLineTemplate = m_vecLineTemplate.size();
									if(m_nNumOfLineTemplate > m_nMaxLineTemplate-2)
									{
										sort(m_vecLineTemplate.begin(), m_vecLineTemplate.end());
										m_vecTemp.clear();
										std::unique_copy(m_vecLineTemplate.begin(),m_vecLineTemplate.end(),std::back_inserter(m_vecTemp));
										(m_vecLineTemplate).swap(m_vecTemp);
									}
						
									dX = (m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt+1).x - m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt).x);
									dY = (m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt+1).y - m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt).y);	
									
									Key.Param[0] = (dX*dX + dY*dY); // length
									Key.Param[1] = m_rAptDef->Param.at(0).fParam.at(0); // line width

									m_vecLineTemplate.push_back(Key.key);

									fWidthOfLine = m_rAptDef->Param.at(0).fParam.at(0); 
									if(m_mapLineInfo.Lookup(fWidthOfLine, sttLineInfo))			
									{
										sttLineInfo.nNumOfLine++;
									}
									else
									{
										sttLineInfo.nNumOfLine = 1;
									}
									m_mapLineInfo.SetAt(fWidthOfLine, sttLineInfo);
								}
							}

							if (m_fptCoord.x < m_fptPrevPos.x)
							{
								m_gObj.Extent.X1 = m_fptCoord.x + m_CurMinMax.X1;
								m_gObj.Extent.X2 = m_fptPrevPos.x + m_CurMinMax.X2;
							}
							else
							{
								m_gObj.Extent.X1 = m_fptPrevPos.x + m_CurMinMax.X1;
								m_gObj.Extent.X2 = m_fptCoord.x + m_CurMinMax.X2;
							}
							if (m_fptCoord.y < m_fptPrevPos.y)
							{
								m_gObj.Extent.Y1 = m_fptCoord.y + m_CurMinMax.Y1;
								m_gObj.Extent.Y2 = m_fptPrevPos.y + m_CurMinMax.Y2;
							}
							else
							{
								m_gObj.Extent.Y1 = m_fptPrevPos.y + m_CurMinMax.Y1;
								m_gObj.Extent.Y2 = m_fptCoord.y + m_CurMinMax.Y2;
							}
						}							  
					}
					SetMaxLayerExtent(m_gObj.Extent);
					if (m_paramIP.bLayerPolarity == LAYER_TYPE::NEGATIVE) 
						m_gObj.Type.nPolarity = DRAW_TYPE::NEGATIVE; 
					else 
						m_gObj.Type.nPolarity = DRAW_TYPE::POSITIVE; 

					m_gObj.Type.nAttr = DRAW_TYPE::PATTERN;
				
					m_pLayerInfo->listObj.push_back(m_gObj);
					ClearObjectData(m_gObj);
					m_gObj.nStPnt = m_nNumOfObjCoord;
					
					m_nObj++;
            
//					fptPrevPos = fptCoord;          
					
					break;
				case 2:
//					fptPrevPos = fptCoord; 
					break;
				case 3:
					m_gObj.dCode = m_nADCode;
            
					m_gObj.Type.nType = DRAW_TYPE::FLASH;
            
					m_pLayerInfo->vecObjCoord.push_back(m_fptCoord);
					m_nNumOfObjCoord++;
        
					m_gObj.Extent.X1 = m_fptCoord.x + m_CurMinMax.X1;
					m_gObj.Extent.X2 = m_fptCoord.x + m_CurMinMax.X2;
					m_gObj.Extent.Y1 = m_fptCoord.y + m_CurMinMax.Y1;
					m_gObj.Extent.Y2 = m_fptCoord.y + m_CurMinMax.Y2;
					SetMaxLayerExtent(m_gObj.Extent);
                                        
					if (m_paramIP.bLayerPolarity == LAYER_TYPE::NEGATIVE) 
						m_gObj.Type.nPolarity = DRAW_TYPE::NEGATIVE;
					else 
						m_gObj.Type.nPolarity = DRAW_TYPE::POSITIVE;
					
					m_gObj.Type.nAttr = DRAW_TYPE::PATTERN;

				
					m_pLayerInfo->listObj.push_back(m_gObj);
					ClearObjectData(m_gObj);
					m_gObj.nStPnt = m_nNumOfObjCoord;

					m_nObj++;                               
//					fptPrevPos = fptCoord;
					break;
				default:
//					fptPrevPos = fptCoord;
                
					if (m_nDCode >= 10)
					{
						m_nADCode = m_nDCode;
						m_pLayerInfo->mapAptList.Lookup(m_nADCode, m_rAptDef);
						m_CurMinMax = m_rAptDef->minmax;
					}
					break;
				}
			}
			else
			{
//				fptPrevPos = fptCoord;
        
				if (m_nDCode >= 10)
				{
					m_nADCode = m_nDCode;
					m_pLayerInfo->mapAptList.Lookup(m_nADCode, m_rAptDef);
					m_CurMinMax = m_rAptDef->minmax;
				}
			}
		}
		else
		{
			if (!(m_nDCode == m_nPrevDCode && m_nADCode == m_nPrevADCode && m_fptCoord.x == m_fptPrevPos.x && m_fptCoord.y == m_fptPrevPos.y))
			{
				m_nPrevADCode = m_nADCode;
				switch(m_nDCode)
				{
				case 1:
					m_pLayerInfo->vecObjCoord.push_back(m_fptCoord);
					m_nNumOfObjCoord++;
					m_pLayerInfo->vecObjCoord.push_back(m_fptPrevPos);
					m_nNumOfObjCoord++;

					m_gObj.dCode = m_nADCode;

					if(m_bParsingGCodeFlag)
					{
						if(m_nGCode > 2)
							m_gObj.Type.nType = DRAW_TYPE::ARC_CCW;
						else
							m_gObj.Type.nType = DRAW_TYPE::ARC_CW;
                    
						m_fptOpt.x += m_fptPrevPos.x;
						m_fptOpt.y += m_fptPrevPos.y;

						m_gObj.Extent = MinMaxArcGRB((DRAW_TYPE::TYPE)m_gObj.Type.nType, m_fptPrevPos, m_fptCoord, m_fptOpt, m_CurMinMax);

						m_pLayerInfo->vecObjCoord.push_back(m_fptOpt);
						m_nNumOfObjCoord++;
					}
					else
					{
						if(m_bParsingIJFlag)
						{
							if(m_nGCode > 2)
								m_gObj.Type.nType = DRAW_TYPE::ARC_CCW;
							else
								m_gObj.Type.nType = DRAW_TYPE::ARC_CW;
                                    							
							m_fptOpt.x += m_fptPrevPos.x;
							m_fptOpt.y += m_fptPrevPos.y;

							m_pLayerInfo->vecObjCoord.push_back(m_fptOpt);
							m_nNumOfObjCoord++;
							m_gObj.Extent = MinMaxArcGRB((DRAW_TYPE::TYPE)m_gObj.Type.nType, m_fptPrevPos, m_fptCoord, m_fptOpt, m_CurMinMax);									               
						}
						else
						{
							if (m_fptCoord.x == m_fptPrevPos.x && m_fptCoord.y == m_fptPrevPos.y) //   ' Flash로 변환
								m_gObj.Type.nType = DRAW_TYPE::FLASH;
							else
							{
								m_gObj.Type.nType = DRAW_TYPE::LINE;
								m_pLayerInfo->mapAptList.Lookup(m_gObj.dCode, m_rAptDef);
								if(m_rAptDef->chStdAptType == _T('C'))
								{
									m_nNumOfLineObject++; // count up line object
									m_nNumOfLineTemplate = m_vecLineTemplate.size();
									if(m_nNumOfLineTemplate > m_nMaxLineTemplate-2)
									{
										sort(m_vecLineTemplate.begin(), m_vecLineTemplate.end());
										m_vecTemp.clear();
										std::unique_copy(m_vecLineTemplate.begin(),m_vecLineTemplate.end(),std::back_inserter(m_vecTemp));
										(m_vecLineTemplate).swap(m_vecTemp);
									}
									
									dX = (m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt+1).x - m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt).x);
									dY = (m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt+1).y - m_pLayerInfo->vecObjCoord.at(m_gObj.nStPnt).y);	
									
									Key.Param[0] = (dX*dX + dY*dY); // length
									Key.Param[1] = m_rAptDef->Param.at(0).fParam.at(0); // line width

									m_vecLineTemplate.push_back(Key.key);
									fWidthOfLine = m_rAptDef->Param.at(0).fParam.at(0); 
									if(m_mapLineInfo.Lookup(fWidthOfLine, sttLineInfo))			
									{
										sttLineInfo.nNumOfLine++;
									}
									else
									{
										sttLineInfo.nNumOfLine = 1;
									}
									m_mapLineInfo.SetAt(fWidthOfLine, sttLineInfo);
								}												
							}
                        
							if (m_fptCoord.x < m_fptPrevPos.x)
							{
								m_gObj.Extent.X1 = m_fptCoord.x + m_CurMinMax.X1;
								m_gObj.Extent.X2 = m_fptPrevPos.x + m_CurMinMax.X2;
							}
							else
							{
								m_gObj.Extent.X1 = m_fptPrevPos.x + m_CurMinMax.X1;
								m_gObj.Extent.X2 = m_fptCoord.x + m_CurMinMax.X2;
							}
							if (m_fptCoord.y < m_fptPrevPos.y)
							{
								m_gObj.Extent.Y1 = m_fptCoord.y + m_CurMinMax.Y1;
								m_gObj.Extent.Y2 = m_fptPrevPos.y + m_CurMinMax.Y2;
							}
							else
							{
								m_gObj.Extent.Y1 = m_fptPrevPos.y + m_CurMinMax.Y1;
								m_gObj.Extent.Y2 = m_fptCoord.y + m_CurMinMax.Y2;
							}
						}
					}
					SetMaxLayerExtent(m_gObj.Extent);
                    
					if (m_paramIP.bLayerPolarity == LAYER_TYPE::NEGATIVE)
						m_gObj.Type.nPolarity = DRAW_TYPE::NEGATIVE; 
					else 
						m_gObj.Type.nPolarity = DRAW_TYPE::POSITIVE;
                
					m_gObj.Type.nAttr = DRAW_TYPE::PATTERN;

			
					m_pLayerInfo->listObj.push_back(m_gObj);
					ClearObjectData(m_gObj);
					m_gObj.nStPnt = m_nNumOfObjCoord;
					
					m_nObj++;                               
//					fptPrevPos = fptCoord;          
					break;
				case 2:
//					fptPrevPos = fptCoord;
					break;
				case 3:
					m_gObj.dCode = m_nADCode;
        
					m_gObj.Type.nType = DRAW_TYPE::FLASH;

					m_pLayerInfo->vecObjCoord.push_back(m_fptCoord);
					m_nNumOfObjCoord++;
            
					m_gObj.Extent.X1 = m_fptCoord.x + m_CurMinMax.X1;
					m_gObj.Extent.X2 = m_fptCoord.x + m_CurMinMax.X2;
					m_gObj.Extent.Y1 = m_fptCoord.y + m_CurMinMax.Y1;
					m_gObj.Extent.Y2 = m_fptCoord.y + m_CurMinMax.Y2;
					SetMaxLayerExtent(m_gObj.Extent);

					if (m_paramIP.bLayerPolarity == LAYER_TYPE::NEGATIVE)
						m_gObj.Type.nPolarity = DRAW_TYPE::NEGATIVE; 
					else 
						m_gObj.Type.nPolarity = DRAW_TYPE::POSITIVE;

					m_gObj.Type.nAttr = DRAW_TYPE::PATTERN;

			
					m_pLayerInfo->listObj.push_back(m_gObj);
					ClearObjectData(m_gObj);
					m_gObj.nStPnt = m_nNumOfObjCoord;
				
					m_nObj++;                               
//					fptPrevPos = fptCoord;
					break;
				default:
					if (m_nDCode >= 10)
					{
						m_nADCode = m_nDCode;
						m_pLayerInfo->mapAptList.Lookup(m_nADCode, m_rAptDef);
						m_CurMinMax = m_rAptDef->minmax;
					}
					break;
				}
			}
			else
			{
//				fptPrevPos = fptCoord;
			}
		}
	}
	m_fptPrevPos = m_fptCoord;
				
	m_bParsingXYFlag = FALSE;
	m_bParsingIJFlag = FALSE;
	m_bParsingGCodeFlag = FALSE;
	m_bParsingdCodeFlag = FALSE;


}

BOOL CFileFormatRs274X::DecodingIgnore(char** pFile, char** pLine)
{
	switch(**pFile)
	{
	case TCHAR(EOB):
		(*pFile)++;
		m_nNumOfDataBlock++; // data block counter in file
		return TRUE;
	case TCHAR(ASCII_CR): 
		(*pFile)++;
		return TRUE;
	case TCHAR(ASCII_LF):
		(*pFile)++;
		m_nNumOfTextLine++;	// line counter in file
		pLine = pFile;
		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::DecodingCoordinate(char** pFile)
{
	CString strErrMsg;

	switch(**pFile)
	{
	case _T('X'):
		m_bParsingXYFlag = TRUE; 
		m_fptCoord.x = GetCoordDataX(&(++(*pFile)));
		if(**pFile == _T('Y')) // Y coordinate of line endpoint
		{
			m_fptCoord.y = GetCoordDataY(&(++(*pFile)));
		}
		if(**pFile == _T('I')) // Y coordinate of line endpoint
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.x = GetCoordDataX(&(++(*pFile)));
		}
		else
			m_fptOpt.x = 0.0;
		if(**pFile == _T('J')) // Y coordinate of line endpoint
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.y = GetCoordDataY(&(++(*pFile)));
		}
		else
			m_fptOpt.y = 0.0;
		if(**pFile == _T('D'))// Exposure
		{
			m_bParsingdCodeFlag = TRUE;
			m_nPrevDCode = m_nDCode;
			m_nDCode = GetIntegerData(&(++(*pFile)));
			switch(m_nDCode)
			{
			case 1: // Exposure On
				break;
			case 2: // Exposure Off
				break;
			case 3: // Flash On ?
				break;
			default:
				strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code D%02d"), m_nDCode);
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
		}
		return TRUE;
	case _T('Y'):
		m_bParsingXYFlag = TRUE;
		m_fptCoord.y = GetCoordDataY(&(++(*pFile)));
		if(**pFile == _T('I')) // Y coordinate of line endpoint
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.x = GetCoordDataX(&(++(*pFile)));
		}
		else
			m_fptOpt.x = 0.0;

		if(**pFile == _T('J')) // Y coordinate of line endpoint
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.y = GetCoordDataY(&(++(*pFile)));
		}
		else
			m_fptOpt.y = 0.0;

		if(**pFile == _T('D'))// Exposure
		{
			m_bParsingdCodeFlag = TRUE;
			m_nPrevDCode = m_nDCode;
			m_nDCode = GetIntegerData(&(++(*pFile)));
			switch(m_nDCode)
			{
			case 1: // Exposure On
				break;
			case 2: // Exposure Off
				break;
			case 3: // Flash On ?
				break;
			default:
				strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code D%02d"),m_nDCode);
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::DecodingFunction(char** pFile)
{
	int nData;
	CString strMsg, strErrMsg;

	switch(**pFile)
	{
	case _T('D'):	// fetch numeric code:D-Code
		if(FunctionDCode(pFile))
			return TRUE;
		break;
	case _T('G'):	// fetch numeric code:G-Code		
		if(FunctionGCode(pFile))
		{
			if(**pFile == EOB)
				(*pFile)++;
			return TRUE;
		}
		break;
	case _T('M'):	// fetch numeric code:M-Code		
		if(FunctionMCode(pFile))
			return TRUE;
		break;
	}

	return FALSE;
}

void CFileFormatRs274X::ApertureThermal(char *pTemp)
{
	float fTemp[3], fData, fRotAngle;
	int nData;

	fData = ParseFloatData(&pTemp,TRUE);// $1 X center point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE);// $2 Y center point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE);// $3 outer diameter 
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE);// $4 inner diameter
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE);// $5 Gap thickness
	m_AptPrimi.fParam.push_back(fData);
	fRotAngle = ParseRotationData(&pTemp,TRUE);// $6 Rotationangle around the origin
	m_AptPrimi.fParam.push_back(fRotAngle);

	fTemp[0] = m_AptPrimi.fParam.at(0);
	fTemp[1] = m_AptPrimi.fParam.at(1);
	fTemp[2] = m_AptPrimi.fParam.at(2);
	fData = fTemp[2]/2.0;

	m_AptPrimi.minmax.X1 = fTemp[0] - fData;
	m_AptPrimi.minmax.Y1 = fTemp[1] - fData;
	m_AptPrimi.minmax.X2 = fTemp[0] + fData;
	m_AptPrimi.minmax.Y2 = fTemp[1] + fData;
}

void CFileFormatRs274X::ApertureMoire(char *pTemp)
{
	float fTemp[8], fData, fRotAngle;
	int nData;
	
	fData = ParseFloatData(&pTemp,TRUE); // $1 X center point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $2 Y center point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $3 outer diameter
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $4 ring thickness
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $5 Gap between rings
	m_AptPrimi.fParam.push_back(fData);
	nData = ParseIntegerData(&pTemp,TRUE); // $6 Maximum number of ring
	m_AptPrimi.fParam.push_back((float)nData+0.1);
	fData = ParseFloatData(&pTemp,TRUE); // $7 Cross hair thickness
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $8 Cross hair length
	m_AptPrimi.fParam.push_back(fData);
	fRotAngle = ParseRotationData(&pTemp,TRUE); // $9 Rotationangle around the origin	
	m_AptPrimi.fParam.push_back(fRotAngle);
	
	fTemp[0] = m_AptPrimi.fParam.at(0);
	fTemp[1] = m_AptPrimi.fParam.at(1);
	fTemp[2] = m_AptPrimi.fParam.at(2);
	fTemp[7] = m_AptPrimi.fParam.at(7);
	if(fTemp[2] > fTemp[7])
	{
		fData = fTemp[2]/2.0;
		m_AptPrimi.minmax.X1 = fTemp[0] - fData;
		m_AptPrimi.minmax.Y1 = fTemp[1] - fData;
		m_AptPrimi.minmax.X2 = fTemp[0] + fData;
		m_AptPrimi.minmax.Y2 = fTemp[1] + fData;
	}
	else
	{
		fData = fTemp[7]/2.0;
		m_AptPrimi.minmax.X1 = fTemp[0] - fData;
		m_AptPrimi.minmax.Y1 = fTemp[1] - fData;
		m_AptPrimi.minmax.X2 = fTemp[0] + fData;
		m_AptPrimi.minmax.Y2 = fTemp[1] + fData;
	}
}

void CFileFormatRs274X::AperturePolygon(char *pTemp)
{
	float fTemp[4], fData, fRotAngle;
	int nData;
	
	m_paramAD.bExposureMode = ParseIntegerData(&pTemp,TRUE);// $1 Exposure on/off toggle
	m_AptPrimi.fParam.push_back((float)m_paramAD.bExposureMode + 0.1);
	nData = ParseIntegerData(&pTemp,TRUE);// $2 Number of sides
	m_AptPrimi.fParam.push_back((float)nData+0.1);
	fData = ParseFloatData(&pTemp,TRUE);// $3 X center point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE);// $4 Y center point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE);// $5 outer diamter
	m_AptPrimi.fParam.push_back(fData);
	fRotAngle = ParseRotationData(&pTemp,TRUE);// $6 Rotationangle around the origin
	m_AptPrimi.fParam.push_back(fRotAngle);

	fTemp[2] = m_AptPrimi.fParam.at(2);
	fTemp[3] = m_AptPrimi.fParam.at(3);
	fData = m_AptPrimi.fParam.at(4)/2.0;

	m_AptPrimi.minmax.X1 = fTemp[2] - fData;
	m_AptPrimi.minmax.Y1 = fTemp[3] - fData;
	m_AptPrimi.minmax.X2 = fTemp[2] + fData;
	m_AptPrimi.minmax.Y2 = fTemp[3] + fData;
}

void CFileFormatRs274X::ApertureOutLine(char *pTemp)
{
	int nNumOfSbusequentPoint, i;
	float fData, fRotAngle, cV, sV, xR;
	FPOINTC fptPos;

	m_paramAD.bExposureMode = ParseIntegerData(&pTemp,TRUE);// $1 Exposure on/off toggle
	m_AptPrimi.fParam.push_back((float)m_paramAD.bExposureMode + 0.1);
	nNumOfSbusequentPoint = ParseIntegerData(&pTemp,TRUE); // $2 Num Of Sbusequent Point
	m_AptPrimi.fParam.push_back((float)nNumOfSbusequentPoint*2.0 + 0.1);

	m_AptPrimi.minmax = m_MaxFrameExtent;
	for(i = 0; i < nNumOfSbusequentPoint*2; i++)
	{
		fData = ParseFloatData(&pTemp,TRUE);
		m_AptPrimi.fParam.push_back(fData);

		if(i%2 == 0)
		{
			if(m_AptPrimi.minmax.X1 > fData)	m_AptPrimi.minmax.X1 = fData;
			if(m_AptPrimi.minmax.X2 < fData)	m_AptPrimi.minmax.X2 = fData;
		}
		else
		{
			if(m_AptPrimi.minmax.Y1 > fData)	m_AptPrimi.minmax.Y1 = fData;
			if(m_AptPrimi.minmax.Y2 < fData)	m_AptPrimi.minmax.Y2 = fData;
		}
	}
	
	fData = ParseFloatData(&pTemp,TRUE);		// Dummy Data Start Point
	fData = ParseFloatData(&pTemp,TRUE);		// Dummy Data Start Point

	fRotAngle = ParseRotationData(&pTemp,TRUE);
	m_AptPrimi.fParam.push_back(fRotAngle);

	if(fRotAngle != 0)
	{
		m_AptPrimi.minmax = m_MaxFrameExtent;
		
		GetCosineSineValue(fRotAngle, &cV, &sV);				
		for(i = 2 ; i <= nNumOfSbusequentPoint*2; i+=2)
		{

			xR = m_AptPrimi.fParam.at(i);
			fptPos.y = m_AptPrimi.fParam.at(i+1);
			
			fptPos.x = (xR * cV) - (fptPos.y * sV);
			fptPos.y = (xR * sV) + (fptPos.y * cV);

			if(m_AptPrimi.minmax.X1 > fptPos.x)	m_AptPrimi.minmax.X1 = fptPos.x;
			if(m_AptPrimi.minmax.Y1 > fptPos.y)	m_AptPrimi.minmax.Y1 = fptPos.y;
			if(m_AptPrimi.minmax.X2 < fptPos.x)	m_AptPrimi.minmax.X2 = fptPos.x;
			if(m_AptPrimi.minmax.Y2 < fptPos.y)	m_AptPrimi.minmax.Y2 = fptPos.y;
			
			m_AptPrimi.fParam.at(i) = fptPos.x;
			m_AptPrimi.fParam.at(i+1) = fptPos.y;
		}
	}
}

void CFileFormatRs274X::ApertureLineL(char *pTemp)
{
	float fData, fRotAngle;
	float fTemp[5];

	m_paramAD.bExposureMode = ParseIntegerData(&pTemp,TRUE);// $1 Exposure on/off toggle
	m_AptPrimi.fParam.push_back((float)m_paramAD.bExposureMode + 0.1);
	fData = ParseFloatData(&pTemp,TRUE); // $2 width
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $3 height
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $4 X lower left point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $5 Y lower left point
	m_AptPrimi.fParam.push_back(fData);
	fRotAngle = ParseRotationData(&pTemp,TRUE); // $6 Rotationangle around the origin	
	m_AptPrimi.fParam.push_back(fRotAngle);

	if(m_AptPrimi.fParam.at(5) == 0)
	{
		fTemp[1] = m_AptPrimi.fParam.at(1);
		fTemp[2] = m_AptPrimi.fParam.at(2);
		fTemp[3] = m_AptPrimi.fParam.at(3);
		fTemp[4] = m_AptPrimi.fParam.at(4);

		m_AptPrimi.minmax.X1 = fTemp[3];
		m_AptPrimi.minmax.Y1 = fTemp[4];
		m_AptPrimi.minmax.X2 = fTemp[3] + fTemp[1];
		m_AptPrimi.minmax.Y2 = fTemp[4] + fTemp[2];
	}
	else
	{
		m_AptPrimi.minmax = MinMaxRectLowerLeftGRB(m_AptPrimi.fParam.at(3), m_AptPrimi.fParam.at(4), m_AptPrimi.fParam.at(1), m_AptPrimi.fParam.at(2), m_AptPrimi.fParam.at(5));
	}					
}

void CFileFormatRs274X::ApertureLineC(char *pTemp)
{
	float fData, fRotAngle;
	float fTemp[5];

	m_paramAD.bExposureMode = ParseIntegerData(&pTemp,TRUE);// $1 Exposure on/off toggle
	m_AptPrimi.fParam.push_back((float)m_paramAD.bExposureMode + 0.1);
	fData = ParseFloatData(&pTemp,TRUE); // $2 rectangle width
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $3 rectangle height
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $4 X center point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $5 Y center point
	m_AptPrimi.fParam.push_back(fData);
	fRotAngle = ParseRotationData(&pTemp,TRUE); // $6 Rotationangle around the origin
	m_AptPrimi.fParam.push_back(fRotAngle);
	
	if(m_AptPrimi.fParam.at(5) == 0)
	{
		fTemp[1] = m_AptPrimi.fParam.at(1)/2.0;
		fTemp[2] = m_AptPrimi.fParam.at(2)/2.0;
		fTemp[3] = m_AptPrimi.fParam.at(3);
		fTemp[4] = m_AptPrimi.fParam.at(4);

		m_AptPrimi.minmax.X1 = fTemp[3] - fTemp[1];
		m_AptPrimi.minmax.Y1 = fTemp[4] - fTemp[2];
		m_AptPrimi.minmax.X2 = fTemp[3] + fTemp[1];
		m_AptPrimi.minmax.Y2 = fTemp[4] + fTemp[2];
	}
	else
	{
		m_AptPrimi.minmax = MinMaxRectCenterGRB(m_AptPrimi.fParam.at(3), m_AptPrimi.fParam.at(4), m_AptPrimi.fParam.at(1), m_AptPrimi.fParam.at(2), m_AptPrimi.fParam.at(5));
	}
}

void CFileFormatRs274X::ApertureLineV(char *pTemp)
{
	float fData, fRotAngle;

	m_paramAD.bExposureMode = ParseIntegerData(&pTemp,TRUE);// $1 Exposure on/off toggle
	m_AptPrimi.fParam.push_back((float)m_paramAD.bExposureMode + 0.1);
	fData = ParseFloatData(&pTemp,TRUE); // $2 line width
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $3 X start point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $4 Y start point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $5 X end point
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $6 Y end point
	m_AptPrimi.fParam.push_back(fData);
	fRotAngle = ParseRotationData(&pTemp,TRUE); // Rotationangle around the origin
	m_AptPrimi.fParam.push_back(fRotAngle);

	m_AptPrimi.minmax = MinMaxRectVectorGRB(m_AptPrimi.fParam.at(2), m_AptPrimi.fParam.at(3), m_AptPrimi.fParam.at(4), m_AptPrimi.fParam.at(5), m_AptPrimi.fParam.at(1), m_AptPrimi.fParam.at(6));
}

void CFileFormatRs274X::ApertureCircle(char *pTemp)
{
	float fData;
	float fTemp[4];

	// $1 Exposure on/off toggle
	m_paramAD.bExposureMode = ParseIntegerData(&pTemp,TRUE);// $1 Exposure on/off toggle
	m_AptPrimi.fParam.push_back((float)m_paramAD.bExposureMode + 0.1);
	fData = ParseFloatData(&pTemp,TRUE); // $2 diameter
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $3 X center position
	m_AptPrimi.fParam.push_back(fData);
	fData = ParseFloatData(&pTemp,TRUE); // $4 Y center position
	m_AptPrimi.fParam.push_back(fData);

	fTemp[2] = m_AptPrimi.fParam.at(2);
	fTemp[3] = m_AptPrimi.fParam.at(3);
	fData = m_AptPrimi.fParam.at(1)/2.0;
	m_AptPrimi.minmax.X1 = fTemp[2] - fData;
	m_AptPrimi.minmax.Y1 = fTemp[3] - fData;
	m_AptPrimi.minmax.X2 = fTemp[2] + fData;
	m_AptPrimi.minmax.Y2 = fTemp[3] + fData;
}

BOOL CFileFormatRs274X::FetchAperturePolygon(char** pFile)
{
	float fData;
	int nData;
	CString strErrMsg;

	(*pFile)++;
	fData = ParseFloatData(&(*pFile)); //outer diameter
	m_AptPrimi.fParam.push_back(fData);
	if(**pFile != _T('X'))
	{
		strErrMsg.Format(_T("RS274X parameter block parsing error:AD P,%c"),**pFile);
		AfxMessageBox(strErrMsg);
		m_bLastFrame = TRUE;
		return TRUE;
	}
	else
	{
		nData = ParseIntegerData(&(++(*pFile))); // number of sides
		m_AptPrimi.fParam.push_back((float)nData);
		if(**pFile == _T('X'))
		{
			fData = ParseFloatData(&(++(*pFile))); // degree of rotation
			m_AptPrimi.fParam.push_back(fData);
			if(**pFile == _T('X'))
			{
				fData = ParseFloatData(&(++(*pFile))); // X-axis hole diameter
				m_AptPrimi.fParam.push_back(fData);
				if(**pFile == _T('X'))
				{
					fData = ParseFloatData(&(++(*pFile))); // Y-axis hole diameter
					m_AptPrimi.fParam.push_back(fData);
				}
			}
		}
	}

	if(m_AptPrimi.fParam.size() < 3)
		m_AptPrimi.fParam.push_back(0.0);

	fData = m_AptPrimi.fParam.at(0)/2.0;

	m_AptPrimi.minmax.X1 = -fData;
	m_AptPrimi.minmax.Y1 = -fData;
	m_AptPrimi.minmax.X2 = fData;							
	m_AptPrimi.minmax.Y2 = fData;		

	return FALSE;
}

BOOL CFileFormatRs274X::FetchApertureObround(char** pFile)
{
	float fData;
	float fTemp[2];
	CString strErrMsg;

	(*pFile)++;
	fData = ParseFloatData(&(*pFile)); // X-axis dimension
	m_AptPrimi.fParam.push_back(fData);
	if(**pFile != _T('X'))
	{
		strErrMsg.Format(_T("RS274X parameter block parsing error:AD O,%c"),**pFile);
		AfxMessageBox(strErrMsg);
		m_bLastFrame = TRUE;
		return TRUE;
	}
	else
	{
		fData = ParseFloatData(&(++(*pFile))); // Y-axis dimension
		m_AptPrimi.fParam.push_back(fData);
		if(**pFile == _T('X'))
		{
			fData = ParseFloatData(&(++(*pFile))); // round hole dimension
			m_AptPrimi.fParam.push_back(fData);
			if(**pFile == _T('X'))
			{
				fData = ParseFloatData(&(++(*pFile))); // rectanle hole dimension
				m_AptPrimi.fParam.push_back(fData);
			}
		}
	}

	fTemp[0] = m_AptPrimi.fParam.at(0)/2.0;
	fTemp[1] = m_AptPrimi.fParam.at(1)/2.0;

	m_AptPrimi.minmax.X1 = -fTemp[0];
	m_AptPrimi.minmax.Y1 = -fTemp[1];
	m_AptPrimi.minmax.X2 = fTemp[0];							
	m_AptPrimi.minmax.Y2 = fTemp[1];

	return FALSE;
}

BOOL CFileFormatRs274X::FetchApertureRect(char** pFile)
{
	float fData;
	float fTemp[2];
	CString strErrMsg;

	(*pFile)++;
	fData = ParseFloatData(&(*pFile)); // X-axis dimension
	m_AptPrimi.fParam.push_back(fData);
	if(**pFile != _T('X'))
	{
		strErrMsg.Format(_T("RS274X parameter block parsing error:AD R,%c"),**pFile);
		AfxMessageBox(strErrMsg);
		m_bLastFrame = TRUE;
		return TRUE;
	}
	else
	{
		fData = ParseFloatData(&(++(*pFile))); // Y-axis dimension
		m_AptPrimi.fParam.push_back(fData);
		if(**pFile == _T('X'))
		{
			fData = ParseFloatData(&(++(*pFile))); // round hole dimension
			m_AptPrimi.fParam.push_back(fData);
			if(**pFile == _T('X'))
			{
				fData = ParseFloatData(&(++(*pFile))); // rectanle hole dimension
				m_AptPrimi.fParam.push_back(fData);
			}
		}
	}
	fTemp[0] = m_AptPrimi.fParam.at(0)/2.0;
	fTemp[1] = m_AptPrimi.fParam.at(1)/2.0;
	m_AptPrimi.minmax.X1 = -fTemp[0];
	m_AptPrimi.minmax.Y1 = -fTemp[1];
	m_AptPrimi.minmax.X2 = fTemp[0];							
	m_AptPrimi.minmax.Y2 = fTemp[1];

	return FALSE;
}

BOOL CFileFormatRs274X::FetchApertureCircle(char** pFile)
{
	float fData;

	(*pFile)++;
	fData = ParseFloatData(&(*pFile)); // radius
	m_AptPrimi.fParam.push_back(fData);
	if(**pFile == _T('X'))
	{
		fData = ParseFloatData(&(++(*pFile))); // round hole dimension
		m_AptPrimi.fParam.push_back(fData);

		if(**pFile == _T('X'))
		{
			fData = ParseFloatData(&(++(*pFile))); // round hole dimension
			m_AptPrimi.fParam.push_back(fData);
		}
	}
	fData = m_AptPrimi.fParam.at(0)/2.0;
	m_AptPrimi.minmax.X1 = -fData;
	m_AptPrimi.minmax.Y1 = -fData;
	m_AptPrimi.minmax.X2 = fData;							
	m_AptPrimi.minmax.Y2 = fData;		
	
	return FALSE;
}

BOOL CFileFormatRs274X::ApertureDefinition(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	TCHAR szAMName[MAX_PATH];
	int i, nData;
	CString strErrMsg;
	int nPrimitive, nLength;
	CString strAMContent;
	FRECTC fRect;

	if(**pFile != _T('D')) 
	{
		strErrMsg.Format(_T("RS274X parameter block parsing error:AD%c"),*pFile);
		AfxMessageBox(strErrMsg);
		m_bLastFrame = TRUE;
		return TRUE;
	}
	else
	{
		// fetch numeric code:D-Code
		(*pFile)++;
		i = 0;
		while(_istdigit(**pFile) && i < MAX_PATH)
			szTemp[i++] = *(*pFile)++;
		szTemp[i] = NULL;
		m_nDCode = _ttoi(szTemp);

		if(!CheckDCodeValue(m_nDCode)) // D00, D04-D09 ignore data block
		{
 			strErrMsg.Format(_T("RS274X parameter block parsing error: not allowable decode %d"),m_nDCode);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		i = 0;
		while(**pFile != _T(',') && **pFile != EOB && i < MAX_PATH)
			szAMName[i++] = *(*pFile)++;
		szAMName[i] = NULL;

		if(m_pLayerInfo->mapAptList.Lookup(m_nDCode, m_rAptDef))
		{
			POSITION pos;
			pos = m_pLayerInfo->mapAptList.GetStartPosition();
			while(pos != NULL)
			{
				m_pLayerInfo->mapAptList.GetNextAssoc(pos, nData, m_rAptDef);
				if(nData == m_nDCode)
				{
					m_pLayerInfo->mapAptList.RemoveKey(nData);
					break;
				}
			}
		}
				
				
		m_pAptDef = new APERTURE_DEF;
		m_pAptDef->Param.clear();

		if(i != 1 || (szAMName[0] !=_T('C') && szAMName[0] !=_T('R') && szAMName[0] !=_T('O') && szAMName[0] !=_T('P')))
		{
			m_pAptDef->chStdAptType = NULL;
			m_pAptDef->strAptName = (CString)szAMName;

			// a special aperature described by aperature macro ??? defined previously by an aperature macro
			m_mapAMMacro.Lookup(szAMName, strAMContent);
			if(**pFile == _T(','))
			{
				if(!ParseApertureMacro(&(++(*pFile)),strAMContent))
				{
					strErrMsg.Format(_T("RS274X parameter block parsing error: not allowable Aperture Macro")); // 내용 정리
					AfxMessageBox(strErrMsg);
					m_bLastFrame = TRUE;
					return TRUE;
				}
			}
			else
			{
				while(**pFile == TCHAR(EOB) || **pFile == TCHAR(ASCII_CR) || **pFile == TCHAR(ASCII_LF))
					(*pFile)++; // ignore CR,LF

				// prohibit large memory allocation
				strAMContent.Remove(TCHAR(ASCII_CR)); // ignore CR
				m_nNumOfTextLine += strAMContent.Remove(TCHAR(ASCII_LF)); // ignore LF
			}

			char *pTemp, *pPrevTemp;

#ifdef UNICODE												
			wchar_t *wc;

			wc = strAMContent.GetBuffer(strAMContent.GetLength());
			nLength = WideCharToMultiByte(CP_ACP, 0, wc, -1, NULL, 0, NULL, NULL);
			
			pPrevTemp = pTemp = new char[nLength];
			int mmmm = strAMContent.GetLength();
			WideCharToMultiByte(CP_ACP, 0, wc, -1, pTemp, nLength, 0, 0);		
			strAMContent.ReleaseBuffer();
#else
			pTemp = strAMContent.GetBuffer(strAMContent.GetLength());
			pPrevTemp = pTemp;
#endif
			m_AptPrimi.fParam.clear();
			do{
				nPrimitive = ParseIntegerData(&pTemp);

				switch(nPrimitive)
				{
				case 1: // circle
					ApertureCircle(pTemp);
					break;
				case 2: // line(vector)
				case 20:
					ApertureLineV(pTemp);
					break;
				case 21: // line(center)
					ApertureLineC(pTemp);
					break;
				case 22: // line(lower left)
					ApertureLineL(pTemp);
					break;
				case 4: //  outline
					ApertureOutLine(pTemp);
					break;
				case 5: // polygon
					AperturePolygon(pTemp);
					break;
				case 6: // moire
					ApertureMoire(pTemp);
					break;
				case 7: // thermal
					ApertureThermal(pTemp);
					break;
				default:
					while(*pTemp != EOB && *pTemp != EOF) // unsupported primitive, ignore block
						pTemp++; // ignore to until ','
					;
				}
						
				if(nPrimitive > 0)
				{
					m_AptPrimi.nPriNum = nPrimitive;
					m_pAptDef->Param.push_back(m_AptPrimi);
					m_AptPrimi.fParam.clear();
				}

				if(*pTemp == TCHAR(EOB))
					pTemp++;

			}while(*pTemp != NULL);// && *pTemp > 0);
#ifdef UNICODE
			delete pPrevTemp;
#endif												
			m_pAptDef->minmax = m_MaxFrameExtent;
			for(i = 0 ; i < m_pAptDef->Param.size() ; i++)
			{
				fRect = m_pAptDef->Param.at(i).minmax;
				
				if(m_pAptDef->minmax.X1 > fRect.X1) m_pAptDef->minmax.X1 = fRect.X1;
				if(m_pAptDef->minmax.Y1 > fRect.Y1) m_pAptDef->minmax.Y1 = fRect.Y1;
				if(m_pAptDef->minmax.X2 < fRect.X2) m_pAptDef->minmax.X2 = fRect.X2;
				if(m_pAptDef->minmax.Y2 < fRect.Y2) m_pAptDef->minmax.Y2 = fRect.Y2;
			}

			m_pLayerInfo->mapAptList.SetAt(m_nDCode, m_pAptDef);	
			m_nNumOfAperture++;
			
		}
		else
		{
			m_pAptDef->chStdAptType = szAMName[0];
			// fetch aperture type
			switch(szAMName[0])
			{
			case _T('C'):	// circle
				if(FetchApertureCircle(pFile))
					return TRUE;
				break;
			case _T('R'):	// rectangle or square
				if(FetchApertureRect(pFile))
					return TRUE;
				break;
			case _T('O'):	// oval, obround
				if(FetchApertureObround(pFile))
					return TRUE;
				break;
			case _T('P'):	// regular polygon
				if(FetchAperturePolygon(pFile))
					return TRUE;
				break;
			default:
				;
			}

			m_AptPrimi.nPriNum = 0;
			m_pAptDef->minmax = m_AptPrimi.minmax;
			m_pAptDef->Param.push_back(m_AptPrimi);
			m_AptPrimi.fParam.clear();
			
			m_pLayerInfo->mapAptList.SetAt(m_nDCode, m_pAptDef);
			m_nNumOfAperture++;
		}
		if(**pFile == EOB)
			(*pFile)++;

		return TRUE;
	}
			
	return FALSE;
}

BOOL CFileFormatRs274X::ParameterAperture(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	TCHAR szAMName[MAX_PATH];
	int i, nData, nLength;
	CString strAMContent;
	CString strErrMsg;

	if(**pFile == _T('A'))
	{
		(*pFile)++;
		switch(**pFile)
		{
		case _T('A'):	// Aperture Assignment - ignored, vector photoplotter command
		case _T('F'):	// Auto Focus - ignored, vector photoplotter command
		case _T('P'):	// Aperture Offset - ignored, vector photoplotter command
		case _T('R'):	// Aperture Record - ignored, vector photoplotter command
		case _T('V'):	// Aperture vVelocity - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		case _T('S'):	// Axis select - warning, should not impact appearance
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)
				szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		case _T('D'):	// Aperature definition , %ADD<D-code number><aperture type>,<modifier>[X<modifier>]*%
			(*pFile)++;
			if(ApertureDefinition(pFile))
				return TRUE;
			break;
		case _T('M'):	// Aperature macro
			(*pFile)++;
			i = 0;
			while(**pFile != EOB && i < MAX_PATH)
				szAMName[i++] = *(*pFile)++;
			szAMName[i] = NULL;
			
			if(**pFile == EOB)
				(*pFile)++; // LF,'*'
			while(**pFile == TCHAR(ASCII_LF) || **pFile == TCHAR(ASCII_CR))
				(*pFile)++; // ignore CR,LF

			nLength = strpbrk(*pFile, "%") - *pFile;
			
			char *pAMParam;
			pAMParam = new char[nLength+1];

			strncpy(pAMParam, *pFile, nLength);
//#ifdef UNDER_VS2008
//			strncpy(pAMParam, *pFile, nLength);
//#else
//			strncpy_s(pAMParam,nLength+1, pFile, nLength);
//#endif
			*(pAMParam+nLength) = NULL;
			strAMContent = (CString)pAMParam;
			delete []pAMParam;
			m_mapAMMacro.SetAt(szAMName, strAMContent);

			(*pFile) += nLength;

			break;

		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:A%c"),*pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}
	
		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterBkMode(char** pFile)
{
	TCHAR szAMName[MAX_PATH];
	int i;
	CString strErrMsg;

	if(**pFile == _T('B'))
	{
		(*pFile)++;

		if(**pFile == _T('G')) // Background Mode - warning, should not impact appearance
		{
			(*pFile)++;
			i = 0;
			while(**pFile != EOB && i < MAX_PATH) szAMName[i++] = *(*pFile)++;
			szAMName[i] = NULL;
			return TRUE;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:A%c"),**pFile);
			m_bLastFrame = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterFS(char** pFile)
{
	int peRepeat;
	int nLN, i, nCoord;
	int	 nDataFormatX, nDataFormatY;
	CString strErrMsg;

	nLN = 0;

	if(**pFile == _T('F'))
	{
		(*pFile)++;

		if(**pFile == _T('S'))
		{
			if(nLN > 0)
			{
				if((m_paramMI.bMirrorX || m_paramMI.bMirrorY) && m_nStartMirrorObj == 0)
					InitMaxLayerExtent();

				if(m_paramMI.bMirrorX)
				{
					m_nEndMirrorObj = m_pLayerInfo->listObj.size();							
					for(i = m_nStartMirrorObj ; i < m_nEndMirrorObj; i++)
					{
						if(i == m_nEndMirrorObj - 1)
							nCoord = m_pLayerInfo->vecObjCoord.size() - m_pLayerInfo->listObj[i].nStPnt;
						else
							nCoord = m_pLayerInfo->listObj[i+1].nStPnt - m_pLayerInfo->listObj[i].nStPnt;

						MirrorVerticalObject(m_pLayerInfo,&m_pLayerInfo->listObj.at(i),nCoord, 0.0);
						SetMaxLayerExtent(m_pLayerInfo->listObj.at(i).Extent);
					}

					m_paramMI.bMirrorX = FALSE;
				}
				if(m_paramMI.bMirrorY)
				{
					m_nEndMirrorObj = m_pLayerInfo->listObj.size();							
					for(i = m_nStartMirrorObj ; i < m_nEndMirrorObj; i++)
					{
						if(i == m_nEndMirrorObj - 1)
							nCoord = m_pLayerInfo->vecObjCoord.size() - m_pLayerInfo->listObj.at(i).nStPnt;
						else
							nCoord = m_pLayerInfo->listObj.at(i+1).nStPnt - m_pLayerInfo->listObj.at(i).nStPnt;

						MirrorHorizontalObject(m_pLayerInfo,&m_pLayerInfo->listObj.at(i),nCoord, 0.0);
						SetMaxLayerExtent(m_pLayerInfo->listObj.at(i).Extent);
					}

					m_paramMI.bMirrorY = FALSE;
				}
				m_nStartMirrorObj = 0;

				StepAndRepeating(m_MnMxSnR);

				m_paramSR.nXRepeat = 0;
				m_paramSR.nYRepeat = 0;
				m_paramSR.psRepeat = 0;
				peRepeat = 0;
				m_xStep = 0;
				m_yStep = 0;

				m_mapAMMacro.RemoveAll();

				// Add Dummy Object
				m_gObj.Type.nType = DRAW_TYPE::FLASH;
				m_gObj.Type.nAttr = DRAW_TYPE::NONE;
				m_gObj.nStPnt = m_nNumOfObjCoord;
				
				m_pLayerInfo->listObj.push_back(m_gObj);
				// <===================================


				//m_nNumOfLineObject += nNumOfLineObject;

				m_nNumOfLineTemplate = m_vecLineTemplate.size();
				if(m_nNumOfLineTemplate>0)
				{
					sort(m_vecLineTemplate.begin(), m_vecLineTemplate.end());
					m_vecTemp.clear();
					std::unique_copy(m_vecLineTemplate.begin(),m_vecLineTemplate.end(),std::back_inserter(m_vecTemp));
					(m_vecLineTemplate).swap(m_vecTemp);
					m_nNumOfLineTemplate = m_vecLineTemplate.size();
				}
				m_vecTemp.clear();


				m_pLayerInfo->Extent = GetMaxLayerExtent();
				m_pLayerInfo->bUnit = m_Unit;
				m_pLayerInfo->nLayerGroupID = m_nLayerGroupID;

				if(m_nIRAngle != 0)
				{
					double cX, cY;

					cX = (m_pLayerInfo->Extent.X1 + m_pLayerInfo->Extent.X2)/2.0;
					cY = (m_pLayerInfo->Extent.X1 + m_pLayerInfo->Extent.X2)/2.0;

					ApplyIROptionGRB(m_pLayerInfo, cX, cY, -m_nIRAngle);
				}
				
				ClearObjectData(m_gObj);	
				m_ArLayerInform.Add(m_pLayerInfo);
			
				m_nNumOfObjCoord = 0;
				m_gObj.nStPnt = 0;
				m_pLayerInfo = new LAYER_INFORM_RS274X;
				m_nLayer++;
				if(m_nLayer == 14)
					m_nLayer = 14;
				m_nNumOfObject = m_ArLayerStruct.GetAt(m_nLayer).nObject;
				m_nNumOfCoord = m_ArLayerStruct.GetAt(m_nLayer).nCoord;
				m_pLayerInfo->listObj.reserve(m_nNumOfObject+1); 
				i = m_pLayerInfo->listObj.capacity();

				m_pLayerInfo->nFormat = CAM_DATA::RS274X;
				m_pLayerInfo->strFilePath = m_sFilePath;
				m_pLayerInfo->vecObjCoord.reserve(m_nNumOfCoord);

				m_nObj = 0;
				m_fLayerScaleX = 1.0;
				m_fLayerScaleY = 1.0;
				InitMaxLayerExtent();
			}
			(*pFile)++;
			if(*(*pFile)++ == _T('L'))// 'L' or 'T':  Use L to leading zeros, Use T to omit trailing zeros
				m_paramFS.bSuppress = FALSE; //  'Leading Zero omitted
			else
				m_paramFS.bSuppress = TRUE;

			if(*(*pFile)++ == _T('A')) // 'A' or 'I':  Use A for absolute coordinate values, Use I for incremental coordinate values
				m_paramFS.bAbsolute = TRUE;
			else
				m_paramFS.bAbsolute = FALSE;

			if(*(*pFile) == _T('X'))
			{
				nDataFormatX = ParseIntegerData(&(++(*pFile))); // Xnm Enter X the number of n integer and m decimal places in the coordinate data for X axis 

				m_lDataFormatX = ldiv(nDataFormatX, 10);
				m_fDividerX = pow(10.0,m_lDataFormatX.rem);
				m_optionFormat.Format(_T("%d%d"),m_lDataFormatX.quot,m_lDataFormatX.rem);
			}
										
			if(*(*pFile) == _T('Y'))
			{
				nDataFormatY =  ParseIntegerData(&(++(*pFile))); // Ynm Enter Y the number of n integer and m decimal places in the coordinate data for Y axis 
				m_lDataFormatY = ldiv(nDataFormatY, 10);
				m_fDividerY = pow(10.0,m_lDataFormatY.rem);
				m_optionFormat.Format(_T("%d%d"),m_lDataFormatY.quot,m_lDataFormatY.rem);
			}

			if(**pFile == EOB) (*pFile)++;

			return TRUE;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:F%c"),*pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}		
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterDashLine(char** pFile)
{
	CString strErrMsg;

	if(**pFile == _T('D'))
	{
		(*pFile)++;
		if(**pFile == _T('L')) // Dash Line specification, ignored, vector photoplotter command
		{
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			return TRUE;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:A%c"),*pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterI(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	CString strErrMsg;
	int i, nData;
	float fData;

	if(**pFile == _T('I'))
	{
		(*pFile)++;

		switch(**pFile)
		{
		case _T('C'): // Input Code - warning, should not impact appearance
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH) szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		case _T('D'): // Input Display - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		case _T('F'): // Include File
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			strErrMsg.Format(_T("RS274X parameter block parsing error: IF command Not supported by gigavis"));
			AfxMessageBox(strErrMsg);
			break;
		case _T('J'): // Image Justify - warning, should not impact appearance
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)	szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		case _T('N'): // Image Name - warning, should not impact appearance
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH) szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		case _T('O'): // Image Offset - warning, should not impact appearance
			(*pFile)++;
			if(**pFile == _T('A'))
				fData = ParseFloatData(&(++(*pFile)),TRUE);
			if(**pFile == _T('B'))
				fData = ParseFloatData(&(++(*pFile)),TRUE);
			break;
		case _T('P'): // Image polarity supported if SF = scale factors in A and B must be 1.0 and MI = mirror values A and B must be 0
			(*pFile)++;
			szTemp[0] = *(*pFile)++;
			szTemp[1] = *(*pFile)++;
			szTemp[2] = *(*pFile)++;
			szTemp[3] = NULL;
			if(!_tcsncmp(szTemp,_T("POS"),3))
			{
				m_paramIP.PolChangeFlag = FALSE;
				m_paramIP.bLayerPolarity = LAYER_TYPE::POSITIVE;
				nData = 0; // Positive
			}
			else
			{
				m_paramIP.PolChangeFlag = TRUE;
				m_paramIP.bLayerPolarity = LAYER_TYPE::NEGATIVE;
				nData = 1; // Negative	
			}
			break;
		case _T('R'): // Image rotation
			m_nIRAngle = ParseIntegerData(&(++(*pFile)));
			break;
		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:I%c"),**pFile);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		if(**pFile == EOB)
			(*pFile)++;

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterKnockOut(char** pFile)
{
	CString strErrMsg;

	if(**pFile == _T('K')) // KnockOut - supported if SF = scale factors in A and B must be 1.0 and MI = mirror values A and B must be 0
	{
		(*pFile)++;

		if(**pFile == _T('O'))	// Knockout
		{
			(*pFile)++;
			return TRUE;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:K%c"),*pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterRotate(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	CString strErrMsg;
	int i;

	if(**pFile == _T('R'))
	{
		(*pFile)++;

		switch(**pFile)
		{
		case _T('C'): // Rotate Symbol - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		case _T('O'): // Rotate Position Data - supported if SF = scale factors in A and B must be 1.0 and MI = mirror values A and B must be 0
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)
				szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:R%c"),*pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterTranslation(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	CString strErrMsg;
	int i;

	if(**pFile == _T('T'))
	{
		(*pFile)++;

		if(**pFile == _T('R')) // Translation - ignored, vector photoplotter command
		{
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:T%c"),*pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterWindowSpec(char** pFile)
{
	CString strErrMsg;

	if(**pFile == _T('W'))
	{
		(*pFile)++;
		
		if(**pFile == _T('I')) // Window Specification - ignored, vector photoplotter command
		{
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:W%c"),**pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterVelocity(char** pFile)
{
	CString strErrMsg;

	if(**pFile == _T('V'))
	{
		(*pFile)++;

		if(**pFile == _T('L')) // Velocity Limit - ignored, vector photoplotter command
		{
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:V%c"),**pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterSequence(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	CString strErrMsg;
	int i;

	if(**pFile == _T('N'))
	{
		(*pFile)++;
		if(**pFile == _T('F') || **pFile == _T('S')) // Sequence Number - warning, should not impact appearance
		{
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)
				szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;

			return TRUE;
		}
		else
		{
			strErrMsg.Format(_T("RS274X parameter block parsing error:M%c"),*pFile);
			m_bLastFrame = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterL(char** pFile)
{
	CHAR szLayerName[MAX_PATH];
	CString strErrMsg;
	int i, nLN = 0;
	BOOL bCheckLNFlag;

	if(**pFile == _T('L'))
	{
		(*pFile)++;

		switch(**pFile)
		{
		case _T('N'):	// Layer name
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)
				szLayerName[i++] = *(*pFile)++;
			szLayerName[i] = NULL;

			m_pLayerInfo->strName = (CString)szLayerName;

			bCheckLNFlag = TRUE;
			nLN += 1000;		

			(*pFile)++;
			break;
		case _T('P'):	// Layer polarity
			TCHAR chLayerPolarity;
			(*pFile)++;
			chLayerPolarity = *(*pFile)++;

			if (m_paramIP.PolChangeFlag) //   '이미지 극성 변환
			{
				if(chLayerPolarity == _T('D'))
					m_paramIP.bLayerPolarity = LAYER_TYPE::NEGATIVE; 
				else 
					m_paramIP.bLayerPolarity = LAYER_TYPE::POSITIVE;
			}
			else
			{
				if(chLayerPolarity == _T('D')) 
					m_paramIP.bLayerPolarity = LAYER_TYPE::POSITIVE; 
				else 
					m_paramIP.bLayerPolarity = LAYER_TYPE::NEGATIVE;
			}

			if(**pFile == EOB)
				(*pFile)++;
			else
			{
				strErrMsg.Format(_T("RS274X parameter block parsing error:LP%c%c"),chLayerPolarity,**pFile);
				m_bLastFrame = TRUE;
				return TRUE;
			}
			break;
		case _T('S'): // Load Symbol - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:L%c"),**pFile);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterM(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	CString strErrMsg;
	int i, nCoord;
					
	if(**pFile == _T('M'))
	{
		(*pFile)++;
		switch(**pFile)
		{
		case _T('I'):	// Mirror image
			if((m_paramMI.bMirrorX || m_paramMI.bMirrorY) && m_nStartMirrorObj == 0)
				InitMaxLayerExtent();

			if(m_paramMI.bMirrorX)
			{
				m_nEndMirrorObj = m_pLayerInfo->listObj.size();							
				for(i = m_nStartMirrorObj ; i < m_nEndMirrorObj; i++)
				{
					if(i == m_nEndMirrorObj - 1)
						nCoord = m_pLayerInfo->vecObjCoord.size() - m_pLayerInfo->listObj[i].nStPnt;
					else
						nCoord = m_pLayerInfo->listObj[i+1].nStPnt - m_pLayerInfo->listObj[i].nStPnt;

					MirrorVerticalObject(m_pLayerInfo,&m_pLayerInfo->listObj.at(i),nCoord, 0.0);
					SetMaxLayerExtent(m_pLayerInfo->listObj.at(i).Extent);
				}

				m_paramMI.bMirrorX = FALSE;
			}
			if(m_paramMI.bMirrorY)
			{
				m_nEndMirrorObj = m_pLayerInfo->listObj.size();							
				for(i = m_nStartMirrorObj ; i < m_nEndMirrorObj; i++)
				{
					if(i == m_nEndMirrorObj - 1)
						nCoord = m_pLayerInfo->vecObjCoord.size() - m_pLayerInfo->listObj.at(i).nStPnt;
					else
						nCoord = m_pLayerInfo->listObj.at(i+1).nStPnt - m_pLayerInfo->listObj.at(i).nStPnt;

					MirrorHorizontalObject(m_pLayerInfo,&m_pLayerInfo->listObj.at(i),nCoord, 0.0);
					SetMaxLayerExtent(m_pLayerInfo->listObj.at(i).Extent);
				}

				m_paramMI.bMirrorY = FALSE;
			}
			m_nStartMirrorObj = 0;

			(*pFile)++;
			if(**pFile == _T('A')) // 'A' or 'B':  Use A0 to disable mirroring, A1 to invert the A-axis. The image will be flipped over tbe B-axis
			{
				(*pFile)++;
				if(*(*pFile)++ == _T('0'))
					m_paramMI.bMirrorX = FALSE;
				else
				{
					m_paramMI.bMirrorX = TRUE;
 					m_nStartMirrorObj = m_nObj;
				}
			}
										
			if(**pFile == _T('B'))// 'A' or 'B':  Use A0 to disable mirroring, A1 to invert the A-axis. The image will be flipped over tbe B-axis
			{
				(*pFile)++;
				if(*(*pFile)++ == _T('0'))
					m_paramMI.bMirrorY = FALSE;
				else
				{
					m_paramMI.bMirrorY = TRUE;
					m_nStartMirrorObj = m_nObj;
				}
			}						
			break;
		case _T('O'):	// Mode(Inch or millimiter units)
			(*pFile)++;
			szTemp[0] = *(*pFile)++;
			szTemp[1] = *(*pFile)++;
			szTemp[2] = NULL;
			if(!_tcsncmp(szTemp,_T("IN"),2))
				m_Unit = LAYER_TYPE::INCH;
			else
				m_Unit = LAYER_TYPE::METRIC; // millimeter
			break;
		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:M%c"),*pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		if(**pFile == EOB)
			(*pFile)++;

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterO(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	CString strErrMsg;
	int i;
	float fData=0.0;

	if(**pFile == _T('O'))
	{
		(*pFile)++;
		switch(**pFile)
		{
		case _T('F'):	// Offset - warning, should not impact appearance
			(*pFile)++;
			if(**pFile == _T('A'))
			{
				fData = ParseFloatData(&(++(*pFile)),TRUE); // the offset along the output device A axis
			}

			if(**pFile == _T('B'))
			{
				fData = ParseFloatData(&(++(*pFile)),TRUE); // the offset along the output device B axis
			}
			break;
		case _T('P'): // Option Stop - supported if SF = scale factors in A and B must be 1.0 and MI = mirror values A and B must be 0
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)
				szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:O%c"),**pFile);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterP(char** pFile)
{
	TCHAR szTemp[MAX_PATH];
	CString strErrMsg;
	int i;
	float fData=0.0;

	if(**pFile == _T('P'))
	{
		(*pFile)++;
		switch(**pFile)
		{
		case _T('D'): // Plotter Destination - warning, should not impact appearance
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)
				szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		case _T('E'): // Perspective - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		case _T('F'): // Film Type - warning, should not impact appearance
			i = 0;
			(*pFile)++;
			while(**pFile != EOB && i < MAX_PATH)
				szTemp[i++] = *(*pFile)++;
			szTemp[i] = NULL;
			break;
		case _T('K'): // Park - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		case _T('O'): // Pen Offset - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:P%c"),**pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::ParameterS(char** pFile)
{
	CString strErrMsg;
	int nData, peRepeat, i;
	float fData;

	if(**pFile == _T('S'))
	{
		(*pFile)++;
		switch(**pFile)
		{
		case _T('C'): // Single Step mode - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		case _T('F'):	// Scale factor
			(*pFile)++;
			if(**pFile == _T('A'))
			{
				fData = ParseScaleData(&(++(*pFile)),TRUE); // the offset along the output device A axis
 				m_pLayerInfo->fScaleX = fData;
			}

			if(**pFile == _T('B'))
			{
				fData = ParseScaleData(&(++(*pFile)),TRUE); // the offset along the output device B axis
				m_pLayerInfo->fScaleY = fData;
			}

			m_fLayerScaleX = m_pLayerInfo->fScaleX;
			m_fLayerScaleY = m_pLayerInfo->fScaleY;
			break;
		case _T('M'):	// Symbol Mirroring
			(*pFile)++;
			if(**pFile == _T('A'))
			{
				nData = ParseIntegerData(&(++(*pFile))); // the offset along the output device A axis
			}
			if(**pFile == _T('B'))
			{
				nData = ParseIntegerData(&(++(*pFile))); // the offset along the output device B axis
			}
			break;

		case _T('R'):	// Step and Repeat
			if (m_paramSR.nXRepeat > 1 || m_paramSR.nYRepeat > 1)
			{
				FPOINTC fPt;
				peRepeat = m_pLayerInfo->listObj.size() - 1 ;

				ClearObjectData(m_gObj);

				m_MnMxSnR = m_MaxFrameExtent;
				for (i = m_paramSR.psRepeat ; i<=peRepeat ; i++)
				{
					m_pTrUtil->AdjustExtentByRect(m_MnMxSnR, m_pLayerInfo->listObj.at(i).Extent);
				}

				m_gObj.dCode = 0;	
				m_gObj.Type.nType = DRAW_TYPE::SNR;
    
				m_gObj.Extent.Y1 = m_MnMxSnR.Y1;
				m_gObj.Extent.Y2 = m_MnMxSnR.Y2; 
				m_gObj.Extent.X1 = m_MnMxSnR.X1;
				m_gObj.Extent.X2 = m_MnMxSnR.X2;

				fPt.x = m_paramSR.psRepeat;
				fPt.y = peRepeat;
				m_pLayerInfo->vecObjCoord.push_back(fPt);
				m_nNumOfObjCoord++;

				fPt.x = m_paramSR.nXRepeat;
				fPt.y = m_paramSR.nYRepeat;
				m_pLayerInfo->vecObjCoord.push_back(fPt);
				m_nNumOfObjCoord++;
    
				fPt.x = m_xStep;
				fPt.y = m_yStep;
				m_pLayerInfo->vecObjCoord.push_back(fPt);
				m_nNumOfObjCoord++;
				
				m_MnMxSnR = GetMaxLayerExtent();
				if((m_paramSR.nXRepeat - 1) * m_xStep > 0)
				{
					if (m_gObj.Extent.X1 < m_MnMxSnR.X1) m_MnMxSnR.X1 = m_gObj.Extent.X1;
					if (m_gObj.Extent.X2 + (m_paramSR.nXRepeat - 1) * m_xStep > m_MnMxSnR.X2) m_MnMxSnR.X2 = m_gObj.Extent.X2   + (m_paramSR.nXRepeat - 1) * m_xStep;
				}
				else
				{
					if (m_gObj.Extent.X1 + (m_paramSR.nXRepeat - 1) * m_xStep < m_MnMxSnR.X1) m_MnMxSnR.X1 = m_gObj.Extent.X1 + (m_paramSR.nXRepeat - 1) * m_xStep;
					if (m_gObj.Extent.X2 > m_MnMxSnR.X2) m_MnMxSnR.X2 = m_gObj.Extent.X2;
				}
				
				if((m_paramSR.nYRepeat - 1) * m_yStep > 0)
				{
					if (m_gObj.Extent.Y1 < m_MnMxSnR.Y1 ) m_MnMxSnR.Y1 = m_gObj.Extent.Y1;
					if (m_gObj.Extent.Y2 + (m_paramSR.nYRepeat - 1) * m_yStep > m_MnMxSnR.Y2 ) m_MnMxSnR.Y2 = m_gObj.Extent.Y2 + (m_paramSR.nYRepeat - 1) * m_yStep;
				}
				else
				{
					if (m_gObj.Extent.Y1 + (m_paramSR.nYRepeat - 1) * m_yStep < m_MnMxSnR.Y1 ) m_MnMxSnR.Y1 = m_gObj.Extent.Y1 + (m_paramSR.nYRepeat - 1) * m_yStep;
					if (m_gObj.Extent.Y2 > m_MnMxSnR.Y2 ) m_MnMxSnR.Y2 = m_gObj.Extent.Y2;
				}

				SetMaxLayerExtent(m_MnMxSnR);
				
				m_gObj.Type.nAttr = DRAW_TYPE::PATTERN;
				m_pLayerInfo->listObj.push_back(m_gObj);

				m_nNumOfSnRObject++;

				ClearObjectData(m_gObj);
				m_gObj.nStPnt = m_nNumOfObjCoord;

				m_nObj++;
			}                    
			m_paramSR.psRepeat = m_nObj;

			(*pFile)++;
			if(**pFile != _T('X'))
			{
				strErrMsg.Format(_T("274X file parsing error: X not found in SR")); 
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
			m_paramSR.nXRepeat = ParseIntegerData(&(++(*pFile))); // The number of times the data is repeated along X-axis
											
			if(**pFile != _T('Y'))
			{
				strErrMsg.Format(_T("274X file parsing error: Y not found in SR"));
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
			m_paramSR.nYRepeat = ParseIntegerData(&(++(*pFile))); // The number of times the data is repeated along Y-axis

			if(**pFile != _T('I'))
			{
				strErrMsg.Format(_T("274X file parsing error: I not found in SR"));
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
			m_xStep = ParseFloatData(&(++(*pFile))); // The step between the repeats along X-axis
			m_xStep *= m_fLayerScaleX;

			if(**pFile != _T('J'))
			{
				strErrMsg.Format(_T("274X file parsing error: J not found in SR"));
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
			m_yStep = ParseFloatData(&(++(*pFile))); // The step between the repeats along X-axis
			m_yStep *= m_fLayerScaleY;

			break;

		case _T('S'): // Single Step mode - ignored, vector photoplotter command
			(*pFile)++;
			while(**pFile != EOB) (*pFile)++;
			break;
		default:
			strErrMsg.Format(_T("RS274X parameter block parsing error:S%c"),**pFile);
			AfxMessageBox(strErrMsg);
			m_bLastFrame = TRUE;
			return TRUE;
		}
		if(**pFile == EOB)
			(*pFile)++;

		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::DecodingParameter(char** pFile)
{
	CString strErrMsg;

	if(**pFile == _T('%'))
	{
		(*pFile)++;

		while(**pFile != _T('%'))
		{
			if(ParameterFS(pFile))
				continue;
			if(ParameterAperture(pFile))
				continue;
			if(ParameterBkMode(pFile))
				continue;
			if(ParameterDashLine(pFile))
				continue;
			if(ParameterKnockOut(pFile))
				continue;
			if(ParameterSequence(pFile))
				continue;
			if(ParameterRotate(pFile))
				continue;
			if(ParameterTranslation(pFile))
				continue;
			if(ParameterVelocity(pFile))
				continue;
			if(ParameterWindowSpec(pFile))
				continue;
			if(ParameterI(pFile))
				continue;
			if(ParameterL(pFile))
				continue;
			if(ParameterM(pFile))
				continue;
			if(ParameterO(pFile))
				continue;
			if(ParameterP(pFile))
				continue;
			if(ParameterS(pFile))
				continue;

			if(**pFile == EOB)
				(*pFile)++;
			else if(**pFile == TCHAR(ASCII_CR))
				(*pFile)++;
			else if(**pFile == TCHAR(ASCII_LF))
			{
				(*pFile)++;
				m_nNumOfTextLine++;
			}
			else
			{
				strErrMsg.Format(_T("RS274X parameter block parsing error:%c"),**pFile);
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
		}

		(*pFile)++;  // erase end '%'
		return TRUE;
	}

	return FALSE;
}

BOOL CFileFormatRs274X::FunctionDCode(char** pFile)
{
	int nData;
	CString strMsg, strErrMsg;

	// fetch numeric code:D-Code
	nData = GetIntegerData(&(++(*pFile)));

	switch(nData)
	{
	case 1: // Exposure and Draw mode on
		m_bParsingXYFlag = TRUE; 
		m_bParsingdCodeFlag = TRUE;
		m_fptCoord = m_fptPrevPos;
		m_fptOpt = FPOINTC(0.0,0.0);
		m_nPrevDCode = m_nDCode;
		m_nDCode = nData;
		break;
	case 2: // Exposure and Draw mode off
		m_bParsingXYFlag = TRUE; 
		m_bParsingdCodeFlag = TRUE;
		m_fptCoord = m_fptPrevPos;
		m_fptOpt = FPOINTC(0.0,0.0);
		m_nPrevDCode = m_nDCode;
		m_nDCode = nData;
		break;
	case 3: // Set flash mode
		m_bParsingXYFlag = TRUE; 
		m_bParsingdCodeFlag = TRUE;
		m_fptCoord = m_fptPrevPos;
		m_fptOpt = FPOINTC(0.0,0.0);
		m_nPrevDCode = m_nDCode;
		m_nDCode = nData;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 0:
		while(**pFile != EOB)  // D00, D04-D09 ignore data block
			(*pFile)++;
		break;
	default: // Select an aperture defined by an AD parameter
		if(m_nDCode < 0 || m_nDCode > 999)
		{
			strMsg.Format(_T("Unsupported D-Code:%d"),m_nDCode);
			AfxMessageBox(strMsg,MB_OK);
		}
		else
		{
			m_nADCode = nData;
			m_pLayerInfo->mapAptList.Lookup(m_nADCode, m_rAptDef);
			m_CurMinMax = m_rAptDef->minmax;
		}
		break;
	}

	return TRUE;
}

BOOL CFileFormatRs274X::FunctionGCode(char** pFile)
{
	int nData;
	CString strMsg, strErrMsg;
	BOOL bRtn = FALSE;

	m_nGCode = GetIntegerData(&(++(*pFile)));
	switch(m_nGCode)
	{
	case 1: // G01(G1) Linear Interpolation
		if(**pFile == _T('X')) // X coordinate of line endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.x = GetCoordDataX(&(++(*pFile)));
		}
		if(**pFile == _T('Y')) // Y coordinate of line endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.y = GetCoordDataY(&(++(*pFile)));
		}
		if(**pFile == _T('D'))// Exposure
		{
			m_bParsingdCodeFlag = TRUE;
			m_nPrevDCode = m_nDCode;
			m_nDCode = GetIntegerData(&(++(*pFile)));
			switch(m_nDCode)
			{
			case 1: // Exposure On
				break;
			case 2: // Exposure Off
				break;
			case 3: // Flash On ?
				break;
			default:
				strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code D%02d"), m_nDCode);
				AfxMessageBox(strMsg,MB_OK);
				m_bLastFrame = TRUE;
				return TRUE;
			}
		}
		break;
	case 2: // Clockwise circular Interpolation : Single Quadrant mode
		m_bParsingGCodeFlag = TRUE;

		if(**pFile == _T('X')) // X coordinate of arc endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.x = GetCoordDataX(&(++(*pFile)));
		}
		if(**pFile == _T('Y')) // Y coordinate of arc endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.y = GetCoordDataY(&(++(*pFile)));
		}
		if(**pFile == _T('I'))	// The offset between the arc start point and the center paralled to the X aixs. The value is always positive.
								// A sign is not allowed. The direction to the center is determined implicitly.
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.x = GetCoordDataX(&(++(*pFile)));
		}
		else
			m_fptOpt.x = 0.0;

		if(**pFile == _T('J')) // The offset between the arc start point and the center paralled to the Y aixs. The value is always positive.
								// A sign is not allowed. The direction to the center is determined implicitly.
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.y = GetCoordDataY(&(++(*pFile)));
		}
		else
			m_fptOpt.y = 0.0;

		if(**pFile == _T('D')) // Exposure
		{
			m_bParsingdCodeFlag = TRUE;
			m_nPrevDCode = m_nDCode;
			m_nDCode = GetIntegerData(&(++(*pFile)));
			switch(m_nDCode)
			{
			case 1: // Exposure On
				break;
			case 2: // Exposure Off
				break;
			case 3: // Flash On ?
				break;
			default:
				strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code D%02d"),m_nDCode);
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
		}
		break;
	case 3: // Counterclockwise circular Interpolation : Single Quadrant mode
		m_bParsingGCodeFlag = TRUE;

		if(**pFile == _T('X')) // X coordinate of arc endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.x = GetCoordDataX(&(++(*pFile)));
		}
		if(**pFile == _T('Y')) // Y coordinate of arc endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.y = GetCoordDataY(&(++(*pFile)));
		}
		if(**pFile == _T('I'))	// The offset between the arc start point and the center paralled to the X aixs. The value is always positive.
								// A sign is not allowed. The direction to the center is determined implicitly.
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.x = GetCoordDataX(&(++(*pFile)));
		}
		else
			m_fptOpt.x = 0.0;
		
		if(**pFile == _T('J')) // The offset between the arc start point and the center paralled to the Y aixs. The value is always positive.
								// A sign is not allowed. The direction to the center is determined implicitly.
		{
			m_bParsingIJFlag = TRUE;
			m_fptOpt.y = GetCoordDataY(&(++(*pFile)));
		}
		else
			m_fptOpt.y = 0.0;

		if(**pFile == _T('D')) // Exposure
		{
			m_bParsingdCodeFlag = TRUE;
			m_nPrevDCode = m_nDCode;
			m_nDCode = GetIntegerData(&(++(*pFile)));
			switch(m_nDCode)
			{
			case 1: // Exposure On
				break;
			case 2: // Exposure Off
				break;
			case 3: // Flash On ?
				break;
			default:
				strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code D%02d"),m_nDCode);
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
		}
		break;
	// The G74,G75 command allow switching between the two modes. A data block becomming G75 enables multi quadrent mode.
	// Every block following it will be interpolated as multi quadrant. until cancelled by a G74
	// A data block containing G74 code turns off multi quadrant mode. reverting to single quadrant mode.
	case 74: // Set single quadrant mode(default)
		m_bImplicitFlag = TRUE;
		break;
	case 75: // Set multi quadrant mode
		m_bImplicitFlag = FALSE;
		break;
	case 4: // G04 ignore data block
		while(**pFile != EOB)
		(*pFile)++;
		break;
	case 36: // outline fill  turn on
		m_gObj.dCode = 0;
		m_nADCode = 0;
		m_bPolygonFlag = TRUE;
		if (m_bImplicitFlag)
			m_gObj.Type.nType = DRAW_TYPE::POLYGON_SQ;   //'G74
		else
			m_gObj.Type.nType = DRAW_TYPE::POLYGON_MQ;   //'G75

		m_gObj.Extent = m_MaxFrameExtent;
		break;
	case 37: // outline fill  turn off
		if (m_bPolygonFlag) // ==============> Polygon Flag ==========>
		{
			if (m_gObj.nStPnt < m_nNumOfObjCoord)
			{
				if (m_paramIP.bLayerPolarity == LAYER_TYPE::NEGATIVE)
					m_gObj.Type.nPolarity = DRAW_TYPE::NEGATIVE; 
				else 
					m_gObj.Type.nPolarity = DRAW_TYPE::POSITIVE;

				m_gObj.Type.nAttr = DRAW_TYPE::PATTERN;
				

				m_pLayerInfo->listObj.push_back(m_gObj);

				ClearObjectData(m_gObj);
				m_gObj.nStPnt = m_nNumOfObjCoord;
				
				m_nObj++;
				m_bPolygonFlag = FALSE;
			}
			else
				m_bPolygonFlag = FALSE;
		}
		break;
	case 54: // select aperture, this historic code optionally precedes an aperture D-code. It is superfluous and deprecated
		if(**pFile == _T('D'))// Exposure
		{
			m_nADCode = GetIntegerData(&(++(*pFile)));
			if(m_nADCode < 10)
			{
				strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code D%02d"),nData);
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
			else
			{
				m_pLayerInfo->mapAptList.Lookup(m_nADCode, m_rAptDef);
				m_CurMinMax = m_rAptDef->minmax;
			}
		}
		break;
	case 55: 
		if(**pFile == _T('X')) // X coordinate of line endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.x = GetCoordDataX(&(++(*pFile)));
		}
		if(**pFile == _T('Y')) // Y coordinate of line endpoint
		{
			m_bParsingXYFlag = TRUE; 
			m_fptCoord.y = GetCoordDataY(&(++(*pFile)));
		}
		if(**pFile == _T('D'))// Exposure
		{
			m_bParsingdCodeFlag = TRUE;
			m_nPrevDCode = m_nDCode;
			m_nDCode = GetIntegerData(&(++(*pFile)));
			switch(m_nDCode)
			{
			case 3: // Flash On ?
				break;
			default:
				strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code D%02d"),m_nDCode);
				AfxMessageBox(strErrMsg);
				m_bLastFrame = TRUE;
				return TRUE;
			}
		}
		break;
	case 70: // Specify inches
		break;
	case 71: // Specify millimeters
		break;
	case 90: // Specify absolute format
		break;
	case 91: // Specify incremental format
		break;
	default:
		strErrMsg.Format(_T("RS274X parameter block parsing error: invalid G-Code G%02d"),m_nGCode);
		AfxMessageBox(strErrMsg);
		m_bLastFrame = TRUE;
		return TRUE;
	}

	if(**pFile == EOB)
		(*pFile)++;

	return TRUE;
}

BOOL CFileFormatRs274X::FunctionMCode(char** pFile)
{
	int nData, i;
	CString strMsg, strErrMsg;

	m_nMCode = GetIntegerData(&(++(*pFile)));
	switch(m_nMCode)
	{
	case 0: // Program stop. This historic code has no effect on the image. It is deprecated.
		break;
	case 1: // Optional Stop. This historic code has no effect on the image. It is deprecated.
		break;
	case 2: // End of program. Ends image generation. Every file must end in a M02
		if((m_paramMI.bMirrorX || m_paramMI.bMirrorY) && m_nStartMirrorObj == 0)
			InitMaxLayerExtent();

		Mirroring();
		StepAndRepeating(m_MnMxSnR, FALSE);
		m_bLastFrame = TRUE;
		return TRUE;
	default:
		strErrMsg.Format(_T("RS274X parameter block parsing error: invalid M-Code M%02d"), m_nMCode);
		AfxMessageBox(strErrMsg);
		m_bLastFrame = TRUE;
		return TRUE;
	}

	return TRUE;
}

int	CFileFormatRs274X::GetIntegerData(char **pFile)
{
	CHAR szData[MAX_PATH];
	int nIndex;
	int nData =0;

	nIndex = 0;

	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit
	if(**pFile == _T('-'))
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}

	while(_istdigit(**pFile) && nIndex < MAX_PATH )
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}
	szData[nIndex] = NULL;

	nData = _ttoi(szData);

	return nData;
}

float CFileFormatRs274X::GetFloatData(char **pFile)
{
	CHAR szData[MAX_PATH];
	int nIndex;
	float fData = 0.0;

	nIndex = 0;

	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit

	if(**pFile == _T('-'))
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}

	while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}
	szData[nIndex] = NULL;

	fData = _tcstod(szData,NULL);

	return fData;
}

float CFileFormatRs274X::GetCoordDataX(char **pFile)
{
	CHAR szData[MAX_PATH];
	float fData=0.0;
	int i = 0, n = 0;
	BOOL bDotFlag = FALSE;

	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit
	if(**pFile == _T('-'))
	{
		szData[i++] = **pFile;
		(*pFile)++;
		n++;
	}

	while((_istdigit(**pFile) || (**pFile == _T('.'))) && i < MAX_PATH )
	{
		if(**pFile == _T('.'))
			bDotFlag = TRUE;

		if(!bDotFlag)
		{
			if(m_paramFS.bSuppress) // Trailing zero option
				if(i-n == m_lDataFormatX.quot)
					szData[i++] = _T('.');
		}
		
		szData[i++] = **pFile;
		(*pFile)++;
	}

	if(!bDotFlag)
	{
		if(m_paramFS.bSuppress) // Trailing zero option
		{
			if(i < m_lDataFormatX.quot)
			{
				for( ; i < m_lDataFormatX.quot ; i++)
					szData[i] = _T('0');
			}

			szData[i] = NULL;
			fData = _tcstod(szData, NULL);
		}
		else
		{
			szData[i] = NULL;
			fData = _ttoi(szData);

			fData = (float)fData/m_fDividerX;
		}
	}
	else
	{
		szData[i] = NULL;
		fData = _tcstod(szData, NULL);
	}
	
	if(m_Unit == LAYER_TYPE::INCH)
		fData *= INCHTOMM;

	fData *= m_fLayerScaleX;

	return fData;
}

float CFileFormatRs274X::GetCoordDataY(char **pFile)
{
	CHAR szData[MAX_PATH];
	float fData=0.0;
	int i = 0, n = 0;
	BOOL bDotFlag = FALSE;


	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit
	if(**pFile == _T('-'))
	{
		szData[i++] = **pFile;
		(*pFile)++;
		n++;
	}

	while((_istdigit(**pFile) || (**pFile == _T('.'))) && i < MAX_PATH )
	{
		if(**pFile == _T('.'))
			bDotFlag = TRUE;

		if(!bDotFlag)
		{
			if(m_paramFS.bSuppress) // Trailing zero option
				if(i-n == m_lDataFormatY.quot)
					szData[i++] = _T('.');
		}			
		szData[i++] = **pFile;
		(*pFile)++;
	}

	if(!bDotFlag)
	{
		if(m_paramFS.bSuppress) // Trailing zero option
		{
			if(i < m_lDataFormatX.quot)
			{
				for( ; i < m_lDataFormatY.quot ; i++)
					szData[i] = _T('0');
			}

			szData[i] = NULL;
			fData = _tcstod(szData, NULL);
		}
		else
		{
			szData[i] = NULL;
			fData = _ttoi(szData);

			fData = (float)fData/m_fDividerY;
		}
	}
	else
	{
		szData[i] = NULL;
		fData = _tcstod(szData, NULL);
	}

	if(m_Unit == LAYER_TYPE::INCH)
		fData *= INCHTOMM;

	fData *= m_fLayerScaleY;

	return fData;
}


int	CFileFormatRs274X::ParseIntegerData(char **pFile, BOOL bArithmetic)
{
	CHAR szData[MAX_PATH];
	float fData = 0.0;
	int nData = 0;
	int nIndex, nNumOfData;

	nNumOfData = 0;
	nIndex = 0;

	nNumOfData = 0;
	nIndex = 0;

	// signed bit 
	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit
	if(**pFile == _T('-'))
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}

	while(_istdigit(**pFile) && nIndex < MAX_PATH )
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}
	szData[nIndex] = NULL;
	nNumOfData++;

	if(bArithmetic)
	{
		while(CheckArithmeticOperator(**pFile))
		{
			if(**pFile == _T('X'))
				szData[nIndex++] = _T('*');
			else
				szData[nIndex++] = **pFile;
			(*pFile)++;


			while(_istdigit(**pFile) || **pFile == _T('.') )
			{
				szData[nIndex++] = **pFile;
				(*pFile)++;
			}


			szData[nIndex] = NULL;
			nNumOfData++;
		}
	}

	if( nNumOfData == 1)
		nData = _ttoi(szData);
	else
		nData = m_PostfixCalc.Calc(szData);

	if(**pFile == _T(','))
		(*pFile)++;

	return nData;
}

float CFileFormatRs274X::ParseFloatData(char **pFile,BOOL bArithmetic)
{
	CHAR szData[MAX_PATH];
	int nIndex, nNumOfData;
	float fData = 0.0;
	nNumOfData = 0;
	nIndex = 0;

	// signed bit 
	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit
	if(**pFile == _T('-'))
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}

	while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}
	szData[nIndex] = NULL;
	nNumOfData++;

	if(bArithmetic)
	{
		while(CheckArithmeticOperator(**pFile))
		{
			if(**pFile == _T('X'))
				szData[nIndex++] = _T('*');
			else
				szData[nIndex++] = **pFile;
			(*pFile)++;

			while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
			{
				szData[nIndex++] = **pFile;
				(*pFile)++;
			}


			szData[nIndex] = NULL;
			nNumOfData++;
		}
	}

	if( nNumOfData == 1)
		fData = _tcstod(szData,NULL);
	else
		fData = m_PostfixCalc.Calc(szData);
	if(m_Unit == LAYER_TYPE::INCH)
		fData *= INCHTOMM;

	if(**pFile == _T(','))
		(*pFile)++;

	
	return fData;
}

float CFileFormatRs274X::ParseScaleData(char **pFile, BOOL bArithmetic)
{
	CHAR szData[MAX_PATH];
	int nIndex, nNumOfData;
	float fData = 1.0;
	nNumOfData = 0;
	nIndex = 0;

	// signed bit 
	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit
	if(**pFile == _T('-'))
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}

	while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}
	szData[nIndex] = NULL;
	nNumOfData++;

	if(bArithmetic)
	{
		while(CheckArithmeticOperator(**pFile))
		{
			if(**pFile == _T('X'))
				szData[nIndex++] = _T('*');
			else
				szData[nIndex++] = **pFile;
			(*pFile)++;

			while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
			{
				szData[nIndex++] = **pFile;
				(*pFile)++;
			}


			szData[nIndex] = NULL;
			nNumOfData++;
		}
	}

	if( nNumOfData == 1)
		fData = _tcstod(szData,NULL);
	else
		fData = m_PostfixCalc.Calc(szData);

	if(**pFile == _T(','))
		(*pFile)++;


	return fData;
}

float CFileFormatRs274X::ParseRotationData(char **pFile, BOOL bArithmetic)
{
	CHAR szData[MAX_PATH];
	int nIndex, nNumOfData;
	float fData = 0.0;
	nNumOfData = 0;
	nIndex = 0;

	// signed bit 
	if(**pFile == _T('+'))
		(*pFile)++; // ignore '+' sign bit
	if(**pFile == _T('-'))
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}

	while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
	{
		szData[nIndex++] = **pFile;
		(*pFile)++;
	}
	szData[nIndex] = NULL;
	nNumOfData++;

	if(bArithmetic)
	{
		while(CheckArithmeticOperator(**pFile))
		{
			if(**pFile == _T('X'))
				szData[nIndex++] = _T('*');
			else
				szData[nIndex++] = **pFile;
			(*pFile)++;

			while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
			{
				szData[nIndex++] = **pFile;
				(*pFile)++;
			}


			szData[nIndex] = NULL;
			nNumOfData++;
		}
	}

	if( nNumOfData == 1)
		fData = _tcstod(szData,NULL);
	else
		fData = m_PostfixCalc.Calc(szData);

	if(**pFile == _T(','))
		(*pFile)++;

	return fData;
}

BOOL CFileFormatRs274X::ParseApertureMacro(char **pFile,CString &strApertureMacro)
{
	int i;
	int nIndex;
	CHAR szData[MAX_PATH];
	
	i = 0;
	nIndex = 0;

	char *pLine = *pFile;

	CStringArray Param;
	while(**pFile != _T('*'))
	{
		nIndex = 0;
		while((_istdigit(**pFile) || **pFile == _T('.')) && nIndex < MAX_PATH )
		{
			szData[nIndex++] = **pFile;
			(*pFile)++;
			i++;
		}
		szData[nIndex] = NULL;
		Param.Add((CString)szData);
		if(**pFile == _T('X'))
			(*pFile)++;
		i++;
	}


	char ch;
	CString strParser;
	CString strData;
	
	TCHAR *szTemp;
	TCHAR *pszContents;


	strApertureMacro.Remove(TCHAR(ASCII_CR)); // ignore CR
	strApertureMacro.Remove(TCHAR(ASCII_LF)); // ignore LF

	szTemp = strParser.GetBuffer(i+strApertureMacro.GetLength()+MAX_PATH);
	pszContents = strApertureMacro.GetBuffer(strApertureMacro.GetLength()+MAX_PATH);
	
	// preprocessing for ignore primitive '0'
	while(*pszContents == _T('0'))
	{
		pszContents++;
		while(*pszContents != TCHAR(EOB))
			pszContents++;
		pszContents++;
	}

	do 
	{
		ch = *pszContents;
		switch(ch)
		{
		case TCHAR(EOB):
			*szTemp++ = *pszContents++;
			while(*pszContents == _T('0'))
			{
				pszContents++;
				while(*pszContents != TCHAR(EOB))
					pszContents++;
			}
			break;
		case _T('$'):
			pszContents++;
			i = 0;
			do{
				ch = *pszContents++;
				szData[i++] = ch;
			}while(_istdigit(ch));
			szData[i] = NULL; 
			
			nIndex = _ttoi(szData);

			if(nIndex <= Param.GetSize())
			{
				strData = Param.GetAt(nIndex-1);

				_tcsncpy(szTemp,strData,strData.GetLength());
				szTemp += strData.GetLength();

				*szTemp++ = ch;
			}
			else
			{
				if(ch == _T('='))
				{
					CString strTemp;
					int nIndexSub;
					float fData;
					strTemp.Empty();
			
					for(i = Param.GetSize() ; i < nIndex ;i++)
						Param.Add(_T("0"));

					while(*pszContents != _T('*'))
					{
						if(*pszContents == _T('$'))
						{
							pszContents++;
							
							i = 0;
							do{
								ch = *pszContents;
								if(_istdigit(ch))
								{
									szData[i++] = ch;
									pszContents++;
								}
								else
									break;
							}while(1);
							szData[i] = NULL; 
							
							nIndexSub = _ttoi(szData);
							if(nIndexSub <= Param.GetSize())
							{
								strTemp += Param.GetAt(nIndexSub-1);
							}
							else
							{
								AfxMessageBox(_T("Aperture Macro Error"));
								return FALSE;
							}
						}
						else
						{
							strTemp += *pszContents++;							
						}
					}
					pszContents++;
					fData = m_PostfixCalc.Calc(strTemp);
					strTemp.Format(_T("%f"), fData);
					Param.SetAt(nIndex-1,strTemp);

				}
				else
				{
					AfxMessageBox(_T("Aperture Macro Error"));
					return FALSE;
				}
			}
			break;
		default:
			*szTemp++ = *pszContents++;
		}
	}while(ch != NULL); 
	
	*szTemp = NULL;
	strApertureMacro.GetBuffer(strParser.GetLength()+1);
	strApertureMacro.Format(_T("%s"),strParser);
	return TRUE;

}




BOOL CFileFormatRs274X::CheckGCodeValue(int nGCode)
{
	switch(nGCode)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 36:
	case 37:
	case 54:
	case 70:
	case 71:
	case 74:
	case 75:
	case 90:
	case 91:
		return TRUE;
	}
	return FALSE;
}

BOOL CFileFormatRs274X::CheckDCodeValue(int nDCode)
{
	if(nDCode < 1 || (nDCode > 3 && nDCode < 10)) // D00, D04-D09 ignore data block
		return FALSE;
	return TRUE;
}

BOOL CFileFormatRs274X::CheckMCodeValue(int nMCode)
{
	if(nMCode < 0 || nMCode > 2) // allowable M-Coce M00, M01, M03 
		return FALSE;
	return TRUE;
}

BOOL CFileFormatRs274X::CheckArithmeticOperator(char ch)
{
	if(ch == _T('+') || ch == _T('-') || ch == _T('/') || ch == _T('X') || ch == _T('='))
		return TRUE;
	return FALSE;
}


FRECTC CFileFormatRs274X::GetMaxLayerExtent()
{
	return m_rectLayerExtent;
}

void CFileFormatRs274X::InitMaxLayerExtent()
{
	m_rectLayerExtent.X1 = 999999999.0;
	m_rectLayerExtent.Y1 = 999999999.0;
	m_rectLayerExtent.X2 = -999999999.0;
	m_rectLayerExtent.Y2 = -999999999.0;
}

void CFileFormatRs274X::SetMaxLayerExtent(FRECTC &rect)
{
	if(m_rectLayerExtent.X1 > rect.X1)
		m_rectLayerExtent.X1 = rect.X1;
	if(m_rectLayerExtent.X2 < rect.X2)
		m_rectLayerExtent.X2 = rect.X2;
	if(m_rectLayerExtent.Y1 > rect.Y1)
		m_rectLayerExtent.Y1 = rect.Y1;
	if(m_rectLayerExtent.Y2 < rect.Y2)
		m_rectLayerExtent.Y2 = rect.Y2;
}


FRECTC CFileFormatRs274X::MinMaxRectVectorGRB(double X1, double Y1, double X2, double Y2, double w, double ang)
{
    FPOINTC pp[4], cp;
	double cV, sV, tX, tY, hh, ww, d;
    FRECTC rect = m_MaxFrameExtent;
   
    cV = cos(ang * DEG2RAD);
    sV = sin(ang * DEG2RAD);
    
    pp[0].x = X1;
    pp[0].y = Y1;
    pp[1].x = X2;
    pp[1].y = Y2;
    
    X1 = (pp[0].x * cV) - (pp[0].y * sV);
    Y1 = (pp[0].x * sV) + (pp[0].y * cV);
    X2 = (pp[1].x * cV) - (pp[1].y * sV);
    Y2 = (pp[1].x * sV) + (pp[1].y * cV);
        
    d = sqrt((X2 - X1) * (X2 - X1) + (Y2 - Y1) * (Y2 - Y1));
    
    hh = Y2 - Y1;
    ww = X2 - X1;
    tY = w / 2 * ww / d;
    tX = w / 2 * hh / d;
    
    pp[0].x = X1 + tX;
    pp[0].y = Y1 - tY;
    pp[1].x = X2 + tX;
    pp[1].y = Y2 - tY;
    pp[2].x = X2 - tX;
    pp[2].y = Y2 + tY;
    pp[3].x = X1 - tX;
    pp[3].y = Y1 + tY;

    for (int i = 0 ; i<4 ; i++)
	{
		m_pTrUtil->AdjustExtentByPoint(rect, pp[i]);
    }
	return rect;
}

FRECTC CFileFormatRs274X::MinMaxRectCenterGRB(double x, double y, double w, double h, double ang)
{
    FPOINTC pp[4], cp;
	double cV, sV, XX, YY;
	FRECTC r;

	r = m_MaxFrameExtent;
    
    cV = cos(ang * DEG2RAD);
    sV = sin(ang * DEG2RAD);
    
//    '좌하
    XX = -w / 2;
    YY = -h / 2;
    pp[0].x = (XX * cV) - (YY * sV) + x;
    pp[0].y = (XX * sV) + (YY * cV) + y;
//    '우하
    XX = w / 2;
    YY = -h / 2;
    pp[1].x = (XX * cV) - (YY * sV) + x;
    pp[1].y = (XX * sV) + (YY * cV) + y;
//    '우상
    XX = w / 2;
    YY = h / 2;
    pp[2].x = (XX * cV) - (YY * sV) + x;
    pp[2].y = (XX * sV) + (YY * cV) + y;
//    '좌상
    XX = -w / 2;
    YY = h / 2;
    pp[3].x = (XX * cV) - (YY * sV) + x;
    pp[3].y = (XX * sV) + (YY * cV) + y;
    
	for (int i = 0 ; i<4 ; i++)
	{
		m_pTrUtil->AdjustExtentByPoint(r, pp[i]);
	}
	return r;
}

FRECTC CFileFormatRs274X::MinMaxRectLowerLeftGRB(double x, double y, double w, double h, double ang)
{
    FPOINTC pp[4], cp;
	double cV, sV, XX, YY;
	FRECTC rect;
    
    cp.x = x + w / 2;
    cp.y = y + h / 2;
    
	rect = m_MaxFrameExtent;

    if (ang != 0)
	{
        cV = cos(ang * DEG2RAD);
        sV = sin(ang * DEG2RAD);
            
        XX = -w / 2;
        YY = -h / 2;
        pp[0].x = (XX * cV) - (YY * sV) + cp.x;
        pp[0].y = (XX * sV) + (YY * cV) + cp.y;
//        '우하
        XX = w / 2;
        YY = -h / 2;
        pp[1].x = (XX * cV) - (YY * sV) + cp.x;
        pp[1].y = (XX * sV) + (YY * cV) + cp.y;
//        '우상
        XX = w / 2;
        YY = h / 2;
        pp[2].x = (XX * cV) - (YY * sV) + cp.x;
        pp[2].y = (XX * sV) + (YY * cV) + cp.y;
//        '좌상
        XX = -w / 2;
        YY = h / 2;
        pp[3].x = (XX * cV) - (YY * sV) + cp.x;
        pp[3].y = (XX * sV) + (YY * cV) + cp.y;
        
	    for (int i = 0 ; i<4 ; i++)
		{
			m_pTrUtil->AdjustExtentByPoint(rect, pp[i]);
        }
	}
    else
	{
        rect.X1 = x;
        rect.X2 = x + w;
        rect.Y1 = y;
        rect.Y2 = y + h;
    }
	return rect;
    
}


void CFileFormatRs274X::GetCosineSineValue(float Ang, float *cV, float *sV)
{
    switch (int(Ang))	//     'CCW
	{
	case 0:
	case 360:
	case -360:
            *cV = 1;
            *sV = 0;
			break;

    case 90:
	case -270:
			*cV = 0;
            *sV = 1;
			break;
    case 180:
	case -180:
            *cV = -1;
            *sV = 0;
			break;

    case 270:
	case -90:
            *cV = 0;
            *sV = -1;
			break;

	default:
            *cV = cos(Ang * DEG2RAD);
            *sV = sin(Ang * DEG2RAD);
			break;
    }
}

void CFileFormatRs274X::Mirroring()
{
	int i, nCoord;

	if(m_paramMI.bMirrorX)
	{
		m_nEndMirrorObj = m_pLayerInfo->listObj.size();							
		for(i = m_nStartMirrorObj ; i < m_nEndMirrorObj; i++)
		{
			if(i == m_nEndMirrorObj - 1)
				nCoord = m_pLayerInfo->vecObjCoord.size() - m_pLayerInfo->listObj[i].nStPnt;
			else
				nCoord = m_pLayerInfo->listObj[i+1].nStPnt - m_pLayerInfo->listObj[i].nStPnt;

			MirrorVerticalObject(m_pLayerInfo,&m_pLayerInfo->listObj.at(i), nCoord, 0.0);
			SetMaxLayerExtent(m_pLayerInfo->listObj.at(i).Extent);
		}

		m_paramMI.bMirrorX = FALSE;
	}
	if(m_paramMI.bMirrorY)
	{
		m_nEndMirrorObj = m_pLayerInfo->listObj.size();							
		for(i = m_nStartMirrorObj ; i < m_nEndMirrorObj; i++)
		{
			if(i == m_nEndMirrorObj - 1)
				nCoord = m_pLayerInfo->vecObjCoord.size() - m_pLayerInfo->listObj.at(i).nStPnt;
			else
				nCoord = m_pLayerInfo->listObj.at(i+1).nStPnt - m_pLayerInfo->listObj.at(i).nStPnt;

			MirrorHorizontalObject(m_pLayerInfo,&m_pLayerInfo->listObj.at(i),nCoord, 0.0);
			SetMaxLayerExtent(m_pLayerInfo->listObj.at(i).Extent);
		}

		m_paramMI.bMirrorY = FALSE;
	}
	m_nStartMirrorObj = 0;

}

void CFileFormatRs274X::MirrorVerticalObject(vector<FPOINTC> &vecCoord,GraphObj *obj, double vAxis)  //'좌우미러링
{
	int k, nCoord;
	byte dType;
	double XX;

    dType = obj->Type.nType;
    
	nCoord = 0;

    switch(dType)
	{
	case DRAW_TYPE::LINE:
 		vecCoord[0].x = -(vecCoord[0].x - vAxis) + vAxis;
        vecCoord[1].x = -(vecCoord[1].x - vAxis) + vAxis;
		break;
    case DRAW_TYPE::FLASH:
 		vecCoord[0].x = -(vecCoord[0].x - vAxis) + vAxis;
		break;        
    case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ:
		nCoord = vecCoord.size();
 		vecCoord[0].x = -(vecCoord[0].x - vAxis) + vAxis;
        for (k=1 ; k < nCoord ;k++)
		{
			if ((vecCoord[k].x == DetGcode && vecCoord[0+k].y == DetGcode) || 
				(vecCoord[k].x == -DetGcode && vecCoord[0+k].y == -DetGcode))
			{
				vecCoord[k].x = -vecCoord[k].x;
				vecCoord[k].y = -vecCoord[k].y;
				k++;
				vecCoord[k].x = -(vecCoord[k].x - vAxis) + vAxis;
				k++;
				vecCoord[k].x = -vecCoord[k].x;
			}
			else
				vecCoord[k].x = -(vecCoord[k].x - vAxis) + vAxis; 
        }
		break;
    case DRAW_TYPE::ARC_CW:
	case DRAW_TYPE::ARC_CCW:
        if (dType == DRAW_TYPE::ARC_CW) 
			obj->Type.nType = DRAW_TYPE::ARC_CCW; 
		else 
			obj->Type.nType = DRAW_TYPE::ARC_CW;
 		vecCoord[0].x = -(vecCoord[0].x - vAxis) + vAxis;
        vecCoord[1].x = -(vecCoord[1].x - vAxis) + vAxis;
        vecCoord[2].x = -(vecCoord[2].x - vAxis) + vAxis;
		break;
    case DRAW_TYPE::SNR:
        vecCoord[2].x = -vecCoord[2].x;
		break;
    }

    XX = obj->Extent.X2;
    obj->Extent.X2 = -(obj->Extent.X1 - vAxis) + vAxis;
    obj->Extent.X1 = -(XX - vAxis) + vAxis;
}

void CFileFormatRs274X::MirrorVerticalObject(LAYER_INFORM_RS274X *pLayerInfo,GraphObj *obj, int nCoord, double vAxis)  //'좌우미러링
{
	int k;
	byte dType;
	double XX;

    dType = obj->Type.nType;
    
    switch(dType)
	{
	case DRAW_TYPE::LINE:
 		pLayerInfo->vecObjCoord[obj->nStPnt].x = -(pLayerInfo->vecObjCoord[obj->nStPnt].x - vAxis) + vAxis;
        pLayerInfo->vecObjCoord[obj->nStPnt+1].x = -(pLayerInfo->vecObjCoord[obj->nStPnt+1].x - vAxis) + vAxis;
		break;
    case DRAW_TYPE::FLASH:
 		pLayerInfo->vecObjCoord[obj->nStPnt].x = -(pLayerInfo->vecObjCoord[obj->nStPnt].x - vAxis) + vAxis;
		break;
    case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ:
 		pLayerInfo->vecObjCoord[obj->nStPnt].x = -(pLayerInfo->vecObjCoord[obj->nStPnt].x - vAxis) + vAxis;
        for (k=1 ; k < nCoord ;k++)
		{
 		   if ((pLayerInfo->vecObjCoord[obj->nStPnt+k].x == DetGcode && pLayerInfo->vecObjCoord[obj->nStPnt+k].y == DetGcode) || 
                (pLayerInfo->vecObjCoord[obj->nStPnt+k].x == -DetGcode && pLayerInfo->vecObjCoord[obj->nStPnt+k].y == -DetGcode))
			{
                pLayerInfo->vecObjCoord[obj->nStPnt+k].x = -pLayerInfo->vecObjCoord[obj->nStPnt+k].x;
                pLayerInfo->vecObjCoord[obj->nStPnt+k].y = -pLayerInfo->vecObjCoord[obj->nStPnt+k].y;
                k++;
                pLayerInfo->vecObjCoord[obj->nStPnt+k].x = -(pLayerInfo->vecObjCoord[obj->nStPnt+k].x - vAxis) + vAxis;
                k++;
                pLayerInfo->vecObjCoord[obj->nStPnt+k].x = -pLayerInfo->vecObjCoord[obj->nStPnt+k].x;
			}
           else
                pLayerInfo->vecObjCoord[obj->nStPnt+k].x = -(pLayerInfo->vecObjCoord[obj->nStPnt+k].x - vAxis) + vAxis; 
        }
		break;
    case DRAW_TYPE::ARC_CW:
	case DRAW_TYPE::ARC_CCW:
        if (dType == DRAW_TYPE::ARC_CW) 
			obj->Type.nType = DRAW_TYPE::ARC_CCW; 
		else 
			obj->Type.nType = DRAW_TYPE::ARC_CW;
 		pLayerInfo->vecObjCoord[obj->nStPnt+0].x = -(pLayerInfo->vecObjCoord[obj->nStPnt+0].x - vAxis) + vAxis;
        pLayerInfo->vecObjCoord[obj->nStPnt+1].x = -(pLayerInfo->vecObjCoord[obj->nStPnt+1].x - vAxis) + vAxis;
        pLayerInfo->vecObjCoord[obj->nStPnt+2].x = -(pLayerInfo->vecObjCoord[obj->nStPnt+2].x - vAxis) + vAxis;
		break;
    case DRAW_TYPE::SNR:
        pLayerInfo->vecObjCoord[obj->nStPnt+2].x = -pLayerInfo->vecObjCoord[obj->nStPnt+2].x;
		break;
    }

    XX = obj->Extent.X2;
    obj->Extent.X2 = -(obj->Extent.X1 - vAxis) + vAxis;
    obj->Extent.X1 = -(XX - vAxis) + vAxis;

	if(obj->Extent.X1 > obj->Extent.X2)
	{
		XX = obj->Extent.X1;
		obj->Extent.X1 = obj->Extent.X2;
		obj->Extent.X2 = XX;
	}
}

void CFileFormatRs274X::MirrorHorizontalObject(vector<FPOINTC> &vecCoord,GraphObj *obj, double hAxis)
{
	int k, nCoord;
	byte dType;
	double YY;

    dType = obj->Type.nType;
    
    switch(dType)
	{
	case DRAW_TYPE::LINE:
 		vecCoord[0].y = -(vecCoord[0].y - hAxis) + hAxis;
        vecCoord[1].y = -(vecCoord[1].y - hAxis) + hAxis;
		break;
    case DRAW_TYPE::FLASH:
 		vecCoord[0].y = -(vecCoord[0].y - hAxis) + hAxis;
		break;
    case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ:
		nCoord = vecCoord.size();
 		vecCoord[0].y = -(vecCoord[0].y - hAxis) + hAxis;
        for (k=1 ; k< nCoord ;k++)
		{
 		   if ((vecCoord[k].x == DetGcode && vecCoord[k].y == DetGcode) || 
                (vecCoord[k].x == -DetGcode && vecCoord[k].y == -DetGcode))
			{
                vecCoord[k].x = -vecCoord[k].x;
                vecCoord[k].y = -vecCoord[k].y;
                k++;
                vecCoord[k].y = -(vecCoord[k].y - hAxis) + hAxis;
                k++;
                vecCoord[k].y = -vecCoord[k].y;
			}
           else
                vecCoord[k].y = -(vecCoord[k].y - hAxis) + hAxis;
        }
		break;
    case DRAW_TYPE::ARC_CW:
	case DRAW_TYPE::ARC_CCW:
        if (dType == DRAW_TYPE::ARC_CW) 
			obj->Type.nType = DRAW_TYPE::ARC_CCW; 
		else 
			obj->Type.nType = DRAW_TYPE::ARC_CW;
		vecCoord[0].y = -(vecCoord[0].y - hAxis) + hAxis;
        vecCoord[1].y = -(vecCoord[1].y - hAxis) + hAxis;
        vecCoord[2].y = -(vecCoord[2].y - hAxis) + hAxis;
		break;
    case DRAW_TYPE::SNR:
        vecCoord[2].y = -vecCoord[2].y;
		if(vecCoord.size()> 3)
			vecCoord[3].y = -vecCoord[3].y;
		break;
    }

    YY = obj->Extent.Y2;
    obj->Extent.Y2 = -(obj->Extent.Y1 - hAxis) + hAxis;
    obj->Extent.Y1 = -(YY - hAxis) + hAxis;
}

void CFileFormatRs274X::MirrorHorizontalObject(LAYER_INFORM_RS274X *pLayerInfo,GraphObj *obj, int nCoord, double hAxis)  //'좌우미러링
{
	int k;
	byte dType;
	double YY;

    dType = obj->Type.nType;
    
    switch(dType)
	{
	case DRAW_TYPE::LINE:
 		pLayerInfo->vecObjCoord[obj->nStPnt].y = -(pLayerInfo->vecObjCoord[obj->nStPnt].y - hAxis) + hAxis;
        pLayerInfo->vecObjCoord[obj->nStPnt+1].y = -(pLayerInfo->vecObjCoord[obj->nStPnt+1].y - hAxis) + hAxis;
		break;
    case DRAW_TYPE::FLASH:
 		pLayerInfo->vecObjCoord[obj->nStPnt].y = -(pLayerInfo->vecObjCoord[obj->nStPnt].y - hAxis) + hAxis;
		break;
    case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ:
 		pLayerInfo->vecObjCoord[obj->nStPnt].y = -(pLayerInfo->vecObjCoord[obj->nStPnt].y - hAxis) + hAxis;
        for (k=1 ; k< nCoord ;k++)
		{
 		   if ((pLayerInfo->vecObjCoord[obj->nStPnt+k].x == DetGcode && pLayerInfo->vecObjCoord[obj->nStPnt+k].y == DetGcode) || 
                (pLayerInfo->vecObjCoord[obj->nStPnt+k].x == -DetGcode && pLayerInfo->vecObjCoord[obj->nStPnt+k].y == -DetGcode))
			{
                pLayerInfo->vecObjCoord[obj->nStPnt+k].x = -pLayerInfo->vecObjCoord[obj->nStPnt+k].x;
                pLayerInfo->vecObjCoord[obj->nStPnt+k].y = -pLayerInfo->vecObjCoord[obj->nStPnt+k].y;
                k++;
                pLayerInfo->vecObjCoord[obj->nStPnt+k].y = -(pLayerInfo->vecObjCoord[obj->nStPnt+k].y - hAxis) + hAxis;
                k++;
                pLayerInfo->vecObjCoord[obj->nStPnt+k].y = -pLayerInfo->vecObjCoord[obj->nStPnt+k].y;
			}
           else
                pLayerInfo->vecObjCoord[obj->nStPnt+k].y = -(pLayerInfo->vecObjCoord[obj->nStPnt+k].y - hAxis) + hAxis;
        }
		break;
    case DRAW_TYPE::ARC_CW:
	case DRAW_TYPE::ARC_CCW:
        if (dType == DRAW_TYPE::ARC_CW) 
			obj->Type.nType = DRAW_TYPE::ARC_CCW; 
		else 
			obj->Type.nType = DRAW_TYPE::ARC_CW;
		pLayerInfo->vecObjCoord[obj->nStPnt+0].y = -(pLayerInfo->vecObjCoord[obj->nStPnt+0].y - hAxis) + hAxis;
        pLayerInfo->vecObjCoord[obj->nStPnt+1].y = -(pLayerInfo->vecObjCoord[obj->nStPnt+1].y - hAxis) + hAxis;
        pLayerInfo->vecObjCoord[obj->nStPnt+2].y = -(pLayerInfo->vecObjCoord[obj->nStPnt+2].y - hAxis) + hAxis;
		break;
    case DRAW_TYPE::SNR:
        pLayerInfo->vecObjCoord[obj->nStPnt+2].y = -pLayerInfo->vecObjCoord[obj->nStPnt+2].y;
		break;
    }

    YY = obj->Extent.Y2;
    obj->Extent.Y2 = -(obj->Extent.Y1 - hAxis) + hAxis;
    obj->Extent.Y1 = -(YY - hAxis) + hAxis;

	if(obj->Extent.Y1 > obj->Extent.Y2)
	{
		YY = obj->Extent.Y1;
		obj->Extent.Y1 = obj->Extent.Y2;
		obj->Extent.Y2 = YY;
	}
}

void CFileFormatRs274X::StepAndRepeating(FRECTC &Extent, BOOL bClearObjectData)
{
	if (m_paramSR.nXRepeat <= 1 && m_paramSR.nYRepeat <= 1)
		return;

	int i, peRepeat;
	FPOINTC fPt;

	if(bClearObjectData)
		ClearObjectData(m_gObj);
	else
		m_gObj.nStPnt = m_nNumOfObjCoord;

	peRepeat = m_pLayerInfo->listObj.size() - 1;

	m_MnMxSnR = m_MaxFrameExtent;
	for(i=m_paramSR.psRepeat ; i<=peRepeat ; i++)
	{
		m_pTrUtil->AdjustExtentByRect(m_MnMxSnR, m_pLayerInfo->listObj.at(i).Extent);
	}

	m_gObj.dCode = 0;	
	m_gObj.Type.nType = DRAW_TYPE::SNR; 

	m_gObj.Extent.Y1 = m_MnMxSnR.Y1;
	m_gObj.Extent.Y2 = m_MnMxSnR.Y2;
	m_gObj.Extent.X1 = m_MnMxSnR.X1;
	m_gObj.Extent.X2 = m_MnMxSnR.X2;

	fPt.x = m_paramSR.psRepeat;
	fPt.y = peRepeat ;
	m_pLayerInfo->vecObjCoord.push_back(fPt);
	m_nNumOfObjCoord++;

	fPt.x = m_paramSR.nXRepeat ;
	fPt.y = m_paramSR.nYRepeat ;
	m_pLayerInfo->vecObjCoord.push_back(fPt);
	m_nNumOfObjCoord++;

	fPt.x = m_xStep;
	fPt.y = m_yStep;
	m_pLayerInfo->vecObjCoord.push_back(fPt);
	m_nNumOfObjCoord++;

	m_MnMxSnR = GetMaxLayerExtent();
	if((m_paramSR.nXRepeat - 1) * m_xStep > 0)
	{
		if (m_gObj.Extent.X1 < m_MnMxSnR.X1) m_MnMxSnR.X1 = m_gObj.Extent.X1;
		if (m_gObj.Extent.X2 + (m_paramSR.nXRepeat - 1) * m_xStep > m_MnMxSnR.X2) m_MnMxSnR.X2 = m_gObj.Extent.X2 + (m_paramSR.nXRepeat - 1) * m_xStep;
	}
	else
	{
		if (m_gObj.Extent.X1 + (m_paramSR.nXRepeat - 1) * m_xStep < m_MnMxSnR.X1) m_MnMxSnR.X1 = m_gObj.Extent.X1 + (m_paramSR.nXRepeat - 1) * m_xStep;
		if (m_gObj.Extent.X2 > m_MnMxSnR.X2) m_MnMxSnR.X2 = m_gObj.Extent.X2;
	}

	if((m_paramSR.nYRepeat - 1) * m_yStep > 0)
	{
		if (m_gObj.Extent.Y1 < m_MnMxSnR.Y1 ) m_MnMxSnR.Y1 = m_gObj.Extent.Y1;
		if (m_gObj.Extent.Y2 + (m_paramSR.nYRepeat - 1) * m_yStep > m_MnMxSnR.Y2 ) m_MnMxSnR.Y2 = m_gObj.Extent.Y2 + (m_paramSR.nYRepeat - 1) * m_yStep;
	}
	else
	{
		if (m_gObj.Extent.Y1 + (m_paramSR.nYRepeat - 1) * m_yStep < m_MnMxSnR.Y1 ) m_MnMxSnR.Y1 = m_gObj.Extent.Y1 + (m_paramSR.nYRepeat - 1) * m_yStep;
		if (m_gObj.Extent.Y2 > m_MnMxSnR.Y2 ) m_MnMxSnR.Y2 = m_gObj.Extent.Y2;
	}

	SetMaxLayerExtent(m_MnMxSnR);
	
	m_gObj.Type.nAttr = DRAW_TYPE::PATTERN;									
	m_pLayerInfo->listObj.push_back(m_gObj);

	m_nNumOfSnRObject++;

	m_gObj.nStPnt = m_nNumOfObjCoord;
	ClearObjectData(m_gObj);

	m_nObj++;                    
}

void CFileFormatRs274X::ApplyIROptionGRB(LAYER_INFORM_RS274X *pLayerInfo, double cX, double cY, double fAng)
{    
	int i, nObj;
	CString temp;
	FPOINTC fC = FPOINTC(cX,cY);

	i = 0;
	nObj = 0;
	nObj = pLayerInfo->listObj.size();
	for (i=0 ; i < nObj-1 ; i++)	//for (i=0 ; i < nObj ; i++)
	{
		RotateTransObject(pLayerInfo,&pLayerInfo->listObj[i],&pLayerInfo->listObj[i+1], fC, fAng, 0.0, 0.0);
	}
	m_pTrUtil->RotateRect(pLayerInfo->Extent, fC, fAng);
}

void CFileFormatRs274X::RotateTransObject(LAYER_INFORM_RS274X *pLayerInfo,GraphObj *obj, GraphObj *Nextobj,FPOINTC fC, double fAng, double tX, double tY)
{
	byte dType;
	FPOINTC p = FPOINTC(0.0,0.0);
	int k = 0, nSize = 0;
	float cV = 1.0, sV = 0.0;
	double XX = 0.0;

	nSize = Nextobj->nStPnt - obj->nStPnt;
    dType = obj->Type.nType;
    
	
	switch(dType)
	{
	case DRAW_TYPE::LINE: //' Line: m_pTrUtil->m_bType[0]============================================================>
		m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], tX, tY);
		m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+1], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+1], tX, tY);
		break;
	case DRAW_TYPE::FLASH: // 'Flash : m_pTrUtil->m_bType[1]
		m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], tX, tY);
		break;    
	case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ: // m_pTrUtil->m_bType[5], m_pTrUtil->m_bType[6] 'PolygonFill with G74, G75
		m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], tX, tY);
		for (k=1 ; k < nSize ; k++)
		{
			if (fabs(pLayerInfo->vecObjCoord.at(k+obj->nStPnt).x) == DetGcode && fabs(pLayerInfo->vecObjCoord.at(k+obj->nStPnt).y) == DetGcode)
			{
                k++;
				m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+k], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+k], tX, tY);
                
                k++;
				m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+k], FPOINTC(0.0,0.0), fAng);
			}
            else
			{
				m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+k], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+k], tX, tY);
            }
        }
		break;
	case DRAW_TYPE::ARC_CW:
	case DRAW_TYPE::ARC_CCW: // dType == TYPE_CW
		m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt], tX, tY);
		m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+1], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+1], tX, tY);
		m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+2], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt+2], tX, tY);
		break;
	case DRAW_TYPE::SNR:	

		m_pTrUtil->cVsV(fAng,&cV,&sV);
        
		XX = pLayerInfo->vecObjCoord[obj->nStPnt+2].x;
        
		pLayerInfo->vecObjCoord[obj->nStPnt+2].x = cV * pLayerInfo->vecObjCoord[obj->nStPnt+2].x - sV * pLayerInfo->vecObjCoord[obj->nStPnt+2].y;
        pLayerInfo->vecObjCoord[obj->nStPnt+2].y = sV * XX + cV * pLayerInfo->vecObjCoord[obj->nStPnt+2].y;

		if(nSize > 5)
		{
			m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2], tX, tY);
			m_pTrUtil->RotatePoint(pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1], fC, fAng); m_pTrUtil->TranslatePoint(pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1], tX, tY);

			if(pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2].x > pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1].x)
			{
				XX = pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2].x;
				pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2].x = pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1].x;
				pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1].x = XX;
			}
			if(pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2].y > pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1].y)
			{
				XX = pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2].y;
				pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 2].y = pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1].y;
				pLayerInfo->vecObjCoord[obj->nStPnt + nSize - 1].y = XX;
			}
		}

		if(fAng == 90 || fAng == -90)
		{
			k = pLayerInfo->vecObjCoord[obj->nStPnt+1].x;
			pLayerInfo->vecObjCoord[obj->nStPnt+1].x = pLayerInfo->vecObjCoord[obj->nStPnt+1].y;
			pLayerInfo->vecObjCoord[obj->nStPnt+1].y = k;
		}
		break;   
	}          
	m_pTrUtil->RotateRect(obj->Extent, fC, fAng);
	m_pTrUtil->TranslateRect(obj->Extent, tX, tY);

}

FRECTC CFileFormatRs274X::MinMaxArcInPolygonGRB(DRAW_TYPE::TYPE dType, FPOINTC P1, FPOINTC P2, FPOINTC PC)
{
	FRECTC r;
	double R, vX1, vX2, vY1, vY2, vK, vD, Ang1, Ang2;
    
//'dType:  cW:m_pTrUtil->m_bType(2), ccW:m_pTrUtil->m_bType(3)
    
    if(P1.x < P2.x)
	{
        r.X1 = P1.x;
        r.X2 = P2.x;
	}
    else
	{
        r.X1 = P2.x;
        r.X2 = P1.x;
    }

    if(P1.y < P2.y)
	{
        r.Y1 = P1.y;
        r.Y2 = P2.y;
	}
    else
	{
        r.Y1 = P2.y;
        r.Y2 = P1.y;
    }
    
    R = sqrt((P1.x - PC.x) * (P1.x - PC.x) + (P1.y - PC.y) * (P1.y - PC.y));
    
    vX1 = P1.x - PC.x;
    vY1 = P1.y - PC.y;
    vX2 = P2.x - PC.x;
    vY2 = P2.y - PC.y;
    vK = (vX1 * vY2) - (vY1 * vX2);
    vD = (vX1 * vX2) + (vY1 * vY2);
    
    if(fabs(vK) < m_dDetZero && vD > 0)
	{
        r.X1 = PC.x - R; // '+ CurMinMax.X1
        r.X2 = PC.x + R; //  '+ CurMinMax.X2
        r.Y1 = PC.y - R; //  '+ CurMinMax.Y1
        r.Y2 = PC.y + R; //  '+ CurMinMax.Y2
        
        return r;
	}
    
    if(fabs((P1.x - PC.x)) < m_dDetZero)
	{
        if(P1.y > PC.y)
            Ang1 = PI * 0.5; //  '90도
        else
            Ang1 = PI * 1.5; //  '270도
	}
    else if(fabs((P1.y - PC.y)) < m_dDetZero)
	{
        if (P1.x > PC.x)
            Ang1 = 0; //         '0도
        else
            Ang1 = PI; //        '180도
	}
    else
	{
        Ang1 = atan((P1.y - PC.y) / (P1.x - PC.x));
        if(Ang1 > 0)
		{
            if (P1.x < PC.x) Ang1 = Ang1 + PI; //     ' 3사분면
		}
        else
		{
			if (P1.x < PC.x)
                Ang1 = Ang1 + PI; //     ' 2사분면
            else
                Ang1 = Ang1 + PI + PI; //    ' 4사분면
        }
	}

    if(fabs((P2.x - PC.x)) < m_dDetZero)
	{
        if(P2.y > PC.y)
            Ang2 = PI * 0.5; //  '90도
        else
            Ang2 = PI * 1.5; //  '270도
    }
    else if(fabs((P2.y - PC.y)) < m_dDetZero)
	{
        if(P2.x > PC.x)
            Ang2 = 0; //         '0도
        else
            Ang2 = PI; //        '180도
	}
    else
	{
        Ang2 = atan((P2.y - PC.y) / (P2.x - PC.x));
        if(Ang2 > 0)
		{
            if(P2.x < PC.x) Ang2 = Ang2 + PI; //     ' 3사분면
		}
        else
		{
            if(P2.x < PC.x)
                Ang2 = Ang2 + PI; //     ' 2사분면
            else
                Ang2 = Ang2 + PI + PI; //    ' 4사분면
        }
    }
    
    if(CheckAngleGRB(0.5 * PI, Ang1, Ang2, dType)) r.Y2 = PC.y + R;
    if(CheckAngleGRB(PI, Ang1, Ang2, dType)) r.X1 = PC.x - R;
    if(CheckAngleGRB(1.5 * PI, Ang1, Ang2, dType)) r.Y1 = PC.y - R;
    if(CheckAngleGRB(0, Ang1, Ang2, dType)) r.X2 = PC.x + R;
    
    
//    r.X1 = r.X1 '+ CurMinMax.X1
//    r.X2 = r.X2 '+ CurMinMax.X2
//    r.Y1 = r.Y1 '+ CurMinMax.Y1
//    r.Y2 = r.Y2 '+ CurMinMax.Y2

	return r;
}

FRECTC CFileFormatRs274X::MinMaxArcGRB(DRAW_TYPE::TYPE dType, FPOINTC P1, FPOINTC P2, FPOINTC PC, FRECTC CurMinMax)
{
	FRECTC r;

	double R, vX1, vX2, vY1, vY2, vK, vD, Ang1, Ang2;
    
    if (P1.x < P2.x)
	{
        r.X1 = P1.x;
        r.X2 = P2.x;
	}
    else
	{
        r.X1 = P2.x;
        r.X2 = P1.x;
    }

    if (P1.y < P2.y)
	{
        r.Y1 = P1.y;
        r.Y2 = P2.y;
	}
    else
	{
        r.Y1 = P2.y;
        r.Y2 = P1.y;
    }
    
    R = sqrt((P1.x - PC.x) * (P1.x - PC.x) + (P1.y - PC.y) * (P1.y - PC.y));
    
    vX1 = P1.x - PC.x;
    vY1 = P1.y - PC.y;
    vX2 = P2.x - PC.x;
    vY2 = P2.y - PC.y;
    vK = (vX1 * vY2) - (vY1 * vX2);
    vD = (vX1 * vX2) + (vY1 * vY2);
    
    if (fabs(vK) < m_dDetZero && vD > 0)
	{
        r.X1 = PC.x - R + CurMinMax.X1;
        r.X2 = PC.x + R + CurMinMax.X2;
        r.Y1 = PC.y - R + CurMinMax.Y1;
        r.Y2 = PC.y + R + CurMinMax.Y2;
        
        return r;
    }
    
    if (fabs((P1.x - PC.x)) < m_dDetZero)
	{
        if (P1.y > PC.y)
            Ang1 = PI * 0.5;	// '90도
        else
            Ang1 = PI * 1.5;	// '270도
	}
    else if (fabs((P1.y - PC.y)) < m_dDetZero)
	{
        if (P1.x > PC.x)
            Ang1 = 0;	//        '0도
        else
            Ang1 = PI;	//       '180도
	}
    else
	{
        Ang1 = atan((P1.y - PC.y) / (P1.x - PC.x));
        if (Ang1 > 0)
		{
            if (P1.x < PC.x) Ang1 = Ang1 + PI;	//    ' 3사분면
		}
        else
		{
            if (P1.x < PC.x)
                Ang1 = Ang1 + PI; //    ' 2사분면
            else
                Ang1 = Ang1 + PI + PI; //   ' 4사분면
        }
    }

    if (fabs((P2.x - PC.x)) < m_dDetZero)
	{
        if (P2.y > PC.y)
            Ang2 = PI * 0.5; // '90도
        else
            Ang2 = PI * 1.5; // '270도
    }
    else if (fabs((P2.y - PC.y)) < m_dDetZero)
	{
        if (P2.x > PC.x)
            Ang2 = 0; //        '0도
        else
            Ang2 = PI; //       '180도
    }
    else
	{
        Ang2 = atan((P2.y - PC.y) / (P2.x - PC.x));
        if (Ang2 > 0)
		{
            if (P2.x < PC.x) Ang2 = Ang2 + PI; //    ' 3사분면
		}
        else
		{
            if (P2.x < PC.x)
                Ang2 = Ang2 + PI; //    ' 2사분면
            else
                Ang2 = Ang2 + PI + PI; //   ' 4사분면
        }
    }
    
    if (CheckAngleGRB(0.5 * PI, Ang1, Ang2, dType)) r.Y2 = PC.y + R;
    if (CheckAngleGRB(PI, Ang1, Ang2, dType)) r.X1 = PC.x - R;
    if (CheckAngleGRB(1.5 * PI, Ang1, Ang2, dType)) r.Y1 = PC.y - R;
    if (CheckAngleGRB(0, Ang1, Ang2, dType)) r.X2 = PC.x + R;

    r.X1 = r.X1 + CurMinMax.X1;
    r.X2 = r.X2 + CurMinMax.X2;
    r.Y1 = r.Y1 + CurMinMax.Y1;
    r.Y2 = r.Y2 + CurMinMax.Y2;

	return r;
}


BOOL CFileFormatRs274X::CheckAngleGRB(double AngP , double Ang1, double Ang2, DRAW_TYPE::TYPE dType)
{
	BOOL rFlag = TRUE;
    
    if (Ang1 > Ang2)
	{
        if (dType == DRAW_TYPE::ARC_CCW)    //ccW
		{
            if (AngP < Ang1 && AngP > Ang2) 
				rFlag = FALSE;
		}
        else
		{
            if (AngP > Ang1 || AngP < Ang2) 
				rFlag = FALSE;
        }
	}
    else
	{
        if (dType == DRAW_TYPE::ARC_CCW)
		{
            if (AngP < Ang1 || AngP > Ang2) 
				rFlag = FALSE;
		}
        else
		{
            if (AngP > Ang1 && AngP < Ang2)
				rFlag = FALSE;
        }
    }

	return(rFlag);
}


void CFileFormatRs274X::AllocDrawList()
{
// 	m_bDrawFirst=TRUE;
	AllocCamDrawStruct();
	SetDrawResolution(m_fDrawArcResolution);
// 	int nAperturecnt = MakeApertureList();
//	SetDrawResolution(m_fDrawArcResolution); // 1/10 of Monitor Pixel resolution
//	int nLineCnt = MakeLineTemplateList();
// 	int nSnRObject = MakeSnRObjectList();
}








BOOL CFileFormatRs274X::Draw(HDC& hdc, const CRect& client)
{
	if(!m_bEnableDraw)
		return FALSE;

	m_hDC = hdc; 
	m_rtClient = client;
	m_sizeWindow.cx = client.Width();
	m_sizeWindow.cy = client.Height();	

	CfRect cfExtent;
	int nNumOfLayer = GetNumOfLayer();

	int i, nLayer;

    for(nLayer=0 ; nLayer<nNumOfLayer; nLayer++)
    {	
		m_nLayerID = nLayer;
		m_pLayerInfo = m_ArLayerInform.GetAt(nLayer);
       
		if (m_ArLayerInform[nLayer]->bView)
		{
			if(nLayer==0)
			{
				if(nNumOfLayer > 0 && m_ArLayerInform.GetAt(0)->bView && m_ArLayerInform.GetAt(0)->bPolarity == LAYER_TYPE::NEGATIVE)
				{
 					if(m_bDisplayBackgroundImg)
 					{
 						cfExtent.left = m_frtDrawFrameExtent.X1;
 						cfExtent.right = m_frtDrawFrameExtent.X2;
 						cfExtent.top = m_frtDrawFrameExtent.Y2;
 						cfExtent.bottom = m_frtDrawFrameExtent.Y1;
 
 						SetForeColor(GREY99);
 						DrawRect(cfExtent, 0, TRUE);
 					}
				}
			}

			for(i=0; i<4; i++)
			{
				if(m_pThreadDrawing[i])
				{
					m_pThreadDrawing[i]->Stop();
					Sleep(100);
					delete m_pThreadDrawing[i];
				}

				m_pThreadDrawing[i] = new CThreadTask;
			}

			m_nPrevAttribute = 16;
			m_nPrevPolarity = 3;

			m_bAliveThreadDrawing[0] = TRUE;
// 			m_bAliveThreadDrawing[1] = TRUE;
// 			m_bAliveThreadDrawing[2] = TRUE;
// 			m_bAliveThreadDrawing[3] = TRUE;
			m_pThreadDrawing[0]->Start(GetSafeHwnd(), this, ThreadDrawing_0);
// 			m_pThreadDrawing[1]->Start(GetSafeHwnd(), this, ThreadDrawing_1);
// 			m_pThreadDrawing[2]->Start(GetSafeHwnd(), this, ThreadDrawing_2);
// 			m_pThreadDrawing[3]->Start(GetSafeHwnd(), this, ThreadDrawing_3);
			while(m_bAliveThreadDrawing[0] || m_bAliveThreadDrawing[1] || m_bAliveThreadDrawing[2] || m_bAliveThreadDrawing[3])
				Sleep(10);

// 
// 			int nObjIndex;
// 			int nNumOfObj;
// 			GraphObj gObj;
// //			nNumOfObj = m_pLayerInfo->listObj.size();
//  			nNumOfObj = m_pLayerInfo->listObj.size()-1;
// 
// 			m_nPrevAttribute = 16;
// 			m_nPrevPolarity = 3;
// 
// 			for (nObjIndex = 0 ; nObjIndex < nNumOfObj ; nObjIndex++) 
// 			{
// 				DispProgress(nObjIndex, nNumOfObj);
// 				
// 				gObj = m_pLayerInfo->listObj.at(nObjIndex);
// 
// 				if(m_bCheckDraw && gObj.Type.nType != DRAW_TYPE::SNR )
// 				{
// 					if(gObj.Type.nAttr != DRAW_TYPE::NONE)
// 					{
// 						if(CheckDrawAllow(nObjIndex))
// 						{
// 							if(gObj.dCode > 0)
// 							{
// 								m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
// 							}
// 							DrawObject(nObjIndex);//MyObjEntity(nObjIndex,bCheck);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					if(gObj.dCode > 0)
// 					{
// 						m_pLayerInfo->mapAptList.Lookup(gObj.dCode, m_pAptDef);
// 					}
// 					DrawObject(nObjIndex);//MyObjEntity(nObjIndex, bCheck);
// 				}
// 			}
// 	
// 
		
		}
	}

// 	m_fExtentStartPos.x -=  m_fptDrawOffset.x;
// 	m_fExtentStartPos.y -= m_fptDrawOffset.y;
// 	m_fExtentEndPos.x -= m_fptDrawOffset.x;
// 	m_fExtentEndPos.y -= m_fptDrawOffset.y;
	m_bCheckDraw=TRUE;


	return TRUE;
}






void CFileFormatRs274X::DispProgress(int nCurr, int nTot)
{
	return;
	
	int nInc = 1;
	int nRatio = 10; // 0 ~ nRatio
	if(nTot > nRatio)
		nInc = nTot / nRatio;
	//if(!(nCurr%nInc))
	//	::SendMessage(m_hParent, WM_DISP_PROCESS, (WPARAM)nCurr+1, (LPARAM)nTot);
	//else if((nTot - nCurr - 1) < 1)
	//	::SendMessage(m_hParent, WM_DISP_PROCESS, (WPARAM)nCurr+1, (LPARAM)nTot);

// 	CDC* cdc = CDC::FromHandle(m_hDC);
// 	CPen p(PS_SOLID, 1, RGB(0, 0, 255));
// 	CPen* op = cdc->SelectObject(&p);
// 
// 	CString sDisp;
// 	sDisp.Format("%d / %d", nCurr+1, nTot);
// 	//cdc->SetBkMode(TRANSPARENT);
// 	cdc->TextOut(20, 20, "sDisp", 27);
// 	//cdc->SetBkMode(OPAQUE);
// 	cdc->SelectObject(op);

}

/*
BOOL CFileFormatRs274X::DrawObject(UINT nObjIndex)	//MyObjEntity
{
	//m_csDrawing.Lock();
    byte dType;
	float  dX, dY, fRadius;
	FPOINTC fptCenter;

	vector <FPOINTC> vecCoord;

	int pNum = 0, i;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;
	FPOINTC *fptCoord;

	GraphObj gObj = m_pLayerInfo->listObj.at(nObjIndex);
		
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
		fParam = &m_pAptDef->Param.at(0).fParam[0];


		if (m_pAptDef->chStdAptType == _T('C')) // Then		
		{		   
			dX = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
			Key.Param[0] = (dX*dX + dY*dY); //length
			Key.Param[1] = m_pAptDef->Param.at(0).fParam[0];// line width

			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID = SearchLineTemplate(Key.key);
				if(nID > -1)
				{
					nID += m_nListLines;//m_glListLines;
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					MyLineList(X1, Y1, X2, Y2, nID);
				}
				else
				{
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					DrawLineRound(X1, Y1, X2, Y2, fParam[0]);
				}
			}
			else
 				DrawLineRound(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0]);
		}		
		else if (m_pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0], fParam[1]);
		}		
		else if(m_pAptDef->strAptName.GetLength() > 0) 
		{
			if(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
				MyLineMacro(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, m_pAptDef->minmax);
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, gObj.Type.nPolarity);
			}
		}
		break;	
	case DRAW_TYPE::FLASH: // 'Flash : bType[1]
		m_nApertureListIndex = m_pAptDef->nGlListIndex;

		fParam = &m_pAptDef->Param.at(0).fParam[0];

		if(m_nApertureListIndex>0)
		{
			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
 				MyCallApertureList(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,m_nApertureListIndex);
				m_csDrawing.Unlock();
				return FALSE;
			}
		}
		
		pNum = m_pAptDef->Param.at(0).fParam.size();
		
		if(m_pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{
			if (pNum == 1)
			{
				fRadius = fParam[0] / 2.0;
				DrawCircle(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if (pNum > 2 && pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
//				SetStencil(FALSE);
				
				if(pNum == 3)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], 0);
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], fParam[3]);
				}	
//				SetStencil(TRUE);
				DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('O'))	//    ' Oval
		{
			if(pNum < 3)
			{
				MyOval(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
			
				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyOval (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('P'))	//    ' Polygon
		{
			int nSide = fParam[1];
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]);      
			}
			else 
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
		 
				if (pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius);
				}
				else if (pNum >= 5)
				{
					 MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[3], fParam[4], 0);
				}
//				 SetStencil(TRUE);
				 MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]); 
//				 glDisable(GL_STENCIL_TEST);
			}
		 }
		else //    ' Userdefined Apperture
		{
			if(m_nApertureListIndex< 1)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);		
			}
			else
			{
				CallList(m_nApertureListIndex);
// 				glPushMatrix();
// 				glTranslatef(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,0);
// 				glCallList(m_nApertureListIndex);
// 				glPopMatrix();	
			}
			
		
		}
		break;
	case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ: // bType[5], bType[6] 'PolygonFill with G74, G75
		MyQuadrantPolygon(&gObj,&m_pLayerInfo->listObj.at(nObjIndex+1));
		break;        
	case DRAW_TYPE::SNR:	// bType[7]
		MySnRObjList(gObj, nObjIndex, 0, 0);
		break;
	case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y,
					m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
					m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
					fParam[0], CW);
		break;
	case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
						fParam[0], CCW);
		break;
	case DRAW_TYPE::TEXT:
		fParam = &m_pAptDef->Param.at(0).fParam[0];
		fptCenter = m_pLayerInfo->vecObjCoord[gObj.nStPnt];

		GetCannedText(m_pAptDef->strAptName, fptCenter, fParam[0]*1.2, vecCoord);
		
		m_nApertureListIndex = m_pAptDef->nGlListIndex;
		
		nIndex = m_pAptDef->Param.at(0).nPriNum;

		if(m_nApertureListIndex>0)
		{
			fptCoord = &vecCoord[0];
			for(i = 0 ; i < vecCoord.size(); i++)
			{
				if(nIndex == 98)
					m_pTrUtil->RotatePoint(fptCoord[i], 0.0, 1.0, fptCenter.x, fptCenter.y);
				MyCallApertureList(fptCoord[i].x, fptCoord[i].y,m_nApertureListIndex);
			}
			m_csDrawing.Unlock();
			return FALSE;
		}
		break;
	}

	//m_csDrawing.Unlock();
	return FALSE;
}

void CFileFormatRs274X::GetCannedText(CString strCannedText, FPOINTC fptStart, float fDistance, vector <FPOINTC> &vecCoord)
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

*//*

int CFileFormatRs274X::SearchLineTemplate(UINT64 nVal)
{
	vector <UINT64>::iterator iter;
	iter = std::lower_bound(m_vecLineTemplate.begin(), m_vecLineTemplate.end(),nVal);
	
	if(iter != m_vecLineTemplate.end() && *iter == nVal)
		return(iter - m_vecLineTemplate.begin());

	return -1;
}

COLORREF CFileFormatRs274X::GetObjectColor(GraphObj &obj)
{
	COLORREF crColor;

	m_nPrevAttribute = obj.Type.nAttr;
	m_nPrevPolarity = obj.Type.nPolarity;

	m_pLayerInfo->mapObjColor.Lookup((DRAW_TYPE::ATTRIBUTE)obj.Type.nAttr, crColor);
	if (m_pLayerInfo->bPolarity == LAYER_TYPE::POSITIVE)
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

void CFileFormatRs274X::SetCanvasPixelResolution()
{
// 	CfPoint3D fpt;
// 	CfPoint3D fptObj1,fptObj2;
// 	CfPoint3D fptWin1,fptWin2;
	double fObjDistanceX,fObjDistanceY;
// 	double fRes;
	
	fObjDistanceX = m_sizeWindow.cx;
	fObjDistanceY = m_sizeWindow.cy;
	
// 	fptObj1.x=0.0;
// 	fptObj1.y=0.0;
// 	fptObj1.z=0.0f;
// 	fptObj2.x=fObjDistanceX;
// 	fptObj2.y=fObjDistanceY;
// 	fptObj2.z=0.0f;

// 	wglMakeCurrent(m_hDC, m_hRC);

// 	double modelMatrix[16];
// 	double projMatrix[16];
// 	int	viewport[4];

// 	glPushMatrix();
// 	glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
// 	glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
// 	glGetIntegerv(GL_VIEWPORT,viewport);
// 
// 	glTranslated(m_fptCamPos.x,m_fptCamPos.y,m_fptCamPos.z);
// 
// 	// transforms the specified object coordinates into window coordinates using modelMatrix, projMatrix, and viewport
// 	gluProject(fptObj1.x, fptObj1.y, fptObj1.z, modelMatrix, projMatrix, viewport, &fptWin1.x,&fptWin1.y,&fptWin1.z);
// 	gluProject(fptObj2.x, fptObj2.y, fptObj2.z, modelMatrix, projMatrix, viewport, &fptWin2.x,&fptWin2.y,&fptWin2.z);	
// 	
// 	glPopMatrix();

// 	fRes =  fObjDistanceX/(fptWin2.x-fptWin1.x);
// 	fRes += fObjDistanceY/(fptWin2.y-fptWin1.y);

// 	m_fCanvasPixelResolution = fRes/2.0;
	m_fCanvasPixelResolution = 1.0;
}

void CFileFormatRs274X::GetArcVertices(vector<FPOINTC> &vecPt, const FPOINTC &ptCenter, float radius, float start_angle, float arc_angle, int dir)
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

void CFileFormatRs274X::GetArcVertices(vector<FPOINT> &vecPt, const FPOINT &ptCenter, double radius, double start_angle, double arc_angle,int dir)
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





*//*


BOOL CFileFormatRs274X::DrawObject_0(UINT nObjIndex)	//MyObjEntity
{
// 	m_csDrawing.Lock();
    byte dType;
	float  dX, dY, fRadius;
	FPOINTC fptCenter;

	vector <FPOINTC> vecCoord;

	int pNum = 0, i;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;
	FPOINTC *fptCoord;

	GraphObj gObj = m_pLayerInfo->listObj.at(nObjIndex);
		
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
		fParam = &m_pAptDef->Param.at(0).fParam[0];


		if (m_pAptDef->chStdAptType == _T('C')) // Then		
		{		   
			dX = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
			Key.Param[0] = (dX*dX + dY*dY); //length
			Key.Param[1] = m_pAptDef->Param.at(0).fParam[0];// line width

			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID = SearchLineTemplate(Key.key);
				if(nID > -1)
				{
					nID += m_nListLines;//m_glListLines;
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					MyLineList(X1, Y1, X2, Y2, nID);
				}
				else
				{
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					DrawLineRound(X1, Y1, X2, Y2, fParam[0]);
				}
			}
			else
 				DrawLineRound(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0]);
		}		
		else if (m_pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0], fParam[1]);
		}		
		else if(m_pAptDef->strAptName.GetLength() > 0) 
		{
			if(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
				MyLineMacro(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, m_pAptDef->minmax);
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, gObj.Type.nPolarity);
			}
		}
		break;	
	case DRAW_TYPE::FLASH: // 'Flash : bType[1]
		m_nApertureListIndex = m_pAptDef->nGlListIndex;

		fParam = &m_pAptDef->Param.at(0).fParam[0];

		if(m_nApertureListIndex>0)
		{
			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
 				MyCallApertureList(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,m_nApertureListIndex);
				m_csDrawing.Unlock();
				return FALSE;
			}
		}
		
		pNum = m_pAptDef->Param.at(0).fParam.size();
		
		if(m_pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{
			if (pNum == 1)
			{
				fRadius = fParam[0] / 2.0;
				DrawCircle(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if (pNum > 2 && pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
//				SetStencil(FALSE);
				
				if(pNum == 3)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], 0);
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], fParam[3]);
				}	
//				SetStencil(TRUE);
				DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('O'))	//    ' Oval
		{
			if(pNum < 3)
			{
				MyOval(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
			
				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyOval (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('P'))	//    ' Polygon
		{
			int nSide = fParam[1];
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]);      
			}
			else 
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
		 
				if (pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius);
				}
				else if (pNum >= 5)
				{
					 MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[3], fParam[4], 0);
				}
//				 SetStencil(TRUE);
				 MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]); 
//				 glDisable(GL_STENCIL_TEST);
			}
		 }
		else //    ' Userdefined Apperture
		{
			if(m_nApertureListIndex< 1)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);		
			}
			else
			{
				CallList(m_nApertureListIndex);
// 				glPushMatrix();
// 				glTranslatef(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,0);
// 				glCallList(m_nApertureListIndex);
// 				glPopMatrix();	
			}
			
		
		}
		break;
	case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ: // bType[5], bType[6] 'PolygonFill with G74, G75
		MyQuadrantPolygon(&gObj,&m_pLayerInfo->listObj.at(nObjIndex+1));
		break;        
	case DRAW_TYPE::SNR:	// bType[7]
		MySnRObjList(gObj, nObjIndex, 0, 0);
		break;
	case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y,
					m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
					m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
					fParam[0], CW);
		break;
	case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
						fParam[0], CCW);
		break;
	case DRAW_TYPE::TEXT:
		fParam = &m_pAptDef->Param.at(0).fParam[0];
		fptCenter = m_pLayerInfo->vecObjCoord[gObj.nStPnt];

		GetCannedText(m_pAptDef->strAptName, fptCenter, fParam[0]*1.2, vecCoord);
		
		m_nApertureListIndex = m_pAptDef->nGlListIndex;
		
		nIndex = m_pAptDef->Param.at(0).nPriNum;

		if(m_nApertureListIndex>0)
		{
			fptCoord = &vecCoord[0];
			for(i = 0 ; i < vecCoord.size(); i++)
			{
				if(nIndex == 98)
					m_pTrUtil->RotatePoint(fptCoord[i], 0.0, 1.0, fptCenter.x, fptCenter.y);
				MyCallApertureList(fptCoord[i].x, fptCoord[i].y,m_nApertureListIndex);
			}
			m_csDrawing.Unlock();
			return FALSE;
		}
		break;
	}

// 	m_csDrawing.Unlock();
	return FALSE;
}


BOOL CFileFormatRs274X::DrawObject_1(UINT nObjIndex)	//MyObjEntity
{
// 	m_csDrawing.Lock();
    byte dType;
	float  dX, dY, fRadius;
	FPOINTC fptCenter;

	vector <FPOINTC> vecCoord;

	int pNum = 0, i;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;
	FPOINTC *fptCoord;

	GraphObj gObj = m_pLayerInfo->listObj.at(nObjIndex);
		
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
		fParam = &m_pAptDef->Param.at(0).fParam[0];


		if (m_pAptDef->chStdAptType == _T('C')) // Then		
		{		   
			dX = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
			Key.Param[0] = (dX*dX + dY*dY); //length
			Key.Param[1] = m_pAptDef->Param.at(0).fParam[0];// line width

			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID = SearchLineTemplate(Key.key);
				if(nID > -1)
				{
					nID += m_nListLines;//m_glListLines;
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					MyLineList(X1, Y1, X2, Y2, nID);
				}
				else
				{
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					DrawLineRound(X1, Y1, X2, Y2, fParam[0]);
				}
			}
			else
 				DrawLineRound(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0]);
		}		
		else if (m_pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0], fParam[1]);
		}		
		else if(m_pAptDef->strAptName.GetLength() > 0) 
		{
			if(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
				MyLineMacro(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, m_pAptDef->minmax);
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, gObj.Type.nPolarity);
			}
		}
		break;	*//*
	case DRAW_TYPE::FLASH: // 'Flash : bType[1]
		m_nApertureListIndex = m_pAptDef->nGlListIndex;

		fParam = &m_pAptDef->Param.at(0).fParam[0];

		if(m_nApertureListIndex>0)
		{
			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
 				MyCallApertureList(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,m_nApertureListIndex);
				m_csDrawing.Unlock();
				return FALSE;
			}
		}
		
		pNum = m_pAptDef->Param.at(0).fParam.size();
		
		if(m_pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{
			if (pNum == 1)
			{
				fRadius = fParam[0] / 2.0;
				DrawCircle(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if (pNum > 2 && pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
//				SetStencil(FALSE);
				
				if(pNum == 3)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], 0);
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], fParam[3]);
				}	
//				SetStencil(TRUE);
				DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('O'))	//    ' Oval
		{
			if(pNum < 3)
			{
				MyOval(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
			
				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyOval (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('P'))	//    ' Polygon
		{
			int nSide = fParam[1];
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]);      
			}
			else 
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
		 
				if (pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius);
				}
				else if (pNum >= 5)
				{
					 MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[3], fParam[4], 0);
				}
//				 SetStencil(TRUE);
				 MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]); 
//				 glDisable(GL_STENCIL_TEST);
			}
		 }
		else //    ' Userdefined Apperture
		{
			if(m_nApertureListIndex< 1)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);		
			}
			else
			{
				CallList(m_nApertureListIndex);
// 				glPushMatrix();
// 				glTranslatef(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,0);
// 				glCallList(m_nApertureListIndex);
// 				glPopMatrix();	
			}
			
		
		}
		break;
	case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ: // bType[5], bType[6] 'PolygonFill with G74, G75
		MyQuadrantPolygon(&gObj,&m_pLayerInfo->listObj.at(nObjIndex+1));
		break;        
	case DRAW_TYPE::SNR:	// bType[7]
		MySnRObjList(gObj, nObjIndex, 0, 0);
		break;
	case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y,
					m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
					m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
					fParam[0], CW);
		break;
	case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
						fParam[0], CCW);
		break;
	case DRAW_TYPE::TEXT:
		fParam = &m_pAptDef->Param.at(0).fParam[0];
		fptCenter = m_pLayerInfo->vecObjCoord[gObj.nStPnt];

		GetCannedText(m_pAptDef->strAptName, fptCenter, fParam[0]*1.2, vecCoord);
		
		m_nApertureListIndex = m_pAptDef->nGlListIndex;
		
		nIndex = m_pAptDef->Param.at(0).nPriNum;

		if(m_nApertureListIndex>0)
		{
			fptCoord = &vecCoord[0];
			for(i = 0 ; i < vecCoord.size(); i++)
			{
				if(nIndex == 98)
					m_pTrUtil->RotatePoint(fptCoord[i], 0.0, 1.0, fptCenter.x, fptCenter.y);
				MyCallApertureList(fptCoord[i].x, fptCoord[i].y,m_nApertureListIndex);
			}
			m_csDrawing.Unlock();
			return FALSE;
		}
		break;
	}

// 	m_csDrawing.Unlock();
	return FALSE;
}

*//*
BOOL CFileFormatRs274X::DrawObject_2(UINT nObjIndex)	//MyObjEntity
{
// 	m_csDrawing.Lock();
    byte dType;
	float  dX, dY, fRadius;
	FPOINTC fptCenter;

	vector <FPOINTC> vecCoord;

	int pNum = 0, i;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;
	FPOINTC *fptCoord;

	GraphObj gObj = m_pLayerInfo->listObj.at(nObjIndex);
		
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
		fParam = &m_pAptDef->Param.at(0).fParam[0];


		if (m_pAptDef->chStdAptType == _T('C')) // Then		
		{		   
			dX = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
			Key.Param[0] = (dX*dX + dY*dY); //length
			Key.Param[1] = m_pAptDef->Param.at(0).fParam[0];// line width

			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID = SearchLineTemplate(Key.key);
				if(nID > -1)
				{
					nID += m_nListLines;//m_glListLines;
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					MyLineList(X1, Y1, X2, Y2, nID);
				}
				else
				{
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					DrawLineRound(X1, Y1, X2, Y2, fParam[0]);
				}
			}
			else
 				DrawLineRound(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0]);
		}		
		else if (m_pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0], fParam[1]);
		}		
		else if(m_pAptDef->strAptName.GetLength() > 0) 
		{
			if(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
				MyLineMacro(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, m_pAptDef->minmax);
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, gObj.Type.nPolarity);
			}
		}
		break;	
	case DRAW_TYPE::FLASH: // 'Flash : bType[1]
		m_nApertureListIndex = m_pAptDef->nGlListIndex;

		fParam = &m_pAptDef->Param.at(0).fParam[0];

		if(m_nApertureListIndex>0)
		{
			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
 				MyCallApertureList(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,m_nApertureListIndex);
				m_csDrawing.Unlock();
				return FALSE;
			}
		}
		
		pNum = m_pAptDef->Param.at(0).fParam.size();
		
		if(m_pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{
			if (pNum == 1)
			{
				fRadius = fParam[0] / 2.0;
				DrawCircle(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if (pNum > 2 && pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
//				SetStencil(FALSE);
				
				if(pNum == 3)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], 0);
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], fParam[3]);
				}	
//				SetStencil(TRUE);
				DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('O'))	//    ' Oval
		{
			if(pNum < 3)
			{
				MyOval(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
			
				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyOval (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('P'))	//    ' Polygon
		{
			int nSide = fParam[1];
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]);      
			}
			else 
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
		 
				if (pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius);
				}
				else if (pNum >= 5)
				{
					 MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[3], fParam[4], 0);
				}
//				 SetStencil(TRUE);
				 MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]); 
//				 glDisable(GL_STENCIL_TEST);
			}
		 }
		else //    ' Userdefined Apperture
		{
			if(m_nApertureListIndex< 1)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);		
			}
			else
			{
				CallList(m_nApertureListIndex);
// 				glPushMatrix();
// 				glTranslatef(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,0);
// 				glCallList(m_nApertureListIndex);
// 				glPopMatrix();	
			}
			
		
		}
		break;
	case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ: // bType[5], bType[6] 'PolygonFill with G74, G75
		MyQuadrantPolygon(&gObj,&m_pLayerInfo->listObj.at(nObjIndex+1));
		break;        
	case DRAW_TYPE::SNR:	// bType[7]
		MySnRObjList(gObj, nObjIndex, 0, 0);
		break;
	case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y,
					m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
					m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
					fParam[0], CW);
		break;
	case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
						fParam[0], CCW);
		break;
	case DRAW_TYPE::TEXT:
		fParam = &m_pAptDef->Param.at(0).fParam[0];
		fptCenter = m_pLayerInfo->vecObjCoord[gObj.nStPnt];

		GetCannedText(m_pAptDef->strAptName, fptCenter, fParam[0]*1.2, vecCoord);
		
		m_nApertureListIndex = m_pAptDef->nGlListIndex;
		
		nIndex = m_pAptDef->Param.at(0).nPriNum;

		if(m_nApertureListIndex>0)
		{
			fptCoord = &vecCoord[0];
			for(i = 0 ; i < vecCoord.size(); i++)
			{
				if(nIndex == 98)
					m_pTrUtil->RotatePoint(fptCoord[i], 0.0, 1.0, fptCenter.x, fptCenter.y);
				MyCallApertureList(fptCoord[i].x, fptCoord[i].y,m_nApertureListIndex);
			}
			m_csDrawing.Unlock();
			return FALSE;
		}
		break;
	}

// 	m_csDrawing.Unlock();
	return FALSE;
}
*//*

BOOL CFileFormatRs274X::DrawObject_3(UINT nObjIndex)	//MyObjEntity
{
// 	m_csDrawing.Lock();
    byte dType;
	float  dX, dY, fRadius;
	FPOINTC fptCenter;

	vector <FPOINTC> vecCoord;

	int pNum = 0, i;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;
	FPOINTC *fptCoord;

	GraphObj gObj = m_pLayerInfo->listObj.at(nObjIndex);
		
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
		fParam = &m_pAptDef->Param.at(0).fParam[0];


		if (m_pAptDef->chStdAptType == _T('C')) // Then		
		{		   
			dX = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
			Key.Param[0] = (dX*dX + dY*dY); //length
			Key.Param[1] = m_pAptDef->Param.at(0).fParam[0];// line width

			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID = SearchLineTemplate(Key.key);
				if(nID > -1)
				{
					nID += m_nListLines;//m_glListLines;
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					MyLineList(X1, Y1, X2, Y2, nID);
				}
				else
				{
					X1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x; Y1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
					X2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x; Y2 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;
					DrawLineRound(X1, Y1, X2, Y2, fParam[0]);
				}
			}
			else
 				DrawLineRound(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0]);
		}		
		else if (m_pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, fParam[0], fParam[1]);
		}		
		else if(m_pAptDef->strAptName.GetLength() > 0) 
		{
			if(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);
				MyLineMacro(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, m_pAptDef->minmax);
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, gObj.Type.nPolarity);
			}
		}
		break;	
	case DRAW_TYPE::FLASH: // 'Flash : bType[1]
		m_nApertureListIndex = m_pAptDef->nGlListIndex;

		fParam = &m_pAptDef->Param.at(0).fParam[0];

		if(m_nApertureListIndex>0)
		{
			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
 				MyCallApertureList(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,m_nApertureListIndex);
				m_csDrawing.Unlock();
				return FALSE;
			}
		}
		
		pNum = m_pAptDef->Param.at(0).fParam.size();
		
		if(m_pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{
			if (pNum == 1)
			{
				fRadius = fParam[0] / 2.0;
				DrawCircle(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if (pNum > 2 && pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
//				SetStencil(FALSE);
				
				if(pNum == 3)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], 0);
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[1], fParam[2], fParam[3]);
				}	
//				SetStencil(TRUE);
				DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1], 0);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('O'))	//    ' Oval
		{
			if(pNum < 3)
			{
				MyOval(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
			
				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[2], fParam[3], 0);
				}
//				SetStencil(TRUE);
				MyOval (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[1]);
//				glDisable(GL_STENCIL_TEST);
			}
		}
		else if(m_pAptDef->chStdAptType == _T('P'))	//    ' Polygon
		{
			int nSide = fParam[1];
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]);      
			}
			else 
			{
// 				glClear(GL_STENCIL_BUFFER_BIT);
// 				glEnable(GL_STENCIL_TEST);
// 				SetStencil(FALSE);
		 
				if (pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fRadius);
				}
				else if (pNum >= 5)
				{
					 MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[3], fParam[4], 0);
				}
//				 SetStencil(TRUE);
				 MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, fParam[0], fParam[2]); 
//				 glDisable(GL_STENCIL_TEST);
			}
		 }
		else //    ' Userdefined Apperture
		{
			if(m_nApertureListIndex< 1)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, gObj.Type.nPolarity);		
			}
			else
			{
				CallList(m_nApertureListIndex);
// 				glPushMatrix();
// 				glTranslatef(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y,0);
// 				glCallList(m_nApertureListIndex);
// 				glPopMatrix();	
			}
			
		
		}
		break;
	case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ: // bType[5], bType[6] 'PolygonFill with G74, G75
		MyQuadrantPolygon(&gObj,&m_pLayerInfo->listObj.at(nObjIndex+1));
		break;        
	case DRAW_TYPE::SNR:	// bType[7]
		MySnRObjList(gObj, nObjIndex, 0, 0);
		break;
	case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y,
					m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
					m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
					fParam[0], CW);
		break;
	case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &m_pAptDef->Param.at(0).fParam[0];

		MyArc (m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y, 
						m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x, m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y, 
						fParam[0], CCW);
		break;
	case DRAW_TYPE::TEXT:
		fParam = &m_pAptDef->Param.at(0).fParam[0];
		fptCenter = m_pLayerInfo->vecObjCoord[gObj.nStPnt];

		GetCannedText(m_pAptDef->strAptName, fptCenter, fParam[0]*1.2, vecCoord);
		
		m_nApertureListIndex = m_pAptDef->nGlListIndex;
		
		nIndex = m_pAptDef->Param.at(0).nPriNum;

		if(m_nApertureListIndex>0)
		{
			fptCoord = &vecCoord[0];
			for(i = 0 ; i < vecCoord.size(); i++)
			{
				if(nIndex == 98)
					m_pTrUtil->RotatePoint(fptCoord[i], 0.0, 1.0, fptCenter.x, fptCenter.y);
				MyCallApertureList(fptCoord[i].x, fptCoord[i].y,m_nApertureListIndex);
			}
			m_csDrawing.Unlock();
			return FALSE;
		}
		break;
	}

// 	m_csDrawing.Unlock();
	return FALSE;
}




void CFileFormatRs274X::CallList(int nListIndex)
{

}

void CFileFormatRs274X::DrawConvexPolygon(const vector <FPOINTC> &fPoint)
{
//	glBegin(GL_POLYGON);

	CDC* cdc = CDC::FromHandle(m_hDC);

	LOGBRUSH lb; 
	lb.lbStyle = PS_SOLID;
	lb.lbColor = RGB(128, 128, 128);

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
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
// 
// 
// 	for( i=1; i<nNumOfVertex; i++)
// 	{
// // 		glVertex2f(fPoint.at(i).x,fPoint.at(i).y);			
// 		fPosCam = fPoint.at(i);
// 		fPosWin = ConvertWindowCoord(fPosCam);
// 		cdc->LineTo(fPosWin.x, fPosWin.y);
// 	}
// 

//	cdc->BeginPath();

	CPoint *pts = new CPoint[nNumOfVertex];
	for( i=0; i<nNumOfVertex; i++)
	{
		fPosCam = fPoint.at(i);
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
//	cdc->EndPath();

// 	int nNumPts;
// 	nNumPts = cdc->GetPath(NULL, NULL, 0);
// 	LPPOINT lpPoints = new POINT[nNumPts];
// 	LPBYTE lpTypes = new BYTE[nNumPts];
// 	nNumPts = cdc->GetPath(lpPoints, lpTypes, nNumPts);

// 	cdc->SetBkMode(OPAQUE);

//	cdc->SetPolyFillMode(WINDING);//ALTERNATE
	cdc->Polygon(pts, nNumOfVertex);
	delete[] pts;

// 	cdc->SetBkMode(TRANSPARENT);



//	cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);


// 	glEnd();
}

*//*

void CFileFormatRs274X::DrawLineRect(float fSx, float fSy, float fEx, float fEy, float fLineWidth, float fLineHeight )
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

void CFileFormatRs274X::DrawConvexPolygon(const FPOINTC *fPoint,int nNumberOfVertex)
{
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, 1, RGB(0, 0, 255));
	CPen* op = cdc->SelectObject(&p);

	int i;
	if(nNumberOfVertex<1)
		return;

	FPOINTC fPosWin, fPosCam;
	fPosCam = fPoint[0];
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

	CPoint *pts = new CPoint[nNumberOfVertex];
	for( i=0; i<nNumberOfVertex; i++)
	{
		fPosCam = fPoint[i];
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	cdc->Polygon(pts, nNumberOfVertex);
	delete[] pts;

	cdc->SelectObject(op);
// 
// 
// //	glBegin(GL_POLYGON);
// 	int i;
// 	for( i=0; i<nNumberOfVertex; i++)
// 	{
// 		;
// //		glVertex2f(fPoint[i].x, fPoint[i].y);
// 	}
// //	glEnd();
// 
}


BOOL CFileFormatRs274X::MyMacro(const CFileFormatRs274X::LPAPERTURE_DEF &pAptDef, float x, float y, byte Pol)
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
		m_pAptDef->nGlListIndex = nParam; // 20170519-syd

// 	glClear(GL_STENCIL_BUFFER_BIT);
// 	glEnable(GL_STENCIL_TEST);

	cdc = CDC::FromHandle(m_hDC);
	p.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
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
				fPosWin = ConvertWindowCoord(fPosCam);
				cdc->MoveTo(fPosWin.x, fPosWin.y);

				pts = new CPoint[nNumOfVertex];
				for( i=0; i<nNumOfVertex; i++)
				{
					fPosCam = vecVertics.at(i);
// 					fPosCam = m_vecVertics.at(i);
					fPosWin = ConvertWindowCoord(fPosCam);
					pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
				}
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
				fPosWin = ConvertWindowCoord(fPosCam);
				cdc->MoveTo(fPosWin.x, fPosWin.y);

				pts = new CPoint[nNumOfVertex];
				for( i=0; i<nNumOfVertex; i++)
				{
					fPosCam = vecVertics.at(i);
// 					fPosCam = m_vecVertics.at(i);
					fPosWin = ConvertWindowCoord(fPosCam);
					pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
				}
				cdc->Polygon(pts, nNumOfVertex);
				delete[] pts;
			}
			break;
		}
	}
// 	glDisable(GL_STENCIL_TEST);
	cdc->SelectObject(op);

	return FALSE;
}

BOOL CFileFormatRs274X::MyRectLowerLeft(float fSx, float fSy, float fLineWidth, float fLineHeight, float fAngle)
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

BOOL CFileFormatRs274X::MyLine(float fSx, float fSy, float fEx, float fEy, float fLineWidth)
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
*//*
BOOL CFileFormatRs274X::MyThermal(float x, float y, float Dmo, float Dmi, float Ang, int num,  float gap, float angle)
{
    float   Ro, Ri, Theta1, Theta2, t, cV1, cV2, sV1, sV2;
	FPOINTC pp[9];
    int i;
  	FPOINTC fpt = FPOINTC(x, y);  

	if(m_nApertureListIndex==-1)
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
	else
	{
		CallList(m_nApertureListIndex);
// 		glPushMatrix();
// 		glTranslatef(x,y,0.0);
// 		glCallList(m_nApertureListIndex);
// 		glPopMatrix();
	}  

	return FALSE;
}

BOOL CFileFormatRs274X::MyPolygon(const FPOINTC *fCoord, int nVertex)
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

BOOL CFileFormatRs274X::MyLineMacro(float fSx, float fSy, float fEx, float fEy, FRECTC fExtent)
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
*//*
int CFileFormatRs274X::GetArcSegment(float fRadius, float fAngle)
{
	if(fAngle<0)
		fAngle=-fAngle;
	
	double fOptSeg = 100*sqrt(fRadius);
	fOptSeg *= (fAngle/360.0);
	
	double fResSeg = (2.0*PI*fRadius) / m_fDrawResolution;
	fResSeg *= (fAngle/360.0);
	
	double fMinSeg = max(2.0, fAngle/60.0);
	double fSeg = max(fMinSeg, min(fOptSeg, fResSeg));
	int nSeg = ceil(fSeg);

	return nSeg;
}

void CFileFormatRs274X::DrawCircle(float cx, float cy, float r, int fill) 
{ 
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushRed(RGB(255, 0, 0));   
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
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

//	cdc->BeginPath();
	CPoint *pts = new CPoint[num_segments];
	for(i = 0; i < num_segments; i++)
	{ 
		tx = -y; 
		ty = x; 
		x += tx * tangetial_factor; 
		y += ty * tangetial_factor; 
		x *= radial_factor; 
		y *= radial_factor; 

		fPosCam.x = x + cx;
		fPosCam.y = y + cy;
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	} 
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
	cdc->Polygon(pts, num_segments);
	delete[] pts;

	if(fill)	// filled circle
		cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);

// 
// 
// // 	if(fill)
// // 		glBegin(GL_POLYGON); // filled circle
// // 	else
// // 		glBegin(GL_LINE_LOOP);// hollow circle since the circle is a closed curve, this is a loop now
// 
// 	for(ii = 0; ii < num_segments; ii++)
// 	{ 
// //		glVertex3f(x + cx, y + cy,0.0f);
// 
// 		tx = -y; 
// 		ty = x; 
// 
// 
// 		x += tx * tangetial_factor; 
// 		y += ty * tangetial_factor; 
// 
// 		x *= radial_factor; 
// 		y *= radial_factor; 
// 	} 
// //	glEnd(); 
//
}

void CFileFormatRs274X::DrawDonut(float fCx, float fCy, float fDmo, float fDmi)
{ 
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, 1, RGB(255, 0, 0));
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

	FPOINTC fPosWin, fPosCam;
	fPosCam.x = x2;
	fPosCam.y = y2;
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

	CPoint *pts = new CPoint[segments+1];
	for(i=0; i<segments+1; i++)
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
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	cdc->Polygon(pts, segments+1);

	fPosCam.x = x1;
	fPosCam.y = y1;
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

	for(i=0; i<segments+1; i++)
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
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	cdc->Polygon(pts, segments+1);

	fPosCam.x = x2;
	fPosCam.y = y2;
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
	for(i=0; i<segments+1; i++)
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
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	cdc->Polygon(pts, segments+1);

	delete[] pts;

	cdc->SelectObject(op);

// 
//
// //	glEnable(GL_STENCIL_TEST);
// //	SetStencil(FALSE);
// 
// //		glBegin(GL_POLYGON);
// 	for(i=0; i<segments+1; i++)
// 	{
// // 		glVertex2f(x2, y2);
// 		tx = -(y2 - fCy); 
// 		ty = x2 - fCx; 
// 		x2 += tx * tangetial_factor; 
// 		y2 += ty * tangetial_factor; 
// 		rx = fCx - x2; 
// 		ry = fCy - y2; 
// 		x2 += rx * radial_factor; 
// 		y2 += ry * radial_factor; 
// 	}
// //		glEnd();
// //	SetStencil(TRUE);
// //		glBegin(GL_POLYGON);
// 	for(i=0; i<segments+1; i++)
// 	{
// // 		glVertex2f(x1, y1);
// 		tx = -(y1 - fCy); 
// 		ty = x1 - fCx; 
// 		x1 += tx * tangetial_factor; 
// 		y1 += ty * tangetial_factor; 
// 		rx = fCx - x1; 
// 		ry = fCy - y1; 
// 		x1 += rx * radial_factor; 
// 		y1 += ry * radial_factor; 
// 	}
// // 		glEnd();
// // 	ClearStencil(TRUE);
// // 	glDisable(GL_STENCIL_TEST);
// // 	glBegin(GL_LINE_LOOP);
// 	for(i=0; i<segments+1; i++)
// 	{
// // 		glVertex2f(x2, y2);
// 		tx = -(y2 - fCy); 
// 		ty = x2 - fCx; 
// 		x2 += tx * tangetial_factor; 
// 		y2 += ty * tangetial_factor; 
// 		rx = fCx - x2; 
// 		ry = fCy - y2; 
// 		x2 += rx * radial_factor; 
// 		y2 += ry * radial_factor; 
// 	}
// // 	glEnd();
// 	
// }

void CFileFormatRs274X::SetStencil(BOOL bStencil)
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
*//*
BOOL CFileFormatRs274X::MyRectCenter(float fSx, float fSy, float fLineWidth, float fLineHeight, float fAngle)
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

void CFileFormatRs274X::DrawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int fill)
{	
	CDC* cdc = CDC::FromHandle(m_hDC);
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
		fPosWin[i] = ConvertWindowCoord(fPosCam[i]);
		pts[i].x = fPosWin[i].x; pts[i].y = fPosWin[i].y;
	}
	pts[4].x = fPosWin[0].x; pts[4].y = fPosWin[0].y;
	
	cdc->MoveTo(pts[0].x, pts[0].y);
	cdc->Polygon(pts, 5);
	delete[] pts;

	if(fill)	// filled circle
		cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);

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

void CFileFormatRs274X::DrawRect(const CfPoint3D &fptStart,const CfPoint3D &fptEnd,int width, int fill)
{
	if(width < 1)		width = 1;
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, width, RGB(0, 0, 255));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushBlue(RGB(0, 0, 255));   
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
		fPosWin[i] = ConvertWindowCoord(fPosCam[i]);
		pts[i].x = fPosWin[i].x; pts[i].y = fPosWin[i].y;
	}
	pts[4].x = fPosWin[0].x; pts[4].y = fPosWin[0].y;
	
	cdc->MoveTo(pts[0].x, pts[0].y);
	cdc->Polygon(pts, 5);
	delete[] pts;

	if(fill)	// filled circle
		cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);

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

BOOL CFileFormatRs274X::MyOval(float fCx, float fCy, float fWidth, float fHeight)
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
BOOL CFileFormatRs274X::MyPolygonNSide(int n, float cX, float cY, float d, float ang)   
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
// 
// 
// 	if(m_nApertureListIndex==-1)
// 	{
// 		DrawConcavePolygon(m_vecVertics);
// 	}
// 	else
// 	{
// // 		glPushMatrix();
// // 		glTranslatef(cX,cY,0);
// // 		glCallList(m_nApertureListIndex);
// // 		glPopMatrix();
// 	}
// 
	return FALSE;
}
*//*

BOOL CFileFormatRs274X::MyQuadrantPolygon(CFileFormatRs274X::GraphObj *obj, CFileFormatRs274X::GraphObj *NextObj)
{	
	int i, npp, nDir;
	double	fRadius, fStartAngle, fEndAngle, fArcAngle;
	FPOINTC pPre, pPos, pIJ, fPt;

	vector <FPOINTC> vecVertics;
	vecVertics.clear();
//	m_vecVertics.clear();

  npp = NextObj->nStPnt -obj->nStPnt  - 1;	
	
	pPre = m_pLayerInfo->vecObjCoord[obj->nStPnt];

    if (npp == 1) //     'Circular Area Fill
	{
        fPt = m_pLayerInfo->vecObjCoord[obj->nStPnt+1];    //' Center Point
        fRadius = sqrt((pPre.x - fPt.x) * (pPre.x - fPt.x) + (pPre.y - fPt.y) * (pPre.y - fPt.y));
		DrawCircle(fPt.x, fPt.y, fRadius);
	}  
    else if (npp >= 2)
	{  
        for (i=1 ; i<=npp ; i++)
		{
			fPt = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
            if (fabs(fPt.x) == DetGcode && fabs(fPt.y) == DetGcode)
			{
                if (fPt.x > 0)	//+DetGCode
					nDir = 1;
                else
					nDir = -1;

	            i++;
                pPos = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
                                
                i++;
                pIJ = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
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
                pPos = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
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

BOOL CFileFormatRs274X::MySnRObjList(CFileFormatRs274X::GraphObj &gObj,int nObjIndex, float ddX, float ddY)
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

	frtCurrentExtent = GetFrameExtent();

	nStObj = m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
	nEdObj = m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;
	
	nXRepeat = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x;
	nYRepeat = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y;


	if(gObj.dCode > 0 && m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
	{
		nListIndex = gObj.dCode;

		for(i = 0 ; i < nXRepeat ; i++)
		{
			for(j = 0 ; j < nYRepeat ; j++)
			{
				if(i !=0 || j != 0)
				{
					dX = i*m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x + ddX;
					dY = j*m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y + ddY;
				
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
				dX = i*m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x + ddX;
				dY = j*m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y + ddY;
				
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

BOOL CFileFormatRs274X::MyObjEntityShift(int k, float dX, float dY)
{
	float dX1, dY1;
	float fRadius;

	int pNum = 0;
	int nID=0, nIndex = 0;
	TEMPLATE_KEY Key;
	
	float *fParam;

	GraphObj gObj = m_pLayerInfo->listObj.at(k);

	if(gObj.Type.nAttr == DRAW_TYPE::NONE)
		return FALSE;

// 	if(m_bFindLineObjPickingMode)
// 	{
// 		if(gObj.Type.nType != DRAW_TYPE::LINE)
// 			return FALSE;
// 	}
	

	byte dType = gObj.Type.nType; 

	LPAPERTURE_DEF pAptDef;
	m_pLayerInfo->mapAptList.Lookup(gObj.dCode, pAptDef);


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
			dX1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x - m_pLayerInfo->vecObjCoord[gObj.nStPnt].x;
			dY1 = m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y - m_pLayerInfo->vecObjCoord[gObj.nStPnt].y;

			Key.Param[0] = (dX1*dX1 + dY1*dY1); // thickness
			Key.Param[1] = pAptDef->Param.at(0).fParam[0];// line width

			if(m_fCanvasPixelResolution > m_fDrawArcResolution) // 1/10 of Monitor Pixel resolution
			{
				nID =SearchLineTemplate(Key.key);
				if(nID > 0)
				{
					nID+=m_nListLines;//m_glListLines;										
					MyLineList(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, nID);
				}
				else
					DrawLineRound(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, fParam[0]);
			}
			else
				DrawLineRound(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, fParam[0]);
		}
		else if (pAptDef->chStdAptType == _T('R')) // Then
		{
			DrawLineRect (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, fParam[0], fParam[1]);
		}
		else if(m_pAptDef->strAptName.GetLength() > 0)
		{
			if(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x && m_pLayerInfo->vecObjCoord[gObj.nStPnt].y == m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y)
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, gObj.Type.nPolarity);
			}
			else
			{
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, gObj.Type.nPolarity);
				MyLineMacro(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, m_pAptDef->minmax);
				MyMacro(m_pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, gObj.Type.nPolarity);
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
				MyCallApertureList(m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,m_nApertureListIndex);
				return FALSE;
			}
		}

		if(pAptDef->chStdAptType == _T('C'))	//    ' Circle
		{

			if (pNum == 1)
			{				
				fRadius = fParam[0] / 2.0;
				DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius); 
			}
			else if (pNum == 2)
			{
				DrawDonut (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1]);
			}
			else if (pNum < 5)
			{
				fRadius = fParam[0] / 2.0;

				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//SetStencil(FALSE);
			
				if(pNum == 3)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[1], fParam[2], 0);
				}
				else if(pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[1], fParam[2], fParam[3]);
				}
				//SetStencil(TRUE);
				DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY,fRadius); 
				//glDisable(GL_STENCIL_TEST);
			}
		}
		else if(pAptDef->chStdAptType == _T('R'))	//    ' Rectangle
		{
			if(pNum < 3)
			{
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1], 0);
			}
			else if(pNum < 5)
			{
				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[2], fParam[3], 0);
				}
				//SetStencil(TRUE);
				MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1], 0);
				//glDisable(GL_STENCIL_TEST);
			}
		}
		else if(pAptDef->chStdAptType == _T('O')) //    ' Oval
		{
			if(pNum < 3)
			{
				MyOval (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1]);
			}
			else if(pNum < 5)
			{
				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//SetStencil(FALSE);

				if (pNum == 3)
				{
					fRadius = fParam[2] / 2.0;
						DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius); 
				}
				else if (pNum == 4)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1], 0);
				}
				//SetStencil(TRUE);
				MyOval (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[1]);
				//glDisable(GL_STENCIL_TEST);
			}				
		}
		else if(pAptDef->chStdAptType == _T('P'))
		{
			int nSide = fParam[1];
			
			if(pNum < 4)
			{
				MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[2]);      
			}
			else
			{
				//glClear(GL_STENCIL_BUFFER_BIT);
				//glEnable(GL_STENCIL_TEST);
				//m_GLDraw.SetStencil(FALSE);

				if(pNum == 4)
				{
					fRadius = fParam[3] / 2.0;
					DrawCircle (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fRadius);
				}
				else if (pNum >= 5)
				{
					MyRectCenter (m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[3], fParam[4], 0);
				}
				//SetStencil(TRUE);
				MyPolygonNSide(nSide, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, fParam[0], fParam[2]);  
				//glDisable(GL_STENCIL_TEST);
			}
		}
		else    //    ' Userdefined Apperture
		{ 
			MyMacro(pAptDef, m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, gObj.Type.nPolarity);
        }
		break;
    case DRAW_TYPE::POLYGON_SQ:
	case DRAW_TYPE::POLYGON_MQ:
		MyQuadrantPolygonShift(&gObj,&m_pLayerInfo->listObj.at(k+1), dX, dY);
		break;
    case DRAW_TYPE::ARC_CW: // dType == TYPE_CW
		fParam = &pAptDef->Param.at(0).fParam[0];

		MyArc ( m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, 
					m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, 
					 m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y+dY, 
					fParam[0], CW);
		break;
    case DRAW_TYPE::ARC_CCW: // dType == TYPE_CCW
		fParam = &pAptDef->Param.at(0).fParam[0];

		MyArc ( m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+1].y+dY, 
					m_pLayerInfo->vecObjCoord[gObj.nStPnt].x+dX, m_pLayerInfo->vecObjCoord[gObj.nStPnt].y+dY, 
					 m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].x+dX,  m_pLayerInfo->vecObjCoord[gObj.nStPnt+2].y+dY, 
					fParam[0], CCW);
		break;
	}

	return FALSE;
}
*//*
BOOL CFileFormatRs274X::MyQuadrantPolygonShift(CFileFormatRs274X::GraphObj *obj, CFileFormatRs274X::GraphObj *Nextobj, float dX, float dY)
{
	int i, npp, nDir;
	double	fRadius, fStartAngle, fEndAngle, fArcAngle;
	FPOINTC pPre, pPos, pIJ, fPt;
	vector <FPOINTC> vecVertics;

	vecVertics.clear();
// 	m_vecVertics.clear();

    npp = Nextobj->nStPnt -obj->nStPnt - 1;

	pPre = m_pLayerInfo->vecObjCoord[obj->nStPnt];
	pPre.x += dX;
	pPre.y += dY;

    if (npp == 1) //     'Circular Area Fill
	{
        fPt = m_pLayerInfo->vecObjCoord[obj->nStPnt+1];    //' Center Point
        fRadius = sqrt((pPre.x - fPt.x) * (pPre.x - fPt.x) + (pPre.y - fPt.y) * (pPre.y - fPt.y));
		DrawCircle(fPt.x, fPt.y, fRadius);
	}  
    else if (npp >= 2)
	{
        for (i = 1 ; i<= npp ; i++)
		{
			fPt = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
            if (fabs(fPt.x) == DetGcode && fabs(fPt.y) == DetGcode )
			{
                if (fPt.x > 0)	//+DetGCode
					nDir = 1;
                else
					nDir = -1;

	            i++;
				pPos = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
                pPos.x += dX;
                pPos.y += dY;
                                
                i++;
                pIJ = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
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
				pPos = m_pLayerInfo->vecObjCoord[obj->nStPnt+i];
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

COLORREF CFileFormatRs274X::GetSnRObjectColor(CFileFormatRs274X::GraphObj &obj)
{
	COLORREF crColor;
	m_nPrevAttribute = obj.Type.nAttr;
	m_nPrevPolarity = obj.Type.nPolarity;

	m_pLayerInfo->mapObjColor.Lookup((DRAW_TYPE::ATTRIBUTE)obj.Type.nAttr, crColor);
	if (m_pLayerInfo->bPolarity == LAYER_TYPE::POSITIVE)
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

BOOL CFileFormatRs274X::CheckDrawAllow(const FRECTC &fObjExtent)
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

double CFileFormatRs274X::GetDistance(CfPoint FirstPoint, CfPoint SecondPoint)
{
	return sqrt(pow(SecondPoint.x-FirstPoint.x,2)+pow(SecondPoint.y-FirstPoint.y,2));
}

BOOL CFileFormatRs274X::MyArc(float fSx, float fSy, float fEx, float fEy, float fCX, float fCY, float fWidth, int nDir)
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

void CFileFormatRs274X::DrawArcRound(float cx, float cy, float r, float start_angle, float arc_angle, float width, int dir, int fill) 
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

void CFileFormatRs274X::DrawConcavePolygon(const vector <FPOINTC> &fPoint)
{
//	m_csDrawing.Lock();
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, 1, RGB(0, 0, 255));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushBlue(RGB(255, 0, 0));   
	CBrush* pOldBrush = cdc->SelectObject(&brushBlue);

	int i,nNumOfVertex;
	nNumOfVertex = fPoint.size();
	if(nNumOfVertex<1)
		return;

	FPOINTC fPosWin, fPosCam;
	fPosCam = fPoint.at(0);
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);

	CPoint *pts = new CPoint[nNumOfVertex];
	for( i=0; i<nNumOfVertex; i++)
	{
		fPosCam = fPoint.at(i);
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	cdc->Polygon(pts, nNumOfVertex);
	delete[] pts;

	cdc->SelectObject(op);
	cdc->SelectObject(pOldBrush);
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

void CFileFormatRs274X::MyCallApertureList(float fx, float fy,int nListNum)
{
	CallList(nListNum);
// 	glPushMatrix();
// 	glTranslatef(fx,fy,0);
// 	glCallList(nListNum);
// 	if(!glIsList(nListNum))
// 		AfxMessageBox(_T("list error"));
// 	glPopMatrix();
}


BOOL CFileFormatRs274X::MyMoire(float x, float y, float fOutDiameter, float fRingWidth, float fGap, int nCircle, float fLineWidth, float fLineLegth, float Ang)
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

void CFileFormatRs274X::DrawLineRound(float fSx, float fSy, float fEx, float fEy, float fLineWidth,BOOL bFill)
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


void CFileFormatRs274X::MyLineList(float X1, float Y1, float X2, float Y2,int nListNum)
{
//	m_csDrawing.Lock();
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, 1, RGB(0, 255, 0));
	CPen* op = cdc->SelectObject(&p);

	int i;
	FPOINTC fPosWin, fPosCam;
	fPosCam.x = X1; fPosCam.y = Y1;
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
	fPosCam.x = X2; fPosCam.y = Y2;
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->LineTo(fPosWin.x, fPosWin.y);

	cdc->SelectObject(op);
//	m_csDrawing.Unlock();

// 
// 	float Xc,Yc;
// 	
// 	Xc=((X2+X1)/2);	
// 	Yc=((Y1+Y2)/2);
// 
// 	float fAngleRad = atan2(Y1-Y2,X1-X2);
// 	float fRotate = RAD2DEG*fAngleRad;
// 
// 	glPushMatrix();
// 	glTranslatef(Xc,Yc,0);
// 	glRotatef(fRotate,0.0f, 0.0f, 1.0f);
// 	
// 	glCallList(nListNum);
// 	glPopMatrix();
}
*//*
void CFileFormatRs274X::DrawPolygonLine(const vector <FPOINTC> &vecPt,int nLineWidth)
{
	if(nLineWidth < 1)		nLineWidth = 1;
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, nLineWidth, RGB(128, 128, 128));
	CPen* op = cdc->SelectObject(&p);

//	float lineWidth;
	int i, nNumOfVertex;
	nNumOfVertex = vecPt.size();
	if(nNumOfVertex<1)
		return;

	FPOINTC fPosWin, fPosCam;
	fPosCam = vecPt.at(0);
	fPosWin = ConvertWindowCoord(fPosCam);
	cdc->MoveTo(fPosWin.x, fPosWin.y);
// 	for( i=1; i<nNumOfVertex; i++)
// 	{
// 		cdc->LineTo(vecPt.at(i).x, vecPt.at(i).y);
// 	}
	CPoint *pts = new CPoint[nNumOfVertex];
	for( i=0; i<nNumOfVertex; i++)
	{
		fPosCam = vecPt.at(i);
		fPosWin = ConvertWindowCoord(fPosCam);
		//cdc->LineTo(fPosWin.x, fPosWin.y);
		pts[i].x = fPosWin.x; pts[i].y = fPosWin.y;
	}
	cdc->Polygon(pts, nNumOfVertex);
	delete[] pts;

	cdc->SelectObject(op);

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


void CFileFormatRs274X::MyMakeLineList(float fLength, float w)
{
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
	CPoint *pts = new CPoint[num_segments];

	for(i = 0; i < num_segments; i++)
	{ 
// 		glVertex2f(fRight_x + fEnd.x, fRight_y + fEnd.y);

		tx = -fRight_y; 
		ty = fRight_x; 
		fRight_x += tx * tangetial_factor; 
		fRight_y += ty * tangetial_factor; 
		fRight_x *= radial_factor; 
		fRight_y *= radial_factor; 

		pts[i].x = fRight_x; pts[i].y = fRight_y;
	} 

	fRight_x = 0.0;
	fRight_y = fRadius;
// 	glVertex2f(fRight_x+fEnd.x, fRight_y + fEnd.y);

	float fLeft_x = 0.0;
	float fLeft_y = fRadius;
	for(i = 0; i < num_segments; i++)
	{ 
// 		glVertex2f(fLeft_x + fStart.x, fLeft_y + fStart.y);

		tx = -fLeft_y; 
		ty = fLeft_x; 
		fLeft_x += tx * tangetial_factor; 
		fLeft_y += ty * tangetial_factor; 
		fLeft_x *= radial_factor; 
		fLeft_y *= radial_factor; 

		pts[i].x = fLeft_x; pts[i].y = fLeft_y;
	} 
	fLeft_x = 0.0;
	fLeft_y = -fRadius;
// 	glVertex2f(fLeft_x+fStart.x, fLeft_y + fStart.y);
// 	glEnd(); 

	delete[] pts;
}


*/



void CFileFormatRs274X::AllocCamDrawStruct()
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

void CFileFormatRs274X::SetForeColor(COLORREF crColor)
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

BOOL CFileFormatRs274X::CheckDrawAllow(UINT nObjIndex)
{
	const double fMargin = 0.5;
	BOOL nDrawShow=1;
// 	if(m_fExtentStartPos.x-fMargin>m_pLayerInfo->listObj.at(nObjIndex).Extent.X2 || m_fExtentEndPos.x+fMargin<m_pLayerInfo->listObj.at(nObjIndex).Extent.X1 || 
// 		m_fExtentStartPos.y-fMargin>m_pLayerInfo->listObj.at(nObjIndex).Extent.Y2 || m_fExtentEndPos.y+fMargin<m_pLayerInfo->listObj.at(nObjIndex).Extent.Y1)
// 	{
// 		nDrawShow=0;
// 	}

	return  nDrawShow;
}

void CFileFormatRs274X::DrawRect(const CfRect &rect,int width, int fill)
{
	CDC* cdc = CDC::FromHandle(m_hDC);
	CPen p(PS_SOLID, 1, RGB(0, 0, 255));
	CPen* op = cdc->SelectObject(&p);
	CBrush brushBlue(RGB(0, 0, 255));   
	CBrush* pOldBrush;

	if(fill)	// filled circle
		pOldBrush = cdc->SelectObject(&brushBlue);

	int i;
	FPOINTC fPosWin[4], fPosCam[4];
	CPoint *pts = new CPoint[5];

	fPosCam[0].x = rect.left;	fPosCam[0].y = rect.top;
	fPosCam[1].x = rect.right;	fPosCam[1].y = rect.top;
	fPosCam[2].x = rect.right;	fPosCam[2].y = rect.bottom;
	fPosCam[3].x = rect.left;	fPosCam[3].y = rect.bottom;

	for( i=0; i<4; i++)
	{
		fPosWin[i] = ConvertWindowCoord(fPosCam[i]);
		pts[i].x = fPosWin[i].x; pts[i].y = fPosWin[i].y;
	}
	pts[4].x = fPosWin[0].x; pts[4].y = fPosWin[0].y;
	
	cdc->MoveTo(pts[0].x, pts[0].y);
	cdc->Polygon(pts, 5);
	delete[] pts;

	if(fill)	// filled circle
		cdc->SelectObject(pOldBrush);
	cdc->SelectObject(op);

// 	float lineWidth;
// 	if(fill)
// 	{
// 		glBegin(GL_QUADS); // filled quad
// 
// 		glVertex3f(rect.left,rect.top,0.0f);
// 		glVertex3f(rect.right,rect.top,0.0f);
// 		glVertex3f(rect.right,rect.bottom,0.0f);
// 		glVertex3f(rect.left,rect.bottom,0.0f);
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
// 			glVertex3f(rect.left,rect.top,0.0f);
// 			glVertex3f(rect.right,rect.top,0.0f);
// 			glVertex3f(rect.right,rect.bottom,0.0f);
// 			glVertex3f(rect.left,rect.bottom,0.0f);
// 			glEnd();
// 
// 			glLineWidth(lineWidth);
// 		}
// 		else
// 		{
// 			glBegin(GL_LINE_LOOP); // hollow quad
// 			glVertex3f(rect.left,rect.top,0.0f);
// 			glVertex3f(rect.right,rect.top,0.0f);
// 			glVertex3f(rect.right,rect.bottom,0.0f);
// 			glVertex3f(rect.left,rect.bottom,0.0f);
// 			glEnd();
// 		}
// 	}

}

FPOINTC CFileFormatRs274X::ConvertWindowCoord(FPOINTC& fPoint)
{
	FPOINTC fP, szWin, OriginWin, OriginCam;
	double dMargin = 0.1;
	szWin.x = double(m_sizeWindow.cx) * (1.0-dMargin);
	szWin.y = double(m_sizeWindow.cy) * (1.0-dMargin);
	OriginWin.x = double(m_sizeWindow.cx) * dMargin / 2.0;
	OriginWin.y = double(m_sizeWindow.cy) * dMargin / 2.0;

	FRECTC camRect = GetFrameExtent();
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
// 	fP.y = (double)m_sizeWindow.cy - (fPoint.y * dResolution + Origin.y);	// Coordinate origin is left-bottom.

	return fP;
}

BOOL CFileFormatRs274X::GetDrillPoints(char ch,FPOINTC fptStart,vector<FPOINTC> &vecPt, float fDistance)
{
	/////////////////////////
	////Drill Point Index
	////0  1  2  3  4  5  6
	////7  8  9  10 11 12 13
	////14 15 16 17 18 19 20
	////21 22 23 24 25 26 27
	////28 29 30 31 32 33 34
	////35 36 37 38 39 40 41
	////42 43 44 45 46 47 48
	/////////////////////////
	BOOL bState = FALSE;
	int *pIndex = NULL;
	int nCount;
	vecPt.clear();
	switch(ch)
	{
	case ',':
		bState = TRUE;
		break;
	case 'A':
		nCount = 16;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 23;
		pIndex[8] = 25;
		pIndex[9] = 27;
		pIndex[10] = 28;
		pIndex[11] = 34;
		pIndex[12] = 35;
		pIndex[13] = 41;
		pIndex[14] = 42;
		pIndex[15] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
		break;
	case 'B':
		nCount = 17;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 7;
		pIndex[4] = 13;
		pIndex[5] = 14;
		pIndex[6] = 20;
		pIndex[7] = 21;
		pIndex[8] = 23;
		pIndex[9] = 25;
		pIndex[10] = 28;
		pIndex[11] = 34;
		pIndex[12] = 35;
		pIndex[13] = 41;
		pIndex[14] = 42;
		pIndex[15] = 44;
		pIndex[16] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
		break;
	case 'C':
		nCount = 11;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 21;
		pIndex[6] = 28;
		pIndex[7] = 35;
		pIndex[8] = 41;
		pIndex[9] = 44;
		pIndex[10] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'D':
		nCount = 16;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 7;
		pIndex[4] = 13;
		pIndex[5] = 14;
		pIndex[6] = 20;
		pIndex[7] = 21;
		pIndex[8] = 27;
		pIndex[9] = 28;
		pIndex[10] = 34;
		pIndex[11] = 35;
		pIndex[12] = 41;
		pIndex[13] = 42;
		pIndex[14] = 44;
		pIndex[15] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'E':
		nCount = 15;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 6;
		pIndex[4] = 7;
		pIndex[5] = 14;
		pIndex[6] = 21;
		pIndex[7] = 23;
		pIndex[8] = 25;
		pIndex[9] = 28;
		pIndex[10] = 35;
		pIndex[11] = 42;
		pIndex[12] = 44;
		pIndex[13] = 46;
		pIndex[14] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
		break;
	case 'F':
		nCount = 12;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 6;
		pIndex[4] = 7;
		pIndex[5] = 14;
		pIndex[6] = 21;
		pIndex[7] = 23;
		pIndex[8] = 25;
		pIndex[9] = 28;
		pIndex[10] = 35;
		pIndex[11] = 42;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
		break;
	case 'G':
		nCount = 13;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 21;
		pIndex[6] = 28;
		pIndex[7] = 32;
		pIndex[8] = 34;
		pIndex[9] = 35;
		pIndex[10] = 41;
		pIndex[11] = 44;
		pIndex[12] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'H':
		nCount = 16;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 23;
		pIndex[8] = 25;
		pIndex[9] = 27;
		pIndex[10] = 28;
		pIndex[11] = 34;
		pIndex[12] = 35;
		pIndex[13] = 41;
		pIndex[14] = 42;
		pIndex[15] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'I':
		nCount = 11;
		pIndex = new int[nCount];
		pIndex[0] = 1;
		pIndex[1] = 3;
		pIndex[2] = 5;
		pIndex[3] = 10;
		pIndex[4] = 17;
		pIndex[5] = 24;
		pIndex[6] = 31;
		pIndex[7] = 38;
		pIndex[8] = 43;
		pIndex[9] = 45;
		pIndex[10] = 47;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'J':
		nCount = 9;
		pIndex = new int[nCount];
		pIndex[0] = 6;
		pIndex[1] = 13;
		pIndex[2] = 20;
		pIndex[3] = 27;
		pIndex[4] = 34;
		pIndex[5] = 35;
		pIndex[6] = 41;
		pIndex[7] = 44;
		pIndex[8] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'K':
		nCount = 13;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 7;
		pIndex[3] = 11;
		pIndex[4] = 14;
		pIndex[5] = 16;
		pIndex[6] = 21;
		pIndex[7] = 28;
		pIndex[8] = 30;
		pIndex[9] = 35;
		pIndex[10] = 39;
		pIndex[11] = 42;
		pIndex[12] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'L':
		nCount = 10;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 7;
		pIndex[2] = 14;
		pIndex[3] = 21;
		pIndex[4] = 28;
		pIndex[5] = 35;
		pIndex[6] = 42;
		pIndex[7] = 44;
		pIndex[8] = 46;
		pIndex[9] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'M':
		nCount = 17;
		pIndex = new int[nCount];
		pIndex[0] = 1;
		pIndex[1] = 5;
		pIndex[2] = 7;
		pIndex[3] = 9;
		pIndex[4] = 11;
		pIndex[5] = 13;
		pIndex[6] = 14;
		pIndex[7] = 17;
		pIndex[8] = 20;
		pIndex[9] = 21;
		pIndex[10] = 27;
		pIndex[11] = 28;
		pIndex[12] = 34;
		pIndex[13] = 35;
		pIndex[14] = 41;
		pIndex[15] = 42;
		pIndex[16] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'N':
		nCount = 17;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 16;
		pIndex[6] = 20;
		pIndex[7] = 21;
		pIndex[8] = 24;
		pIndex[9] = 27;
		pIndex[10] = 28;
		pIndex[11] = 32;
		pIndex[12] = 34;
		pIndex[13] = 35;
		pIndex[14] = 41;
		pIndex[15] = 42;
		pIndex[16] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'O':
		nCount = 14;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 27;
		pIndex[8] = 28;
		pIndex[9] = 34;
		pIndex[10] = 35;
		pIndex[11] = 41;
		pIndex[12] = 44;
		pIndex[13] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'P':
		nCount = 13;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 7;
		pIndex[4] = 13;
		pIndex[5] = 14;
		pIndex[6] = 20;
		pIndex[7] = 21;
		pIndex[8] = 23;
		pIndex[9] = 25;
		pIndex[10] = 28;
		pIndex[11] = 35;
		pIndex[12] = 42;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'Q':
		nCount = 16;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 27;
		pIndex[8] = 28;
		pIndex[9] = 34;
		pIndex[10] = 35;
		pIndex[11] = 39;
		pIndex[12] = 41;
		pIndex[13] = 44;
		pIndex[14] = 46;
		pIndex[15] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'R':
		nCount = 16;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 7;
		pIndex[4] = 13;
		pIndex[5] = 14;
		pIndex[6] = 20;
		pIndex[7] = 21;
		pIndex[8] = 23;
		pIndex[9] = 25;
		pIndex[10] = 28;
		pIndex[11] = 32;
		pIndex[12] = 35;
		pIndex[13] = 40;
		pIndex[14] = 42;
		pIndex[15] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'S':
		nCount = 12;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 23;
		pIndex[6] = 25;
		pIndex[7] = 34;
		pIndex[8] = 35;
		pIndex[9] = 41;
		pIndex[10] = 44;
		pIndex[11] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'T':
		nCount = 10;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 6;
		pIndex[4] = 10;
		pIndex[5] = 17;
		pIndex[6] = 24;
		pIndex[7] = 31;
		pIndex[8] = 38;
		pIndex[9] = 45;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'U':
		nCount = 14;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 27;
		pIndex[8] = 28;
		pIndex[9] = 34;
		pIndex[10] = 35;
		pIndex[11] = 41;
		pIndex[12] = 44;
		pIndex[13] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'V':
		nCount = 13;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 27;
		pIndex[8] = 29;
		pIndex[9] = 33;
		pIndex[10] = 37;
		pIndex[11] = 39;
		pIndex[12] = 45;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'W':
		nCount = 17;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 27;
		pIndex[8] = 28;
		pIndex[9] = 31;
		pIndex[10] = 34;
		pIndex[11] = 35;
		pIndex[12] = 37;
		pIndex[13] = 39;
		pIndex[14] = 41;
		pIndex[15] = 43;
		pIndex[16] = 47;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'X':
		nCount = 13;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 8;
		pIndex[3] = 12;
		pIndex[4] = 16;
		pIndex[5] = 18;
		pIndex[6] = 24;
		pIndex[7] = 30;
		pIndex[8] = 32;
		pIndex[9] = 36;
		pIndex[10] = 40;
		pIndex[11] = 42;
		pIndex[12] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'Y':
		nCount = 10;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 6;
		pIndex[2] = 8;
		pIndex[3] = 12;
		pIndex[4] = 16;
		pIndex[5] = 18;
		pIndex[6] = 24;
		pIndex[7] = 31;
		pIndex[8] = 38;
		pIndex[9] = 45;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case 'Z':
		nCount = 13;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 6;
		pIndex[4] = 12;
		pIndex[5] = 18;
		pIndex[6] = 24;
		pIndex[7] = 30;
		pIndex[8] = 36;
		pIndex[9] = 42;
		pIndex[10] = 44;
		pIndex[11] = 46;
		pIndex[12] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '0':
		nCount = 14;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 8;
		pIndex[3] = 12;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 21;
		pIndex[7] = 27;
		pIndex[8] = 28;
		pIndex[9] = 34;
		pIndex[10] = 36;
		pIndex[11] = 40;
		pIndex[12] = 44;
		pIndex[13] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '1':
		nCount = 10;
		pIndex = new int[nCount];
		pIndex[0] = 3;
		pIndex[1] = 8;
		pIndex[2] = 10;
		pIndex[3] = 17;
		pIndex[4] = 24;
		pIndex[5] = 31;
		pIndex[6] = 38;
		pIndex[7] = 43;
		pIndex[8] = 45;
		pIndex[9] = 47;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '2':
		nCount = 12;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 20;
		pIndex[5] = 25;
		pIndex[6] = 30;
		pIndex[7] = 35;
		pIndex[8] = 42;
		pIndex[9] = 44;
		pIndex[10] = 46;
		pIndex[11] = 48;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '3':
		nCount = 12;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 20;
		pIndex[5] = 23;
		pIndex[6] = 25;
		pIndex[7] = 34;
		pIndex[8] = 35;
		pIndex[9] = 41;
		pIndex[10] = 44;
		pIndex[11] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '4':
		nCount = 13;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 11;
		pIndex[4] = 14;
		pIndex[5] = 18;
		pIndex[6] = 21;
		pIndex[7] = 23;
		pIndex[8] = 25;
		pIndex[9] = 27;
		pIndex[10] = 32;
		pIndex[11] = 39;
		pIndex[12] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '5':
		nCount = 14;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 6;
		pIndex[4] = 7;
		pIndex[5] = 14;
		pIndex[6] = 16;
		pIndex[7] = 18;
		pIndex[8] = 27;
		pIndex[9] = 34;
		pIndex[10] = 35;
		pIndex[11] = 41;
		pIndex[12] = 44;
		pIndex[13] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '6':
		nCount = 14;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 21;
		pIndex[6] = 23;
		pIndex[7] = 25;
		pIndex[8] = 28;
		pIndex[9] = 34;
		pIndex[10] = 35;
		pIndex[11] = 41;
		pIndex[12] = 44;
		pIndex[13] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '7':
		nCount = 11;
		pIndex = new int[nCount];
		pIndex[0] = 0;
		pIndex[1] = 2;
		pIndex[2] = 4;
		pIndex[3] = 6;
		pIndex[4] = 7;
		pIndex[5] = 12;
		pIndex[6] = 18;
		pIndex[7] = 24;
		pIndex[8] = 30;
		pIndex[9] = 36;
		pIndex[10] = 42;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '8':
		nCount = 14;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 23;
		pIndex[7] = 25;
		pIndex[8] = 28;
		pIndex[9] = 34;
		pIndex[10] = 35;
		pIndex[11] = 41;
		pIndex[12] = 44;
		pIndex[13] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '9':
		nCount = 14;
		pIndex = new int[nCount];
		pIndex[0] = 2;
		pIndex[1] = 4;
		pIndex[2] = 7;
		pIndex[3] = 13;
		pIndex[4] = 14;
		pIndex[5] = 20;
		pIndex[6] = 23;
		pIndex[7] = 25;
		pIndex[8] = 27;
		pIndex[9] = 34;
		pIndex[10] = 35;
		pIndex[11] = 41;
		pIndex[12] = 44;
		pIndex[13] = 46;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '+':
		nCount = 5;
		pIndex = new int[nCount];
		pIndex[0] = 17;
		pIndex[1] = 22;
		pIndex[2] = 24;
		pIndex[3] = 26;
		pIndex[4] = 31;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '-':
		nCount = 3;
		pIndex = new int[nCount];
		pIndex[0] = 22;
		pIndex[1] = 24;
		pIndex[2] = 26;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '/':
		nCount = 7;
		pIndex = new int[nCount];
		pIndex[0] = 6;
		pIndex[1] = 12;
		pIndex[2] = 18;
		pIndex[3] = 24;
		pIndex[4] = 30;
		pIndex[5] = 36;
		pIndex[6] = 42;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	case '*':
		nCount = 12;
		pIndex = new int[nCount];
		pIndex[0] = 7;
		pIndex[1] = 13;
		pIndex[2] = 16;
		pIndex[3] = 18;
		pIndex[4] = 21;
		pIndex[5] = 23;
		pIndex[6] = 25;
		pIndex[7] = 27;
		pIndex[8] = 30;
		pIndex[9] = 32;
		pIndex[10] = 35;
		pIndex[11] = 41;
		GetDrillIndex(fptStart, pIndex, nCount, vecPt, fDistance);
		bState = TRUE;
	    break;
	default:
	    break;
	}
	if(pIndex)
	{
		delete pIndex;
		pIndex = NULL;
	}
	return bState;
}

void CFileFormatRs274X::GetDrillIndex(FPOINTC fptStart,int* arIndex,int nCount,vector<FPOINTC> &vecPt, float fDistance)
{
	FPOINTC fptBuf;
	int nIndex;
	int i, nCol,nRow;
	for(i=0;i<nCount;i++)
	{
		nIndex = arIndex[i];
		nCol = nIndex%7;
		nRow = nIndex/7;
		fptBuf.x = fptStart.x+nCol*fDistance*0.5;
		fptBuf.y = fptStart.y+(6-nRow)*fDistance;
		vecPt.push_back(fptBuf);
	}
	return ;
}



void CFileFormatRs274X::SetPixelResolution(double dPixelResolution)
{
	m_dPixelResolution = dPixelResolution;
}