#include "stdafx.h"
#include "ExpressionParts.h"

// ************************************************************
//	 Clear()
//************************************************************
void CustomFunction::Clear()
{
	for (size_t i = 0; i < _params.size(); i++)
	{
		delete _params[i];
	}
}

// ************************************************************
//	 ParseName()
//************************************************************
void CustomFunction::ParseName(CString name)
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

// ************************************************************
//	 InitOverloads()
//************************************************************
void CustomFunction::InitOverloads()
{
	switch (this->FunctionId())
	{
		case fnGeometryToWkt:
			description("Returns the area of the current feature. Current feature must have polygon type.");
			break;
		case fnArea: 
			description("Returns the area of the current feature. Current feature must have polygon type.");
			break;
		case fnLength: 
			description("Returns the length of the current feature. Current feature must have polyline type.");
			break;
		case fnPerimeter: 
			description("Returns the perimeter of the current feature. Current feature must have polygon type.");
			break;
		case fnX: 
			description("Retrieves X coordinate of the first point of current feature.");
			break;
		case fnY: 
			description("Retrieves Y coordinate of the first point of current feature.");
			break;
		case fnXat: 
			description("Retrieves X coordinate of the specified point of the current feature.");
			AddParameter("pointIndex", "Point index to return the value from. Must be in [0, numPoints] interval.");
			break;
		case fnYat: 
			description("Retrieves Y coordinate of the specified point of the current feature.");
			AddParameter("pointIndex", "Point index to return the value from. Must be in [0, numPoints] interval.");
			break;
		case fnIf: 
			description("Returns either true_value or false_value depending on the results of evaluating of first argument.");
			AddParameter("expr", "Boolean expression to evaluate (must return either true or false).");
			AddParameter("true_value", "Value to be returned if the expression is true.");
			AddParameter("false_value", "Value to be returned if the expression is false.");
			break;
		case fnToInt: 
			description("Converts a string to integer number.");
			AddParameter("string", "The input value (string, numeric or boolean) to convert.");
			break;
		case fnToReal: 
			description("Converts value to a real number.");
			AddParameter("string", "The input value (string, numeric or boolean) to convert.");
			break;
		case fnToString: 
			description("Converts numeric or boolean value to string.");
			AddParameter("string", "The input value (numeric or boolean) to convert.");
			break;
		case fnLower: 
			description("Converts a string to lower case letters.");
			AddParameter("string", "The input string.");
			break;
		case fnUpper: 
			description("Converts a string to upper case letters.");
			AddParameter("string", "The input string.");
			break;
		case fnTitleCase: 
			description("Converts all words of a string to title case (all words lower case with leading capital letter).");
			AddParameter("string", "The input string.");
			break;
		case fnTrim: 
			description("Removes all leading and trailing whitespace from a string.");
			AddParameter("string", "The input string.");
			break;
		case fnLen: 
			description("Returns the length of a string.");
			AddParameter("string", "The input string.");
			break;
		case fnReplace: 
			description("Replaces sub string in the input string.");
			AddParameter("input", "The input string.");
			AddParameter("before", "The string to replace.");
			AddParameter("after", "The new string to replace the old one with.");
			break;
		case fnSubstr: 
			description("Return a part of a string.");
			AddParameter("input", "The input string.");
			AddParameter("pos", "The start position to extract from.");
			//The length of the string to extract.
			break;
		case fnConcat: 
			description("Concatenates several strings to one.");
			AddParameter("str1", "First string argument.");
			AddParameter("str2", "Second string argument.");
			break;
		case fnStrpos: 
			description("Return the index (position) of a sub string in the string.");
			AddParameter("input", "The input string.");
			AddParameter("substring", "Sub string to look for.");
			break;
		case fnLeft: 
			description("Returns a substring that contains the n leftmost characters of the string.");
			AddParameter("input", "The input string.");
			AddParameter("length", "The number of characters from the left to return.");
			break;
		case fnRight: 
			description("Returns a substring that contains the n rightmost characters of the string.");
			AddParameter("input", "The input string.");
			AddParameter("length", "The number of characters from the right to return.");
			break;	
		case fnLPad:
			description("Adds specified character to the left of the string until it reaches specified length.");
			AddParameter("input", "The input string.");
			AddParameter("length", "New length of the string.");
			AddParameter("pad", "Character to pad with.");
			break;
		case fnRPad:
			description("Adds specified character to the right of the string until it reaches specified length.");
			AddParameter("input", "The input string.");
			AddParameter("length", "New length of the string.");
			AddParameter("pad", "Character to pad with.");
			break;
		case fnSqrt: 
			description("Returns the square root of x.");
			AddParameter("x", "Value whose square root is computed.");
			break;
		case fnAbs: 
			description("Returns the absolute value of x: |x|.");
			AddParameter("x", "Value whose absolute value is returned.");
			break;
		case fnCos: 
			description("Returns cosine of the angle.");
			AddParameter("angle", "Angle in radians.");
			break;
		case fnSin: 
			description("Returns sine of the angle.");
			AddParameter("angle", "Angle in radians.");
			break;
		case fnTan: 
			description("Returns tangent of the angle.");
			AddParameter("angle", "Angle in radians.");
			break;
		case fnAsin: 
			description("Returns reverse sine, i.e. angle in radians sine of which equals the argument.");
			AddParameter("number", "Sine value.");
			break;
		case fnAcos: 
			description("Returns reverse cosine, i.e. angle in radians cosine of which equals the argument.");
			AddParameter("number", "Cosine value.");
			break;
		case fnAtan: 
			description("Returns reverse tangent, i.e. angle in radians tangent of which equals the argument.");
			AddParameter("number", "Tangent value.");
			break;
		case fnAtan2: 
			description("Returns reverse tangent, i.e. angle in radians tangent of which equals the argument.");
			AddParameter("y", "Value representing the proportion of the y-coordinate.");
			AddParameter("x", "Value representing the proportion of the x-coordinate.");
			break;
		case fnExp: 
			description("Returns the natural logarithm of x.");
			AddParameter("x", "Value whose logarithm is calculated.");
			break;
		case fnLn:
			description("Returns the natural logarithm of x.");
			AddParameter("x", "Value whose logarithm is calculated (must be positive).");
			break;
		case fnLog10: 
			description("Returns the base 10 logarithm of x.");
			AddParameter("x", "Value whose logarithm is calculated (must be positive).");
			break;
		case fnLog: 
			description("Returns logarithm of x with base b.");
			AddParameter("base", "Base of the algorithm (must be positive).");
			AddParameter("x", "Value whose logarithm is calculated (must be positive).");
			break;
		case fnRound: 
			description("Returns the integral value that is nearest to x, with halfway cases rounded away from zero.");
			AddParameter("base", "Value to round.");
			break;
		case fnRand: 
			description("Returns a pseudo-random integral number in the range between min and max.");
			AddParameter("min", "Minimum value.");
			AddParameter("max", "Maximum value.");
			break;
		case fnRandf: 
			description("Returns a pseudo-random real number in the range between min and max.");
			AddParameter("min", "Minimum value.");
			AddParameter("max", "Maximum value.");
			break;
		case fnMax: 
			description("Returns maximum numeric value from the list of arguments. Unlimited number of parameters is accepted.");
			AddParameter("val1", "First numeric value.");
			AddParameter("val2", "Second numeric value.");
			break;
		case fnMin: 
			description("Returns minimum numeric value from the list of arguments. Unlimited number of parameters is accepted.");
			AddParameter("val1", "First numeric value.");
			AddParameter("val2", "Second numeric value.");
			break;
		case fnClamp: 
			description("Restricts an input value to a specified range.");
			AddParameter("min", "The largest value input is allowed to take.");
			AddParameter("input", "Value which will be restricted to the range specified by minimum and maximum.");
			AddParameter("max", "The largest value input is allowed to take.");
			break;
		case fnFloor: 
			description("Rounds x downward, returning the largest integral value that is not greater than x.");
			AddParameter("x", "The value of x rounded downward.");
			break;
		case fnCeil: 
			description("Rounds x upward, returning the smallest integral value that is greater than x.");
			AddParameter("x", "The value of x rounded upward.");
			break;
		case fnPi:
			description("Rounds value of pi constant (3.14159265359).");
			break;
	}
}

