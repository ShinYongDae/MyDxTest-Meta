#if !defined(AFX_FILEFORMATRS274X_H__46FE0FB9_2786_4105_B6AF_C0445CD7520A__INCLUDED_)
#define AFX_FILEFORMATRS274X_H__46FE0FB9_2786_4105_B6AF_C0445CD7520A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileFormatRs274X.h : header file
//

#include <afxtempl.h>
#include <vector>

#include "MyData.h"
#include "PostfixCalc.h"
#include "TransformUtil.h"
#include "ThreadTask.h"

using namespace std;

// The FRECTC structure defines the Left-Bottom and Right-Top coordinates of a point.
// struct FRECTC
// {
// 	// Constructor.
// 	// Parameters: see member variables.
// 	FRECTC(float px1=0.0,float py1=0.0,float px2=0.0,float py2=0.0)
// 	{
// 		X1 = px1;
// 		Y1 = py1;
// 		X2 = px2;
// 		Y2 = py2;
// 	}	
// 	float X1; // Specifies the X1-coordinate of a Left-Bottom point.
// 	float Y1; // Specifies the Y1-coordinate of a Left-Bottom point.
// 	float X2; // Specifies the X1-coordinate of a Right-Top point.
// 	float Y2; // Specifies the Y1-coordinate of a Right-Top point.
// };
// typedef CArray<FRECTC, FRECTC> CArFRect;
// 
// // The FPOINTC structure defines the x- and y-coordinates of a point.
// struct FPOINTC{
// 	// Constructor.
// 	// Parameters: see member variables.
// 	FPOINTC(float px=0.0, float py=0.0)
// 	{
// 		x = px;
// 		y = py;
// 	}	
// 	float x; // Specifies the x-coordinate of a point.
// 	float y; // Specifies the y-coordinate of a point.
// };
// typedef CArray<FPOINTC, FPOINTC> CArFPoint;



//++++++++++++++++++++++++++++++++++++++++++++++++
// Draw Object data structure
// struct DrawListAPerture
// {
// 	vector <int> nDcode;
// 	vector <int> nListNum;
// };

// typedef struct tagMirrorImage
// {
// 	tagMirrorImage()
// 	{
// 		nStartObj = 0;
// 		nEndObj = 0;
// 		bMirrorX = FALSE;
// 		bMirrorY = FALSE;
// 	}
// 	BOOL bMirrorX;
// 	BOOL bMirrorY;
// 	int nStartObj;
// 	int nEndObj;
// }MIRROR_IMAGE, *LPMIRROR_IMAGE;
// 
// namespace LAYER_TYPE
// {
// 	enum STEP
// 	{
// 		PIECE,
// 		STRIP,
// 		PANEL
// 	};
// 
// 	enum TYPE
// 	{
// 		PATTERN = 0,
// 		THROUGH_HOLE= 1,
// 		VIA_HOLE= 2,
// 		PAD= 3,
// 		SOLDER_REGIST= 4,
// 		INSPECTION_AREA= 5,
// 		MASK= 6,
// 		MASK_FREE= 7,
// 		ANOTHER_LAYER_DRILL = 8,	//LIQUID_PHOTO_RESIST= 8,
// 		VIRTUAL_DRILL= 9,
// 		ROUTER= 10,
// 		GROUND= 11,
// 		FILLED_VIA= 12,
// 		UNDER_LAYER= 13,
// 		COVERLAY= 14,
// 		LEAD= 15,
// 		NETLIST = 16
// 	};
// 	enum POLARITY
// 	{
// 		POSITIVE,
// 		NEGATIVE,
// 	};
// 	enum UNIT
// 	{
// 		METRIC,
// 		INCH,
// 	};
// };

namespace CAM_DATA
{
	enum FORMAT 
	{ 
		RS274X = 0,
		RS274D,		// rect zoom
		ODB,
		DXF,
		DPF,
		EXCELLON,
		GDS2,
		UNKNOWN
	};
};

