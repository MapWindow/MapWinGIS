// SelectionList.cpp : Implementation of CSelectionList
#include "stdafx.h"
#include "SelectionList.h"
#include "Templates.h"

// ****************************************************************
//						Add()						         
// ****************************************************************
STDMETHODIMP CSelectionList::Add(LONG layerHandle, LONG shapeIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_shapes.push_back(SelectedItem(layerHandle, shapeIndex));
	return S_OK;
}

// ****************************************************************
//						get_NumLayers()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_Count(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _shapes.size();
	return S_OK;
}

// ****************************************************************
//						get_LayerHandle()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_LayerHandle(LONG index, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (index < 0 || index >= (long)_shapes.size())
	{
		*retVal = -1;
		return S_OK;
	}
	*retVal = _shapes[index].LayerHandle;
	return S_OK;
}

// ****************************************************************
//						get_ShapeIndex()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_ShapeIndex(LONG index, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (index < 0 || index >= (long)_shapes.size())
	{
		*retVal = -1;
		return S_OK;
	}
	*retVal = _shapes[index].ShapeIndex;
	return S_OK;
}

// ****************************************************************
//						Clear()						         
// ****************************************************************
STDMETHODIMP CSelectionList::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_shapes.clear();
	return S_OK;
}

// ****************************************************************
//				RemoveByLayerHandle()						         
// ****************************************************************
STDMETHODIMP CSelectionList::RemoveByLayerHandle(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	for (long i = (long)_shapes.size() - 1; i >= 0; i--) 
	{
		if (_shapes[i].LayerHandle == layerHandle) 
		{
			_shapes.erase(_shapes.begin() + i);
		}
	}
	return S_OK;
}
