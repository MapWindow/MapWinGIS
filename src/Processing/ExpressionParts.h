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

class CExpressionValue;
class COperation;
class CElement;
class CExpressionPart;
class CFunction;

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

	bool bln() { return _bln; }
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
//     CFunction
// ********************************************************
// Statically defined function (treated as expression part)
class CFunction
{
private:
	CString _name;
	int _numParams;
	bool _useGeometry;
	CString _group;
	ExpressionFunction _fn;
	vector<CString> _aliases;

public:
	CFunction(CString name, int numParams, ExpressionFunction function, CString group, bool useGeometry = false)
		: _numParams(numParams), _useGeometry(useGeometry), _group(group), _fn(function)
	{
		ParseName(name);
	}

	void ParseName(CString name)
	{
		_aliases.clear();

		int position = 0;

		CString part = name.Tokenize(";", position);
		while (part.GetLength() > 0)
		{
			_aliases.push_back(part);
			part = name.Tokenize(";", position);
		}

		if (_aliases.size() == 0)
		{
			_aliases.push_back(name);
		}
	}

	vector<CString>* GetAliases() { return &_aliases; }

	CString name() { return _aliases[0]; }

	int numParams() { return _numParams; }

	bool useGeometry() { return _useGeometry; }

	CString group() { return _group; }

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
	CString expression;					// for debugging only
	vector<CElement*> elements;			// fields, operators, literals
	vector<CExpressionPart*> arguments;
	CFunction* function;
	CExpressionValue* val;				// resulting value
	int activeCount;					// number of unprocessed elements

	CExpressionPart()
	{
		activeCount = 0;
		function = NULL;
		val = NULL;
	}

	~CExpressionPart()
	{
		ClearElements();
		ClearArguments();
	}

	bool isFunction() { return function != NULL; }

private:
	void ClearArguments()
	{
		for (size_t i = 0; i < arguments.size(); i++) {
			delete arguments[i];
		}

		arguments.clear();
	}
	void ClearElements()
	{
		for (size_t i = 0; i < elements.size(); i++) {
			delete elements[i];
		}

		elements.clear();
	}
};