namespace DRAW_TYPE
{
	enum TYPE
	{
		LINE,
		ARC_CW,
		ARC_CCW,
		POLYGON,
		SURFACE,
		POLYGON_SQ, // Single quadrant arc for RS274X
		POLYGON_MQ, // Multi quadrant arc for RS274X
		FLASH,
		TEXT,
		SNR
	};

	enum ATTRIBUTE
	{
		PATTERN,
		THROUGH_HOLE,
		VIA_HOLE,
		LEAD,
		DUMMY,
		VIRTUAL_DRILL,
		FINE_PAD,
		FILLED_VIA,
		GROUND,
		FIDUCIAL,
		ROUTE,
		INSPECTION,
		RESERVE1,
		ASD_DRILL,
		RESERVE2,
		NONE
	};

	enum POLARITY
	{
		POSITIVE,
		NEGATIVE
	};

	enum ORIENT
	{
		DEGREE_0 = 0,
		DEGREE_90 = 1,
		DEGREE_180 = 2,
		DEGREE_270 = 3,
		MIRROR_D0 = 4,
		MIRROR_D90 = 5,
		MIRROR_D180 = 6,
		MIRROR_D270 = 7
	};

	enum TEXT_VERSION
	{
		PREVIOUS = 0,
		CURRENT = 1
	};
}

// struct ENVIRONMENT_RS274X_FORMAT
// {
// 	ENVIRONMENT_RS274X_FORMAT()
// 	{
// 		nAutoMirrorMode = 0;
// 		nAutoRotateAngle = 0;
// 		fDetermineZero = 0.00001;
// 		strInputFolder = _T("D:\\GerberData");
// 		nFormatStatement = 43;
// 		strLeadingTrailingZeros = _T("LZ");
// 	}
// 	int		nAutoMirrorMode;
// 	int		nAutoRotateAngle;
// 	int		nFormatStatement;
// 	float	fDetermineZero;
// 	CString strInputFolder;	
// 	CString strLeadingTrailingZeros;
// };

// struct ENVIRONMENT_RS274D_FORMAT
// {
// 	ENVIRONMENT_RS274D_FORMAT()
// 	{
// 		nAutoMirrorMode = 0;
// 		nAutoRotateAngle = 0;
// 		fDetermineZero = 0.00001;
// 		strInputFolder = _T("D:\\GerberData");
// 		nFormatStatement = 43;
// 		strLeadingTrailingZeros = _T("LZ");
// 	}
// 	int		nAutoMirrorMode;
// 	int		nAutoRotateAngle;
// 	int		nFormatStatement;
// 	float	fDetermineZero;
// 	CString strInputFolder;	
// 	CString strLeadingTrailingZeros;
// };


//++++++++++++++++++++++++++++++++++++++++++++++++





#define EOB _T('*')

#define	ASCII_STX		(0x02)
#define	ASCII_ETX		(0x03)
#define	ASCII_ENQ		(0x05)
#define	ASCII_LF		(0x0a)
#define	ASCII_CR		(0x0d)
#define	ASCII_XON		(0x11)
#define	ASCII_XOFF		(0x13)
#define ASCII_SPACE		(0x20)

#define INCHTOMM	25.4

//<Drawing>
#define STENCIL_CLEAR 0
#define STENCIL_DARK 1
#define STENCIL_NORMAL 2

//#define WM_DISP_PROCESS	(WM_USER+274)

/////////////////////////////////////////////////////////////////////////////
// CFileFormatRs274X window

class CFileFormatRs274X : public CWnd
{
	CWnd* m_pParent;
	BOOL m_bEnableDraw;
	double m_dPixelResolution;

public:
	HWND m_hParent;

	enum DIRECTION
	{
		CCW = -1,
		CW = 1
	};

	union TEMPLATE_KEY{
		float Param[2];
		UINT64 key;
	};

