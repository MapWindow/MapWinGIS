/**************************************************************************************
 * File name: Expression.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CExpression
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/ 
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ************************************************************************************** 
 * Contributor(s): 
 * (Open source contributors should list themselves and their modifications here). */
 
#include "stdafx.h"
#include "Expression.h"
#include <map>

// *****************************************************************
//		ParseBrackets()
// *****************************************************************
bool CExpression::ParseBrackets(CString s, CString& ErrorMessage)
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
			_parts.push_back(part);
		}
		else
		{
			ErrorMessage = _errorMessage;
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
void CExpression::ReplacePart(CString& s, int begin, int end, int& count)
{
	CString strReplace;
	strReplace.Format("#%i", count);
	ReplaceSubString(s, begin, end - begin + 1, strReplace);
	count++;
}

// *****************************************************************
//		Clear()
// *****************************************************************
void CExpression::Clear()
{
	ReleaseMemory();

	for (size_t i = 0; i < _parts.size(); i++)
	{
		for (size_t j = 0; j < _parts[i]->elements.size(); j++)
		{
			delete _parts[i]->elements[j];
		}
		delete _parts[i];
	}
	for (size_t i = 0; i < _operations.size(); i++)
	{
		delete _operations[i];
	}
	_variables.clear();
	_parts.clear();
	_operations.clear();
	_strings.clear();
}

// *****************************************************************
//		ReleaseMemory()
// *****************************************************************
void CExpression::ReleaseMemory()
{
	for (size_t i = 0; i < _parts.size(); i++)
	{
		for (size_t j = 0; j < _parts[i]->elements.size(); j++)
		{
			if (_parts[i]->elements[j]->type == etValue || _parts[i]->elements[j]->type == etPart)
			{
				CExpressionValue* v = _parts[i]->elements[j]->calcVal;
				v->clearMatrix();

				v = _parts[i]->elements[j]->val;
				v->clearMatrix();
			}
		}
	}
}

// *****************************************************************
//		ParseExpression()
// *****************************************************************
// building list of operation; UseFields: true - only fields form attribute table; 
// false - variables, the values of which must be set
bool CExpression::ParseExpression(CString s, bool useFields, CString& ErrorMessage)
{
	_saveOperations = true;
	_useFields = useFields;

    Clear();
    
    int count = 0;
	
	if (s.GetLength() == 0)  
	{
		return false;
	}

	if (!ReplaceStringConstants(s, count, ErrorMessage))
	{
		return false;
	}

	if (!ReplaceFieldNames(s, count, ErrorMessage))
	{
		return false;
	}

	ReplaceFunctions(s);
	
	if (!ParseBrackets(s, ErrorMessage))
	{
		return false;
	}
	
	_strings.clear();

	BuildFieldList();
	
    return true;
}

// *****************************************************************
//		ReplaceStringConstants()
// *****************************************************************
bool CExpression::ReplaceStringConstants(CString s, int& count, CString& ErrorMessage)
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
						_strings.push_back(s.Mid(begin + 1, i - begin - 1));
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
			ErrorMessage = "Unpaired text quotes";
			return false;
		}
	}

	return true;
}

// *****************************************************************
//		ReplaceFieldNames()
// *****************************************************************
bool CExpression::ReplaceFieldNames(CString s, int& count, CString& ErrorMessage)
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
					ErrorMessage = "\"[\" character inside field name";
					return false;
				}
			}

			if (s.Mid(i, 1) == ']')
			{
				if (begin == -1)
				{
					ErrorMessage = "\"]\" character without opening bracket";
					return false;
				}
				else
				{
					if (i > begin + 1)   // at least one character
					{
						_strings.push_back(s.Mid(begin + 1, i - begin - 1));
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
						ErrorMessage = "Empty field name: []";
						return false;
					}
				}
			}
		}

		if (begin != -1)
		{
			ErrorMessage = "Unpaired text quotes";
			return false;
		}
	}

	return true;
}

