#include "stdafx.h"
#include "ExpressionParser.h"
#include "Functions.h"

// *******************************************************************
//		IsDecimal()
// *******************************************************************
bool ExpressionParser::IsDecimal(wchar_t chr, bool& exponential) {
	if (chr >= L'0' && chr <= L'9')
		return true;

	if ((chr == L'.') || (chr == L','))	// specify decimal separator explicitly
	{
		return true;
	}

	if ((chr == L'e') || (chr == L'E')) {
		exponential = true;
		return true;
	}

	if (exponential && (chr == L'+' || chr == L'-')) {
		exponential = false;
		return true;
	}

	exponential = false;	// +, - can be in the next position after e only
	return false;
}

// *******************************************************************
//		IsDecimal()
// *******************************************************************
bool ExpressionParser::IsDecimal(CStringW& str) {
	for (int i = 0; i < str.GetLength(); i++) {
		if ((str[i] >= L'0') && (str[i] < L'9'))
			continue;

		if ((str[i] == L'.') || (str[i] == L',') || (str[i] == L'e') || (str[i] == L'E'))	 // specify decimal separator explicitly
			continue;

		return false;
	}

	return true;
}

// *******************************************************************
//		IsDecimalZero()
// *******************************************************************
bool ExpressionParser::IsDecimalZero(CStringW& str) {
	for (int i = 0; i < str.GetLength(); i++) {
		if ((str[i] != L'0') && (str[i] != L'.') && (str[i] != L','))		// TODO: check decimal separator in more general way
			return false;
	}
	return true;
}

// *******************************************************************
//		IsInteger()
// *******************************************************************
bool ExpressionParser::IsInteger(CStringW& str) {
	for (int i = 0; i < str.GetLength(); i++) {
		if (str[i] < L'0' || str[i] > L'9')
			return false;
	}
	return true;
}

// *******************************************************************
//		IsOperator()
// *******************************************************************
bool ExpressionParser::IsOperator(wchar_t s) {
	switch (s) {
	case L'<':
	case L'>':
	case L'=':
	case L'+':
	case L'-':
	case L'*':
	case L'/':
	case L'\\':
	case L'^':
	case L':':
		return true;
	}

	return false;
}

// *******************************************************************
//		IsFunctionName()
// *******************************************************************
bool ExpressionParser::IsFunctionName(wchar_t s) {
	return isalnum(s) || s == L'$' || s == L'_';
}

// *******************************************************************
//		IsOperator()
// *******************************************************************
bool ExpressionParser::IsOperator(CStringW s) {
	if (s.GetLength() == 0) {
		return IsOperator(s[0]);
	} else {
		s = s.MakeUpper();

		if (s.Compare(L"AND") == 0 ||
			s.Compare(L"OR") == 0 ||
			s.Compare(L"XOR") == 0 ||
			s.Compare(L"MOD") == 0) {
			return true;
		}

		return false;
	}
}

// *****************************************************************
//		Parse()
// *****************************************************************
// building list of operation; UseFields: true - only fields form attribute table; 
// false - variables, the values of which must be set
bool ExpressionParser::Parse(CustomExpression* expression, CStringW s, bool useFields) {
	_expression = expression;

	int count = 0;

	if (s.GetLength() == 0) {
		return false;
	}

	if (!ReplaceStringLiterals(s, count)) {
		return false;
	}

	if (!ReplaceFieldNames(s, count)) {
		return false;
	}

	if (!ReplaceParameterlessFunctions(s)) {
		return false;
	}

	s.Replace(L" ", L"");

	if (!ParseTree(s)) {
		return false;
	}

	return true;
}

// *****************************************************************
//		ParseTree()
// *****************************************************************
bool ExpressionParser::ParseTree(CStringW s) {
	bool found = true;
	CStringW temp;

	while (found) {
		// seeking brackets
		int begin, end;
		found = GetBrackets(s, begin, end);

		if (found) {
			int fnBegin;
			CustomFunction* fn = ParseFunction(s, begin - 1, fnBegin);

			if (fn) {
				if (!ParseArgumentList(GetInnerString(s, begin, end), fn)) {
					return false;
				}

				bool finish = fnBegin == 0 && end == s.GetLength() - 1;      // there is nothing but function left

				ReplacePart(s, fnBegin, end);

				if (finish) {
					break;
				}

				continue;
			}
		}

		CStringW expression = found ? GetInnerString(s, begin, end) : s;

		CExpressionPart* part = ParseExpressionPart(expression);

		if (part) {
			_expression->AddPart(part);
		} else {
			return false;
		}

		if (found) {
			ReplacePart(s, begin, end);
		}
	}

	return true;
}