	typedef struct tagLineInfo{
		tagLineInfo()
		{
			nNumOfLine = 0;
			dCode = 0;
			nLayerID = 0;
			nStepID = 0;
		}
		int nNumOfLine;
		int dCode;
		int nLayerID;
		int nStepID;
	}LINE_INFO, LPLINE_INFO;

	struct GraphObj
	{
 		// Constructor.
		// Parameters: see member variables.
		GraphObj()
		{
			dCode = 0;
			dAttrListCode = 0;
			Type.nType = 0;
			Type.nAttr = 0;
			Type.nPolarity = 0;
			Type.nOrient = 0;
			Type.nReserve1 = 0;
			Type.nReserve2 = 0;
			Type.nParentid =0;
			nStPnt = 0;
		}
		union
		{
			struct TYPE
			{
				UINT16 nType : 4 ;		// allocation 4 bit, max 16 : Line arc Polygon 
				UINT16 nAttr : 6 ;		// allocation 5 bit, max 64 Drill or Pattern
				UINT16 nPolarity : 1 ;	// allocation 1 bit, Positive or Negative
				UINT16 nOrient : 3 ;	// allocation 3 bit, max 8
				UINT16 nReserve1 : 1 ;	// allocation 1 bit, max 2
				UINT16 nReserve2 : 1 ;	// allocation 1 bit, max 2
				UINT16 nParentid : 16 ;	// allocation 16 bit, max 65535
			};
			TYPE Type;
			UINT nValue;
		};

		UINT dCode;	 // Specifies the Pointer to the Standard Symbol of the Object.
		UINT dAttrListCode;				 // (Specifies the number of vertexes if the object is surface type)
		int nStPnt;		 // Start Point
		FRECTC Extent;   // Specifies the minmax area of an Object

	};
	typedef CArray<GraphObj, GraphObj> CArGraphObj;


	typedef struct tagAptPrimitive
	{
		tagAptPrimitive()
		{
			nPriNum = 0;
			fParam.clear();
			minmax = FRECTC();
		}
		BYTE nPriNum;
		vector <float> fParam;
		FRECTC minmax;
	}APERTURE_PRIMITIVE, *LPAPERTURE_PRIMITIVE;

	typedef struct tagApertureDef
	{	
		tagApertureDef()
		{
			nGlListIndex = 0;
			chStdAptType = NULL;
			strAptName.Empty();
			Param.clear();
			minmax = FRECTC();
		}
		UINT	nGlListIndex;
		TCHAR	chStdAptType;	// 'C' 'R' 'O' 'P
		CString strAptName;
		vector <APERTURE_PRIMITIVE> Param;
		FRECTC minmax;
	}APERTURE_DEF, *LPAPERTURE_DEF;


