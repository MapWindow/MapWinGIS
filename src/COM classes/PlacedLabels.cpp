// PlacedLabels.cpp : Implementation of CPlacedLabels

#include "stdafx.h"
#include "PlacedLabels.h"

// *****************************************************************
//		get_NumShapes()
// *****************************************************************
STDMETHODIMP CPlacedLabels::get_NumIndex(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = static_cast<long>(_indexes.size());
	return S_OK;
}


// *****************************************************************
//		GetIndexes()
// *****************************************************************
STDMETHODIMP CPlacedLabels::GetIndexes(SAFEARRAY** retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retval = SafeArrayCreateVector(VT_I4, 0, static_cast<ULONG>(_indexes.size()));
	SafeArrayLock((*retval));
	auto pData = static_cast<long*>((*retval)->pvData);
	for(size_t i = 0; i < _indexes.size(); i++)
	{
		pData[i] = _indexes[i];
	}
	SafeArrayUnlock((*retval));

	return S_OK;
}


// *****************************************************************
//		GetIndexes()
// *****************************************************************
STDMETHODIMP CPlacedLabels::SetVector(int* indexes, const int length)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	for(int i = 0; i < length; i++)
	{
		_indexes.push_back(indexes[i]);
	}

	return S_OK;
}