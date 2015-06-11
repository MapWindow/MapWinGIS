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

enum tkFunction
{
	fnNone = -1,
	fnSin = 0,
	fnCos = 1,
	fnTan = 2,
	fnCtan = 3,
    fnSubString = 4,
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
	RasterMatrix* _matrix;
	GDALRasterBand* _band;
	tkValueType _type;
	CString _str;
	double _dbl;
	bool _bln;
public:	

	CExpressionValue(void)
	{
		_dbl = 0.0;
		_bln = false;
		_type = vtDouble;
		_matrix = NULL;
		_band = NULL;
	}

	~CExpressionValue()
	{
		clearMatrix();
	}

	CExpressionValue& operator=(CExpressionValue& val)
	{
		if (this == &val)
			return *this;

		_bln = val.bln();
		_dbl = val.dbl();
		_str = val.str();
		_type = val.type();
		return *this;
	}	

	bool bln() {return _bln; }
	CString str() { return _str; }
	double dbl() { return _dbl; }
	tkValueType type() { return _type; }
	RasterMatrix* matrix() { return _matrix; }
	GDALRasterBand* band() { return _band; }

	bool IsDouble() { return _type == vtDouble; }
	bool isBoolean() { return _type == vtBoolean; }
	bool isString() { return _type == vtString; }
	bool IsFloatArray() { return _type == vtFloatArray; }

	void bln(bool value) 
	{ 
		_bln = value; 
		_type = vtBoolean;
	}
	
	void str(CString s)
	{
		_str = s;
		_type = vtString;
	}
	
	void dbl(double dbl)
	{
		_dbl = dbl;
		_type = vtDouble;
	}

	void band(GDALRasterBand* band) 
	{ 
		_band = band; 
	}

	void matrix(RasterMatrix* m)
	{
		clearMatrix();
		
		_matrix = m;
		_type = vtFloatArray;
	}

	void clearMatrix()
	{
		if (_matrix) {
			delete _matrix;
			_matrix = NULL;
		}
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
	std::vector<CElement*> elements; // fields, operators, constants
	CString expression;	            // for debugging
	CExpressionValue* val;
	int activeCount;
	tkFunction functionId;
	bool isFunction;
	vector<CExpressionPart*> arguments;

	CExpressionPart()
	{
		functionId = fnNone;
		isFunction = false;
		activeCount = 0;
		val = NULL;
	}
};

typedef bool(*ExpressionFunction)(const vector<CExpressionValue*>& arguments, IShape* shape, CExpressionValue& result);

class CFunction
{
private:
	CString _name;
	int _numParams;
	bool _useGeometry;
	CString _group;
	ExpressionFunction _fn;

public:
	CFunction(CString name, int numParams, ExpressionFunction function, CString group, bool useGeometry = false)
		: _name(name), _numParams(numParams), _useGeometry(useGeometry), _group(group), _fn(function)
	{

	}

	CString name() { return _name; }

	int numParams() { return _numParams; }

	bool usesgeometry() { return _useGeometry; }

	CString group() { return _group; }

	bool fn(vector<CExpressionValue*>& arguments, IShape* shape, CExpressionValue& result)
	{
		return _fn(arguments, shape, result);
	}
};

// Parses and calculates math expression, potentially with variable fields
// See supported operators below
class CExpression
{
private:
	
public:
	CExpression() {
		_floatFormat = m_globalSettings.floatNumberFormat;
	}
	~CExpression()
	{
		Clear();
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
	CString _floatFormat;

private:
	// parsing the expression
	CExpressionPart* ParseExpressionPart(CString s);
	bool ReadValue(CString s, int& position, CElement* element);
	bool ReadOperation(CString s, int& position, CElement& element);
	bool GetBrackets(CString expression, int& begin, int& end, CString open = "(", CString close = ")");

	// calculation of expression
	bool FindOperation(CExpressionPart* part, COperation& operation); 
	bool CalculateOperation(CExpressionPart* part,  COperation& operation); 
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
	TwoArgOperator GetMatrixOperation(tkOperation op);
	void Reset();
	void BuildFieldList();
	int GetParameterCount(int functionId);
	void ResetActiveCountForParts();
	bool CalculateNextOperationWithinPart(CExpressionPart* part, CString& errorMessage, int& operationCount);
	bool FinishPart(CExpressionPart* part);
	bool EvaluateFunction(CExpressionPart* part);
	void EvaluatePart();
	bool EvaluatePart(CExpressionPart* part, CString& errorMessage, int& operationCount);

	

public:
	void Clear();
	void ReleaseMemory();
	CString GetFloatFormat() { return _floatFormat; }
	void SetFloatFormat(CString value) { _floatFormat = value; }

	CExpressionValue* Calculate(CString& errorMessage);

	bool Parse(CString s, bool useFields, CString& error);
	bool ReadFieldNames(ITable* tbl);
	void SetFields(vector<CString>& fields);
	void AddPart(CExpressionPart* part) { _parts.push_back(part); }
	
	vector<CString>* GetStrings() { return &_strings; }
	vector<CString>* GetFields() { return &_fields; }
	bool GetUseFields() { return _useFields; }

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
		_variables[FieldId]->val->dbl(newVal);
	}
	void put_FieldValue(int FieldId, BSTR newVal)
	{
		USES_CONVERSION;
		_variables[FieldId]->val->str(OLE2CA(newVal));
	}
	void put_FieldValue(int FieldId, CString newVal)
	{
		_variables[FieldId]->val->str(newVal);
	}
	void put_FieldValue(int FieldId, bool newVal)
	{
		_variables[FieldId]->val->bln(newVal);
	}

	void SetErrorMessage(CString msg) { _errorMessage = msg; }
	void SetErrorPosition(int position){ _errorPosition = position; }
};