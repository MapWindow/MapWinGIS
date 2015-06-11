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
	CExpression* _expression;

private:
	bool ParseBrackets(CString s);
	void ReplacePart(CString& s, int begin, int end, int& count);
	bool ReplaceStringConstants(CString s, int& count);
	bool ReplaceFieldNames(CString s, int& count);
	CExpressionPart* ParseExpressionPart(CString s);
	bool ReadValue(CString s, int& position, CElement* element);
	bool ReadOperation(CString s, int& position, CElement& element);
	bool GetBrackets(CString expression, int& begin, int& end, CString openingSymbol = "(", CString closingSymbol = ")");
	void SkipSpaces(CString s, int& position);
	bool IsDecimal(CString& str);
	bool IsDecimal(char chr, bool& exponential);
	bool IsDecimalZero(CString& str);
	bool IsInteger(CString& str);
	void ReplaceFunctions(CString& expression);
	bool ParseArgumentList(CString s, int functionId);
	int ParseFunctionId(CString s, int begin);
	int TryParseFunction(CString& s, int begin, int end);
	void ReplaceSubString(CString& s, int begin, int length, CString replacement);

	void SetErrorMessage(CString msg) { _expression->SetErrorMessage(msg); }
	void SetErrorPosition(int position) { _expression->SetErrorPosition(position); }
public:
	bool ParseExpression(CExpression* expression, CString s, bool useFields);

	
};

