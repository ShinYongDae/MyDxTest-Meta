// PostfixCalc.cpp: implementation of the CPostfixCalc class.
/*
Algorithm for Infix to Postfix 

One of the applications of stack is in the evaluation of arithmetic expressions.
To evaluate any arithmetic expression we convert the infix expression to postfix.
Then evaluate the postfix expression using a stack.

 Define a stack
 Go through each character in the string
 If it is between 0 to 9, append it to output string.
 If it is left brace push to stack
 If it is operator *+-/ then 
    If the stack is empty push it to the stack
       If the stack is not empty then start a loop:
          If the top of the stack has higher precedence
             Then pop and append to output string
             Else break
           Push to the stack
 
If it is right brace then
   While stack not empty and top not equal to left brace
    Pop from stack and append to output string

  Finally pop out the left brace.
 
If there is any input in the stack pop and append to the output string.
*/
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PostfixCalc.h"
#include <assert.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPostfixCalc::CPostfixCalc()
{
	mVarMap.RemoveAll();
}

CPostfixCalc::~CPostfixCalc()
{
	mVarMap.RemoveAll();
}

// register new variable to variable array and return it's index 
int CPostfixCalc::RegisterNewVariable(const TCHAR *s)
{
	double fValue;
	int nSizeOfVar;

	if(!mVarMap.Lookup(s,fValue))
		mVarMap.SetAt(s,0.0);
	
	nSizeOfVar = mVarMap.GetCount();

	return nSizeOfVar;
}

//--------------------------------------------------------------------
// tokening: read string and parsing operator, operand, variable, parentheses, delimiter
//--------------------------------------------------------------------
int CPostfixCalc::GetToken(const TCHAR **expr, TCHAR *token)
{
    const TCHAR *x = *expr;
    TCHAR *t = token;
    TTYPE ttype = END;

    *t = '\0';

    // ignore space character
    while (*x && _tcschr(_T(" \t\r\n"),*x))
		++x;

    if (*x =='\0')  // end of string
	{
        *token = '\0';
        ttype = END;
    }
    else if (_tcschr(_T("+-*/=%"), *x))  // operator
	{
		*t++ = *x++;
		*t = '\0';
		ttype = OPERATOR;
    }
    else if (_tcschr(_T("()"), *x)) // parentheses
	{
        *t++ = *x++;
        *t = '\0';
        ttype = PARENTHESES;
    }
    else if (_tcschr(_T(";,"), *x)) // delimiter
	{
        *t++ = *x++;
        *t = '\0';
        ttype = DELIMITER;
    }
    else if (_istdigit(*x) || *x == '.')  // number
	{
        int dot;
        do {
            dot = (*x == '.');
            *t++ = *x++;
        } while( (_istdigit(*x) || (!dot && *x == '.')));
        *t = '\0';
        ttype = NUMBER;
    }
    else if (_istalpha(*x))  // variable
	{
        do {
            *t++ = *x++;
        } while (_istalnum(*x));
        *t = '\0';
        ttype = VARIABLE;
    }
    else // error
	{
		AfxMessageBox(_T("Unknowgn character Error at get_token() func"));
		return UNKNOWN;
    }

    *expr = x;

    return ttype;
}

// Check and return operator precedence, i.e priority 
int CPostfixCalc::GetPrecedence(char op)
{
    switch (op)
    {
    case '(':
    case ')': return 0;
    case '=': return 10;
    case '+':
    case '-': return 20;
    case '*':
    case '/':
    case '%': return 30;
    }
    return 0;
}

/*
convert infix to postfix format
---------------------------------------------------------------------
1. ( 이면, push
2. ) 이면, 스택에서 ( 이 나올 때까지 pop 하여 출력
3. 연산자이면, 스택에서 우선순위가 높지 않은 연산자가 나올 때까지
   pop 하여 출력하고 자신을 push
4. 피연산자이면, 출력
5. 모든 입력이 끝나면 스택에 있는 연산자들을 모두 pop 하여 출력
*/
// Convert infix to postfix

