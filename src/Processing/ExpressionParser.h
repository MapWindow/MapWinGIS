#pragma once
#include "CustomExpression.h"

class ExpressionParser
{

public:	
	ExpressionParser()
		: _expression(NULL)
	{
			
	}
	
private:
	CustomExpression* _expression;

private:
	bool IsDecimal(CStringW& str);
	bool IsDecimal(wchar_t chr, bool& exponential);
	bool IsDecimalZero(CStringW& str);
	bool IsInteger(CStringW& str);
	bool IsOperator(wchar_t s);
	bool IsOperator(CStringW s);
	bool IsFunctionName(wchar_t s);

	bool ParseTree(CStringW s);
	CustomFunction* ParseFunction(CStringW& s, int begin, int& fnBegin);
	bool ParseArgumentList(CStringW s, CustomFunction* fn);

	bool GetBrackets(CStringW expression, int& begin, int& end, CStringW openingSymbol = "(", CStringW closingSymbol = ")");
	CStringW GetInnerString(CStringW& s, int begin, int end);

	CExpressionPart* ParseExpressionPart(CStringW s);
	bool ReadValue(CStringW s, int& position, CElement* element);
	bool ReadOperation(CStringW s, int& position, CElement& element);
	
	void ReplacePart(CStringW& s, int begin, int end);
	bool ReplaceStringLiterals(CStringW& s, int& count);
	bool ReplaceFieldNames(CStringW& s, int& count);
	void ReplaceSubString(CStringW& s, int begin, int length, CStringW replacement);

	void SetErrorMessage(CStringW msg) { _expression->SetErrorMessage(msg); }
	void SetErrorPosition(int position) { _expression->SetErrorPosition(position); }
public:
	bool Parse(CustomExpression* expression, CStringW s, bool useFields);
	bool ReplaceParameterlessFunctions(CStringW& s);

};

