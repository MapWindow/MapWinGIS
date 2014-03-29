#pragma once

class ShapeValidator
{
public:
	ShapeValidator(void) {};
	~ShapeValidator(void) {};

	static IShapeValidationInfo* Validate(IShapefile* sf, tkShapeValidationMode validationMode, 
			tkShapeValidationType validationType, CString className, CString methodName, CString parameterName,
			bool selectedOnly, bool editingInPlace = false);
	
};