// *******************************************************************
//		GetInnerString()
// *******************************************************************
CStringW ExpressionParser::GetInnerString(CStringW& s, int begin, int end) {
	return s.Mid(begin + 1, end - begin - 1);
}

// *******************************************************************
//		GetBrackets()
// *******************************************************************
// Returns positions of the first inner brackets
bool ExpressionParser::GetBrackets(CStringW expression, int& begin, int& end,
	CStringW openingSymbol, CStringW closingSymbol) {
	// closing bracket
	end = -1;
	for (int i = 0; i < expression.GetLength(); i++) {
		if (expression.Mid(i, 1) == closingSymbol) {
			end = i;
			break;
		}
	}

	if (end == -1) {
		return false;
	}

	// opening bracket
	for (int i = end; i >= 0; i--) {
		if (expression.Mid(i, 1) == openingSymbol) {
			begin = i;
			return true;
		}
	}
	return false;
}

// ************************************************************
//	 ParseFunction()
//************************************************************
// Tries to parse function name from the position of the opening bracket going backwards
CustomFunction* ExpressionParser::ParseFunction(CStringW& s, int begin, int& fnBegin) {
	int i = begin;

	CStringW sub;

	// reading function name from the end
	while (i >= 0) {
		if (!IsFunctionName(s[i])) {
			break;
		}

		sub = s[i] + sub;
		i--;
	}

	CustomFunction* fn = parser::GetFunction(sub);
	if (fn) {
		fnBegin = i + 1;
		return fn;
	}

	return NULL;
}

// ************************************************************
//	 ParseArgumentList()
//************************************************************
bool ExpressionParser::ParseArgumentList(CStringW s, CustomFunction* fn) {
	if (!fn) return false;

	CExpressionPart* part = new CExpressionPart();
	part->expression = s;
	part->function = fn;

	int pos = 0;
	CStringW ct;

	ct = s.Tokenize(L";", pos);

	while (ct.GetLength() != 0) {
		CExpressionPart* arg = ParseExpressionPart(ct);

		if (arg) {
			arg->isArgument = true;
			_expression->AddPart(arg);

			// argument list holds references to parts which are calculated before function
			// there is no need to delete arguments, as they will be deleted when parts list is cleared
			part->arguments.push_back(arg);
		} else {
			// error message is set above			
			delete part;
			return false;
		}

		ct = s.Tokenize(L";", pos);
	};

	CStringW errorMessage;
	if (!fn->CheckArguments(part->arguments.size(), errorMessage)) {
		SetErrorMessage(errorMessage);
		delete part;
		return false;
	}

	_expression->AddPart(part);
	return true;
}

// *****************************************************************
//		ParseExpressionPart
// *****************************************************************
// Creates elements from the part of expression in the brackets
CExpressionPart* ExpressionParser::ParseExpressionPart(CStringW s) {
	bool readVal = true;	// true - reading values and unary operations; false - reading binary operations

	// adding a part
	CExpressionPart* part = new CExpressionPart();
	part->expression = s;

	for (int i = 0; i < s.GetLength(); i++) {
		if (i >= s.GetLength())
			break;

		// reading element
		CElement* element = new CElement();
		bool result = readVal ? ReadValue(s, i, element) : ReadOperation(s, i, *element);

		if (!result) {
			delete element;
			delete part;
			return NULL;
		}

		// saving element
		part->elements.push_back(element);

		//in case operation was unary the next element should be value as well
		if (element->operation != operNOT && element->operation != operChangeSign) {
			readVal = !readVal;
		}
	}

	if (part->elements.size() == 0) {
		SetErrorMessage(L"表达式部分为空");
		delete part;
		return NULL;
	}

	if (part->elements[part->elements.size() - 1]->type == etOperation) {
		SetErrorMessage(L"运算符缺少右边操作数");
		delete part;
		return NULL;
	}

	return part;
}

