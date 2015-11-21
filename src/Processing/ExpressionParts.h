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

	operLike = 20,
};

class CExpressionValue;
class COperation;
class CElement;
class CExpressionPart;
class CustomFunction;

typedef bool(*ExpressionFunction)(const vector<CExpressionValue*>& arguments, IShape* shape, CExpressionValue& result);

// ********************************************************
//     COperation
// ********************************************************
class COperation
{
public:
	int id;					// id of operator element
	int left;				// id of the left operand (element)
	int right;				// id of the right operand (element)
	bool binaryOperation;   // whether it's binary operation, i.e. with 2 operands
};

// ********************************************************
//     CExpressionValue
// ********************************************************
// Represents single value within expression, either double, boolean, string or matrix
class CExpressionValue
{
	RasterMatrix* _matrix;
	GDALRasterBand* _band;
	tkValueType _type;
	CStringW _str;
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

	bool bln() { return _bln; }
	CStringW str() { return _str; }
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

	void str(CStringW s)
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

	void copyFrom(CExpressionValue& value)
	{
		switch (value.type())
		{
			case vtBoolean:
				bln(value.bln());
				break;
			case vtString:
				str(value.str());
				break;
			case vtDouble:
				dbl(value.dbl());
				break;
			case vtFloatArray:
			// TODO: implement for geometry
				break;
		}
	}
};

// ********************************************************
//     CElement
// ********************************************************
// An element of the expression tree, either operator or operand
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

// ********************************************************
//     FunctionParameter
// ********************************************************
class FunctionParameter
{
public:
	CStringW description;
	CStringW name;	

	FunctionParameter(CStringW name, CStringW description)
		:name(name), description(description)
	{
		
	}
};

// ********************************************************
//     Function
// ********************************************************
// Statically defined function (treated as expression part)
class CustomFunction
{
private:
	bool _useGeometry;
	tkFunctionGroup _group;
	ExpressionFunction _fn;
	vector<CString> _aliases;
	CStringW _description;
	FunctionId _fnId;
	vector<FunctionParameter*> _params;

public:
	CustomFunction(FunctionId fnId, CString name, int numParams, ExpressionFunction function, tkFunctionGroup group, bool useGeometry = false)
		: _fnId(fnId), _useGeometry(useGeometry), _group(group), _fn(function)
	{
		ParseName(name);

		InitOverloads();
	}

	~CustomFunction()
	{
		Clear();
	}

	void Clear();

	void InitOverloads();

	void ParseName(CString name);

	vector<CString>* getAliases() { return &_aliases; }

	CString GetName()  { return _aliases[0]; }

	int numParams() { return _params.size(); }

	bool useGeometry() { return _useGeometry; }

	tkFunctionGroup group() { return _group; }

	CStringW description() { return _description; }
	void description(CStringW value) { _description = value; }

	FunctionId FunctionId() { return _fnId;	}

	CStringW GetSignature();

	void AddParameter(CStringW name, CStringW description) 
	{
		FunctionParameter* p = new FunctionParameter(name, description);
		_params.push_back(p);
	}

	FunctionParameter* getParameter(int parameterIndex)
	{
		if (parameterIndex < 0 || parameterIndex >= static_cast<int>(_params.size()))
		{
			return NULL;
		}

		return _params[parameterIndex];
	}

	bool call(vector<CExpressionValue*>& arguments, IShape* shape, CExpressionValue& result)
	{
		return _fn(arguments, shape, result);
	}
};

// ********************************************************
//     CExpressionPart
// ********************************************************
// Part of the expression inside brackets
class CExpressionPart
{
public:
	CString expression;					 // for debugging only
	bool isArgument;					 // for debugging only
	vector<CElement*> elements;			 // fields, operators, literals
	vector<CExpressionPart*> arguments;  // references to other parts
	CustomFunction* function;
	CExpressionValue* val;				 // resulting value
	int activeCount;					 // number of unprocessed elements

	CExpressionPart()
	{
		isArgument = false;
		activeCount = 0;
		function = NULL;
		val = NULL;
	}

	~CExpressionPart()
	{
		ClearElements();
	}

	bool isFunction() { return function != NULL; }

private:
	void ClearElements()
	{
		for (size_t i = 0; i < elements.size(); i++) {
			delete elements[i];
		}

		elements.clear();
	}
};