	typedef struct tagLayerInform_RS274X
	{
		tagLayerInform_RS274X()
		{
			strName.Empty();
			strFilePath.Empty();

			Type = LAYER_TYPE::PATTERN;
			bPolarity = LAYER_TYPE::POSITIVE;
			bUnit = LAYER_TYPE::METRIC;
			Step = LAYER_TYPE::PANEL;
			nFormat = CAM_DATA::RS274X;

			fScaleX = 1.0;
			fScaleY = 1.0;

			fAdjScaleX = 1.0;
			fAdjScaleY = 1.0;
			fAdjOffsetX = 0.0;
			fAdjOffsetY = 0.0;

			bView = TRUE;
			Extent = FRECTC();
			Exterior = 0;
			nOrder = 0;
			
			nLayerGroupID = 0;
			vecObjCoord.clear();
			vecObjExtent.clear();
		}

		BOOL bView;
		int Exterior;
		int nLayerGroupID;
		int nOrder;

		CString strFilePath; 
		CString strName; // fetch layer name from gerber file
		
		LAYER_TYPE::TYPE Type;
		LAYER_TYPE::POLARITY  bPolarity;	
		LAYER_TYPE::UNIT bUnit; // Metric Or Inch	
		LAYER_TYPE::STEP Step;
		CAM_DATA::FORMAT nFormat;

		float fScaleX;		// X axis scale factor from gerber file analysis
		float fScaleY;		// Y axis scale factor from gerber file analysis

		float fAdjScaleX;	// scaling X by external cam operation
		float fAdjScaleY;	// scaling X by external cam operation
		float fAdjOffsetX;	// translate X by external cam operation
		float fAdjOffsetY;	// translate X by external cam operation
		
		FRECTC Extent;
	//	vector <UINT64> vecDrawTemplateList;
		vector <GraphObj> listObj; 
		vector <FPOINTC> vecObjCoord;	
		vector <FRECTC> vecObjExtent;
		CMap <DRAW_TYPE::ATTRIBUTE,DRAW_TYPE::ATTRIBUTE,COLORREF,COLORREF> mapObjColor;
		CMap <int, int, LPAPERTURE_DEF, LPAPERTURE_DEF> mapAptList;
		
	}LAYER_INFORM_RS274X, LAYER_INFORM_EXCELLON, *LPLAYER_INFORM_RS274X, *LPLAYER_INFORM_EXCELLON;
	typedef CArray <LPLAYER_INFORM_RS274X,LPLAYER_INFORM_RS274X> CArLayerInform_RS274X;



	typedef struct tagLayerStruct
	{
		UINT nObject;
		UINT nCoord;
	}LAYER_STRUCT, *LPLAYER_STRUCT;


	struct ParamFS	// Parameter 'FS'(Format Statement) data structure
	{
		ParamFS()
		{
			bSuppress=FALSE;
			bAbsolute=TRUE;
		}
		BOOL bSuppress; //  (FALSE) : 'Leading Zero omitted , (TRUE) : 'Trailing Zero omitted
		BOOL bAbsolute; // 'A' or 'I':  Use A for absolute coordinate values, Use I for incremental coordinate values
	};

	struct ParamSR	// Parameter 'SR'(Step & Repeat) data structure
	{
		ParamSR()
		{
			nXRepeat=0;
			nYRepeat=0;
			psRepeat=0;
		}
		int nXRepeat, nYRepeat;
		int psRepeat;
	};

	struct ParamMI	// Parameter 'MI'(Mirror Image) data structure
	{
		ParamMI()
		{
			bMirrorX=FALSE;
			bMirrorY=FALSE;
		}

		BOOL bMirrorX, bMirrorY;
	};

	struct ParamAD	// Parameter 'AD'(Aperture Discription) data structure
	{
		ParamAD()
		{
			bExposureMode=FALSE;
		}
		BOOL bExposureMode;
	};

	struct ParamIP	// Parameter 'IP'(Image Polarity) data structure
	{
		ParamIP()
		{
			PolChangeFlag=FALSE;
			bLayerPolarity=LAYER_TYPE::POSITIVE;
		}

		BOOL PolChangeFlag;
		BOOL bLayerPolarity;
	};

	APERTURE_PRIMITIVE m_AptPrimi;
	LPAPERTURE_DEF m_pAptDef, m_rAptDef;
	LPLAYER_INFORM_RS274X m_pLayerInfo;
	CArLayerInform_RS274X m_ArLayerInform;
	CArray <LAYER_STRUCT,LAYER_STRUCT> m_ArLayerStruct;


	CTransformUtil *m_pTrUtil;
	CPostfixCalc m_PostfixCalc;

	CMap <CString, LPCTSTR, CString, LPCTSTR> m_mapAMMacro;
	CMap <float, float, LINE_INFO, LINE_INFO> m_mapLineInfo;

	vector <UINT64> m_vecLineTemplate;

	HDC m_hDC;

	UINT m_nNumOfAperture;
	UINT m_nNumOfLineTemplate;
	UINT m_nNumOfSnRObject;

	float m_fCanvasPixelResolution, m_fDrawArcResolution;
	FRECTC m_FrameExtent;
	CSize m_sizeWindow;

