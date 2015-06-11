#include "stdafx.h"
#include "ExpressionParser.h"

// *****************************************************************
//		ParseExpression()
// *****************************************************************
// building list of operation; UseFields: true - only fields form attribute table; 
// false - variables, the values of which must be set
bool ExpressionParser::ParseExpression(CExpression* expression, CString s, bool useFields)
{
	_expression = expression;

	int count = 0;

	if (s.GetLength() == 0)
	{
		return false;
	}

	if (!ReplaceStringConstants(s, count))
	{
		return false;
	}

	if (!ReplaceFieldNames(s, count))
	{
		return false;
	}

	ReplaceFunctions(s);

	if (!ParseBrackets(s))
	{
		return false;
	}

	return true;
}

// *****************************************************************
//		ParseBrackets()
// *****************************************************************
bool ExpressionParser::ParseBrackets(CString s)
{
	int count = 0;
	bool found = true;
	CString temp;

	while (found)
	{
		// seeking brackets
		int begin, end;
		found = GetBrackets(s, begin, end);

		if (found)
		{
			int fnId = TryParseFunction(s, begin, end);

			if (fnId != -1)
			{
				if (!ParseArgumentList(s.Mid(begin + 1, end - begin - 1), fnId))
				{
					return false;
				}

				temp.Format("%d", fnId);
				begin -= temp.GetLength() + 2;		// replacing function id as well (!1!)

				ReplacePart(s, begin, end, count);

				continue;
			}
		}

		CString expression = found ? s.Mid(begin + 1, end - begin - 1) : s;

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
			ReplacePart(s, begin, end, count);
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

// *****************************************************************
//		ReplaceStringConstants()
// *****************************************************************
bool ExpressionParser::ReplaceStringConstants(CString s, int& count)
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
bool ExpressionParser::ReplaceFieldNames(CString s, int& count)
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
		SkipSpaces(s, i);
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
	if (end == -1)
		return false;

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
//		SkipSpaces()
// ************************************************************
void ExpressionParser::SkipSpaces(CString s, int& position)
{
	while (position < s.GetLength() && s.Mid(position, 1) == " ")
	{
		position++;
	}
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

// ************************************************************
//	 TryParseFunction()
//************************************************************
// begin, end - position of brackets
// name of the function is already substituted, like !1!
// any white spaces are removed
int ExpressionParser::TryParseFunction(CString& s, int begin, int end)
{
	if (begin < 3) return -1;   // !1!(1) - first bracket must be at least a third position

	if (s[begin - 1] != '!') return -1;

	int tempBegin = begin - 2;

	return ParseFunctionId(s, tempBegin);
}

// ************************************************************
//	 ParseFunctionId()
//************************************************************
// Starts to read function id from its back, for example !15 starting from "5"
int ExpressionParser::ParseFunctionId(CString s, int begin)
{
	int i = begin;
	while (i >= 0)
	{
		if (s[i] == '!')
		{
			s = s.Mid(i + 1, begin - i);
			if (IsInteger(s))
			{
				return atoi(s);
			}
		}

		if (s[i] < '0' && s[i] > '9') return -1;

		i--;
	}

	return -1;
}

// ************************************************************
//	 ParseArgumentList()
//************************************************************
bool ExpressionParser::ParseArgumentList(CString s, int functionId)
{
	CExpressionPart* part = new CExpressionPart();
	part->expression = s;
	part->isFunction = true;
	part->functionId = (tkFunction)functionId;

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
			// TODO: delete previous arguments
			delete part;
			return false;
		}

		ct = s.Tokenize(";", pos);
	};

	//int paramCount = GetParameterCount(functionId);
	//if (part->arguments.size() != paramCount)
	//{
	//	// TODO: report invalid number of parameters
	//	// TODO: delete arguments
	//	delete part;
	//	return false;
	//}

	//_parts.push_back(part);
	return true;
}

// ************************************************************
//	 ReplaceFunctions()
//************************************************************
void ExpressionParser::ReplaceFunctions(CString& expression)
{
	vector<CString> list{ "sin", "cos", "tan", "ctan" };

	CString replace;
	for (size_t i = 0; i < list.size(); i++)
	{
		replace.Format("!%d!", i);
		expression.Replace(list[i], replace);
	}

	expression.Replace(" ", "");

	Debug::WriteLine("After fn substitute: %s", expression);
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