// *******************************************************************
//		ReadValue()
// *******************************************************************
// Parses string from the given position
bool ExpressionParser::ReadValue(CStringW s, int& position, CElement* element) {
	USES_CONVERSION;
	CStringW sub;		// substring
	wchar_t chr = s[position];

	switch (chr) {
	case L'{': {
		sub = "";

		position++;
		chr = s[position];

		while (isdigit(chr)) {
			sub += chr;
			position++;
			if (position > s.GetLength() - 1)
				break;
			chr = s[position];
		}
		position--;

		// writing the number of bracket
		if (IsInteger(sub)) {
			unsigned int index = _wtoi((LPCWSTR)sub);
			sub = (*_expression->GetStrings())[index];
		} else {
			SetErrorMessage(L"读入字段名出错");
			return false;
		}

		if (position >= s.GetLength()) {
			SetErrorMessage(L"左中括号 [ 不能出现在字段名中");
			return false;
		}

		// the next character should be ]
		position++;
		chr = s[position];

		if (chr != '}') {
			SetErrorMessage(L"没有右大括号 }");
			return false;
		}


		if (_expression->GetUseFields()) {
			// searching the field
			element->fieldIndex = -1;
			CStringW str = sub.MakeLower();

			vector<CStringW>* fields = _expression->GetFields();

			for (unsigned int i = 0; i < fields->size(); i++) {
				if ((*fields)[i] == str) {
					element->isField = true;
					element->type = etValue;
					element->fieldName = sub;
					element->fieldIndex = i;
				}
			}
			if (element->fieldIndex == -1) {
				SetErrorMessage(L"没有字段：" + sub);
				return false;
			}
		} else {
			//if (_useVariables)
			element->type = etValue;
			CString s(sub);
			element->fieldName = s;
			element->isField = true;
		}
		break;
	}
	case L'"': {
		sub = "";
		position++;
		chr = s[position];

		while ((position < s.GetLength() - 1) && (chr != L'"')) {
			sub += chr;
			position++;
			if (position > s.GetLength() - 1)
				break;
			chr = s[position];
		}
		element->type = etValue;
		element->val->str(sub);
		break;
	}

	case L'0':
	case L'1':
	case L'2':
	case L'3':
	case L'4':
	case L'5':
	case L'6':
	case L'7':
	case L'8':
	case L'9':
	{
		sub = "";
		bool exponential = false;
		while (IsDecimal(chr, exponential)) {
			sub += chr;
			position++;
			if (position > s.GetLength() - 1)
				break;
			chr = s[position];
		}
		position--;

		double val = Utility::wtof_custom((LPCWSTR)sub);
		if (val != 0.0) {
			element->type = etValue;
			element->val->dbl(val);
		} else // if (val == 0.0)			// fun駎ion returns 0.0 in case string can't be represented as number
		{
			if (IsDecimalZero(sub)) {
				element->type = etValue;
				element->val->dbl(val);
			} else {
				SetErrorMessage(L"值不是一个数字：" + sub);
			}
		}
		break;
	}
	case L'T':
	case L't':
		if (s.Mid(position, 4).MakeLower() == L"true") {
			position += 3;
			element->val->bln(true);
			element->type = etValue;
		}
		break;
	case L'F':
	case L'f':
		if (s.Mid(position, 5).MakeLower() == L"false") {
			position += 4;
			element->val->bln(false);
			element->type = etValue;
		}
		break;
	case L'N':
	case L'n':
		if (s.Mid(position, 3).MakeLower() == L"not") {
			position += 2;
			element->type = etOperation;
			element->priority = 5;
			element->operation = operNOT;
		}
		break;
	case L'-':
		element->type = etOperation;
		element->priority = 3;
		element->operation = operChangeSign;
		element->type = etOperation;
		break;
	case L'&':
		position++;
		chr = s[position];

		while (isdigit(chr)) {
			sub += chr;
			position++;
			if (position > s.GetLength() - 1)
				break;
			chr = s[position];
		}
		position--;

		// writing the number of bracket
		if (IsInteger(sub)) {
			element->type = etValue;
			unsigned int index = _wtoi(LPCWSTR(sub));

			vector<CStringW>* strings = _expression->GetStrings();
			element->val->str(index < strings->size() ? (*strings)[index] : L"");
		} else {
			return false;
		}
		break;


	case L'#':
		sub = "";
		position++;
		chr = s[position];

		while (isdigit(chr)) {
			sub += chr;
			position++;
			if (position > s.GetLength() - 1)
				break;
			chr = s[position];
		}
		position--;

		// writing the number of bracket
		if (IsInteger(sub)) {
			element->partIndex = _wtoi(LPCWSTR(sub));
			element->type = etPart;
		} else {
			return false;
		}
		break;
	default:

		SetErrorMessage(L"表达式需要操作数，但这里的字符是：" + s.Mid(position, 1));
		SetErrorPosition(position);
		return false;
	}
	return true;
}

