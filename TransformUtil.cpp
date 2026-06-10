// TransformUtil.cpp: implementation of the CTransformUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TransformUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef PI
	#define PI 3.141592653589793f
#endif

const double DEG2RAD = PI/180.0;
const double RAD2DEG = 180.0/PI;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTransformUtil::CTransformUtil()
{
	m_MirrorLRMatrix.v0 = -1.0;
	m_MirrorLRMatrix.v4 = 1.0;
	
	m_MirrorTBMatrix.v0 = 1.0;
	m_MirrorTBMatrix.v4 = -1.0;


	// set rotation matrix parameter : CCW direction
	float cV,sV;
	cVsV(90.0, &cV, &sV);	
	m_Rotate90Matrix.v0 = cV; // cosine value
	m_Rotate90Matrix.v1 = sV; // sine value
	m_Rotate90Matrix.v3 = -sV; // sine value
	m_Rotate90Matrix.v4 = cV; // cosine value
	cVsV(180.0, &cV, &sV);
	m_Rotate180Matrix.v0 = cV; // cosine value
	m_Rotate180Matrix.v1 = sV; // sine value
	m_Rotate180Matrix.v3 = -sV; // sine value
	m_Rotate180Matrix.v4 = cV; // cosine value
	cVsV(270.0, &cV, &sV);
	m_Rotate270Matrix.v0 = cV; // cosine value
	m_Rotate270Matrix.v1 = sV; // sine value
	m_Rotate270Matrix.v3 = -sV; // sine value
	m_Rotate270Matrix.v4 = cV; // cosine value

	SetActionCode(0);

}

CTransformUtil::~CTransformUtil()
{
}

void CTransformUtil::Initialze(Matrix3by3 &m)
{
	m = Matrix3by3();
}

Matrix3by3 CTransformUtil::GetTransformMatrix()
{
	return m_TransformMatrix;
}

void CTransformUtil::SetTransformMatrix(Matrix3by3 &Matrix)
{
	m_TransformMatrix = Matrix;
}

double CTransformUtil::GetcVr()
{
	return m_fCosine;
}
double CTransformUtil::GetsVr()
{
	return m_fSine;
}

int CTransformUtil::GetActionCode()
{
	return m_nActionCode;
}

// 함수명 변경
// SetActionCode->SetRotateParameter
void CTransformUtil::SetActionCode(int nActionCode)
{
//////////////////////
// nActionCode
// 0: None
// 1: L-R Mirror	
// 2: T-B Mirror	
// 3: CCW 180 Rotate
// 4: CCW 90, CW 270 Rotate
// 5: CCW 270, CW 90 Rotate

	m_nActionCode = nActionCode;
	switch(nActionCode)
    {
	case 3:	// CCW180
		m_fCosine = -1.0;
		m_fSine = 0.0;
		break;
	case 4: //'CCW 90
	case 14: 
	case 24:
	case 41:
	case 42: 
		m_fCosine = 0.0;
		m_fSine = 1.0;
		break;
	case 5: // 'CCW270 or CW 90
	case 15:
	case 25:
	case 51: 
	case 52: 
		m_fCosine = 0.0;
		m_fSine = -1.0;
		break;
	default: // 'CW 0
		m_fCosine = 1.0;
		m_fSine = 0.0;
		break;
    }
}

// 함수명 변경
// SetActionCode->GetRotateParameter
void CTransformUtil::cVsV(float fAng, float *cV, float *sV)
{
	int nAng = fmod(fAng+360.0,360.0); // normalize angle
    switch (nAng) // 'CCW
	{
	case 0:
        *cV = 1.0;
        *sV = 0.0;
		break;
    case 90:
		*cV = 0.0;
        *sV = 1.0;
		break;
    case 180:
        *cV = -1.0;
        *sV = 0.0;
		break;
    case 270:
        *cV = 0.0;
        *sV = -1.0;
		break;
	default:
        *cV = cos(fAng * DEG2RAD);
        *sV = sin(fAng * DEG2RAD);
		break;
    }
}

Matrix3by3 CTransformUtil::fDot3by3(const Matrix3by3 &a, const Matrix3by3 &b)
{
    Matrix3by3 c(a.v0 * b.v0 + a.v1 * b.v3 + a.v2 * b.v6,
				a.v0 * b.v1 + a.v1 * b.v4 + a.v2 * b.v7,
				a.v0 * b.v2 + a.v1 * b.v5 + a.v2 * b.v8,
				a.v3 * b.v0 + a.v4 * b.v3 + a.v5 * b.v6,
				a.v3 * b.v1 + a.v4 * b.v4 + a.v5 * b.v7,
				a.v3 * b.v2 + a.v4 * b.v5 + a.v5 * b.v8,
				a.v6 * b.v0 + a.v7 * b.v3 + a.v8 * b.v6,
				a.v6 * b.v1 + a.v7 * b.v4 + a.v8 * b.v7,
				a.v6 * b.v2 + a.v7 * b.v5 + a.v8 * b.v8);	
    return c;
}

