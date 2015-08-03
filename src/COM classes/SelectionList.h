// SelectionList.h : Declaration of the CSelectionList
#pragma once
#include "SelectedItem.h"
using namespace std;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


class ATL_NO_VTABLE CSelectionList :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CSelectionList, &CLSID_SelectionList>,
	public IDispatchImpl<ISelectionList, &IID_ISelectionList, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CSelectionList()
	{
		_pUnkMarshaler = NULL;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SELECTIONLIST)


	BEGIN_COM_MAP(CSelectionList)
		COM_INTERFACE_ENTRY(ISelectionList)
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
	STDMETHOD(AddShape)(LONG layerHandle, LONG shapeIndex);
	STDMETHOD(get_Count)(LONG* pVal);
	STDMETHOD(get_LayerHandle)(LONG index, LONG* pVal);
	STDMETHOD(get_ShapeIndex)(LONG index, LONG* pVal);
	STDMETHOD(Clear)();
	STDMETHOD(RemoveByLayerHandle)(LONG layerHandle);
	STDMETHOD(AddPixel)(LONG layerHandle, LONG rasterX, LONG rasterY);
	STDMETHOD(get_LayerType)(LONG index, tkLayerType* pVal);
	STDMETHOD(get_RasterX)(LONG index, LONG* pVal);
	STDMETHOD(get_RasterY)(LONG index, LONG* pVal);
	
private:
	vector<SelectedItem*> _items;

public:
	void UpdatePixelBounds(long layerHandle, IImage* source, bool polygon);
	SelectedItem* GetItem(int index);
};
OBJECT_ENTRY_AUTO(__uuidof(SelectionList), CSelectionList)
