// Histogram.cpp : Implementation of CHistogram

#include "stdafx.h"
#include "Histogram.h"

// ********************************************************
//	  get_Count
// ********************************************************
void CHistogram::Inject(int numBuckets, double minValue, double maxValue, int* values, bool allocatedByGdal)
{
	_numBuckets = numBuckets;
	_values = values;
	_minValue = minValue;
	_maxValue = maxValue;
	_allocatedByGdal = allocatedByGdal;
}

// *************************************************************
//	  ErrorMessage()
// *************************************************************
void CHistogram::ErrorMessage(CString msg)
{
	CallbackHelper::ErrorMsg("Histogram", NULL, m_globalSettings.emptyBstr, msg);
}

// *************************************************************
//	  Clear()
// *************************************************************
void CHistogram::Clear()
{
	if (_numBuckets != 0)
	{
		if (_allocatedByGdal)
		{
			VSIFree(_values);
		}
		else
		{
			delete[] _values;
		}
	}

	_values = NULL;
	_minValue = 0.0;
	_maxValue = 0.0;
	_numBuckets = 0;
}

// ********************************************************
//	  get_NumBuckets
// ********************************************************
STDMETHODIMP CHistogram::get_NumBuckets(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _numBuckets;

	return S_OK;
}

// ********************************************************
//	  get_MinValue
// ********************************************************
STDMETHODIMP CHistogram::get_MinValue(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _minValue;

	return S_OK;
}

// ********************************************************
//	  get_MaxValue
// ********************************************************
STDMETHODIMP CHistogram::get_MaxValue(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = _maxValue;

	return S_OK;
}

// ********************************************************
//	  get_Count
// ********************************************************
STDMETHODIMP CHistogram::get_Count(LONG bucketIndex, LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!ValidateBucketIndex(bucketIndex))
	{
		*pVal = 0;
		return S_OK;
	}
	
	*pVal = _values[bucketIndex];
	
	return S_OK;
}

// ********************************************************
//	  get_Value
// ********************************************************
STDMETHODIMP CHistogram::get_Value(LONG bucketIndex, DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!ValidateBucketIndex(bucketIndex))
	{
		*pVal = 0.0;
		return S_OK;
	}
		
	*pVal = _minValue + (_maxValue - _minValue) / _numBuckets * bucketIndex;

	return S_OK;
}

// ********************************************************
//	  ValidateBucketIndex
// ********************************************************
bool CHistogram::ValidateBucketIndex(LONG bucketIndex)
{
	if (bucketIndex < 0 || bucketIndex >= _numBuckets)
	{
		ErrorMessage("Bucket index out of bounds.");
		return false;
	}

	return true;
}
