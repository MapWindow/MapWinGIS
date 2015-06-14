#pragma once
#include "Expression.h"

class ExpressionParser
{

public:	
	ExpressionParser()
		: _expression(NULL)
	{
			
	}
	
private:
	Expression* _expression;

private:
	bool IsDecimal(CString& str);
	bool IsDecimal(char chr, bool& exponential);
	bool IsDecimalZero(CString& str);
	bool IsInteger(CString& str);
	bool IsOperator(char s);
	bool IsOperator(CString s);
	bool IsFunctionName(char s);

	bool ParseTree(CString s, int partCount);
	CFunction* ParseFunction(CString& s, int begin, int& fnBegin);
	bool ParseArgumentList(CString s, CFunction* fn);

	bool GetBrackets(CString expression, int& begin, int& end, CString openingSymbol = "(", CString closingSymbol = ")");
	CString GetInnerString(CString& s, int begin, int end);

	CExpressionPart* ParseExpressionPart(CString s);
	bool ReadValue(CString s, int& position, CElement* element);
	bool ReadOperation(CString s, int& position, CElement& element);
	
	void ReplacePart(CString& s, int begin, int end, int& count);
	bool ReplaceStringLiterals(CString& s, int& count);
	bool ReplaceFieldNames(CString& s, int& count);
	void ReplaceSubString(CString& s, int begin, int length, CString replacement);

	void SetErrorMessage(CString msg) { _expression->SetErrorMessage(msg); }
	void SetErrorPosition(int position) { _expression->SetErrorPosition(position); }
public:
	bool Parse(Expression* expression, CString s, bool useFields);
	bool ReplaceParameterlessFunctions(CString& s, int& partCount);

};