BOOL CPostfixCalc::InfixToPostfix(const TCHAR *Infix, CString &Postfix)
{
	
//     TCHAR token[32];
//     TTYPE ttype;
//     TCHAR ch;
// 	TCHAR tempCh;
// 
// 	// 단항 연산자에 대하여 괄호 삽입
// 	CString strTemp = Infix;
// 
// 	const TCHAR *pTemp;
// 	pTemp = Infix;
// 
// 	int p,i=0;
// 	BOOL bAdd = FALSE;
// 	while(pTemp = _tcspbrk(pTemp,_T("+-*/=%")))
// 	{
// 		tempCh = *(++pTemp);
// 		if(tempCh == _T('-') || tempCh == _T('+'))
// 		{
// 			bAdd = TRUE;
// 			p = (pTemp-Infix+i);
// 			strTemp.Insert(p,_T("("));
// 			i++;
// 			do{
// 				ch = *(++pTemp);
// 				if (_tcschr(_T("+-*/=%"), ch)) //edit by kjc
// 				{
// 					bAdd = FALSE;
// 					p = (pTemp-Infix+i);
// 					strTemp.Insert(p,_T(")"));
// 					i++;
// 					break;
// 				}
// 			}while(ch);
// 			if(bAdd)
// 				strTemp += _T(")");
// 		}
// 	}
// 	pTemp = (const TCHAR *)strTemp;
/////////////////////////////add by kjc
    TCHAR token[32];
    TTYPE ttype;
    TCHAR ch;
	TCHAR tempCh;
	CString strTemp=_T("");
	CString strMsg;
	BOOL bOperator = FALSE;
	BOOL bMark = FALSE;
	int i, nMarkCount = 0;
	const TCHAR *pTemp;
	while(*Infix)
	{
		
		if(strchr("+-*/=%", *Infix))					
		{
			if(bOperator)
			{
				if(strchr("*/=%", *Infix))
				{
					strMsg.Format(_T("error mark %c"),*Infix);
					AfxMessageBox(strMsg);
					return FALSE;
				}
				strTemp +='(';
				nMarkCount++;
			}
			else
			{
				bOperator = TRUE;
			}
		}
		else
			bOperator = FALSE;

		if(isdigit(*Infix) || *Infix == '.')
		{
			if(nMarkCount>0)
				bMark = TRUE;
			else
				bMark = FALSE;
		}
		else
		{	
			if(bMark)
			{
				for(i=0; i<nMarkCount; i++)
					strTemp += ')';	
				nMarkCount = 0;
				bMark = FALSE;
			}
		}
		strTemp += *Infix++;
		
	}
	for(i=0; i<nMarkCount; i++)
		strTemp += ')';	
	pTemp = (const TCHAR *)strTemp;
///////////////////////////////////////////////////////////////////////////////////////////////////	
	
	while (!OpStack.empty()) // initialize stack
		OpStack.pop();

	BOOL bOneOperand;
	int nOperand = 0;

    while( (ttype=(TTYPE)GetToken(&pTemp,token)) != END) // process until infix string end
    {   
		// read token and get token type
		switch(ttype)
		{
		case UNKNOWN:
			return FALSE;
        case DELIMITER: // delimiter
			break;
        case PARENTHESES: // parentheses
            switch(token[0])
			{
            case '(': // save '(' on stack
				OpStack.push('(');
				break; 
            case ')': // pop stack until matching '('
				while (!OpStack.empty())
                {
					ch=OpStack.top();
                    if(ch != '(')
					{
						Postfix += ch;
						Postfix += " ";
						OpStack.pop();
					}
					else
						break;
				}
				OpStack.pop(); // remove the '('
                break;
            }
            break;
        case OPERATOR: // process stack operators of greater precedence
			ch = token[0];
            bOneOperand = nOperand%2;
			if(bOneOperand==0)
			{
				if(ch == '+' || ch == '-')
				{
					Postfix += "0";
					Postfix += " ";
				}
			}
			while ( !OpStack.empty())
            {				
				tempCh = OpStack.top();
				if(GetPrecedence(ch) <= GetPrecedence(tempCh))
				{
					Postfix += tempCh;
					Postfix += " ";
					OpStack.pop();
				}
				else
					break;
            }
			OpStack.push(ch); // save new operator
			nOperand = 0;
            break;
        case NUMBER: // append number type operand to end of postfix string
			Postfix += token;
			Postfix += " ";
            
			nOperand++;
			break;
        case VARIABLE: // append variable type operand to end of postfix string
			Postfix += token;
			Postfix += " ";
            break;
        }
    }

	// append to postfixExp the operators remaining on the stack
	while ( !OpStack.empty())
    {
		tempCh = OpStack.top();
		Postfix += tempCh;
		Postfix += " ";
		OpStack.pop();
	}

	return TRUE;
}

