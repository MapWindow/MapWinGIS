#include "stdafx.h"
#include "ExpressionParser.h"
#include "Functions.h"

// *******************************************************************
//		IsDecimal()
// *******************************************************************
bool ExpressionParser::IsDecimal(char chr, bool& exponential)
{
	if (chr >= '0' && chr <= '9')
		return true;

	if ((chr == '.') || (chr == ','))	// specify decimal separator explicitly
	{
		return true;
	}

	if ((chr == 'e') || (chr == 'E'))
	{
		exponential = true;
		return true;
	}

	if (exponential && (chr == '+' || chr == '-'))
	{
		exponential = false;
		return true;
	}

	exponential = false;	// +, - can be in the next position after e only
	return false;
}

// *******************************************************************
//		IsDecimal()
// *******************************************************************
bool ExpressionParser::IsDecimal(CString& str)
{
	for (int i = 0; i < str.GetLength(); i++)
	{
		if ((str[i] >= '0') && (str[i] < '9'))
			continue;

		if ((str[i] == '.') || (str[i] == ',') || (str[i] == 'e') || (str[i] == 'E'))	// specify decimal separator explicitly
			continue;

		return false;
	}

	return true;
}

// *******************************************************************
//		IsDecimalZero()
// *******************************************************************
bool ExpressionParser::IsDecimalZero(CString& str)
{
	for (int i = 0; i < str.GetLength(); i++)
	{
		if ((str[i] != '0') && (str[i] != '.') && (str[i] != ','))		// TODO: check decimal separator in more general way
			return false;
	}
	return true;
}

// *******************************************************************
//		IsInteger()
// *******************************************************************
bool ExpressionParser::IsInteger(CString& str)
{
	for (int i = 0; i < str.GetLength(); i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return false;
	}
	return true;
}

// *******************************************************************
//		IsOperator()
// *******************************************************************
bool ExpressionParser::IsOperator(char s)
{
	switch (s)
	{
		case '<':
		case '>' :
		case '=' :
		case '+' :
		case '-' :
		case '*' :
		case '/' :
		case '\\':
		case '^':
		case ':':
			return true;
	}

	return false;
}

// *******************************************************************
//		IsFunctionName()
// *******************************************************************
bool ExpressionParser::IsFunctionName(char s)
{
	return isalnum(s) || s == '$';
}