// point transform(mirror,rotate,translate) by matrix
FPOINTC CTransformUtil::PointByMatrix(const FPOINTC &p)
{
	FPOINTC fpt((p.x * m_TransformMatrix.v0) + (p.y * m_TransformMatrix.v3) + m_TransformMatrix.v6,
				(p.x * m_TransformMatrix.v1) + (p.y * m_TransformMatrix.v4) + m_TransformMatrix.v7);
	return fpt;
}
FRECTD CTransformUtil::ExtentByMatrix(const FRECTD &fRect)
{
	FRECTD rect;
	FPOINTC fptPoint[4];
    
    fptPoint[0].x = (fRect.X1 * m_TransformMatrix.v0) + (fRect.Y1 * m_TransformMatrix.v3) + m_TransformMatrix.v6;
    fptPoint[0].y = (fRect.X1 * m_TransformMatrix.v1) + (fRect.Y1 * m_TransformMatrix.v4) + m_TransformMatrix.v7;
    fptPoint[1].x = (fRect.X2 * m_TransformMatrix.v0) + (fRect.Y2 * m_TransformMatrix.v3) + m_TransformMatrix.v6;
    fptPoint[1].y = (fRect.X2 * m_TransformMatrix.v1) + (fRect.Y2 * m_TransformMatrix.v4) + m_TransformMatrix.v7;
    fptPoint[2].x = (fRect.X2 * m_TransformMatrix.v0) + (fRect.Y1 * m_TransformMatrix.v3) + m_TransformMatrix.v6;
    fptPoint[2].y = (fRect.X2 * m_TransformMatrix.v1) + (fRect.Y1 * m_TransformMatrix.v4) + m_TransformMatrix.v7;
    fptPoint[3].x = (fRect.X1 * m_TransformMatrix.v0) + (fRect.Y2 * m_TransformMatrix.v3) + m_TransformMatrix.v6;
    fptPoint[3].y = (fRect.X1 * m_TransformMatrix.v1) + (fRect.Y2 * m_TransformMatrix.v4) + m_TransformMatrix.v7;   
	rect.X1 = rect.X2 = fptPoint[0].x;
	rect.Y1 = rect.Y2 = fptPoint[0].y;

	for(int i=1;i<4;i++)
	{
		if(fptPoint[i].x <rect.X1)
		{
			rect.X1 = fptPoint[i].x;
		}
		else if(fptPoint[i].x >rect.X2)
		{
			rect.X2 = fptPoint[i].x;
		}
		if(fptPoint[i].y <rect.Y1)
		{
			rect.Y1 = fptPoint[i].y;
		}
		else if(fptPoint[i].y >rect.Y2)
		{
			rect.Y2 = fptPoint[i].y;
		}
	}
	return rect;
}
// point of rect(4 vertics) transform(mirror,rotate,translate) by matrix
FRECTC CTransformUtil::RectByMatrix(const FRECTC &fRect)
{
	FRECTC rect;
	double X1, Y1, X2, Y2;
    
    X1 = (fRect.X1 * m_TransformMatrix.v0) + (fRect.Y1 * m_TransformMatrix.v3) + m_TransformMatrix.v6;
    Y1 = (fRect.X1 * m_TransformMatrix.v1) + (fRect.Y1 * m_TransformMatrix.v4) + m_TransformMatrix.v7;
    X2 = (fRect.X2 * m_TransformMatrix.v0) + (fRect.Y2 * m_TransformMatrix.v3) + m_TransformMatrix.v6;
    Y2 = (fRect.X2 * m_TransformMatrix.v1) + (fRect.Y2 * m_TransformMatrix.v4) + m_TransformMatrix.v7;
    
	if(X1 < X2)
	{
        rect.X1 = X1;
        rect.X2 = X2;
    }
	else
	{
        rect.X1 = X2;
        rect.X2 = X1;
    }
    
    if(Y1 < Y2)
	{
        rect.Y1 = Y1;
        rect.Y2 = Y2;
    }
	else
	{
        rect.Y1 = Y2;
        rect.Y2 = Y1;
    }
	return rect;
}

