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
	_shapes[layerHandle].push_back(shapeIndex);
	return S_OK;
}

// ****************************************************************
//						get_NumLayers()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_NumLayers(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = _shapes.size();
	return S_OK;
}

// ****************************************************************
//						get_LayerHandle()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_LayerHandle(LONG index, LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = -1;
	int i = 0;
	std::map <long, vector<long>>::iterator p = _shapes.begin();
	while (p != _shapes.end())
	{
		if (i == index)
		{
			*pVal = p->first;
			break;
		}
		i++;
		p++;
	}
	return S_OK;
}

// ****************************************************************
//						get_ShapeIndices()						         
// ****************************************************************
STDMETHODIMP CSelectionList::get_ShapeIndices(LONG index, VARIANT* pVal, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;
	int i = 0;
	std::map <long, vector<long>>::iterator p = _shapes.begin();
	while (p != _shapes.end())
	{
		if (i == index)
		{
			*retVal = Templates::Vector2SafeArray(&p->second, VT_I4, pVal) ? VARIANT_TRUE : VARIANT_FALSE;
			break;
		}
		i++;
	}
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