	FRECTC GetFrameExtent();
	BOOL GetDrillPoints(char ch,FPOINTC fptStart,vector<FPOINTC> &vecPt, float fDistance);
	void GetDrillIndex(FPOINTC fptStart,int* arIndex,int nCount,vector<FPOINTC> &vecPt, float fDistance);

private:
	CRect m_rtClient;

	CString m_sFilePath;

	int m_nLayer;
	GraphObj m_gObj;
	int m_nObj;
	int m_nNumOfObject, m_nNumOfCoord;


	int m_nStartMirrorObj, m_nEndMirrorObj;
	BOOL m_bLastFrame;
	double m_fDividerX, m_fDividerY;

	BOOL m_bParsingXYFlag, m_bParsingIJFlag;
	BOOL m_bParsingGCodeFlag, m_bParsingdCodeFlag;
	BOOL m_bPolygonFlag, m_bImplicitFlag;

	int m_nPrevDCode, m_nPrevADCode;
	int m_nDCode, m_nGCode, m_nMCode, m_nADCode;

	float m_fLayerScaleX;
	float m_fLayerScaleY;

	float m_xStep, m_yStep;

	int m_nMaxLineTemplate;
	vector <UINT64> m_vecTemp;

	UINT m_nNumOfLayer;
	int m_nNumOfObjCoord;
	int  m_nNumOfDataBlock;
	int  m_nNumOfTextLine;
	UINT m_nNumOfLineObject;

	int m_nLayerGroupID;
	int m_nIRAngle; // Image Rotation angle

	ParamFS m_paramFS;
	ParamSR m_paramSR;
	ParamAD m_paramAD;
	ParamMI m_paramMI;
	ParamIP m_paramIP;

	LAYER_TYPE::UNIT m_Unit;
	ldiv_t m_lDataFormatX, m_lDataFormatY;
	FRECTC m_MaxFrameExtent;
	FRECTC m_rectLayerExtent;	
	FRECTC m_MnMxSnR, m_CurMinMax;

	CString m_optionUnit, m_optionFormat; 

	FPOINTC m_fptPrevPos;
	FPOINTC m_fptCoord, m_fptOpt;

	float m_dDetZero;

	//<Drawing>
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
// 	float m_fDrawResolution;

	int m_nActionCode;
// 	BOOL m_bFindLineObjPickingMode;


	BOOL DoDecoding(char* pFile, char* pLine);
	//void FreeMemory();

	BOOL DecodingIgnore(char** pFile, char** pLine);
	BOOL DecodingCoordinate(char** pFile);
	BOOL DecodingFunction(char** pFile);
	BOOL DecodingParameter(char** pFile);
	void DecodingParsingFlag();

	BOOL FunctionDCode(char** pFile);
	BOOL FunctionGCode(char** pFile);
	BOOL FunctionMCode(char** pFile);

	BOOL ParameterFS(char** pFile);
	BOOL ParameterAperture(char** pFile);
	BOOL ParameterBkMode(char** pFile);
	BOOL ParameterDashLine(char** pFile);
	BOOL ParameterKnockOut(char** pFile);
	BOOL ParameterSequence(char** pFile);
	BOOL ParameterRotate(char** pFile);
	BOOL ParameterTranslation(char** pFile);
	BOOL ParameterVelocity(char** pFile);
	BOOL ParameterWindowSpec(char** pFile);
	BOOL ParameterI(char** pFile);
	BOOL ParameterL(char** pFile);
	BOOL ParameterM(char** pFile);
	BOOL ParameterO(char** pFile);
	BOOL ParameterP(char** pFile);
	BOOL ParameterS(char** pFile);

	BOOL ApertureDefinition(char** pFile);
	void ApertureCircle(char *pTemp);
	void ApertureLineV(char *pTemp);
	void ApertureLineC(char *pTemp);
	void ApertureLineL(char *pTemp);
	void ApertureOutLine(char *pTemp);
	void AperturePolygon(char *pTemp);
	void ApertureMoire(char *pTemp);
	void ApertureThermal(char *pTemp);

