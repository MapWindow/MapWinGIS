// WmsLayer.h : Declaration of the CWmsLayer
#pragma once
#include "WmsCustomProvider.h"
#include "TileManager.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CWmsLayer :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CWmsLayer, &CLSID_WmsLayer>,
	public IDispatchImpl<IWmsLayer, &IID_IWmsLayer, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CWmsLayer()
		: _manager(false), _screenBuffer(NULL)
	{
		m_pUnkMarshaler = NULL;
		_key = SysAllocString(L"");
		_lastErrorCode = tkNO_ERROR; 
		_provider = new WmsCustomProvider();

		gReferenceCounter.AddRef(tkInterface::idWmsLayer);
	}

	virtual ~CWmsLayer();

	DECLARE_REGISTRY_RESOURCEID(IDR_WmsLayer)

	BEGIN_COM_MAP(CWmsLayer)
		COM_INTERFACE_ENTRY(IWmsLayer)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(
			GetControllingUnknown(), &m_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		m_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> m_pUnkMarshaler;

public:
	STDMETHOD(get_Id)(LONG* pVal);
	STDMETHOD(put_Id)(LONG newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(put_Name)(BSTR newVal);
	STDMETHOD(get_BoundingBox)(IExtents** pVal);
	STDMETHOD(put_BoundingBox)(IExtents* newVal);
	STDMETHOD(get_Epsg)(LONG* pVal);
	STDMETHOD(put_Epsg)(LONG newVal);
	STDMETHOD(get_Layers)(BSTR* pVal);
	STDMETHOD(put_Layers)(BSTR newVal);
	STDMETHOD(get_BaseUrl)(BSTR* pVal);
	STDMETHOD(put_BaseUrl)(BSTR newVal);
	STDMETHOD(get_Format)(BSTR* pVal);
	STDMETHOD(put_Format)(BSTR newVal);
	STDMETHOD(get_IsEmpty)(VARIANT_BOOL* pVal);
	STDMETHOD(get_MapExtents)(IExtents** pVal);
	STDMETHOD(Close)();
	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR state, VARIANT_BOOL* retVal);
	STDMETHOD(get_GeoProjection)(IGeoProjection** pVal);
	STDMETHOD(get_Opacity)(BYTE* pVal);
	STDMETHOD(put_Opacity)(BYTE newVal);
	STDMETHOD(get_Brightness)(FLOAT* pVal);
	STDMETHOD(put_Brightness)(FLOAT newVal);
	STDMETHOD(get_Contrast)(FLOAT* pVal);
	STDMETHOD(put_Contrast)(FLOAT newVal);
	STDMETHOD(get_Saturation)(FLOAT* pVal);
	STDMETHOD(put_Saturation)(FLOAT newVal);
	STDMETHOD(get_Hue)(FLOAT* pVal);
	STDMETHOD(put_Hue)(FLOAT newVal);
	STDMETHOD(get_Gamma)(FLOAT* pVal);
	STDMETHOD(put_Gamma)(FLOAT newVal);
	STDMETHOD(get_UseCache)(VARIANT_BOOL* pVal);
	STDMETHOD(put_UseCache)(VARIANT_BOOL newVal);
	STDMETHOD(get_DoCaching)(VARIANT_BOOL* pVal);
	STDMETHOD(put_DoCaching)(VARIANT_BOOL newVal);
	STDMETHOD(get_TransparentColor)(OLE_COLOR* pVal);
	STDMETHOD(put_TransparentColor)(OLE_COLOR newVal);
	STDMETHOD(get_UseTransparentColor)(VARIANT_BOOL* pVal);
	STDMETHOD(put_UseTransparentColor)(VARIANT_BOOL newVal);
	STDMETHOD(get_Version)(tkWmsVersion* pVal);
	STDMETHOD(put_Version)(tkWmsVersion newVal);
	STDMETHOD(get_Styles)(BSTR* pVal);
	STDMETHOD(put_Styles)(BSTR newVal);

private:	
	BSTR _key;
	long _lastErrorCode;
	WmsCustomProvider* _provider;
	TileManager _manager;
	Gdiplus::Bitmap* _screenBuffer;
	
public:
	// properties
	WmsCustomProvider* get_InnerProvider() { return _provider; }

public:
	// methods
	void Load(IMapViewCallback* map, bool isSnapshot = false, CString key = "");
	void ResizeBuffer(int cx, int cy) ;
	TileManager* get_Manager() { return &_manager; }
	Gdiplus::Bitmap* get_ScreenBuffer() { return _screenBuffer; }
	CPLXMLNode* SerializeCore(CString ElementName);
	bool DeserializeCore(CPLXMLNode* node);
};

OBJECT_ENTRY_AUTO(__uuidof(WmsLayer), CWmsLayer)
