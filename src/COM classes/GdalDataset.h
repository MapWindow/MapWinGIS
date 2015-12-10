// GdalDataset.h : Declaration of the CGdalDataset

#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CGdalDataset :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGdalDataset, &CLSID_GdalDataset>,
	public IDispatchImpl<IGdalDataset, &IID_IGdalDataset, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGdalDataset() : _dataset(NULL)
	{
		_pUnkMarshaler = NULL;
		_proxy = false;

		gReferenceCounter.AddRef(idGdalDataset);
	}

	~CGdalDataset() 
	{
		if (!_proxy) {
			Close();
		}

		gReferenceCounter.Release(idGdalDataset);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GDALDATASET)

	BEGIN_COM_MAP(CGdalDataset)
		COM_INTERFACE_ENTRY(IGdalDataset)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, _pUnkMarshaler.p)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		return CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &_pUnkMarshaler.p);
		return S_OK;
	}

	void FinalRelease()
	{
		_pUnkMarshaler.Release();
	}

	CComPtr<IUnknown> _pUnkMarshaler;

public:
	STDMETHOD(get_Driver)(IGdalDriver** pVal);
	STDMETHOD(Open)(BSTR Filename, VARIANT_BOOL readOnly, VARIANT_BOOL* retVal);
	STDMETHOD(Close)();
	STDMETHOD(SetGeoTransform)(DOUBLE xLeft, DOUBLE dX, DOUBLE yProjOnX, DOUBLE yTop, DOUBLE xProjOnY, DOUBLE dY, VARIANT_BOOL* retVal);
	STDMETHOD(SetProjection)(BSTR Projection, VARIANT_BOOL* retVal);

private:
	GDALDataset* _dataset;
	bool _proxy;		// we can't close this datasource, since it's owned by someone else

private:
	bool CheckState();
	void ErrorMessage(CString msg);
	bool CheckOnwer();

public:
	void Inject(GDALDataset* ds) 
	{
		 _dataset = ds; 
		 _proxy = true; 
	}
};

OBJECT_ENTRY_AUTO(__uuidof(GdalDataset), CGdalDataset)