// *****************************************************************
//		ParseExpressionPart
// *****************************************************************
// Creates elements from the part of expression in the brackets
CExpressionPart* CExpression::ParseExpressionPart(CString s)
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
bool CExpression::ReadValue(CString s, int& position, CElement* element)
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

					while ( isdigit(chr) )
					{
						sub += chr;
						position++;
						if (position > s.GetLength() - 1)
							break;
						chr = s[position];
					}
                    position--;
					
					// writing the number of bracket
					if ( IsInteger(sub) )
					{
						unsigned int index = atoi(LPCTSTR(sub));
						sub = _strings[index];
					}
					else
					{
						_errorMessage = "Error while reading field name";
						return false;
					}
					
					if (position >= s.GetLength())
					{
						_errorMessage = "The closing square bracket wasn't found";
						return false;
					}

					// the next character should be ]
					position++;
                    chr = s[position];

					if (chr != '}')
					{
						_errorMessage = "The closing square bracket wasn't found";
						return false;
					}
                    
					if (_useFields)
					{
						// searching the field
						element->fieldIndex = -1;
						CString str = sub.MakeLower();
						for (unsigned int i = 0; i < _fields.size(); i++)
						{
							if (_fields[i] == str)
							{
								element->isField = true;
								element->type = etValue;		
								element->fieldName = sub;
								element->fieldIndex = i;
							}
						}
						if (element->fieldIndex == - 1)
						{
							_errorMessage = "Field wasn't found: " + sub;
							return false;
						}
					}
					else //if (_useVariables)
					{
						element->type = etValue;	
						CString s(sub);
						element->fieldName = s;
						element->isField= true;
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
                    while ( IsDecimal(chr, exponential) )
					{
						sub += chr;
						position++;
						if (position > s.GetLength() - 1)
							break;
						chr = s[position];
					}
					position--;
                    
					double val = Utility::atof_custom((LPCTSTR)sub);
					if ( val != 0.0 )
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
							_errorMessage = "The value is not a number: " + sub;
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
					
					while ( isdigit(chr) )
					{
						sub += chr;
						position++;
						if (position > s.GetLength() - 1)
							break;
						chr = s[position];
					}
                    position--;
					
					// writing the number of bracket
					if ( IsInteger(sub) )
					{
						element->type = etValue;
						unsigned int index = atoi(LPCTSTR(sub));
						element->val->str(index < _strings.size() ? _strings.at(index) : "");
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
					
					while ( isdigit(chr) )
					{
						sub += chr;
						position++;
						if (position > s.GetLength() - 1)
							break;
						chr = s[position];
					}
                    position--;
                    
					// writing the number of bracket
					if ( IsInteger(sub) )
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
							
			_errorMessage = "Operand expected. The character found: " + s.Mid(position,1);
					_errorPosition = position;
					return false;
	}
	return true;
}

// *******************************************************************
//		ReadOperation()
// *******************************************************************
// Reading the operation from the given position
bool CExpression::ReadOperation(CString s, int& position, CElement& element)
{
    char chr = s[position];

	switch ( chr )
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
						_errorMessage = "Operator expected. The character found: " + s.Mid(position,1);
						_errorPosition = position;
						return false;
					}
	}
    return true;
}

