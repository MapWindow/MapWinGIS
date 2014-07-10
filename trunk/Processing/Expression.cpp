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
 // Sergei Leschinski (lsu) 25 june 2010 - created the file.

#include "stdafx.h"
#include "Expression.h"
#include <map>

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
	bool found = true;
	
	if (s.GetLength() == 0)  return false;

	// replacing string constants
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
	
	// replacing field names
	found = true;
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

	count = 0;
	found = true;
	while ( found )
	{
		// seeking brackets
		int begin, end;
		found = GetBrackets(s, begin, end);
		
		CString expression;
		
		if ( found )	expression = s.Mid(begin + 1, end - begin -1);
		else			expression = s;
	      
		if (!ParseExpressionPart( expression )) 
		{
			ErrorMessage = _errorMessage;
			return false;
		}
	      
		if ( found )
		{
			CString strReplace;
			strReplace.Format("#%i", count);
			ReplaceSubString(s, begin, end - begin + 1, strReplace);
			count++;
		}
	}
	
	_strings.clear();

	// building field list for faster access
	for (unsigned int i = 0; i < _parts.size(); i++)
	{
		CExpressionPart* part = _parts[i];
		for (unsigned long j = 0; j < part->elements.size(); j++)
		{
			if ( part->elements[j]->isField )
			{
				_variables.push_back(part->elements[j]);
			}
		}
	}
    return true;
}

