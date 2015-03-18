// SelectionList.h : Declaration of the CSelectionList
#pragma once
using namespace std;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


class ATL_NO_VTABLE CSelectionList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSelectionList, &CLSID_SelectionList>,
	public IDispatchImpl<ISelectionList, &IID_ISelectionList, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CSelectionList()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SELECTIONLIST)


	BEGIN_COM_MAP(CSelectionList)
		COM_INTERFACE_ENTRY(ISelectionList)
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
	STDMETHOD(Add)(LONG layerHandle, LONG shapeIndex);
	STDMETHOD(get_Count)(LONG* pVal);
	STDMETHOD(get_LayerHandle)(LONG index, LONG* pVal);
	STDMETHOD(get_ShapeIndex)(LONG index, LONG* pVal);
	STDMETHOD(Clear)();
	STDMETHOD(RemoveByLayerHandle)(LONG layerHandle);

private:
	struct SelectedItem {
		long LayerHandle;
		long ShapeIndex;
		
		SelectedItem(long layerHandle, long shapeIndex) {
			LayerHandle = layerHandle;
			ShapeIndex = shapeIndex;
		}
	};

	vector<SelectedItem> _shapes;
};
OBJECT_ENTRY_AUTO(__uuidof(SelectionList), CSelectionList)
