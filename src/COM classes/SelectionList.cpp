// SelectionList.cpp : Implementation of CSelectionList
#include "stdafx.h"
#include "SelectionList.h"
#include "Templates.h"

// ****************************************************************
//						Add()						         
// ****************************************************************
STDMETHODIMP CSelectionList::AddShape(LONG layerHandle, LONG shapeIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	_items.push_back(new SelectedItem(layerHandle, shapeIndex));
	return S_OK;
}

// ****************************************************************
//						get_NumLayers()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_Count(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _items.size();
	return S_OK;
}

// ****************************************************************
//						get_LayerHandle()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_LayerHandle(LONG index, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (index < 0 || index >= (long)_items.size())
	{
		*retVal = -1;
		return S_OK;
	}

	*retVal = _items[index]->LayerHandle;

	return S_OK;
}

// ****************************************************************
//						get_ShapeIndex()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_ShapeIndex(LONG index, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (index < 0 || index >= (long)_items.size())
	{
		*retVal = -1;
		return S_OK;
	}
	*retVal = _items[index]->ShapeIndex;
	return S_OK;
}

// ****************************************************************
//						Clear()						         
// ****************************************************************
STDMETHODIMP CSelectionList::Clear()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for (size_t i = 0; i < _items.size(); i++)
	{
		delete _items[i];
		_items[i] = NULL;
	}

	_items.clear();

	return S_OK;
}

// ****************************************************************
//				RemoveByLayerHandle()						         
// ****************************************************************
STDMETHODIMP CSelectionList::RemoveByLayerHandle(LONG layerHandle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	for (long i = (long)_items.size() - 1; i >= 0; i--) 
	{
		if (_items[i]->LayerHandle == layerHandle) 
		{
			delete _items[i];
			_items.erase(_items.begin() + i);
		}
	}

	return S_OK;
}

// ****************************************************************
//				AddPixel()						         
// ****************************************************************
STDMETHODIMP CSelectionList::AddPixel(LONG layerHandle, LONG rasterX, LONG rasterY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	_items.push_back(new SelectedItem(layerHandle, rasterX, rasterY));

	return S_OK;
}

// ****************************************************************
//				TogglePixel()						         
// ****************************************************************
STDMETHODIMP CSelectionList::TogglePixel(LONG layerHandle, LONG rasterX, LONG rasterY)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	for (size_t i = 0; i < _items.size(); i++)
	{
		if (_items[i]->Match(layerHandle, rasterX, rasterY))
		{
			_items.erase(_items.begin() + i);
			return S_OK;
		}
	}

	AddPixel(layerHandle, rasterX, rasterY);

	return S_OK;
}

// ****************************************************************
//				get_LayerType()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_LayerType(LONG index, tkLayerType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (index < 0 || index >= (long)_items.size())
	{
		*pVal = tkLayerType::ltUndefined;
		return S_OK;
	}

	*pVal = _items[index]->LayerType;

	return S_OK;
}

// ****************************************************************
//				get_RasterX()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_RasterX(LONG index, LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (index < 0 || index >= (long)_items.size())
	{
		*pVal = tkLayerType::ltUndefined;
		return S_OK;
	}

	*pVal = _items[index]->RasterX;

	return S_OK;
}

// ****************************************************************
//				get_RasterY()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_RasterY(LONG index, LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (index < 0 || index >= (long)_items.size())
	{
		*pVal = tkLayerType::ltUndefined;
		return S_OK;
	}

	*pVal = _items[index]->RasterY;

	return S_OK;
}

// ****************************************************************
//				UpdatePixelBounds()						         
// ****************************************************************
void CSelectionList::UpdatePixelBounds(long layerHandle, IImage* source, bool polygon)
{
	if (!source) {
		CallbackHelper::AssertionFailed("Unexpected NULL parameter in CSelectionList::UpdatePixelBounds.");
		return;
	}
	
	for (size_t i = 0; i < _items.size(); i++)
	{
		_items[i]->Polygon = polygon;

		if (_items[i]->LayerHandle == layerHandle && !_items[i]->Calculated)
		{
			long rasterX = _items[i]->RasterX;
			long rasterY = _items[i]->RasterY;

			double x, y;
			source->ImageToProjection(rasterX, rasterY, &x, &y);

			double x2, y2;
			source->ImageToProjection(rasterX + 1, rasterY + 1, &x2, &y2);

			_items[i]->AssignShapePixel(x, y, x2, y2);

			_items[i]->Calculated = true;
		}
	}
}

// ****************************************************************
//				GetItem()						         
// ****************************************************************
SelectedItem* CSelectionList::GetItem(int index)
{
	if (index < 0 || index >= (long)_items.size())
	{
		return NULL;
	}

	return _items[index];
}