// ************************************************************
//	 GetSignature()
//************************************************************
CStringW CustomFunction::GetSignature()
{
	USES_CONVERSION;
	CStringW name = A2W(GetName());

	if (name.Left(1) == "$")
	{
		return name;
	}

	name += L"(";

	for (size_t i = 0; i < _params.size(); i++)
	{
		name += _params[i]->name;

		if (i < _params.size() - 1)
		{
			name += "; ";
		}
	}

	name += L")";

	return name;
}

// ***********************************************************
//	 ClearElements()
//************************************************************
void CExpressionPart::ClearElements()
{
	ReleaseValue();

	for (size_t i = 0; i < elements.size(); i++) {
		delete elements[i];
	}

	elements.clear();
}

// ***********************************************************
//	 ReleaseValue()
//************************************************************
void CExpressionPart::ReleaseValue()
{
	if (function && val)
	{
		// functions don't have nested elements, only parts,
		// so we own the element, and must release it
		delete val;
		val = NULL;
	}
}

// ***********************************************************
//	 Reset()
//************************************************************
void CExpressionPart::Reset()
{
	int size = elements.size();
	for (int j = 0; j < size; j++)
	{
		CElement* el = elements[j];
		el->wasCalculated = false;
		el->turnedOff = false;
	}

	ReleaseValue();
}