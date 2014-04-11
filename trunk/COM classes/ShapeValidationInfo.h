// ShapeValidationInfo.h : Declaration of the CShapeValidationInfo

#pragma once
#include "resource.h"       // main symbols
#include "MapWinGIS.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CShapeValidationInfo
class ATL_NO_VTABLE CShapeValidationInfo :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShapeValidationInfo, &CLSID_ShapeValidationInfo>,
	public IDispatchImpl<IShapeValidationInfo, &IID_IShapeValidationInfo, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CShapeValidationInfo()
	{
		validationType = tkShapeValidationType::svtInput;
		validationMode = tkShapeValidationMode::NoValidation;
		validationStatus = tkShapeValidationStatus::WasntValidated;
		wereInvalidCount = 0;
		fixedCount = 0;
		stillInvalidCount = 0;
		skippedCount = 0;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SHAPEVALIDATIONINFO)

	BEGIN_COM_MAP(CShapeValidationInfo)
		COM_INTERFACE_ENTRY(IShapeValidationInfo)
		COM_INTERFACE_ENTRY(IDispatch)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	STDMETHOD(get_ClassName)( BSTR* retVal);
	STDMETHOD(get_MethodName)( BSTR* retVal);
	STDMETHOD(get_ParameterName)( BSTR* retVal);
	STDMETHOD(get_ValidationType)( tkShapeValidationType* retVal);
	STDMETHOD(get_ValidationMode)( tkShapeValidationMode* retVal);
	STDMETHOD(get_IsValid)( VARIANT_BOOL* retVal);
	STDMETHOD(get_Status)( tkShapeValidationStatus* retVal);
	STDMETHOD(get_WereInvalidCount)( int* retVal);
	STDMETHOD(get_StillInvalidCount)( int* retVal);
	STDMETHOD(get_FixedCount)( int* retVal);
	STDMETHOD(get_SkippedCount)( int* retVal);
	STDMETHOD(get_WasValidated)( VARIANT_BOOL* retVal);

	CString className;
	CString methodName;
	CString parameterName;
	tkShapeValidationType validationType;
	tkShapeValidationMode validationMode;
	tkShapeValidationStatus validationStatus;
	int wereInvalidCount;
	int fixedCount;
	int stillInvalidCount;
	int skippedCount;
};

OBJECT_ENTRY_AUTO(__uuidof(ShapeValidationInfo), CShapeValidationInfo)