double CPostfixCalc::Calculator(char op, structOperand v1, structOperand v2)
{
	double fData;

    if(v2.type == VAR) // if second parameter is variable
	{		
		v2.fValue = (mVarMap.Lookup(v2.strName,fData) == 0) ? 0 : fData;
    }

    if(op == '=') // substitution operator
    {
        if(v1.type == VAR)
		{
			mVarMap.SetAt(v1.strName,v2.fValue);
			return v2.fValue;
        }
        else
		{
            /* error */
			AfxMessageBox(_T("substitution operator error at calc() func"));
        }
    }

    if(v1.type == VAR) // if first parameter is variable
	{
		v1.fValue = (mVarMap.Lookup(v1.strName,fData) == 0) ? 0 : fData;
    }



	switch (op)
	{
	case '+': return v1.fValue+v2.fValue;
	case '-': return v1.fValue-v2.fValue;
	case '*': return v1.fValue*v2.fValue;
	case '/': if(v2.fValue == 0.0) //assert(v2.fValue != 0);
				AfxMessageBox(_T("divide by 0 error"));
				return v1.fValue/v2.fValue;
	case '%': if(v2.fValue ==0.0) //assert(v2.fValue != 0);
				AfxMessageBox(_T("divide by 0 error"));
				return fmod(v1.fValue,v2.fValue);
	}
	return 0;
}


//Calculate postfix method
//---------------------------------------------------------------------
//1. if token is number then push
//2. if token is operator then two value poping from stack and calcuate and push the it's result to stack 
//3. pop final result
double CPostfixCalc::CalcPostfix(const TCHAR *Postfix)
{
    TCHAR token[32];
    TTYPE ttype;
    structOperand v1,v2;
	int nStackSize;
    
	nStackSize = ValStack.size();
	while(!ValStack.empty()) // stack_clear;
		ValStack.pop();

    while( (ttype=(TTYPE)GetToken(&Postfix,token)) != END)
    {
        switch(ttype)
		{
        case OPERATOR: // if operator
            v2 = ValStack.top(); // second operand
			ValStack.pop();
            v1 = ValStack.top(); // first operand
			ValStack.pop();
            v1.fValue = Calculator(token[0],v1,v2);  // calculation
            v1.type = NUM;
            ValStack.push(v1); // push the result data
 			break;
        case NUMBER: // if token type is number then convert the string to data and push to stack
			v1.fValue = _tcstod(token,0);
            v1.type = NUM; // specify the number type
            ValStack.push(v1); // push the number
            break;
        case VARIABLE: // if token type is variable then read it's value and push to stack
            v1.type = VAR; // specify the variable type
            v1.strName = token;
			RegisterNewVariable(token);
            ValStack.push(v1);
			break;
		case UNKNOWN:
			return 0;
            break;
        }
    }

	double fData;
	v1 = ValStack.top(); // final result
	if(v1.type == VAR) // if the variable index is saved?
	{ 
		v1.fValue = (mVarMap.Lookup(v1.strName,fData) == 0) ? 0 : fData;
	}

	ValStack.pop();
	nStackSize = ValStack.size();

    return v1.fValue;
}

double CPostfixCalc::Calc(const TCHAR *Infix)
{
	double fData = 0;
	CString strPostfixExp;

	if(InfixToPostfix(Infix,strPostfixExp))
		fData = CalcPostfix(strPostfixExp);

	return fData;

}
////////////////////////////////////////////////////////
// for simulate 