// *******************************************************************
//		ReadOperation()
// *******************************************************************
// Reading the operation from the given position
bool ExpressionParser::ReadOperation(CStringW s, int& position, CElement& element) {
	wchar_t chr = s[position];

	switch (chr) {
	case L'<':       // <, <>, <, "<:>"
		if (s.Mid(position, 2) == L"<=") {
			element.type = etOperation;
			element.priority = 4;
			element.operation = operLessEqual;
			position++;
		} else if (s.Mid(position, 2) == L"<>") {
			element.type = etOperation;
			element.priority = 4;
			element.operation = operNotEqual;
			position++;
		} else {
			element.type = etOperation;
			element.priority = 4;
			element.operation = operLess;
		}
		break;

	case L'>':   // >, >=

		if (s.Mid(position, 2) == L">=") {
			element.type = etOperation;
			element.priority = 4;
			element.operation = operGrEqual;
			position++;
		} else {
			element.type = etOperation;
			element.priority = 4;
			element.operation = operGreater;
		}
		break;
	case L'=':
	{
		element.type = etOperation;
		element.priority = 4;
		element.operation = operEqual;
		break;
	}
	case L'+':
	{
		element.type = etOperation;
		element.priority = 3;
		element.operation = operPlus;
		break;
	}
	case L'-':
	{
		element.type = etOperation;
		element.priority = 3;
		element.operation = operMinus;
		break;
	}
	case L'*':
	{
		element.type = etOperation;
		element.priority = 2;
		element.operation = operMult;
		break;
	}
	case L'/':
	{
		element.type = etOperation;
		element.priority = 2;
		element.operation = operDiv;
		break;
	}
	case L'\\':
	{
		element.type = etOperation;
		element.priority = 2;
		element.operation = operDivInt;
		break;
	}
	case L'^':
	{
		element.type = etOperation;
		element.priority = 1;
		element.operation = operExpon;
		break;
	}
	case L':':
	{
		element.type = etOperation;
		element.priority = 7;
		element.operation = operCONSEQ;
		break;
	}
	case L'i':
	case L'I':
		if (s.Mid(position, 5).MakeUpper() == L"ILIKE") {
			element.type = etOperation;
			element.priority = 3;
			element.operation = operILike;
			position += 4;
		}
		break;
	case L'l':
	case L'L':
		if (s.Mid(position, 4).MakeUpper() == L"LIKE") {
			element.type = etOperation;
			element.priority = 3;
			element.operation = operLike;
			position += 3;
		}
		break;
	case L'm':
	case L'M':
		if (s.Mid(position, 3).MakeUpper() == L"MOD") {
			element.type = etOperation;
			element.priority = 3;
			element.operation = operMOD;
			position += 2;
		}
		break;
	case L'a':
	case L'A':
		if (s.Mid(position, 3).MakeUpper() == L"AND") {
			element.type = etOperation;
			element.priority = 5;
			element.operation = operAND;
			position += 2;
		}
		break;
	case L'o':
	case L'O':
		if (s.Mid(position, 2).MakeUpper() == L"OR") {
			element.type = etOperation;
			element.priority = 6;
			element.operation = operOR;
			position++;
		}
		break;
	case L'x':
	case L'X':
		if (s.Mid(position, 3).MakeUpper() == L"XOR") {
			element.type = etOperation;
			element.priority = 6;
			element.operation = operXOR;
			position += 2;
		}
		break;
	default:
	{
		SetErrorMessage(L"表达式需要运算符，但这里的字符是：" + s.Mid(position, 1));
		SetErrorPosition(position);
		return false;
	}
	}
	return true;
}

