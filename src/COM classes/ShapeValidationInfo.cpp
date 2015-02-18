// ShapeValidationInfo.cpp : Implementation of CShapeValidationInfo

#include "stdafx.h"
#include "ShapeValidationInfo.h"

// ***********************************************
//		ClassName
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_ClassName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = A2BSTR(className);
	return S_OK;
}

// ***********************************************
//		MethodName
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_MethodName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = A2BSTR(methodName);
	return S_OK;
}

// ***********************************************
//		ParameterName
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_ParameterName(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*retVal = A2BSTR(parameterName);
	return S_OK;
}

// ***********************************************
//		ValidationType
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_ValidationType(tkShapeValidationType* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = validationType;
	return S_OK;
}

// ***********************************************
//		ValidationMode
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_ValidationMode(tkShapeValidationMode* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = validationMode;
	return S_OK;
}

// ***********************************************
//		IsValid
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_IsValid(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = (stillInvalidCount > 0 || skippedCount > 0) ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// ***********************************************
//		Status
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_Status(tkShapeValidationStatus* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = validationStatus;
	return S_OK;
}

// ***********************************************
//		WereInvalidCount
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_WereInvalidCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = wereInvalidCount;
	return S_OK;
}

// ***********************************************
//		StillInvalidCount
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_StillInvalidCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = stillInvalidCount;
	return S_OK;
}

// ***********************************************
//		FixedCount
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_FixedCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = wereInvalidCount;
	return S_OK;
}

// ***********************************************
//		SkippedCount
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_SkippedCount(int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = skippedCount;
	return S_OK;
}

// ***********************************************
//		ErrorCount
// ***********************************************
STDMETHODIMP CShapeValidationInfo::get_WasValidated( VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = validationMode != tkShapeValidationMode::NoValidation;
	return S_OK;
}
