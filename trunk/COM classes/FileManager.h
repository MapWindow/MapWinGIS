// FileManager.h : Declaration of the CFileManager

#pragma once
#include "MapWinGIS.h"
#include <comsvcs.h>

// CFileManager
class ATL_NO_VTABLE CFileManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileManager, &CLSID_FileManager>,
	public IDispatchImpl<IFileManager, &IID_IFileManager, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CFileManager()
	{
		USES_CONVERSION;
		m_key = A2BSTR("");
		m_lastErrorCode = tkNO_ERROR;
		m_globalCallback = NULL;
		_lastOpenStrategy = fosAutoDetect;
		_lastOpenIsSuccess = true;
		_lastOpenFilename = L"";
	};
	~CFileManager()
	{
		if (m_globalCallback)
			m_globalCallback->Release();
	};

	DECLARE_REGISTRY_RESOURCEID(IDR_FILEMANAGER)

	BEGIN_COM_MAP(CFileManager)
		COM_INTERFACE_ENTRY(IFileManager)
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
	STDMETHOD(get_IsSupportedBy)(BSTR Filename, tkSupportType supportType, VARIANT_BOOL* retVal);
	STDMETHOD(get_IsSupported)(BSTR Filename, VARIANT_BOOL* retVal);
	STDMETHOD(get_IsRgbImage)(BSTR Filename, VARIANT_BOOL* retVal);
	STDMETHOD(get_IsGrid)(BSTR Filename, VARIANT_BOOL* retVal);
	STDMETHOD(get_IsVectorLayer)(BSTR Filename, VARIANT_BOOL* retVal);
	STDMETHOD(get_OpenStrategy)(BSTR Filename, tkFileOpenStrategy* retVal);
	STDMETHOD(get_CanOpenAs)(BSTR Filename, tkFileOpenStrategy strategy, VARIANT_BOOL* retVal);
	STDMETHOD(get_HasProjection)(BSTR Filename, VARIANT_BOOL* retVal);
	STDMETHOD(get_GeoProjection)(BSTR Filename, IGeoProjection** retVal);
	STDMETHOD(get_IsSameProjection)(BSTR Filename, IGeoProjection* projection, VARIANT_BOOL* retVal);
	STDMETHOD(Open)(BSTR Filename, tkFileOpenStrategy openStrategy, ICallback* callback, IDispatch** retVal);
	STDMETHOD(OpenShapefile)(BSTR Filename, ICallback* callback, IShapefile** retVal);
	STDMETHOD(OpenRaster)(BSTR Filename, tkFileOpenStrategy openStrategy, ICallback* callback, IImage** retVal);
	STDMETHOD(DeleteDatasource)(BSTR Filename, VARIANT_BOOL* retVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_LastOpenStrategy)(tkFileOpenStrategy* retVal);
	STDMETHOD(get_LastOpenFilename)(BSTR* retVal);
	STDMETHOD(get_LastOpenIsSuccess)(VARIANT_BOOL* retVal);
private:
	tkFileOpenStrategy CFileManager::get_OpenStrategyCore(BSTR Filename);
	bool NeedProxyForGrid(CStringW filename);
	
	tkFileOpenStrategy _lastOpenStrategy;
	bool _lastOpenIsSuccess;
	CStringW _lastOpenFilename;
	CString test;
	ICallback * m_globalCallback;
	void ErrorMessage(long ErrorCode);
	long m_lastErrorCode;
	BSTR m_key;
};

OBJECT_ENTRY_AUTO(__uuidof(FileManager), CFileManager)