void CTransformUtil::PointByActionCode(FPOINTC &fpt, FPOINTC fCenter, int nActionCode)
{
	ldiv_t Rem = ldiv(nActionCode, 10);

	switch(Rem.quot)
	{
	case TRANSFORM_DATA::MIRROR_V:
		MirrorVerticalPoint(fpt, fCenter.x);
		break;
	case TRANSFORM_DATA::MIRROR_H:
		MirrorHorizontalPoint(fpt, fCenter.y);
		break;
	case TRANSFORM_DATA::ROTATE_180:
		RotatePoint180(fpt, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CCW:
		RotatePoint270(fpt, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CW:
		RotatePoint90(fpt, fCenter);
		break;
	}

	switch(Rem.rem)
	{
	case TRANSFORM_DATA::MIRROR_V:
		MirrorVerticalPoint(fpt, fCenter.x);
		break;
	case TRANSFORM_DATA::MIRROR_H:
		MirrorHorizontalPoint(fpt, fCenter.y);
		break;
	case TRANSFORM_DATA::ROTATE_180:
		RotatePoint180(fpt, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CCW:
		RotatePoint270(fpt, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CW:
		RotatePoint90(fpt, fCenter);
		break;
	}
}

void CTransformUtil::RectByActionCode(FRECTC &fRect, FPOINTC fCenter, int nActionCode)
{
	ldiv_t Rem = ldiv(nActionCode, 10);

	switch(Rem.quot)
	{
	case TRANSFORM_DATA::MIRROR_V:
		MirrorVerticalRect(fRect, fCenter.x);
		break;
	case TRANSFORM_DATA::MIRROR_H:
		MirrorHorizontalRect(fRect, fCenter.y);
		break;
	case TRANSFORM_DATA::ROTATE_180:
		RotateRect180(fRect, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CCW:
		RotateRect270(fRect, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CW:
		RotateRect90(fRect, fCenter);
		break;
	}

	switch(Rem.rem)
	{
	case TRANSFORM_DATA::MIRROR_V:
		MirrorVerticalRect(fRect, fCenter.x);
		break;
	case TRANSFORM_DATA::MIRROR_H:
		MirrorHorizontalRect(fRect, fCenter.y);
		break;
	case TRANSFORM_DATA::ROTATE_180:
		RotateRect180(fRect, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CCW:
		RotateRect270(fRect, fCenter);
		break;
	case TRANSFORM_DATA::ROTATE_CW:
		RotateRect90(fRect, fCenter);
		break;
	}
}

// Set mirror parameter to mSource 
void CTransformUtil::GetMirrorMatrix(Matrix3by3 &mSource, BOOL bLRMirror)
{
	if(bLRMirror)
		mSource = fDot3by3(mSource, m_MirrorLRMatrix);
	else
		mSource = fDot3by3(mSource, m_MirrorTBMatrix);
}

// Set rotate parameter to mSource
void CTransformUtil::GetRotateMatrix(Matrix3by3 &mSource,double fAng) 
{
	Matrix3by3 mRotate;
	float cV, sV;
	cVsV(fAng,&cV,&sV);
	mRotate.v0 = cV;
	mRotate.v1 = sV;
	mRotate.v3 = -sV;
	mRotate.v4 = cV;
	mSource = fDot3by3(mSource, mRotate);
}

// Set translate parameter to mSource 
void CTransformUtil::GetTranslateMatrix(Matrix3by3 &mSource,double fTx,double fTy) 
{
	Matrix3by3 mTranslate;
	mTranslate.v6 = fTx;
	mTranslate.v6 = fTy;

	mSource = fDot3by3(mSource,mTranslate);
}


// Set mirror parameter to m_TransformMatrix 
void CTransformUtil::MirrorMatrix(BOOL bLRMirror)
{
	if(bLRMirror)
		m_TransformMatrix = fDot3by3(m_TransformMatrix, m_MirrorLRMatrix);
	else
		m_TransformMatrix = fDot3by3(m_TransformMatrix, m_MirrorTBMatrix);
}
// Set translate parameter to mSource
void CTransformUtil::TranslateMatrix(double fTx,double fTy) 
{
	Matrix3by3 mTranslate;
	mTranslate.v6 = fTx;
	mTranslate.v6 = fTy;
	m_TransformMatrix = fDot3by3(m_TransformMatrix,mTranslate);
}

// Set rotate parameter to m_TransformMatrix by rotate angle
void CTransformUtil::RotateMatrix(double fAng) 
{
	Matrix3by3 mRotate;
	float cV, sV;
	switch((int)fAng)
	{
	case 0:
		break;
	case 90:
		RotateMatrix90();
		break;
	case 180:
		RotateMatrix180();
		break;
	case 270:
		RotateMatrix270();
		break;
	default:
		cV = cos(fAng * DEG2RAD);
		sV = sin(fAng * DEG2RAD);
		mRotate.v0 = cV;
		mRotate.v1 = sV;
		mRotate.v3 = -sV;
		mRotate.v4 = cV;
		m_TransformMatrix = fDot3by3(m_TransformMatrix, mRotate);
	}
}
// Set rotate CCW90 degrees parameter to m_TransformMatrix
void CTransformUtil::RotateMatrix90() 
{
	m_TransformMatrix = fDot3by3(m_TransformMatrix, m_Rotate90Matrix);
}
// Set rotate CCW180 degrees parameter to m_TransformMatrix
void CTransformUtil::RotateMatrix180() 
{
	m_TransformMatrix = fDot3by3(m_TransformMatrix, m_Rotate180Matrix);
}
// Set rotate CCW270 degrees parameter to m_TransformMatrix
void CTransformUtil::RotateMatrix270() 
{
	m_TransformMatrix = fDot3by3(m_TransformMatrix, m_Rotate270Matrix);
}

Matrix3by3 CTransformUtil::MakeGDS2Matrix(int nTmLevel, BOOL bXAxisMirror, double dAngle, CfPoint fptTranslate, double dMag)
{
	Matrix3by3 mMatrix;

	BOOL bMirrorRotateScale=TRUE;
	BOOL bTranslate = FALSE;
	double dRad = dAngle * DEG2RAD;
	double cV = cos(dRad);
	double sV = sin(dRad);
	Matrix3by3 mMatrixScale = Matrix3by3(dMag,0.0,0.0,0.0,dMag,0.0,0.0,0.0,1.0);
//	Initialze(m_TranslateMatrix);

	m_TranslateMatrix.v6 = fptTranslate.x;
	m_TranslateMatrix.v7 = fptTranslate.y;	
	if(fptTranslate.x != 0.0 || fptTranslate.y != 0.0)
	{
		// translate
		bTranslate = TRUE;
	}

	if(bXAxisMirror)
	{
		if(cV == 1.0 && sV == 0.0)
		{
			if(dMag != 1.0)
			{
				m_RotateMatrix = fDot3by3(m_MirrorTBMatrix , mMatrixScale);
			}
			else
			{
				m_RotateMatrix = m_MirrorTBMatrix;
			}
		}
		else
		{
			// set clockwise rotate parameter to matrix
			//Initialze(m_RotateMatrix);
			m_RotateMatrix.v0 = cV;
			m_RotateMatrix.v1 = sV;
			m_RotateMatrix.v3 = -sV;
			m_RotateMatrix.v4 = cV;
			m_RotateMatrix = fDot3by3(m_MirrorTBMatrix , m_RotateMatrix);
			if(dMag != 1.0)
			{
				m_RotateMatrix = fDot3by3(m_RotateMatrix , mMatrixScale);
			}
		}
	}
	else
	{
		if(cV == 1.0 && sV == 0.0)
		{
			if(dMag != 1.0)
			{
				m_RotateMatrix = mMatrixScale;
			}
			else
			{
				bMirrorRotateScale = FALSE;
			}
		}
		else
		{
			// set clockwise rotate parameter to matrix
			//Initialze(m_RotateMatrix);
			m_RotateMatrix.v0 = cV;
			m_RotateMatrix.v1 = sV;
			m_RotateMatrix.v3 = -sV;
			m_RotateMatrix.v4 = cV;
			if(dMag != 1.0)
			{
				m_RotateMatrix = fDot3by3(m_RotateMatrix , mMatrixScale);
			}
		}

	}
	if(bMirrorRotateScale)
	{
		if(bTranslate)
			mMatrix = fDot3by3(m_RotateMatrix, m_TranslateMatrix);
		else
			mMatrix = m_RotateMatrix;
	}
	else
	{
		if(bTranslate)
		{
			mMatrix.v6 = fptTranslate.x; 
			mMatrix.v7 = fptTranslate.y;
		}
	}
	// make accumulated transform matrix
	if(nTmLevel == 0)
		m_TransformMatrix = mMatrix;
	else
		m_TransformMatrix = fDot3by3(mMatrix,m_PrevMatrix[nTmLevel-1]);
	
	// store current transform matrix
	m_PrevMatrix[nTmLevel] = m_TransformMatrix;

	return m_TransformMatrix;
}
Matrix3by3 CTransformUtil::MakeSnRMatrix(int nTmLevel, BOOL bLRMirror, int nActionCode, double cV, double sV, double tx, double ty, double fDatumX, double fDatumY) 
{
	Matrix3by3 mMatrix;

	BOOL bRotateMirror=TRUE;
	BOOL bTranslate = FALSE;
	BOOL bOffset = FALSE;

//	Initialze(m_TranslateMatrix);
	m_TranslateMatrix.v6 = 0.0;
	m_TranslateMatrix.v7 = 0.0;
	if(fDatumX != 0.0 || fDatumY != 0.0)
	{
		// offset
//		Initialze(m_OffsetMatrix);
		m_OffsetMatrix.v6 = -fDatumX;
		m_OffsetMatrix.v7 = -fDatumY;
		bOffset = TRUE;

		m_TranslateMatrix.v6 = fDatumX;
		m_TranslateMatrix.v7 = fDatumY;
	}
	
	if(tx != 0.0 || ty != 0.0)
	{
		// translate

		m_TranslateMatrix.v6 += tx;
		m_TranslateMatrix.v7 += ty;
		bTranslate = TRUE;
	}

	if(cV == 1.0 && sV == 0.0)
	{
		bRotateMirror = FALSE;
	}
	else
	{
		// set clockwise rotate parameter to matrix
		//Initialze(m_RotateMatrix);
		m_RotateMatrix.v0 = cV;
		m_RotateMatrix.v1 = sV;
		m_RotateMatrix.v3 = -sV;
		m_RotateMatrix.v4 = cV;
	}
	
	// mirror
	if( bLRMirror )
	{
		if(bRotateMirror)
			m_RotateMatrix = fDot3by3(m_RotateMatrix, m_MirrorLRMatrix);
		else
		{
			m_RotateMatrix.v0 = -1.0;
			m_RotateMatrix.v1 = 0.0;
			m_RotateMatrix.v3 = 0.0;
			m_RotateMatrix.v4 = 1.0;
		}
		bRotateMirror = TRUE;
	}


	// make this transform matrix
	if(bOffset)
	{
		if(bRotateMirror)
			mMatrix = fDot3by3(m_OffsetMatrix, fDot3by3(m_RotateMatrix, m_TranslateMatrix));
		else
		{
			// offset은 Rotate에만 유효하므로 Rotate가 존재 하지 않을 경우 반영하지 않는다.
			//mMatrix.v6 = tx+fDatumX; 
			//mMatrix.v7 = ty+fDatumY; 
			mMatrix.v6 = tx; 
			mMatrix.v7 = ty; 
		}
	}
	else
	{
		if(bRotateMirror)
		{
			if(bTranslate)
				mMatrix = fDot3by3(m_RotateMatrix, m_TranslateMatrix);
			else
				mMatrix = m_RotateMatrix;
		}
		else
		{
			mMatrix.v6 = tx; 
			mMatrix.v7 = ty;
		}
	}

	// make accumulated transform matrix
	if(nTmLevel == 0)
		m_TransformMatrix = mMatrix;
	else
		m_TransformMatrix = fDot3by3(mMatrix,m_PrevMatrix[nTmLevel-1]);
	
	// store current transform matrix
	m_PrevMatrix[nTmLevel] = m_TransformMatrix;

	if(nActionCode == 0)
		return m_TransformMatrix;

//////////////////////
// nActionCode
// 0: None
// 1: L-R Mirror	
// 2: T-B Mirror	
// 3: CCW 180 Rotate
// 4: CCW 90, CW 270 Rotate
// 5: CCW 270, CW 90 Rotate

 	ldiv_t Rem = ldiv(nActionCode, 10);
	switch(Rem.quot)
	{
		case 1: // L-R Mirror
			MirrorMatrix(TRUE);
			break;
        case 2: // T-B Mirror
			MirrorMatrix(FALSE);
			break;
        case 3: // CCW 180 Rotate
			RotateMatrix180();
			break;
		case 4: // CCW 90 Rotate
			RotateMatrix90();
			break;
		case 5: // CCW 270 Rotate
			RotateMatrix270();
			break;
	}
	switch(Rem.rem)
	{
		case 1: // L-R Mirror
			MirrorMatrix(TRUE);
			break;
        case 2: // T-B Mirror
			MirrorMatrix(FALSE);
			break;
        case 3: // CCW 180 Rotate
			RotateMatrix180();
			break;
		case 4: // CCW 90 Rotate
			RotateMatrix90();
			break;
		case 5: // CCW 270 Rotate
			RotateMatrix270();
			break;
	}

	return m_TransformMatrix;
}

BOOL CTransformUtil::GetScaleRect(FRECTC *rect, float fScaleX, float fScaleY)
{
	rect->X1 *= fScaleX;
	rect->Y1 *= fScaleY;
	rect->X2 *= fScaleX;
	rect->Y2 *= fScaleY;

	return FALSE;
}

// 함수명 변경 CheckPointInRect
BOOL CTransformUtil::CheckPointInRect(FRECTC &fRect, float x, float y)
{
    if (fRect.X1 <= x && x <= fRect.X2 && fRect.Y1 <= y && y <= fRect.Y2 )
        return TRUE;
    else
        return FALSE;
}

// 함수명 변경 CheckRecxInRect
// Parameter 순서 변경
BOOL CTransformUtil::CheckRectInRect(FRECTC *Inner, FRECTC *Outer)
{
    if (Outer->X1 <= Inner->X1 && Outer->X2 >= Inner->X2 && Outer->Y1 <= Inner->Y1 && Outer->Y2 >= Inner->Y2) 
		return TRUE;
	
	return FALSE;
}
BOOL CTransformUtil::CheckRectInRect(CfRect *Inner, CfRect *Outer)
{
    if (Outer->left <= Inner->left && Outer->right >= Inner->right && Outer->bottom <= Inner->bottom && Outer->top >= Inner->top) 
		return TRUE;
	
	return FALSE;
}
// Parameter 순서 변경
BOOL CTransformUtil::CheckUnionRect(FRECTC *fRect1, FRECTC *fRect2)
{   
	if(fRect1->Y2 < fRect2->Y1 || fRect1->Y1 > fRect2->Y2 || fRect1->X2 < fRect2->X1 || fRect1->X1 > fRect2->X2)
		return FALSE;
	return TRUE;
}
// ReadODB로 이동
void CTransformUtil::MirrorVerticalRect(FRECTC &fRect, double vAxis) 
{
    FRECTC rRect;

	rRect = fRect;
    rRect.X1 = -(fRect.X2 - vAxis) + vAxis;
    rRect.X2 = -(fRect.X1 - vAxis) + vAxis;

	fRect = rRect;
}
// DrawODB로 이동
void CTransformUtil::MirrorHorizontalRect(FRECTC &fRect, double hAxis)
{
    FRECTC rRect;

	rRect = fRect;
    rRect.Y1 = -(fRect.Y2 - hAxis) + hAxis;
    rRect.Y2 = -(fRect.Y1 - hAxis) + hAxis;
	fRect = rRect;
}
// DrawODB로 이동
void CTransformUtil::MirrorHorizontalPoint(FPOINTC &p, double hAxis)
{
	FPOINTC r;

    r.x = p.x;
    if (hAxis < p.y) 
		r.y = hAxis - fabs(p.y - hAxis); 
	else 
		r.y = hAxis + fabs(p.y - hAxis);
	p = r;
}
// DrawODB로 이동
void CTransformUtil::MirrorVerticalPoint(FPOINTC &p, double vAxis)
{
	FPOINTC r;
    if (vAxis < p.x)
		r.x = vAxis - fabs(p.x - vAxis); 
	else 
		r.x = vAxis + fabs(p.x - vAxis);
    r.y = p.y;

	p = r;
}

// ReadODB class로 이동
void CTransformUtil::TranslatePoint(FPOINTC &p, double dX, double dY)
{
//	FPOINTC pt(p.x + dX,p.y + dY);
//	p = pt;
	p.x += dX;
	p.y += dY;
}

// readODB class로 이동
void CTransformUtil::TranslateRect(FRECTC &R, double dX, double dY)
{
//	FRECTC mn( R.X1 + dX, R.Y1 + dY, R.X2 + dX, R.Y2 + dY);
//	R = mn;
	R.X1 += dX;
	R.Y1 += dY;
	R.X2 += dX;
	R.Y2 += dY;
}

// Draw274X, DRAWODB로 이동
void CTransformUtil::RotatePoint(FPOINTC &p, double cV, double sV, double cX, double cY)
{
	FPOINTC pt(((p.x - cX) * cV) - ((p.y - cY) * sV) + cX,((p.x - cX) * sV) + ((p.y - cY) * cV) + cY);
	p = pt;
}

// Draw274X, DRAWODB로 이동
void CTransformUtil::RotatePoint(FPOINTC &fpt, FPOINTC &fCenter, double fAng)
{
	float cV, sV;
	switch((int)fAng)
	{
	case 90:
		RotatePoint90(fpt,fCenter);
		break;
	case 180:
		RotatePoint180(fpt,fCenter);
		break;
	case 270:
		RotatePoint270(fpt,fCenter);
		break;
	default:
		cV = cos(fAng * DEG2RAD);
		sV = sin(fAng * DEG2RAD);
		FPOINTC pt(((fpt.x - fCenter.x) * cV) - ((fpt.y - fCenter.y) * sV) + fCenter.x,
				   ((fpt.x - fCenter.x) * sV) + ((fpt.y - fCenter.y) * cV) + fCenter.y);
		fpt= pt;
	}
}

void CTransformUtil::RotatePoint(FPOINTC &fpt, FPOINTC &fCenter)
{
	FPOINTC pt(((fpt.x - fCenter.x) * m_fCosine) - ((fpt.y - fCenter.y) * m_fSine) + fCenter.x,
		       ((fpt.x - fCenter.x) * m_fSine) + ((fpt.y - fCenter.y) * m_fCosine) + fCenter.y);
	fpt= pt;
}

void CTransformUtil::RotatePoint90(FPOINTC &fpt, FPOINTC &fCenter)
{
	FPOINTC pt(-fpt.y + fCenter.y + fCenter.x,fpt.x - fCenter.x + fCenter.y);
	fpt= pt;
}

void CTransformUtil::RotatePoint180(FPOINTC &fpt, FPOINTC &fCenter)
{
	FPOINTC pt(-fpt.x + fCenter.x + fCenter.x,-fpt.y + fCenter.y + fCenter.y);
	fpt= pt;
}

void CTransformUtil::RotatePoint270(FPOINTC &fpt, FPOINTC &fCenter)
{
	FPOINTC pt(fpt.y - fCenter.y + fCenter.x,-fpt.x + fCenter.x + fCenter.y);
	fpt= pt;
}

void CTransformUtil::RotateRect(FRECTC &fRect, FPOINTC &fCenter, double fAng)
{
    double X1, Y1, X2, Y2;
	FRECTC rRect;
	float cV, sV;

	switch((int)fAng)
	{
	case 90:
		RotateRect90(fRect,fCenter);
		break;
	case 180:
		RotateRect180(fRect,fCenter);
		break;
	case 270:
		RotateRect270(fRect,fCenter);
		break;
	default:
		cV = cos(fAng * DEG2RAD);
		sV = sin(fAng * DEG2RAD);

		// rotate left-bottom and right top point
		X1 = ((fRect.X1 - fCenter.x) * cV) - ((fRect.Y1 - fCenter.y) * sV) + fCenter.x;
		Y1 = ((fRect.X1 - fCenter.x) * sV) + ((fRect.Y1 - fCenter.y) * cV) + fCenter.y;
		X2 = ((fRect.X2 - fCenter.x) * cV) - ((fRect.Y2 - fCenter.y) * sV) + fCenter.x;
		Y2 = ((fRect.X2 - fCenter.x) * sV) + ((fRect.Y2 - fCenter.y) * cV) + fCenter.y;
    
		if (X1 < X2)
		{
			rRect.X1 = X1;
			rRect.X2 = X2;
		}
		else
		{
			rRect.X1 = X2;
			rRect.X2 = X1;
		}
    
		if (Y1 < Y2)
		{
			rRect.Y1 = Y1;
			rRect.Y2 = Y2;
		}
		else
		{
			rRect.Y1 = Y2;
			rRect.Y2 = Y1;
		}
		// rotate left-top and right bottom point
		X1 = ((fRect.X1 - fCenter.x) * cV) - ((fRect.Y2 - fCenter.y) * sV) + fCenter.x;
		Y1 = ((fRect.X1 - fCenter.x) * sV) + ((fRect.Y2 - fCenter.y) * cV) + fCenter.y;
		X2 = ((fRect.X2 - fCenter.x) * cV) - ((fRect.Y1 - fCenter.y) * sV) + fCenter.x;
		Y2 = ((fRect.X2 - fCenter.x) * sV) + ((fRect.Y1 - fCenter.y) * cV) + fCenter.y;

		if (X1 < X2)
		{
			fRect.X1 = X1;
			fRect.X2 = X2;
		}
		else
		{
			fRect.X1 = X2;
			fRect.X2 = X1;
		}
    
		if (Y1 < Y2)
		{
			fRect.Y1 = Y1;
			fRect.Y2 = Y2;
		}
		else
		{
			fRect.Y1 = Y2;
			fRect.Y2 = Y1;
		}

		if(fRect.X1 > rRect.X1)	fRect.X1 = rRect.X1; 
		if(fRect.Y1 > rRect.Y1)	fRect.Y1 = rRect.Y1;   
		if(fRect.X2 < rRect.X2)	fRect.X2 = rRect.X2;   
		if(fRect.Y2 < rRect.Y2)	fRect.Y2 = rRect.Y2;
	}
}

void CTransformUtil::RotateRect90(FRECTC &fRect, FPOINTC &fpt)
{
    double X1, Y1, X2, Y2;

    X1 = -fRect.Y1 + fpt.y + fpt.x;
    Y1 = fRect.X1 - fpt.x + fpt.y;
    X2 = -fRect.Y2 + fpt.y + fpt.x;
    Y2 = fRect.X2 - fpt.x + fpt.y;
   
    if (X1 < X2)
	{
		fRect.X1 = X1;
        fRect.X2 = X2;
	}
    else
	{
        fRect.X1 = X2;
        fRect.X2 = X1;
    }
    
    if (Y1 < Y2)
	{
		fRect.Y1 = Y1;
        fRect.Y2 = Y2;
	}
    else
	{
		fRect.Y1 = Y2;
        fRect.Y2 = Y1;
    }
}

void CTransformUtil::RotateRect180(FRECTC &fRect, FPOINTC &fpt)
{
    double X1, Y1, X2, Y2;

    X1 = -fRect.X1 + fpt.x + fpt.x;
    Y1 = -fRect.Y1 + fpt.y + fpt.y;
    X2 = -fRect.X2 + fpt.x + fpt.x;
    Y2 = -fRect.Y2 + fpt.y + fpt.y;
    
    if (X1 < X2)
	{
		fRect.X1 = X1;
        fRect.X2 = X2;
	}
    else
	{
        fRect.X1 = X2;
        fRect.X2 = X1;
    }
    
    if (Y1 < Y2)
	{
		fRect.Y1 = Y1;
        fRect.Y2 = Y2;
	}
    else
	{
		fRect.Y1 = Y2;
        fRect.Y2 = Y1;
    }
}

void CTransformUtil::RotateRect270(FRECTC &fRect, FPOINTC &fpt)
{
    double X1, Y1, X2, Y2;

    X1 = ((fRect.Y1 - fpt.y)) + fpt.x;
    Y1 = -((fRect.X1 - fpt.x)) + fpt.y;
    X2 = ((fRect.Y2 - fpt.y)) + fpt.x;
    Y2 = -((fRect.X2 - fpt.x)) + fpt.y;
    
    if (X1 < X2)
	{
		fRect.X1 = X1;
        fRect.X2 = X2;
	}
    else
	{
        fRect.X1 = X2;
        fRect.X2 = X1;
    }
    
    if (Y1 < Y2)
	{
		fRect.Y1 = Y1;
        fRect.Y2 = Y2;
	}
    else
	{
		fRect.Y1 = Y2;
        fRect.Y2 = Y1;
    }
}

void CTransformUtil::AdjustExtentByPoint(FRECTC &Extent, const FPOINTC &pt)
{
	if (pt.x < Extent.X1)
		Extent.X1 = pt.x;
	if (pt.y < Extent.Y1)
		Extent.Y1 = pt.y;
	if (pt.x > Extent.X2)
		Extent.X2 = pt.x;
	if (pt.y > Extent.Y2)
		Extent.Y2 = pt.y;
}

void CTransformUtil::AdjustExtentByRect(FRECTC &Extent, const FRECTC &rect)
{
	if(Extent.X1 > rect.X1)
		Extent.X1 = rect.X1;
	if(Extent.X2 < rect.X2)
		Extent.X2 = rect.X2;
	if(Extent.Y1 > rect.Y1)
		Extent.Y1 = rect.Y1;
	if(Extent.Y2 < rect.Y2)
		Extent.Y2 = rect.Y2;
}

void CTransformUtil::SortRectX(vector <FRECTC> &rectExtent, int nStart, int nEnd)
{
	int i, j, k;
	float fValue;

	if(nStart >= nEnd)
		return;

	i = nStart -1;
	j = nEnd+1;
	k = (nStart + nEnd)/2;
	fValue = rectExtent.at(k).X1;
	do 
	{
		do 
		{
			i++;
		} while(rectExtent.at(i).X1 < fValue);
		
		do 
		{
			j--;
		} while(rectExtent.at(j).X1 > fValue);
		
		if(i < j)
			SwapRect(rectExtent, i, j);
	} while(i < j);

	SortRectX(rectExtent, nStart, i - 1);
	SortRectX(rectExtent, j + 1, nEnd);
}

void CTransformUtil::SortRectY(vector <FRECTC> &rectExtent, int nStart, int nEnd)
{
	int i, j, k;
	float fValue;

	if(nStart >= nEnd)
		return;

	i = nStart -1;
	j = nEnd +1;
	k = (nStart + nEnd)/2;
	fValue = -rectExtent.at(k).Y1;
	do 
	{
		do 
		{
			i++;
		} while(-rectExtent.at(i).Y1 < fValue);
		
		do 
		{
			j--;
		} while(-rectExtent.at(j).Y1 > fValue);
		
		if(i < j)
			SwapRect(rectExtent, i, j);
	} while(i < j);

	SortRectY(rectExtent, nStart, i - 1);
	SortRectY(rectExtent, j + 1, nEnd);
}

void CTransformUtil::SwapRect(vector <FRECTC> &rectExtent, int nIndex1, int nIndex2)
{
	FRECTC fRect;

	fRect = rectExtent.at(nIndex1);
	rectExtent.at(nIndex1) = rectExtent.at(nIndex2);
	rectExtent.at(nIndex2) = fRect;
}

