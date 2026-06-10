// TransformUtil.h: interface for the CTransformUtil class.
//
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
////                               Transform.h Ver 3.6                             ////
////                             Developer: CAM PART                               ////
////                               Corp   : GIGAVIS                                ////
////																			   ////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_TRANSFORMUTIL_H__33FF8710_3ABD_40DA_967C_F3930549D754__INCLUDED_)
#define AFX_TRANSFORMUTIL_H__33FF8710_3ABD_40DA_967C_F3930549D754__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "Entities.h"
//#include "GlobalDefine.h"
#include "MyData.h"

#include <afxtempl.h>
#include <vector>
#include <math.h>

using namespace std;


// //The FRECTC structure defines the Left-Bottom and Right-Top coordinates of a point.
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
// //The FPOINTC structure defines the x- and y-coordinates of a point.
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



// The FRECTC structure defines the Left-Bottom and Right-Top coordinates of a point.
// struct FRECTD{
// 	// Constructor.
// 	// Parameters: see member variables.
// 	FRECTD(double px1=0.0,double py1=0.0,double px2=0.0,double py2=0.0)
// 	{
// 		X1 = px1;
// 		Y1 = py1;
// 		X2 = px2;
// 		Y2 = py2;
// 	}	
// 	double X1; // Specifies the X1-coordinate of a Left-Bottom point.
// 	double Y1; // Specifies the Y1-coordinate of a Left-Bottom point.
// 	double X2; // Specifies the X1-coordinate of a Right-Top point.
// 	double Y2; // Specifies the Y1-coordinate of a Right-Top point.
// };
// typedef CArray<FRECTD, FRECTD> CArDRect;




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


namespace TRANSFORM_DATA
{
	enum TYPE 
	{ 
		NONE,
		MIRROR_V,		// rect zoom
		MIRROR_H,
		ROTATE_180,
		ROTATE_CCW,
		ROTATE_CW
	};
};



// struct Matrix3by3
// {
// 	// Constructor.
// 	// Parameters: see member variables.
// 	Matrix3by3(double pv0 =1.0, double pv1=0.0, double pv2=0.0,double pv3=0.0, double pv4=1.0, double pv5=0.0,double pv6=0.0, double pv7=0.0, double pv8=1.0)
// 	{
// 		v0 = pv0;
// 		v1 = pv1;
// 		v2 = pv2;
// 
// 		v3 = pv3;
// 		v4 = pv4;
// 		v5 = pv5;
// 
// 		v6 = pv6;
// 		v7 = pv7;
// 		v8 = pv8;
// 	}
// 	double v0, v1, v2;
// 	double v3, v4, v5;
// 	double v6, v7, v8;
// };
// typedef CArray<Matrix3by3, Matrix3by3> CArMatrix3by3;



class CTransformUtil  
{
private:
	Matrix3by3 m_Matrix[10];
	Matrix3by3 m_PrevMatrix[10];

	Matrix3by3 m_RotateMatrix;
	Matrix3by3 m_Rotate90Matrix;
	Matrix3by3 m_Rotate180Matrix;
	Matrix3by3 m_Rotate270Matrix;

	Matrix3by3 m_OffsetMatrix;
	Matrix3by3 m_TranslateMatrix;
	Matrix3by3 m_MirrorLRMatrix;
	Matrix3by3 m_MirrorTBMatrix;
	Matrix3by3 m_ScaleMatrix;

	Matrix3by3 m_TransformMatrix;

	double m_fCosine, m_fSine; // matrix calculation factor for translate or ratate 

//	int m_nActionCode;
	void Initialze(Matrix3by3 &m);
	
	Matrix3by3 fDot3by3(const Matrix3by3 &a, const Matrix3by3 &b); // matrix multiply
	void TranslateMatrix(double fTx,double fTy);
	void MirrorMatrix(BOOL bLRMirror); 
	void RotateMatrix(double fAng); 
	void RotateMatrix90(); 
	void RotateMatrix180();
	void RotateMatrix270();