// *****************************************************************
//		ParseExpressionPart
// *****************************************************************
// Creates elements from the part of expression in the brackets
bool CExpression::ParseExpressionPart(CString s)
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
		if ( readVal )
		{
			if (! ReadValue(s, i, element))
				return false;
		}
		else
		{
			if (!ReadOperation(s, i, *element))
				return false;
		}
		
		// saving element
		part->elements.push_back(element);
		
		//in case operation was unary the next element should be value as well
		if  (element->operation != operNOT && element->operation != operChangeSign)
		{
			readVal = !readVal;
		}
	}

	if (part->elements.size() > 0)
	{
		_parts.push_back(part);
		return true;	
	}
	else
	{
		return false;
	}
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
						Debug::WriteLine(element->fieldName);
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
					element->val->type = vtString;
					element->val->str = sub;
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
						element->val->type = vtDouble;
						element->val->dbl = val;
					}
					else // if (val == 0.0)			// funñtion returns 0.0 in case string can't be represented as number
					{
						if (IsDecimalZero(sub))
						{
							element->type = etValue;
							element->val->type = vtDouble;
							element->val->dbl = val;
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
						element->val->type = vtBoolean;	
						element->val->bln = true;
						element->type = etValue;
					}
					break;
		case 'F':
		case 'f':
					if (s.Mid(position, 5).MakeLower() == "false")
					{
						position += 4;
						element->val->type = vtBoolean;	
						element->val->bln = false;
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
						element->val->type = vtString;
						unsigned int index = atoi(LPCTSTR(sub));
						if ( index < _strings.size())
							element->val->str = _strings.at(index);
						else
							element->val->str = "";
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
    // initializing in case of repeating calculations
	for (unsigned int i = 0; i < _parts.size(); i++ )
	{
		CExpressionPart* part = _parts[i];
		int size = part->elements.size();
		for ( int j = 0; j < size; j++ )
		{
			CElement* el = part->elements[j];
			el->wasCalculated = false;
			el->turnedOff = false;
		}
	}
    
	//int operation, left, right;
	COperation* operation = NULL;;
	unsigned int partIndex = 0;		// we begin from the inner most bracket
	bool success = false;
	
	// if the operations should be cached we'll ensure that there is no obsolete data in vector
	if (_saveOperations)
		_operations.clear();
	
	// in case we got cached operations
	int operationCount = 0;
	for (unsigned int i = 0; i < _parts.size(); i++)
		_parts[i]->activeCount = _parts[i]->elements.size();

    do
	{
		CExpressionPart* part = _parts[partIndex];

		// if there is more then one element, then definitely some operation must be present
		if ( part->elements.size() > 1 )
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
				if ( !operation )
					operation = new COperation();
				found = FindOperation(part, *operation);
			}

			if ( found )
			{
				if (!CalculateOperation(part, *operation))
				{
					errorMessage = _errorMessage;
					return NULL;
				}
			}
			else
			{
				errorMessage = _errorMessage;
				return NULL;
			}

			if ( operation->binaryOperation )
				part->activeCount -= 2;
			else
				part->activeCount -= 1;
		}
		
		// if there is only one element left, we'll finalize the part
		if ( part->activeCount == 1 )
		{
			int size = part->elements.size();
			for (int i = 0; i < size; i++)
			{
				if ( !part->elements[i]->turnedOff )
				{
					part->val = GetValue(part, i);
					part->elements[i]->turnedOff = true;
					partIndex++;
					break;
				}
			}
			
			if ( partIndex >= _parts.size() )
			{
				// we closed the last part
				success = true;
				break;
			}
			else
			{
				// we are shifting to the next part				
				part = _parts[partIndex];
			}
		}
	} while ( true );
    
	// operation were saved - no need to cache any more
	if (_saveOperations)
	{
		delete operation;
		_saveOperations = false;
	}

	if ( success )
		return _parts[_parts.size() - 1]->val;
	else
		return NULL;
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
		Debug::WriteLine("%d", valRight->type);

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
				if ( valLeft->type == vtBoolean && valRight->type == vtBoolean )
				{
					if (oper == operOR )			elLeft->calcVal->bln = valLeft->bln || valRight->bln;
					else if ( oper == operAND )		elLeft->calcVal->bln = valLeft->bln && valRight->bln;
					else if ( oper == operXOR )		elLeft->calcVal->bln = (valLeft->bln || valRight->bln) && !(valLeft->bln && valRight->bln);
					else if ( oper == operCONSEQ )	elLeft->calcVal->bln = (!valLeft->bln || valLeft->bln && valRight->bln);
					elLeft->calcVal->type = vtBoolean;
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
				if ( valRight->type == vtBoolean )
				{
					elRight->calcVal->bln = !(valRight->bln);
					elRight->calcVal->type = vtBoolean;
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
				
				if ( valLeft->type == valRight->type )
				{
					if (valLeft->type == vtFloatArray)
					{
						RasterMatrix* matrix = new RasterMatrix(*valLeft->matrix);
						matrix->twoArgumentOperation(GetMatrixOperation(oper), *valRight->matrix );
						elLeft->calcVal->matrix = matrix;
						elLeft->calcVal->type = vtFloatArray;
					}
					else
					{
						if ( valLeft->type == vtBoolean)
						{
							if		( oper == operLess )			elLeft->calcVal->bln = valLeft->bln < valRight->bln;
							else if ( oper == operLessEqual )		elLeft->calcVal->bln = valLeft->bln <= valRight->bln;
							else if ( oper == operGreater )			elLeft->calcVal->bln = valLeft->bln > valRight->bln;
							else if ( oper == operGrEqual )			elLeft->calcVal->bln = valLeft->bln >= valRight->bln;
							else if ( oper == operEqual	)			elLeft->calcVal->bln = valLeft->bln == valRight->bln;
							else if ( oper == operNotEqual )		elLeft->calcVal->bln = valLeft->bln != valRight->bln;
						}
						else if ( valLeft->type == vtDouble )
						{
							if		( oper == operLess )			elLeft->calcVal->bln = valLeft->dbl < valRight->dbl;
							else if ( oper == operLessEqual )		elLeft->calcVal->bln = valLeft->dbl <= valRight->dbl;
							else if ( oper == operGreater )			elLeft->calcVal->bln = valLeft->dbl > valRight->dbl;
							else if ( oper == operGrEqual )			elLeft->calcVal->bln = valLeft->dbl >= valRight->dbl;
							else if ( oper == operEqual	)			elLeft->calcVal->bln = valLeft->dbl == valRight->dbl;
							else if ( oper == operNotEqual )		elLeft->calcVal->bln = valLeft->dbl != valRight->dbl;
						}
						else if ( valLeft->type == vtString )
						{
							int res = valLeft->str.MakeLower().Compare(valRight->str.MakeLower());

							if		( oper == operLess && res < 0 )			elLeft->calcVal->bln = true;
							else if ( oper == operLessEqual && res <= 0)	elLeft->calcVal->bln = true;
							else if ( oper == operGreater && res > 0)		elLeft->calcVal->bln = true;
							else if ( oper == operGrEqual && res >= 0)		elLeft->calcVal->bln = true;
							else if ( oper == operEqual	&& res == 0)		elLeft->calcVal->bln = true;
							else if ( oper == operNotEqual && res != 0)		elLeft->calcVal->bln = true;
							else											elLeft->calcVal->bln = false;
						}
						elLeft->calcVal->type = vtBoolean;
					}
				}
				else if(valLeft->type == vtFloatArray && valRight->type == vtDouble)
				{
					RasterMatrix* matrix = new RasterMatrix(*valLeft->matrix);
					
					float* data = new float[1];
					data[0] = (float)valRight->dbl;
					RasterMatrix* right = new RasterMatrix(1, 1, data, matrix->nodataValue() );
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *right);
					delete right;

					elLeft->calcVal->matrix = matrix;
					elLeft->calcVal->type = vtFloatArray;
				}
				else if(valLeft->type == vtDouble && valRight->type == vtFloatArray)
				{
					RasterMatrix* matrix = new RasterMatrix(*valRight->matrix);

					float* data = new float[1];
					data[0] = (float)valLeft->dbl;
					RasterMatrix* left = new RasterMatrix(1, 1, data, matrix->nodataValue() );
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *left);
					delete left;

					elLeft->calcVal->matrix = matrix;
					elLeft->calcVal->type = vtFloatArray;
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
				if ( valRight->type == vtDouble )
				{
					elRight->calcVal->dbl = - valRight->dbl;
					elRight->calcVal->type = vtDouble;
				}
				else if (valRight->type == vtFloatArray)
				{
					RasterMatrix* matrix = valRight->matrix->Clone(true);
					matrix->changeSign();
					elRight->calcVal->matrix = matrix;
					elRight->calcVal->type = vtFloatArray;
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
				if (valLeft->type != vtFloatArray && valRight->type != vtFloatArray && oper == operPlus)
				{
					// in case of strings we'll use concatenation here, therefore plus is treated separetely;
					if ( valLeft->type == vtDouble && valRight->type == vtDouble )
					{
						elLeft->calcVal->type = vtDouble;
						elLeft->calcVal->dbl = valLeft->dbl + valRight->dbl;
					}
					else if ( valLeft->type == vtDouble && valRight->type == vtString)
					{
						CString s;
						s.Format("%g", valLeft->dbl);
						elLeft->calcVal->type = vtString;
						elLeft->calcVal->str = s + valRight->str;
					}
					else if ( valLeft->type == vtString && valRight->type == vtDouble)
					{
						CString s;
						s.Format("%g", valRight->dbl);
						elLeft->calcVal->type = vtString;
						elLeft->calcVal->str = valLeft->str + s;
					}
					else if ( valLeft->type == vtString && valRight->type == vtString)
					{
						elLeft->calcVal->type = vtString;
						elLeft->calcVal->str = valLeft->str + valRight->str;
					}
					else
					{
						_errorMessage = "+ operation isn't allowed for boolean values.";
						return false;
					}
					break;
				}
				
				if ( valLeft->type == vtDouble && valRight->type == vtDouble )
				{
					if		( oper == operMinus )	elLeft->calcVal->dbl = valLeft->dbl - valRight->dbl;
					else if ( oper == operDiv )		
					{
						if (valRight->dbl == 0.0)
						{
							_errorMessage = "Division by zero";
						}
						else
						{
							elLeft->calcVal->dbl = valLeft->dbl / valRight->dbl;
						}
					}
					else if ( oper == operMult )	elLeft->calcVal->dbl = valLeft->dbl * valRight->dbl;
					else if ( oper == operExpon )	elLeft->calcVal->dbl = pow(valLeft->dbl, valRight->dbl);
					else if ( oper == operDivInt )	
						if (valRight->dbl == 0.0)
						{
							_errorMessage = "Division by zero";
						}
						else
						{
							elLeft->calcVal->dbl = double((int)valLeft->dbl / (int)valRight->dbl);
						}
					else if ( oper == operMOD )		elLeft->calcVal->dbl = double((int)valLeft->dbl % (int)valRight->dbl);
					elLeft->calcVal->type = vtDouble;
				}
				else if (valLeft->type == vtFloatArray && valRight->type == vtFloatArray )
				{
					RasterMatrix* matrix = valLeft->matrix->Clone(true);
					elLeft->calcVal->matrix = matrix;
					elLeft->calcVal->type = vtFloatArray;
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *valRight->matrix);
				}
				else if (valLeft->type == vtFloatArray && valRight->type == vtDouble )
				{
					RasterMatrix* matrix = valLeft->matrix->Clone(true);
					elLeft->calcVal->matrix = matrix;
					elLeft->calcVal->type = vtFloatArray;

					float* data = new float[1];
					data[0] = (float)valRight->dbl;
					RasterMatrix* right = new RasterMatrix(1, 1, data, matrix->nodataValue() );
					matrix->twoArgumentOperation(GetMatrixOperation(oper), *right);
					delete right;
				}
				else if (valLeft->type == vtDouble && valRight->type == vtFloatArray )
				{
					RasterMatrix* matrix = valRight->matrix->Clone(true);
					elLeft->calcVal->matrix = matrix;
					elLeft->calcVal->type = vtFloatArray;

					float* data = new float[1];
					data[0] = (float)valLeft->dbl;
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
	
	Debug::WriteLine("Type %d", (int)(element->val)->type);
	Debug::WriteLine("Type %d", (int)val->type);

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
//	 SetFieldNames()
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
				BSTR s;
				fld->get_Name(&s);
				CString str = OLE2CA(s);
				_fields.push_back(str.MakeLower());
				SysFreeString(s);
				fld->Release();
			}
		}
		return true;
	}
}