	BOOL FetchApertureCircle(char** pFile);
	BOOL FetchApertureRect(char** pFile);
	BOOL FetchApertureObround(char** pFile);
	BOOL FetchAperturePolygon(char** pFile);

	int	GetIntegerData(char **pFile);
	float GetFloatData(char **pFile);
	float GetCoordDataX(char **pFile);
	float GetCoordDataY(char **pFile);
	void GetCosineSineValue(float Ang, float *cV, float *sV);
	//void GetCannedText(CString strCannedText, FPOINTC fptStart, float fDistance, vector <FPOINTC> &vecCoord);

	float ParseFloatData(char **pFile, BOOL bArithmetic=FALSE);
	int	ParseIntegerData(char **pFile, BOOL bArithmetic=FALSE);
	float ParseScaleData(char **pFile, BOOL bArithmetic=FALSE);
	float ParseRotationData(char **pFile, BOOL bArithmetic=FALSE);
	BOOL ParseApertureMacro(char **pFile,CString &strApertureMacro);

	BOOL CheckGCodeValue(int nGCode);
	BOOL CheckDCodeValue(int nDCode);
	BOOL CheckMCodeValue(int nMCode);

	FRECTC MinMaxRectVectorGRB(double X1, double Y1, double X2, double Y2, double w, double ang);
	FRECTC MinMaxRectCenterGRB(double x, double y, double w, double h, double ang);
	FRECTC MinMaxRectLowerLeftGRB(double X, double Y, double w, double h, double ang);
	FRECTC MinMaxArcInPolygonGRB(DRAW_TYPE::TYPE dType, FPOINTC p1, FPOINTC p2, FPOINTC pC);
	FRECTC MinMaxArcGRB(DRAW_TYPE::TYPE dType, FPOINTC P1, FPOINTC P2, FPOINTC PC, FRECTC CurMinMax);

	void Mirroring();
	void MirrorVerticalObject(LAYER_INFORM_RS274X *pLayerInfo,GraphObj *obj, int nCoord, double vAxis);  //'좌우미러링
	void MirrorVerticalObject(vector<FPOINTC> &vecCoord,GraphObj *obj, double vAxis);  //'좌우미러링
	void MirrorHorizontalObject(vector<FPOINTC> &vecCoord,GraphObj *obj, double hAxis);  //'상하미러링
	void MirrorHorizontalObject(LAYER_INFORM_RS274X *pLayerInfo,GraphObj *obj, int nCoord, double hAxis);  //'상하미러링

	void StepAndRepeating(FRECTC &Extent, BOOL bClearObjectData=TRUE);
	void ApplyIROptionGRB(LAYER_INFORM_RS274X *pLayerInfo, double cX, double cY, double ang);
	void RotateTransObject(LAYER_INFORM_RS274X *pLayerInfo,GraphObj *obj, GraphObj *Nextobj,FPOINTC fC, double fAng, double tX, double tY);

	BOOL CheckAngleGRB(double AngP , double Ang1, double Ang2, DRAW_TYPE::TYPE dType);
	BOOL CheckArithmeticOperator(char ch);
	void ClearObjectData(GraphObj &gObj);
	void ResetObjectData(GraphObj &gObj);
	void ResetData();
	void InitVal();
	void InitMaxLayerExtent();
	FRECTC GetMaxLayerExtent();
	void SetMaxLayerExtent(FRECTC &rect);
	FRECTC GetLayerExtent(int nLayer);
	void SetFrameExtent(FRECTC fRect);

	//<Drawing>
	BOOL CheckDrawAllow(UINT nObjIndex);
	void SetDrawArcResolution(float fResolution);
	void AllocDrawList();

// 	int MakeApertureList();
//	int MakeLineTemplateList();
//	int MakeSnRObjectList();
	//void MyMakeLineList(float fLength, float w);
	void SetDrawResolution(float fResolution);
	void AllocCamDrawStruct();
	void SetForeColor(COLORREF crColor);

