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
	CGdalDataset()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GDALDATASET)

	BEGIN_COM_MAP(CGdalDataset)
		COM_INTERFACE_ENTRY(IGdalDataset)
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



};

OBJECT_ENTRY_AUTO(__uuidof(GdalDataset), CGdalDataset)