// *****************************************************************
//		ReplaceStringConstants()
// *****************************************************************
bool ExpressionParser::ReplaceStringLiterals(CStringW& s, int& count) {
	bool found = true;

	while (found) {
		int begin = -1;
		found = false;
		for (long i = 0; i < s.GetLength(); i++) {
			if (s.Mid(i, 1) == '\"') {
				if (begin == -1) {
					begin = i;    // it's an opening quotes
				} else {
					// it's closing quotes
					if (i > begin + 1)   // at least one character
					{
						_expression->GetStrings()->push_back(s.Mid(begin + 1, i - begin - 1));
						CStringW strReplace;
						strReplace.Format(L"&%i", count);
						ReplaceSubString(s, begin, i - begin + 1, strReplace);
						count++;
						begin = -1;
						found = true;
						break;
					} else {
						begin = -1;
					}
				}
			}
		}

		if (begin != -1) {
			SetErrorMessage(L"文本的引号必须是成对的 \"\"");
			return false;
		}
	}

	return true;
}

// *****************************************************************
//		ReplaceFieldNames()
// *****************************************************************
bool ExpressionParser::ReplaceFieldNames(CStringW& s, int& count) {
	bool found = true;

	while (found) {
		int begin = -1;
		found = false;
		for (long i = 0; i < s.GetLength(); i++) {
			if (s.Mid(i, 1) == '[') {
				if (begin == -1) {
					begin = i;
				} else {
					SetErrorMessage(L"字段名里面不能有左中括号 [");
					return false;
				}
			}

			if (s.Mid(i, 1) == ']') {
				if (begin == -1) {
					SetErrorMessage(L"右中括号 ] 必须和左中括号 [ 成对出现");
					return false;
				} else {
					if (i > begin + 1) {
						// at least one character
						_expression->GetStrings()->push_back(s.Mid(begin + 1, i - begin - 1));
						CStringW strReplace;
						strReplace.Format(L"{%i}", count);
						ReplaceSubString(s, begin, i - begin + 1, strReplace);
						count++;
						begin = -1;
						found = true;
						break;
					} else {
						SetErrorMessage(L"中括号里面必须有字段名 []");
						return false;
					}
				}
			}
		}

		if (begin != -1) {
			SetErrorMessage(L"文本的引号必须是成对的 \"\"");
			return false;
		}
	}

	return true;
}

// *****************************************************************
//		ReplacePart()
// *****************************************************************
void ExpressionParser::ReplacePart(CStringW& s, int begin, int end) {
	int partCount = _expression->get_PartCount();
	CStringW strReplace;
	strReplace.Format(L"#%i", partCount - 1);   // refer to previously added part
	ReplaceSubString(s, begin, end - begin + 1, strReplace);
}

// ************************************************************
//	 ReplaceInPosition()
//************************************************************
void ExpressionParser::ReplaceSubString(CStringW& s, int begin, int length, CStringW replacement) {
	CStringW part1, part2;

	if (begin > 0)
		part1 = s.Left(begin);

	if ((begin + length) < s.GetLength())
		part2 = s.Right(s.GetLength() - (begin + length));

	s = part1 + replacement + part2;
}

// ************************************************************
//	 ReplaceParameterlessFunctions()
//************************************************************
bool ExpressionParser::ReplaceParameterlessFunctions(CStringW& s) {
	int pos = s.Find(L"$");

	while (pos != -1) {
		CStringW sub;
		int i;
		for (i = pos; i < s.GetLength(); i++) {
			if (IsFunctionName(s[i])) {
				sub += s[i];
			} else {
				break;
			}
		}

		CustomFunction* fn = parser::GetFunction(sub);

		if (fn) {
			CExpressionPart* part = new CExpressionPart();
			part->expression = s;
			part->function = fn;

			_expression->AddPart(part);

			ReplacePart(s, pos, i - 1);
		} else {
			SetErrorMessage(CStringW(Debug::Format("函数名称不合法：%s", CString(sub))));
			return false;
		}

		pos = s.Find(L"$");
	}

	return true;
}