// *******************************************************************
//		IsOperator()
// *******************************************************************
bool ExpressionParser::IsOperator(CString s)
{
	if (s.GetLength() == 0)
	{
		return IsOperator(s[0]);
	}
	else
	{
		s = s.MakeUpper();

		if (s.Compare("AND") == 0 ||
			s.Compare("OR") == 0 ||
			s.Compare("XOR") == 0 ||
			s.Compare("MOD") == 0)
		{
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
bool ExpressionParser::Parse(CustomExpression* expression, CString s, bool useFields)
{
	_expression = expression;

	int count = 0;

	if (s.GetLength() == 0)
	{
		return false;
	}

	if (!ReplaceStringLiterals(s, count))
	{
		return false;
	}

	if (!ReplaceFieldNames(s, count))
	{
		return false;
	}

	int partCount = 0;
	if (!ReplaceParameterlessFunctions(s, partCount))
	{
		return false;
	}

	s.Replace(" ", "");

	if (!ParseTree(s, partCount))
	{
		return false;
	}

	return true;
}

// *****************************************************************
//		ParseTree()
// *****************************************************************
bool ExpressionParser::ParseTree(CString s, int partCount)
{
	bool found = true;
	CString temp;

	while (found)
	{
		// seeking brackets
		int begin, end;
		found = GetBrackets(s, begin, end);

		if (found)
		{
			int fnBegin;
			CustomFunction* fn = ParseFunction(s, begin - 1, fnBegin);

			if (fn)
			{
				if (!ParseArgumentList(GetInnerString(s, begin, end), fn))
				{
					return false;
				}

				bool finish = fnBegin == 0 && end == s.GetLength() - 1;      // there is nothing but function left

				ReplacePart(s, fnBegin, end, partCount);

				if (finish)
				{
					break;
				}

				continue;
			}
		}

		CString expression = found ? GetInnerString(s, begin, end) : s;

		CExpressionPart* part = ParseExpressionPart(expression);

		if (part)
		{
			_expression->AddPart(part);
		}
		else
		{
			return false;
		}

		if (found)
		{
			ReplacePart(s, begin, end, partCount);
		}
	}

	return true;
}

// *******************************************************************
//		GetInnerString()
// *******************************************************************
CString ExpressionParser::GetInnerString(CString& s, int begin, int end)
{
	return s.Mid(begin + 1, end - begin - 1);
}

// *******************************************************************
//		GetBrackets()
// *******************************************************************
// Returns positions of the first inner brackets
bool ExpressionParser::GetBrackets(CString expression, int& begin, int& end, CString openingSymbol, CString closingSymbol)
{
	// closing bracket
	end = -1;
	for (int i = 0; i < expression.GetLength(); i++)
	{
		if (expression.Mid(i, 1) == closingSymbol)
		{
			end = i;
			break;
		}
	}

	if (end == -1) {
		return false;
	}

	// opening bracket
	for (int i = end; i >= 0; i--)
	{
		if (expression.Mid(i, 1) == openingSymbol)
		{
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
CustomFunction* ExpressionParser::ParseFunction(CString& s, int begin, int& fnBegin)
{
	int i = begin;

	CString sub;

	while (i >= 0)
	{
		if (!IsFunctionName(s[i]))
		{
			return NULL;
		}

		sub = s[i] + sub;

		CustomFunction* fn = parser::GetFunction(sub);

		if (fn) 
		{
			fnBegin = i;
			return fn;
		}
	
		i--;
	}

	return NULL;
}

// ************************************************************
//	 ParseArgumentList()
//************************************************************
bool ExpressionParser::ParseArgumentList(CString s, CustomFunction* fn)
{
	if (!fn) return false;

	CExpressionPart* part = new CExpressionPart();
	part->expression = s;
	part->function = fn;

	int pos = 0;
	CString ct;

	ct = s.Tokenize(";", pos);

	while (ct.GetLength() != 0)
	{
		CExpressionPart* arg = ParseExpressionPart(ct);

		if (arg)
		{
			part->arguments.push_back(arg);
		}
		else
		{
			// error message is set above			
			delete part;
			return false;
		}

		ct = s.Tokenize(";", pos);
	};

	if (part->arguments.size() != fn->numParams())
	{
		CString s = Debug::Format("Invalid number of parameters: %s: %d; expected %d", fn->name(), part->arguments.size(), fn->numParams());
		SetErrorMessage(s);
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
CExpressionPart* ExpressionParser::ParseExpressionPart(CString s)
{
	bool readVal = true;	// true - reading values and unary operations; false - reading binary operations

	// adding a part
	CExpressionPart* part = new CExpressionPart();
	part->expression = s;

	for (int i = 0; i < s.GetLength(); i++)
	{
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
		if (element->operation != operNOT && element->operation != operChangeSign)
		{
			readVal = !readVal;
		}
	}

	if (part->elements.size() == 0)
	{
		delete part;
		return NULL;
	}

	return part;
}

// *******************************************************************
//		ReadValue()
// *******************************************************************
// Parses string from the given position
bool ExpressionParser::ReadValue(CString s, int& position, CElement* element)
{
	CString sub;		// substring
	char chr = s[position];

	switch (chr)
	{
		case '{':
		{
			sub = "";

			position++;
			chr = s[position];

			while (isdigit(chr))
			{
				sub += chr;
				position++;
				if (position > s.GetLength() - 1)
					break;
				chr = s[position];
			}
			position--;

			// writing the number of bracket
			if (IsInteger(sub))
			{
				unsigned int index = atoi(LPCTSTR(sub));
				sub = (*_expression->GetStrings())[index];
			}
			else
			{
				SetErrorMessage("Error while reading field name");
				return false;
			}

			if (position >= s.GetLength())
			{
				SetErrorMessage("The closing square bracket wasn't found");
				return false;
			}

			// the next character should be ]
			position++;
			chr = s[position];

			if (chr != '}')
			{
				SetErrorMessage("The closing square bracket wasn't found");
				return false;
			}

			
			if (_expression->GetUseFields())
			{
				// searching the field
				element->fieldIndex = -1;
				CString str = sub.MakeLower();
				
				vector<CString>* fields = _expression->GetFields();

				for (unsigned int i = 0; i < fields->size(); i++)
				{
					if ((*fields)[i] == str)
					{
						element->isField = true;
						element->type = etValue;
						element->fieldName = sub;
						element->fieldIndex = i;
					}
				}
				if (element->fieldIndex == -1)
				{
					SetErrorMessage("Field wasn't found: " + sub);
					return false;
				}
			}
			else //if (_useVariables)
			{
				element->type = etValue;
				CString s(sub);
				element->fieldName = s;
				element->isField = true;
			}
			break;
		}
		case '"':
		{
			sub = "";
			position++;
			chr = s[position];

			while ((position < s.GetLength() - 1) && (chr != '"'))
			{
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

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			sub = "";
			bool exponential = false;
			while (IsDecimal(chr, exponential))
			{
				sub += chr;
				position++;
				if (position > s.GetLength() - 1)
					break;
				chr = s[position];
			}
			position--;

			double val = Utility::atof_custom((LPCTSTR)sub);
			if (val != 0.0)
			{
				element->type = etValue;
				element->val->dbl(val);
			}
			else // if (val == 0.0)			// funñtion returns 0.0 in case string can't be represented as number
			{
				if (IsDecimalZero(sub))
				{
					element->type = etValue;
					element->val->dbl(val);
				}
				else
				{
					SetErrorMessage("The value is not a number: " + sub);
				}
			}
			break;
		}
		case 'T':
		case 't':
			if (s.Mid(position, 4).MakeLower() == "true")
			{
				position += 3;
				element->val->bln(true);
				element->type = etValue;
			}
			break;
		case 'F':
		case 'f':
			if (s.Mid(position, 5).MakeLower() == "false")
			{
				position += 4;
				element->val->bln(false);
				element->type = etValue;
			}
			break;
		case 'N':
		case 'n':
			if (s.Mid(position, 3).MakeLower() == "not")
			{
				position += 2;
				element->type = etOperation;
				element->priority = 5;
				element->operation = operNOT;
			}
			break;
		case '-':
			element->type = etOperation;
			element->priority = 3;
			element->operation = operChangeSign;
			element->type = etOperation;
			break;
		case '&':
			position++;
			chr = s[position];

			while (isdigit(chr))
			{
				sub += chr;
				position++;
				if (position > s.GetLength() - 1)
					break;
				chr = s[position];
			}
			position--;

			// writing the number of bracket
			if (IsInteger(sub))
			{
				element->type = etValue;
				unsigned int index = atoi(LPCTSTR(sub));

				vector<CString>* strings = _expression->GetStrings();
				element->val->str(index < strings->size() ? (*strings)[index] : "");
			}
			else
			{
				return false;
			}
			break;


		case '#':
			sub = "";
			position++;
			chr = s[position];

			while (isdigit(chr))
			{
				sub += chr;
				position++;
				if (position > s.GetLength() - 1)
					break;
				chr = s[position];
			}
			position--;

			// writing the number of bracket
			if (IsInteger(sub))
			{
				element->partIndex = atoi(LPCTSTR(sub));
				element->type = etPart;
			}
			else
			{
				return false;
			}
			break;
		default:

			SetErrorMessage("Operand expected. The character found: " + s.Mid(position, 1));
			SetErrorPosition(position);
			return false;
	}
	return true;
}

// *******************************************************************
//		ReadOperation()
// *******************************************************************
// Reading the operation from the given position
bool ExpressionParser::ReadOperation(CString s, int& position, CElement& element)
{
	char chr = s[position];

	switch (chr)
	{
		case '<':       // <, <>, <, "<:>"
			if (s.Mid(position, 2) == "<=")
			{
				element.type = etOperation;
				element.priority = 4;
				element.operation = operLessEqual;
				position++;
			}
			else if (s.Mid(position, 2) == "<>")
			{
				element.type = etOperation;
				element.priority = 4;
				element.operation = operNotEqual;
				position++;
			}
			else
			{
				element.type = etOperation;
				element.priority = 4;
				element.operation = operLess;
			}
			break;

		case '>':   // >, >=

			if (s.Mid(position, 2) == ">=")
			{
				element.type = etOperation;
				element.priority = 4;
				element.operation = operGrEqual;
				position++;
			}
			else
			{
				element.type = etOperation;
				element.priority = 4;
				element.operation = operGreater;
			}
			break;
		case '=':
		{
			element.type = etOperation;
			element.priority = 4;
			element.operation = operEqual;
			break;
		}
		case '+':
		{
			element.type = etOperation;
			element.priority = 3;
			element.operation = operPlus;
			break;
		}
		case '-':
		{
			element.type = etOperation;
			element.priority = 3;
			element.operation = operMinus;
			break;
		}
		case '*':
		{
			element.type = etOperation;
			element.priority = 2;
			element.operation = operMult;
			break;
		}
		case '/':
		{
			element.type = etOperation;
			element.priority = 2;
			element.operation = operDiv;
			break;
		}
		case '\\':
		{
			element.type = etOperation;
			element.priority = 2;
			element.operation = operDivInt;
			break;
		}
		case '^':
		{
			element.type = etOperation;
			element.priority = 1;
			element.operation = operExpon;
			break;
		}
		case ':':
		{
			element.type = etOperation;
			element.priority = 7;
			element.operation = operCONSEQ;
			break;
		}
		case 'l':
		case 'L':
			if (s.Mid(position, 4).MakeUpper() == "LIKE")
			{
				element.type = etOperation;
				element.priority = 3;
				element.operation = operLike;
				position += 3;
			}
			break;
		case 'm':
		case 'M':
			if (s.Mid(position, 3).MakeUpper() == "MOD")
			{
				element.type = etOperation;
				element.priority = 3;
				element.operation = operMOD;
				position += 2;
			}
			break;
		case 'a':
		case 'A':
			if (s.Mid(position, 3).MakeUpper() == "AND")
			{
				element.type = etOperation;
				element.priority = 5;
				element.operation = operAND;
				position += 2;
			}
			break;
		case 'o':
		case 'O':
			if (s.Mid(position, 2).MakeUpper() == "OR")
			{
				element.type = etOperation;
				element.priority = 6;
				element.operation = operOR;
				position++;
			}
			break;
		case 'x':
		case 'X':
			if (s.Mid(position, 3).MakeUpper() == "XOR")
			{
				element.type = etOperation;
				element.priority = 6;
				element.operation = operXOR;
				position += 2;
			}
			break;
		default:
		{
			SetErrorMessage("Operator expected. The character found: " + s.Mid(position, 1));
			SetErrorPosition(position);
			return false;
		}
	}
	return true;
}

// *****************************************************************
//		ReplaceStringConstants()
// *****************************************************************
bool ExpressionParser::ReplaceStringLiterals(CString& s, int& count)
{
	bool found = true;

	while (found)
	{
		int begin = -1;
		found = false;
		for (long i = 0; i < s.GetLength(); i++)
		{
			if (s.Mid(i, 1) == '\"')
			{
				if (begin == -1)
				{
					begin = i;    // it's an opening quotes
				}
				else
				{
					// it's closing quotes
					if (i > begin + 1)   // at least one character
					{
						_expression->GetStrings()->push_back(s.Mid(begin + 1, i - begin - 1));
						CString strReplace;
						strReplace.Format("&%i", count);
						ReplaceSubString(s, begin, i - begin + 1, strReplace);
						count++;
						begin = -1;
						found = true;
						break;
					}
					else
					{
						begin = -1;
					}
				}
			}
		}

		if (begin != -1)
		{
			SetErrorMessage("Unpaired text quotes");
			return false;
		}
	}

	return true;
}

// *****************************************************************
//		ReplaceFieldNames()
// *****************************************************************
bool ExpressionParser::ReplaceFieldNames(CString& s, int& count)
{
	bool found = true;

	while (found)
	{
		int begin = -1;
		found = false;
		for (long i = 0; i < s.GetLength(); i++)
		{
			if (s.Mid(i, 1) == '[')
			{
				if (begin == -1)
				{
					begin = i;
				}
				else
				{
					SetErrorMessage("\"[\" character inside field name");
					return false;
				}
			}

			if (s.Mid(i, 1) == ']')
			{
				if (begin == -1)
				{
					SetErrorMessage("\"]\" character without opening bracket");
					return false;
				}
				else
				{
					if (i > begin + 1)   // at least one character
					{
						_expression->GetStrings()->push_back(s.Mid(begin + 1, i - begin - 1));
						CString strReplace;
						strReplace.Format("{%i}", count);
						ReplaceSubString(s, begin, i - begin + 1, strReplace);
						count++;
						begin = -1;
						found = true;
						break;
					}
					else
					{
						SetErrorMessage("Empty field name: []");
						return false;
					}
				}
			}
		}

		if (begin != -1)
		{
			SetErrorMessage("Unpaired text quotes");
			return false;
		}
	}

	return true;
}

// *****************************************************************
//		ReplacePart()
// *****************************************************************
void ExpressionParser::ReplacePart(CString& s, int begin, int end, int& count)
{
	CString strReplace;
	strReplace.Format("#%i", count);
	ReplaceSubString(s, begin, end - begin + 1, strReplace);
	count++;
}

// ************************************************************
//	 ReplaceInPosition()
//************************************************************
void ExpressionParser::ReplaceSubString(CString& s, int begin, int length, CString replacement)
{
	CString part1, part2;

	if (begin > 0)
		part1 = s.Left(begin);

	if ((begin + length) < s.GetLength())
		part2 = s.Right(s.GetLength() - (begin + length));

	s = part1 + replacement + part2;
}

// ************************************************************
//	 ReplaceParameterlessFunctions()
//************************************************************
bool ExpressionParser::ReplaceParameterlessFunctions(CString& s, int& partCount)
{
	int pos = s.Find("$");
	
	while (pos != -1)
	{
		CString sub;
		int i;
		for (i = pos; i < s.GetLength(); i++)
		{
			if (IsFunctionName(s[i]))
			{
				sub += s[i];
			}
			else
			{
				break;
			}
		}

		CustomFunction* fn = parser::GetFunction(sub);

		if (fn)
		{
			CExpressionPart* part = new CExpressionPart();
			part->expression = s;
			part->function = fn;

			_expression->AddPart(part); 

			ReplacePart(s, pos, i - 1, partCount);
		}
		else
		{
			SetErrorMessage(Debug::Format("Invalid function name: %s", sub));
			return false;
		}

		pos = s.Find("$");
	}

	return true;
}

