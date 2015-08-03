#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CGdalDriverManager :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGdalDriverManager, &CLSID_GdalDriverManager>,
	public IDispatchImpl<IGdalDriverManager, &IID_IGdalDriverManager, &LIBID_MapWinGIS, /*wMajor =*/ 4, /*wMinor =*/ 9>
{
public:
	CGdalDriverManager()
	{
		_pUnkMarshaler = NULL;
		GDALAllRegister(); 
		_manager = GetGDALDriverManager();
		_manager->AutoLoadDrivers();
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GDALDRIVERMANAGER)

	BEGIN_COM_MAP(CGdalDriverManager)
		COM_INTERFACE_ENTRY(IGdalDriverManager)
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
	STDMETHOD(get_DriverCount)(LONG* pVal);
	STDMETHOD(get_Driver)(LONG driverIndex, IGdalDriver** pVal);
	STDMETHOD(get_DriverByName)(BSTR driverName, IGdalDriver** pVal);
	
private:
	GDALDriverManager* _manager;

public:
	void CreateDriverInstance(GDALDriver* driver, IGdalDriver** pVal);
	
};

OBJECT_ENTRY_AUTO(__uuidof(GdalDriverManager), CGdalDriverManager)