// *******************************************************************
//		GetBrackets()
// *******************************************************************
// Returns positions of the first inner brackets
bool CExpression::GetBrackets(CString expression, int& begin, int& end, CString openingSymbol, CString closingSymbol)
{
    // closing bracket
    end = -1;
	for (int i = 0; i < expression.GetLength(); i++)
	{
		if ( expression.Mid(i, 1) == closingSymbol )
		{
			end = i;
			break;
		}
	}
    if (end == -1)
		return false;
    
	// opening bracket
    for (int i = end; i >= 0 ; i--)
	{
		if ( expression.Mid(i, 1) == openingSymbol )
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
void CExpression::SkipSpaces(CString s, int& position)
{
	while (position < s.GetLength() && s.Mid(position, 1) == " ")
	{
		position++;
	}
}

// *******************************************************************
//		IsDecimal()
// *******************************************************************
bool CExpression::IsDecimal( CString& str )
{
	for (int i = 0; i< str.GetLength(); i++)
	{
		if ( (str[i] >= '0') && (str[i] < '9') )
			continue;
		if ((str[i] == '.') || (str[i] == ',') || (str[i] == 'e') || (str[i] == 'E' ))	// specify decimal separator explicitly
			continue;
		
		return false;
	}
	return true;
}

bool CExpression::IsDecimal( char chr, bool& exponential )
{
	if ( chr >= '0' && chr <= '9')
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
bool CExpression::IsDecimalZero(CString& str)
{
	for (int i = 0; i< str.GetLength(); i++)
	{
		if ((str[i] != '0') && (str[i] != '.') && (str[i] != ','))		// TODO: check decimal separator in more general way
		return false;
	}
	return true;
}

// *******************************************************************
//		IsInteger()
// *******************************************************************
bool CExpression::IsInteger( CString& str )
{
	for (int i = 0; i< str.GetLength(); i++)
	{
		if ( str[i] < '0' || str[i] > '9' )
			return false;
	}
	return true;
}


// /////////////////////////////////////////////////////////////////////
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//	 Calculate()
// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// /////////////////////////////////////////////////////////////////////
// Calculates the whole expression
CExpressionValue* CExpression::Calculate(CString& errorMessage)
{
	Reset();
    
	bool success = false;
	
	// if the operations should be cached we'll ensure that there is no obsolete data in vector
	if (_saveOperations)
	{
		_operations.clear();
	}

	ResetActiveCountForParts();

	int operationCount = 0;			// in case we got cached operations
	unsigned int partIndex = 0;		// we begin from the inner most bracket

	do
	{
		CExpressionPart* part = _parts[partIndex];
		
		if (!EvaluatePart(part, errorMessage, operationCount))
		{
			return false;
		}

		partIndex++;

		if (partIndex >= _parts.size())
		{
			// we closed the last part
			success = true;
			break;
		}
	}
	while (true);
    
	// operation was saved - no need to cache any more
	if (_saveOperations)
	{
		//delete operation;
		_saveOperations = false;
	}

	return success ? _parts[_parts.size() - 1]->val : NULL;
}

// *******************************************************************
//		EvaluatePart()
// *******************************************************************
bool CExpression::EvaluatePart(CExpressionPart* part, CString& errorMessage, int& operationCount)
{
	do
	{
		if (part->isFunction)
		{
			if (!EvaluateFunction(part))
			{
				return false;
			}
		}
		else
		{
			if (!CalculateNextOperationWithinPart(part, errorMessage, operationCount))
			{
				return false;
			}
		}

		if (FinishPart(part))
		{
			return true;
		}
	} 
	while (true);
}

// *******************************************************************
//		EvaluateFunction()
// *******************************************************************
bool CExpression::EvaluateFunction(CExpressionPart* part)
{
	if (!part->isFunction) return false;

	CString errorMessage;
	int operationCount = 0;

	// first lets evaluate arguments
	for (size_t i = 0; i < part->arguments.size(); i++)
	{
		CExpressionPart* arg = part->arguments[i];
		arg->activeCount = arg->elements.size();

		if (!EvaluatePart(arg, errorMessage, operationCount))
		{
			return false;
		}
	}

	CExpressionValue* value = part->arguments[0]->val;
	if (!value->IsDouble())
	{
		Debug::WriteError("Double value is expected");
		return false;
	}

	//part->val = new CExpressionValue();
	//part->val->type = vtDouble;

	//// call the function
	//switch (part->functionId)
	//{
	//	case fnCos:
	//		part->val->dbl = cos(value->dbl);
	//		break;
	//	case fnSin:
	//		part->val->dbl = sin(value->dbl);
	//		break;	
	//	case fnTan:
	//		part->val->dbl = tan(value->dbl);
	//		break;
	//	case fnCtan:
	//		part->val->dbl = 1 / tan(value->dbl);
	//		break;
	//}

	return true;
}

// *******************************************************************
//		CalculateNextOperationWithinPart()
// *******************************************************************
bool CExpression::CalculateNextOperationWithinPart(CExpressionPart* part, CString& errorMessage, int& operationCount)
{
	COperation* operation = NULL;;

	// if there is more then one element, then definitely some operation must be present
	if (part->elements.size() > 1)
	{
		// reading caching operation
		bool found = false;
		if (!_saveOperations)
		{
			operation = _operations[operationCount];
			operationCount++;
			found = true;
		}
		else
		{
			if (!operation)
				operation = new COperation();
			found = FindOperation(part, *operation);
		}

		if (found)
		{
			if (!CalculateOperation(part, *operation))
			{
				errorMessage = _errorMessage;
				return false;
			}
		}
		else
		{
			errorMessage = _errorMessage;
			return false;
		}

		part->activeCount -= operation->binaryOperation ? 2 : 1;
	}

	return true;
}

// *******************************************************************
//		FinishPart()
// *******************************************************************
// if there is only one element left, we'll finalize the part
bool CExpression::FinishPart(CExpressionPart* part)
{
	if (part->isFunction)
	{
		// part->val is already set
		return true;
	}

	if (part->activeCount == 1)
	{
		int size = part->elements.size();
		for (int i = 0; i < size; i++)
		{
			if (!part->elements[i]->turnedOff)
			{
				part->val = GetValue(part, i);
				part->elements[i]->turnedOff = true;
				return true;
			}
		}
	}

	return false;
}

// *******************************************************************
//		ResetActiveCount()
// *******************************************************************
void CExpression::ResetActiveCountForParts()
{
	for (unsigned int i = 0; i < _parts.size(); i++)
	{
		_parts[i]->activeCount = _parts[i]->elements.size();
	}
}

// *******************************************************************
//		Reset()
// *******************************************************************
// initializing in case of repeating calculations
void CExpression::Reset()
{
	for (unsigned int i = 0; i < _parts.size(); i++)
	{
		CExpressionPart* part = _parts[i];
		int size = part->elements.size();
		for (int j = 0; j < size; j++)
		{
			CElement* el = part->elements[j];
			el->wasCalculated = false;
			el->turnedOff = false;
		}
	}
}

// *******************************************************************
//		FindOperation()
// *******************************************************************
// Seeks operation with the highest priority and operands. Operation, left, right - indices of elements
bool CExpression::FindOperation(CExpressionPart* part, COperation& operation) // int& operation, int& left, int& right)
{
	// seeking operation
	bool found = false;
	int priority = 255;
	
	std::vector<CElement*>* elements = &part->elements;
	int size = elements->size();
	for (int i = 0; i < size; i++)
	{
		CElement* element =  (*elements)[i];
		if ( !element->turnedOff )
		{
			if ( element->type == etOperation )
			{
				if ( element->priority < priority )
				{
					found = true;
					priority = element->priority;
					operation.id = i;
				}
			}
		}
	}
    if ( !found )
	{
		_errorMessage = "Failed to find operation";
		return false;
	} 
	
	// seeking right operand
    operation.left = operation.right = -1;
	for (int i = operation.id + 1; i < size; i++)
	{
		CElement* element =  (*elements)[i];
		if (! element->turnedOff )
		{
			if ( element->type == etOperation )
			{
				if ( element->operation != operNOT && element->operation != operChangeSign)
				{
					_errorMessage = "There is operator in place of value";
					return false;
				}
			}
			else
			{
				operation.right = i; 
				break;
			}
		}
	}
	if ( operation.right == -1 )
	{
		_errorMessage = "No right operand was found";
		return false;
	}
	
	// if the operator is binary, seeking left operand
	if ( (*elements)[operation.id]->operation != operNOT && 
		 (*elements)[operation.id]->operation != operChangeSign )
	{
		for (int i = operation.id -1; i >= 0; i-- )
		{
			if (! (*elements)[i]->turnedOff )
			{
				operation.left = i; 
				break;
			}
		}
		if ( operation.left == -1 )
		{
			_errorMessage = "No left operand was found";
			return false;
		}
		operation.binaryOperation = true;
	}
	else
		operation.binaryOperation = false;
	
	// caching operations
	if (_saveOperations)
	{
		COperation* op = new COperation();
		op->left = operation.left;
		op->right = operation.right;
		op->id = operation.id;
		op->binaryOperation = operation.binaryOperation;
		_operations.push_back(op);
	}
    return true;
}

// *************************************************************
//	 GetMatrixOperation()
// *************************************************************
TwoArgOperator CExpression::GetMatrixOperation(tkOperation op)
{
	switch(op)
	{
		case operEqual: return TwoArgOperator::opEQ;
		case operNotEqual: return TwoArgOperator::opNE;
		case operLessEqual: return TwoArgOperator::opLE;
		case operGrEqual: return TwoArgOperator::opGE;
		case operGreater: return TwoArgOperator::opGT;
		case operLess: return TwoArgOperator::opLT;
		case operOR: return TwoArgOperator::opOR;
		case operAND: return TwoArgOperator::opAND;
		
		case operPlus: return TwoArgOperator::opPLUS;
		case operMinus: return TwoArgOperator::opMINUS;
		case operDiv: return TwoArgOperator::opDIV;
		case operMult: return TwoArgOperator::opMUL;
		case operDivInt: return TwoArgOperator::opDIV;
		case operExpon: return TwoArgOperator::opPOW;
		
		case operNOT: 
		case operXOR: 
		case operCONSEQ: 
		case operChangeSign: 
			return TwoArgOperator::opNONE;
	}
	return TwoArgOperator::opNONE;
}

// *************************************************************
//	 CalculateOperation()
// *************************************************************
bool CExpression::CalculateOperation( CExpressionPart* part, COperation& operation) //int left, int operation, int right )
{
	CExpressionValue* valLeft = NULL; 
	CExpressionValue* valRight = NULL; 
	CElement* elLeft = NULL;
	CElement* elRight = NULL;

	tkOperation oper = part->elements[operation.id]->operation;
	if (oper == operNOT || oper == operChangeSign )
	{
		valRight = GetValue(part, operation.right);	// these are unary operator and we read only right operand
		elRight = part->elements[operation.right];
	}
	else
	{
		valRight = GetValue(part, operation.right);	// these are binary operators as we read left and right operands

		valLeft = GetValue(part, operation.left);
		elLeft = part->elements[operation.left];
	}

	switch ( oper )
	{
		// logical operators
		case operOR:
		case operAND:
		case operXOR:
		case operCONSEQ:
			{
				if ( valLeft->isBoolean() && valRight->isBoolean() )
				{
					if (oper == operOR )			elLeft->calcVal->bln(valLeft->bln() || valRight->bln());
					else if ( oper == operAND )		elLeft->calcVal->bln(valLeft->bln() && valRight->bln());
					else if ( oper == operXOR )		elLeft->calcVal->bln((valLeft->bln() || valRight->bln()) && !(valLeft->bln() && valRight->bln()));
					else if ( oper == operCONSEQ )	elLeft->calcVal->bln((!valLeft->bln() || valLeft->bln() && valRight->bln()));
				}
				else if (valLeft->IsFloatArray() && valRight->IsFloatArray())
				{
					if (oper == operOR || oper == operAND)			
					{
						if (elLeft->calcVal != valLeft) 
						{
							elLeft->calcVal->matrix(new RasterMatrix(*valLeft->matrix()));
						}

						RasterMatrix* matrix = elLeft->calcVal->matrix();
						matrix->twoArgumentOperation(GetMatrixOperation(oper), *valRight->matrix() );
					}
				}
				else
				{
					_errorMessage = "The operands of CONSEQUENCE operation must have boolean type";
					return false;
				}
				break;
			}
		case operNOT:
			{
				if ( valRight->isBoolean() )
				{
					elRight->calcVal->bln(!(valRight->bln()));
				}
				else
				{
					_errorMessage = "NOT operator can can be applied to boolean values only";
					return false;
				}
				break;
			}
		// comparison operators
		case operLess:
		case operLessEqual:
		case operGreater:
		case operGrEqual:
		case operEqual:
		case operNotEqual:
			{
				
				if ( valLeft->type() == valRight->type() )
				{
					if (valLeft->IsFloatArray())
					{
						if (elLeft->calcVal != valLeft)
						{
							elLeft->calcVal->matrix(new RasterMatrix(*valLeft->matrix()));
						}

						RasterMatrix* matrix = elLeft->calcVal->matrix();
						matrix->twoArgumentOperation(GetMatrixOperation(oper), *valRight->matrix() );
					}
					else
					{
						if ( valLeft->isBoolean())
						{
							if (oper == operLess)				elLeft->calcVal->bln(valLeft->bln() < valRight->bln());
							else if (oper == operLessEqual)		elLeft->calcVal->bln(valLeft->bln() <= valRight->bln());
							else if (oper == operGreater)		elLeft->calcVal->bln(valLeft->bln() > valRight->bln());
							else if (oper == operGrEqual)		elLeft->calcVal->bln(valLeft->bln() >= valRight->bln());
							else if (oper == operEqual)			elLeft->calcVal->bln(valLeft->bln() == valRight->bln());
							else if (oper == operNotEqual)		elLeft->calcVal->bln(valLeft->bln() != valRight->bln());
						}
						else if ( valLeft->IsDouble() )
						{
							if (oper == operLess)				elLeft->calcVal->bln(valLeft->dbl() < valRight->dbl());
							else if (oper == operLessEqual)		elLeft->calcVal->bln(valLeft->dbl() <= valRight->dbl());
							else if (oper == operGreater)		elLeft->calcVal->bln(valLeft->dbl() > valRight->dbl());
							else if (oper == operGrEqual)		elLeft->calcVal->bln(valLeft->dbl() >= valRight->dbl());
							else if (oper == operEqual)			elLeft->calcVal->bln(valLeft->dbl() == valRight->dbl());
							else if (oper == operNotEqual)		elLeft->calcVal->bln(valLeft->dbl() != valRight->dbl());
						}
						else if ( valLeft->isString() )
						{
							int res = valLeft->str().MakeLower().Compare(valRight->str().MakeLower());

							if		( oper == operLess && res < 0 )			elLeft->calcVal->bln(true);
							else if (oper == operLessEqual && res <= 0)		elLeft->calcVal->bln(true);
							else if (oper == operGreater && res > 0)		elLeft->calcVal->bln(true);
							else if (oper == operGrEqual && res >= 0)		elLeft->calcVal->bln(true);
							else if (oper == operEqual	&& res == 0)		elLeft->calcVal->bln(true);
							else if (oper == operNotEqual && res != 0)		elLeft->calcVal->bln(true);
							else											elLeft->calcVal->bln(true);
						}
					}
				}
				else if(valLeft->IsFloatArray() && valRight->IsDouble())
				{
					if (elLeft->calcVal != valLeft)
					{
						elLeft->calcVal->matrix(new RasterMatrix(*valLeft->matrix()));
					}

					RasterMatrix* matrix = elLeft->calcVal->matrix();
					
					float* data = new float[1];
					data[0] = (float)valRight->dbl();

					RasterMatrix* right = new RasterMatrix(1, 1, data, matrix->nodataValue() );
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *right);
					delete right;
				}
				else if(valLeft->IsDouble() && valRight->IsFloatArray())
				{
					RasterMatrix* matrix = new RasterMatrix(*valRight->matrix());
					elLeft->calcVal->matrix(matrix);

					float* data = new float[1];
					data[0] = (float)valLeft->dbl();

					RasterMatrix* left = new RasterMatrix(1, 1, data, matrix->nodataValue() );
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *left);

					delete left;
				}
				else
				{
					_errorMessage = "Inconsistent types for comparison operation";
					return false;
				}
				break;
			}
		case operChangeSign:
			{
				if ( valRight->IsDouble() )
				{
					elRight->calcVal->dbl(- valRight->dbl());
				}
				else if (valRight->IsFloatArray())
				{
					if (elRight->calcVal != valRight)
					{
						elRight->calcVal->matrix(valRight->matrix()->Clone(true));
					}

					elRight->calcVal->matrix()->changeSign();
				}
				else
				{
					_errorMessage = "It's not allowed to change sign of non-numeric operands";
					return false;
				}
				break;
			}

		// arithmetic operators
        case operMinus:
        case operMult:
        case operExpon:
		case operMOD:
		case operDiv:
		case operDivInt:
		case operPlus:
			{
				if (!valLeft->IsFloatArray() && !valRight->IsFloatArray() && oper == operPlus)
				{
					// in case of strings we'll use concatenation here, therefore plus is treated separetely;
					if ( valLeft->IsDouble() && valRight->IsDouble() )
					{
						elLeft->calcVal->dbl(valLeft->dbl() + valRight->dbl());
					}
					else if ( valLeft->IsDouble() && valRight->isString())
					{
						CString s;
						s.Format(_floatFormat, valLeft->dbl());
						elLeft->calcVal->str(s + valRight->str());
					}
					else if ( valLeft->isString() && valRight->IsDouble())
					{
						CString s;
						s.Format(_floatFormat, valRight->dbl());
						elLeft->calcVal->str(valLeft->str() + s);
					}
					else if ( valLeft->isString() && valRight->isString())
					{
						elLeft->calcVal->str(valLeft->str() + valRight->str());
					}
					else
					{
						_errorMessage = "+ operation isn't allowed for boolean values.";
						return false;
					}
					break;
				}
				
				if ( valLeft->IsDouble() && valRight->IsDouble() )
				{
					if		( oper == operMinus )	elLeft->calcVal->dbl(valLeft->dbl() - valRight->dbl());
					else if ( oper == operDiv )		
					{
						if (valRight->dbl() == 0.0)
						{
							_errorMessage = "Division by zero";
						}
						else
						{
							elLeft->calcVal->dbl(valLeft->dbl() / valRight->dbl());
						}
					}
					else if ( oper == operMult )	elLeft->calcVal->dbl(valLeft->dbl() * valRight->dbl());
					else if ( oper == operExpon )	elLeft->calcVal->dbl(pow(valLeft->dbl(), valRight->dbl()));
					else if ( oper == operDivInt )	
						if (valRight->dbl() == 0.0)
						{
							_errorMessage = "Division by zero";
						}
						else
						{
							elLeft->calcVal->dbl(double((int)valLeft->dbl() / (int)valRight->dbl()));
						}
					else if ( oper == operMOD )		elLeft->calcVal->dbl(double((int)valLeft->dbl() % (int)valRight->dbl()));
				}
				else if (valLeft->IsFloatArray() && valRight->IsFloatArray() )
				{
					if (elLeft->calcVal != valLeft) {
						elLeft->calcVal->matrix(valLeft->matrix()->Clone(true));
					}
					RasterMatrix* matrix = elLeft->calcVal->matrix();
					
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *valRight->matrix());
				}
				else if (valLeft->IsFloatArray() && valRight->IsDouble() )
				{
					if (elLeft->calcVal != valLeft)
						elLeft->calcVal->matrix(valLeft->matrix()->Clone(true));
					RasterMatrix* matrix = elLeft->calcVal->matrix();

					float* data = new float[1];
					data[0] = (float)valRight->dbl();
					RasterMatrix* right = new RasterMatrix(1, 1, data, matrix->nodataValue() );
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *right);
					delete right;
				}
				else if (valLeft->IsDouble() && valRight->IsFloatArray() )
				{
					RasterMatrix* matrix = valRight->matrix()->Clone(true);
					elLeft->calcVal->matrix(matrix);

					float* data = new float[1];
					data[0] = (float)valLeft->dbl();
					RasterMatrix* left = new RasterMatrix(1, 1, data, matrix->nodataValue() );
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *left);
					delete left;
				}
				else
				{
					_errorMessage = "Arithmetic operations can be applied to numbers only";
					return false;
				}
				break;
			}
		default:
			{
				_errorMessage = "Unsupported operation";
				return false;
			}
	}
	
	if (oper == operNOT || oper == operChangeSign)
	{
		// unary operator
		elRight->wasCalculated = true;
		part->elements[operation.id]->turnedOff = true;
	}
	else
	{
		// binary operator
		elLeft->wasCalculated = true;
		part->elements[operation.id]->turnedOff = true;
		part->elements[operation.right]->turnedOff = true;
	}

	return true;
}

