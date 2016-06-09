#pragma once

class ShapeValidator
{
public:
	ShapeValidator(void) {};
	~ShapeValidator(void) {};

	static IShapeValidationInfo* Validate(IShapefile* sf, tkShapeValidationMode validationMode, 
			tkShapeValidationType validationType, CString className, CString methodName, CString parameterName,
			ICallback* callback, BSTR& key, bool selectedOnly, bool reportOnly = false);
	
};