	void DrawRect(const CfRect &rect,int width = 0, int fill=TRUE);
	FPOINTC ConvertWindowCoord(FPOINTC& fPoint);

/*	
	
	void CallList(int nListIndex);


	BOOL CheckDrawAllow(const FRECTC &FMinMax);
	int  SearchLineTemplate(UINT64 nVal);

	COLORREF GetObjectColor(GraphObj &obj);
	int  GetArcSegment(float fRadius,float fAngle=360.0);
	double GetDistance(CfPoint FirstPoint,CfPoint SecondPoint); // 두점 사이의 거리를 얻는다 
	void GetArcVertices(vector<FPOINT> &vecPt, const FPOINT &ptCenter, double radius, double start_angle, double arc_angle, int dir);
	void GetArcVertices(vector<FPOINTC> &vecPt,const FPOINTC &ptCenter,float radius, float start_angle, float arc_angle, int dir);
	COLORREF GetSnRObjectColor(CFileFormatRs274X::GraphObj &obj);

	FPOINTC OriginTranslateCoord(vector <FPOINTC> &vecCoord, GraphObj &gObj, FRECTC fExtent);
	void TranslateCoordbyActionCode(vector <FPOINTC> &vecCoord, GraphObj &gObj, FPOINTC fptCenter);
	
	void RotateTranslateExtent(FRECTC &Extent, double cV, double sV, double cX, double cY, double dX, double dY);
	void RotateObject90(vector<FPOINTC> &vecCoord,GraphObj *obj,FPOINTC fC);

	void SetCanvasPixelResolution();
	void SetStencil(int nMode);
	
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
	BOOL MyThermal(float x, float y, float Dmo, float Dmi, float Ang, int num,  float gap, float Angle);
	BOOL MyPolygon(const FPOINTC *fCoord, int nVertex);
	BOOL MyLine(float fSx, float fSy, float fEx, float fEy, float fLineWidth);
	BOOL MyRectLowerLeft(float fSx, float fSy, float fLineWidth, float fLineHeight, float fAngle);

	BOOL DrawObject(UINT nObjIndex);
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
*/
	void DispProgress(int nCurr, int nTot);


// Construction
public:
	CFileFormatRs274X(CWnd* pParent=NULL);

// Attributes
public:
	BOOL m_bAliveThreadDrawing[4];
	CThreadTask* m_pThreadDrawing[4]; // CThreadTask class, handles the threading code

	float m_fDrawResolution;


// Operations
public:
	CCriticalSection m_csDrawing;
	static UINT ThreadDrawing_0(LPVOID lpContext);
	static UINT ThreadDrawing_1(LPVOID lpContext);
	static UINT ThreadDrawing_2(LPVOID lpContext);
	static UINT ThreadDrawing_3(LPVOID lpContext);
	void Drawing_0();
	void Drawing_1();
	void Drawing_2();
	void Drawing_3();
	//BOOL DrawObject_0(UINT nObjIndex);
	//BOOL DrawObject_1(UINT nObjIndex);
	//BOOL DrawObject_2(UINT nObjIndex);
	//BOOL DrawObject_3(UINT nObjIndex);

	CString Load(CString sPath);
	BOOL Decoding(char* pFile);
	void CloseDecoding();
	BOOL Draw(HDC& hdc, const CRect& client);
	void SetPixelResolution(double dPixelResolution);

	int GetNumOfLayer();
	int	GetNumOfAperture();
	int	GetNumOfLineTemplate();
	int	GetNumOfSnR();

	void FreeMemory();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileFormatRs274X)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileFormatRs274X();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileFormatRs274X)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEFORMATRS274X_H__46FE0FB9_2786_4105_B6AF_C0445CD7520A__INCLUDED_)