// ************************************************************
//	 GetValue()
//************************************************************
inline CExpressionValue* CExpression::GetValue(CExpressionPart* part, int elementId )
{
	CElement* element = part->elements[elementId];
	CExpressionValue* val = NULL;

	if ( element->wasCalculated )		val = element->calcVal;
	else if (element->partIndex != -1)	val = _parts[element->partIndex]->val;
	else								val = element->val;
	
	return val;
}

// ************************************************************
//	 ReplaceInPosition()
//************************************************************
void CExpression::ReplaceSubString(CString& s, int begin, int length, CString replacement)
{
	CString part1, part2;  

    if ( begin > 0 )	
		part1 = s.Left(begin);
    
    if ( (begin + length) < s.GetLength())
		part2 = s.Right(s.GetLength() - (begin + length));
    
    s = part1 + replacement + part2;
}

// ************************************************************
//	 ReadFieldNames()
//************************************************************
bool CExpression::ReadFieldNames(ITable* tbl)
{
	_fields.clear();
	
	if ( !tbl )
	{
		return false;
	}
	else
	{
		long numFields;
		tbl->get_NumFields(&numFields);
		
		for ( int i = 0; i < numFields; i++ )
		{
			IField* fld = NULL;
			tbl->get_Field(i, &fld);
			if ( fld )
			{
				USES_CONVERSION;
				CComBSTR bstr;
				fld->get_Name(&bstr);
				CString str = OLE2CA(bstr);
				_fields.push_back(str.MakeLower());
				fld->Release();
			}
		}
		return true;
	}
}