	void RotatePoint(FPOINTC &fpt, FPOINTC &fCenter);
public:
	int m_nActionCode;
	LAYER_TYPE::UNIT m_bUnit;

	CTransformUtil();
	virtual ~CTransformUtil();

	double GetcVr();
	double GetsVr();

	int GetActionCode();
	void SetActionCode(int nActionCode);
	
	void GetMirrorMatrix(Matrix3by3 &mSource, BOOL bLRMirror);
	void GetRotateMatrix(Matrix3by3 &mSource,double fAng); 
	void GetTranslateMatrix(Matrix3by3 &mSource,double fTx,double fTy);

	Matrix3by3 GetTransformMatrix();
	void SetTransformMatrix(Matrix3by3 &Matrix);
	void cVsV(float Ang, float *cV, float *sV); // Get Rotate parameter, cosine and sine value

	FPOINTC PointByMatrix(const FPOINTC &p);
	FRECTC RectByMatrix(const FRECTC &fRect);
	FRECTD ExtentByMatrix(const FRECTD &fRect);

	void PointByActionCode(FPOINTC &fpt, FPOINTC fCenter, int nActionCode);
	void RectByActionCode(FRECTC &fRect, FPOINTC fCenter, int nActionCode);

	Matrix3by3 MakeSnRMatrix(int TmLevel, BOOL ObjMirrorFlag, int nActionCode, double cV, double sV, double tx, double ty, double fDatumX, double fDatumY); 
	Matrix3by3 MakeGDS2MatrixEX(int nTmLevel, BOOL bXAxisMirror, double dAngle, CfPoint fptTranslate, double dMag);
	Matrix3by3 MakeGDS2Matrix(int nTmLevel, BOOL bXAxisMirror, double dAngle, CfPoint fptTranslate, double dMag); 
	BOOL GetScaleRect(FRECTC *rect, float fScaleX, float fScaleY);

	BOOL CheckPointInRect(FRECTC &fRect, float x, float y);
	BOOL CheckRectInRect(FRECTC *Inner, FRECTC *Outer);
	BOOL CheckRectInRect(CfRect *Inner, CfRect *Outer);
	BOOL CheckUnionRect(FRECTC *fRect1, FRECTC *fRect2);
	
	void MirrorVerticalRect(FRECTC &fRect, double vAxis);
	void MirrorHorizontalRect(FRECTC &fRect, double hAxis);
	void MirrorHorizontalPoint(FPOINTC &p, double hAxis);
	void MirrorVerticalPoint(FPOINTC &p, double vAxis);

	void TranslatePoint(FPOINTC &p, double dX, double dY);
	void TranslateRect(FRECTC &R, double dX, double dY);

	void RotatePoint(FPOINTC &p, double cV, double sV, double cX, double cY);
	void RotatePoint(FPOINTC &fpt, FPOINTC &fCenter, double fAng);
	void RotatePoint90(FPOINTC &fpt, FPOINTC &fCenter);
	void RotatePoint180(FPOINTC &fpt, FPOINTC &fCenter);
	void RotatePoint270(FPOINTC &fpt, FPOINTC &fCenter);

	void RotateRect(FRECTC &fRect, FPOINTC &fCenter, double fAng);
	void RotateRect90(FRECTC &fExtent, FPOINTC &fpt=FPOINTC(0,0));
	void RotateRect180(FRECTC &fExtent, FPOINTC &fpt=FPOINTC(0,0));
	void RotateRect270(FRECTC &fExtent, FPOINTC &fpt=FPOINTC(0,0));

	void AdjustExtentByPoint(FRECTC &Extent, const FPOINTC &pt);
	void AdjustExtentByRect(FRECTC &Extent, const FRECTC &rect);

	void SortRectX(vector <FRECTC> &rectExtent, int nStart, int nEnd);
	void SortRectY(vector <FRECTC> &rectExtent, int nStart, int nEnd);
	void SwapRect(vector <FRECTC> &rectExtent, int nIndex1, int nIndex2);

};

#endif // !defined(AFX_TRANSFORMUTIL_H__33FF8710_3ABD_40DA_967C_F3930549D754__INCLUDED_)
