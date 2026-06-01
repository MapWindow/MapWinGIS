// PlacedLabels.h : Declaration of the CPlacedLabels
#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

// CPlacedLabels
class ATL_NO_VTABLE CPlacedLabels :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CPlacedLabels, &CLSID_PlacedLabels>,
	public IDispatchImpl<IPlacedLabels, &IID_IPlacedLabels, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CPlacedLabels()
	{
		_pUnkMarshaler = nullptr;
		//_hotTracking = VARIANT_TRUE;
		//_mode = imAllLayers;
		//_color = RGB(255, 0, 0); //RGB(30, 144, 255);
		//_activeLayer = -1;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_PLACEDLABELS)

	BEGIN_COM_MAP(CPlacedLabels)
		COM_INTERFACE_ENTRY(IPlacedLabels)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;

public:
	STDMETHOD(get_NumIndex)(/*[out, retval]*/ long* pVal);
	STDMETHOD(GetIndexes)( /*[out, retval]*/ SAFEARRAY** retval);
	STDMETHOD(SetVector)(int* indexes, int length);

private:
	std::vector<int> _indexes;
};
OBJECT_ENTRY_AUTO(__uuidof(PlacedLabels), CPlacedLabels)