// ************************************************************
//	 SetFields()
//************************************************************
void CExpression::SetFields(vector<CString>& fields)
{
	_fields.clear();
	_fields.insert(_fields.end(), fields.begin(), fields.end());
}

// *****************************************************************
//		BuildFieldList()
// *****************************************************************
void CExpression::BuildFieldList()
{
	for (unsigned int i = 0; i < _parts.size(); i++)
	{
		CExpressionPart* part = _parts[i];
		for (unsigned long j = 0; j < part->elements.size(); j++)
		{
			if (part->elements[j]->isField)
			{
				_variables.push_back(part->elements[j]);
			}
		}
	}
}

// ************************************************************
//	 TryParseFunction()
//************************************************************
// begin, end - position of brackets
// name of the function is already substituted, like !1!
// any white spaces are removed
int CExpression::TryParseFunction(CString& s, int begin, int end)
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
int CExpression::ParseFunctionId(CString s, int begin)
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
bool CExpression::ParseArgumentList(CString s, int functionId)
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

	int paramCount = GetParameterCount(functionId);
	if (part->arguments.size() != paramCount)
	{
		// TODO: report invalid number of parameters
		// TODO: delete arguments
		delete part;
		return false;
	}

	_parts.push_back(part);
	return true;	
}

// ************************************************************
//	 ReplaceFunctions()
//************************************************************
void CExpression::ReplaceFunctions(CString& expression)
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
//	 GetArgumentCount()
//************************************************************
int CExpression::GetParameterCount(int functionId)
{
	switch (functionId)
	{
		// TODO: list functions with different number of parameters
		default:
			return 1;
	}
}