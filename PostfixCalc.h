// PostfixCalc.h: interface for the CPostfixCalc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSTFIXCALC_H__5FD8A064_6C4D_47B6_BFBA_6E82230B702D__INCLUDED_)
#define AFX_POSTFIXCALC_H__5FD8A064_6C4D_47B6_BFBA_6E82230B702D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stack>
#include <afxtempl.h>

using namespace std;

typedef enum { UNKNOWN,END, PARENTHESES, DELIMITER, OPERATOR, NUMBER, VARIABLE} TTYPE;
typedef enum { VAR, NUM } VTYPE;

// stack element
typedef struct {		
	double  fValue;		// number
	CString strName;	// Variable Name
	VTYPE type;			// type: Variable or number
} structOperand;	

class CPostfixCalc  
{
public:
	CPostfixCalc();
	virtual ~CPostfixCalc();

private:
	CMap<CString,LPCTSTR,double,double> mVarMap;	// Variable map
	
	stack <char> OpStack;			// stack for Operator
	stack <structOperand> ValStack;	// stack for Value;

	int RegisterNewVariable(const TCHAR *s);
	int GetToken(const TCHAR **expr, TCHAR *token);
	int GetPrecedence(char op);
	double Calculator(char op, structOperand v1, structOperand v2);

public:
	BOOL InfixToPostfix(const TCHAR *Infix, CString &Postfix);
	double CalcPostfix(const TCHAR *Postfix);
	double Calc(const TCHAR *Infix);

};

#endif // !defined(AFX_POSTFIXCALC_H__5FD8A064_6C4D_47B6_BFBA_6E82230B702D__INCLUDED_)
