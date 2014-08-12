/**************************************************************************************
 * File name: Expression.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of CExpression
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

#pragma once
#include "RasterMatrix.h"

enum tkElementType
{
	etValue = 0,
	etOperation = 1,
	etPart = 2,
	etNone = 3,
};

enum tkOperation
{
	operEqual = 0,		// =
	operNotEqual = 1,   // <>
	operLessEqual = 2,	// <=
	operGrEqual = 3,	// >=
	operGreater = 4,    // >
	operLess = 5,       // <

	operOR = 6,         // OR
	operAND = 7,        // AND
	operNOT = 8,        // NOT (unary)
	operXOR = 9,        // XOR
	operCONSEQ = 10,    // :> consequence (if left operand is true, the right operand must be true as well; is this conditions isn't fullfiled returns false)

	operPlus = 11,      // +
	operMinus = 12,     // -
	operDiv = 13,       // /
	operMult = 14,      // *
	operMOD = 15,       // MOD
	operDivInt = 16,    /* \ */
	operExpon = 17,     // ^
	operChangeSign = 18, // - (unary)

	operNone = 19,
};


class COperation
{
public:	
	int id;
	int left;
	int right;
	bool binaryOperation;
};

// A single value within expression, either double, boolean, string or matrix
class CExpressionValue
{
public:	
	RasterMatrix* matrix;
	GDALRasterBand* band;
	tkValueType type;
	CString str;
	double dbl;
	bool bln;

	CExpressionValue(void)
	{
		dbl = 0.0;
		bln = false;
		type = vtDouble;
		matrix = NULL;
		band = NULL;
	}

	~CExpressionValue()
	{
		if (matrix)
			delete matrix;
	}

	CExpressionValue& operator=(const CExpressionValue& val)
	{
		if (this == &val)
			return *this;

		this->bln = val.bln;
		this->dbl = val.dbl;
		this->str = val.str;
		this->type = val.type;
		return *this;
	}	
};

class CElement
{
public:		
	CElement()
	{
		wasCalculated = false;
		type = etNone;
		turnedOff = false;
		priority = 255;

		operation = operNone;
		isField = false;
		fieldIndex = -1;
		partIndex = -1;

		val = new CExpressionValue();
		calcVal = new CExpressionValue();
	}

	~CElement()
	{
		delete val;
		delete calcVal;
	}

	CString fieldName;       // name of field (in [square brackets])
	tkElementType type;      // type of element
	tkOperation operation;	 // type of operation
	CExpressionValue* val;		// initial value
	CExpressionValue* calcVal;	// value after calculation (in case of consecutive calculations it doesn't rewrite the initial value)

	// therefor no additional parsing is needed
	// parameters
	int priority;            // priority of operation, with less absolute values of priority are preformed first
	int fieldIndex;			 // index of field in attribute table
	bool isField;			 // the element is field from table

	// performing calculation
	bool wasCalculated;      // the value has been calculated, so calculated value should be used henceforth
	bool turnedOff;          // turned off till the end of calculation
	int partIndex;           // the element is result of calculations on the bracket with given index
};

// part of expression in brackets
class CExpressionPart
{
public:		
	//int number;						// порядоквый номер
	std::vector<CElement*> elements; // fields, operators, constants
	CString expression;	            // for debugging
	CExpressionValue* val;
	int activeCount;

	CExpressionPart()
	{
		activeCount = 0;
		val = NULL;
	}
};

// Parses and calculates math expression, potentially with variable fields
// See supported operators below
class CExpression
{
private:
	
public:
	CExpression()
	{
		//Debug::WriteLine("Constructor");
	}
	~CExpression()
	{
		Clear();
	}
	void Clear()
	{
		for(size_t i = 0; i < _parts.size(); i++)
		{
			for(size_t j = 0; j < _parts[i]->elements.size(); j++)
			{
				delete _parts[i]->elements[j];
			}
			delete _parts[i];
		}
		for(size_t i = 0; i < _operations.size(); i++)
		{
			delete _operations[i];
		}
		_variables.clear();
		_parts.clear();
		_operations.clear();
		_strings.clear();
	}
	void ReleaseMemory()
	{
		for(size_t i = 0; i < _parts.size(); i++)
		{
			for(size_t j = 0; j < _parts[i]->elements.size(); j++)
			{
				if (_parts[i]->elements[j]->type == etValue)
				{
					CExpressionValue* v = _parts[i]->elements[j]->calcVal;
					if (v->matrix)
					{
						delete v->matrix;
						v->matrix = NULL;
					}
				}
			}
		}
	}

private:
	std::vector<CExpressionPart*> _parts;
	std::vector<CElement*> _variables;
	std::vector<CString> _fields;
	std::vector<COperation*> _operations;
	std::vector<CString> _strings;
	bool _useFields;
	bool _saveOperations;
	CString _errorMessage;	// the description of error
	int _errorPosition;		// the position of error
	int _errorLength;		// the length sub string with error
	
public:
	CExpressionValue* Calculate(CString& errorMessage);
	bool ParseExpression(CString s, bool useFields, CString& error);
	bool ReadFieldNames(ITable* tbl);

	// variable fields
	int get_NumFields()								
	{
		return _variables.size();
	}
	CString get_FieldName(int FieldId)
	{
		CString s = _variables[FieldId]->fieldName;
		return s;
	}
	int get_FieldIndex(int FieldId)
	{
		return _variables[FieldId]->fieldIndex;
	}
	
	CExpressionValue* get_FieldValue(int FieldId)
	{
		return _variables[FieldId]->val;
	}
	void put_FieldValue(int FieldId, double newVal)
	{
		_variables[FieldId]->val->type = vtDouble;
		_variables[FieldId]->val->dbl = newVal;
	}
	void put_FieldValue(int FieldId, BSTR newVal)
	{
		USES_CONVERSION;
		_variables[FieldId]->val->type = vtString;
		_variables[FieldId]->val->str = OLE2CA(newVal);
	}
	void put_FieldValue(int FieldId, CString newVal)
	{
		_variables[FieldId]->val->type = vtString;
		_variables[FieldId]->val->str = newVal;
	}
	void put_FieldValue(int FieldId, bool newVal)
	{
		_variables[FieldId]->val->type = vtBoolean;
		_variables[FieldId]->val->bln = newVal;
	}

private:
	// parsing the expression
	bool ParseExpressionPart(CString s);
	bool ReadValue(CString s, int& position, CElement* element);
	bool ReadOperation(CString s, int& position, CElement& element);
	bool GetBrackets(CString expression, int& begin, int& end, CString open = "(", CString close = ")");

	// calculation of expression
	bool FindOperation(CExpressionPart* part, COperation& operation); //int& operation, int& left, int& right);
	bool CalculateOperation(CExpressionPart* part,  COperation& operation); //int left, int operation, int right);
	inline CExpressionValue* GetValue(CExpressionPart* part, int elementId);

	// utility functions
	void SkipSpaces(CString s, int& position);
	bool IsInteger(CString& str);
	bool IsDecimalZero(CString& str);
	bool IsDecimal( CString& str );
	bool IsDecimal( char chr, bool& exponential );
	void ReplaceSubString(CString& s, int begin, int length, CString replacement);

	// interaction with table
	void SetFieldValues(ITable* tbl);
	TwoArgOperator CExpression::GetMatrixOperation(tkOperation op);
	CString CExpression::Test();
};


