// GdalRasterBand.cpp : Implementation of CGdalRasterBand

#include "stdafx.h"
#include "GdalRasterBand.h"
#include "RasterBandHelper.h"
#include "Templates.h"
#include "Histogram.h"

// *************************************************************
//	  CheckBand()
// *************************************************************
bool CGdalRasterBand::CheckBand()
{
	if (!_band)
	{
		ErrorMessage("GdalRasterBand object isn't initialized.");
		return false;
	}
	return true;
}

// *************************************************************
//	  ErrorMessage()
// *************************************************************
void CGdalRasterBand::ErrorMessage(CString msg)
{
	CallbackHelper::ErrorMsg("GdalRasterBand", NULL, m_globalSettings.emptyBstr, msg);
}

// *************************************************************
//	  get_NoDataValue()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_NoDataValue(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0.0;

	if (!CheckBand()) return S_OK;

	int success;
	*pVal = _band->GetNoDataValue(&success);

	if (!success)
	{
		ErrorMessage("Failed to retrieve no data value.");
	}

	return S_OK;
}

// *************************************************************
//	  get_Minimum()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Minimum(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0.0;

	if (!CheckBand()) return S_OK;

	int success;
	*pVal = _band->GetMinimum(&success);

	if (!success)
	{
		ErrorMessage("Failed to retrieve minimum.");
	}

	return S_OK;
}

// *************************************************************
//	  get_Maximum()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Maximum(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0.0;

	if (!CheckBand()) return S_OK;

	int success;
	*pVal = _band->GetMaximum(&success);

	if (!success)
	{
		ErrorMessage("Failed to retrieve maximum.");
	}

	return S_OK;
}

// *************************************************************
//	  get_OverviewCount()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_OverviewCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = 0;

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetOverviewCount();

	return S_OK;
}

// *************************************************************
//	  get/put_ColorInterpretation()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_ColorInterpretation(tkColorInterpretation* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = (tkColorInterpretation)_band->GetColorInterpretation();

	return S_OK;
}

STDMETHODIMP CGdalRasterBand::put_ColorInterpretation(tkColorInterpretation newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	_band->SetColorInterpretation((GDALColorInterp)newVal);		// error will be returned by GDAL callback

	return S_OK;
}

// *************************************************************
//	  get_DataType()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_DataType(tkGdalDataType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = (tkGdalDataType)_band->GetRasterDataType();

	return S_OK;
}

// *************************************************************
//	  get_XSize()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_XSize(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetXSize();

	return S_OK;
}

// *************************************************************
//	  get_YSize()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_YSize(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetYSize();

	return S_OK;
}

// *************************************************************
//	  get_BlockSizeX()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_BlockSizeX(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (!CheckBand()) return S_OK;

	int x, y;
	 _band->GetBlockSize(&x, &y);

	 *pVal = x;

	return S_OK;
}

// *************************************************************
//	  get_BlockSizeY()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_BlockSizeY(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	int x, y;
	_band->GetBlockSize(&x, &y);

	*pVal = y;

	return S_OK;
}

// *************************************************************
//	  get_UnitType()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_UnitType(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	USES_CONVERSION;
	*pVal = A2BSTR(_band->GetUnitType());

	return S_OK;
}

// *************************************************************
//	  get_Scale()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Scale(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetScale();

	return S_OK;
}

// *************************************************************
//	  get_Offset()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Offset(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	*pVal = _band->GetOffset();

	return S_OK;
}

// *************************************************************
//	  get_HasColorTable()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_HasColorTable(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	GDALColorTable* table = _band->GetColorTable();

	*pVal = table != NULL;

	return S_OK;
}

// *************************************************************
//	  get_MetadataCount()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_MetadataCount(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	char** data = _band->GetMetadata();
	*pVal = CSLCount(data);

	return S_OK;
}

// *************************************************************
//	  get_MetadataItem()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_MetadataItem(LONG itemIndex, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = A2BSTR("");

	if (!CheckBand()) return S_OK;

	char** data = _band->GetMetadata();
	if (itemIndex < 0 || itemIndex >= CSLCount(data))
	{
		ErrorMessage("Metadata item index is out of range.");
		return S_OK;
	}
	else
	{
		char* item = const_cast<char*>(CSLGetField(data, itemIndex));
		*pVal = A2BSTR(item);
		return S_OK;
	}

	return S_OK;
}

// *************************************************************
//	  get_ColorTable()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_ColorTable(IGridColorScheme** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!CheckBand()) return S_OK;

	RasterBandHelper::ColorTableToColorScheme(_band, pVal);

	return S_OK;
}

// *************************************************************
//	  GetUniqueValues()
// *************************************************************
STDMETHODIMP CGdalRasterBand::GetUniqueValues(LONG maxCount, VARIANT* arr, VARIANT_BOOL* result)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*result = VARIANT_FALSE;
	
	if (!CheckBand()) return S_OK;

	GDALDataType dataType = RasterBandHelper::GetSimpleDataType(_band);

	if (dataType == GDT_Unknown)
	{
		ErrorMessage("GdalRasterBand::GetUniqueValues: invalid data type.");
		return S_OK;
	}

	VARIANT_BOOL clipped;
	switch (dataType)
	{
		case GDT_Float64:
		case GDT_Float32:
		{
			set<double> values;
			if (RasterBandHelper::GetUniqueValues(_band, dataType, values, maxCount, &clipped))
			{
				Templates::Set2SafeArray(&values, VT_R8, arr);
				*result = VARIANT_TRUE;
			}
			break;
		}
		case GDT_Int32:
		{
			set<int> values;
			if (RasterBandHelper::GetUniqueValues(_band, dataType, values, maxCount, &clipped))
			{
				Templates::Set2SafeArray(&values, VT_I4, arr);
				*result = VARIANT_TRUE;
			}
			break;
		}
	}

	return S_OK;
}

// *************************************************************
//	  GetDefaultHistogram()
// *************************************************************
STDMETHODIMP CGdalRasterBand::GetDefaultHistogram(VARIANT_BOOL forceCalculate, IHistogram** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = NULL;
	if (!CheckBand()) return S_OK;

	GUIntBig* values = NULL;
	double minValue, maxValue;
	int numBuckets;

	CallbackParams params("Retrieving default histogram");

	CPLErr err = _band->GetDefaultHistogram(&minValue, &maxValue, &numBuckets, &values, forceCalculate ? 1 : 0, GDALProgressCallback, &params);

	CallbackHelper::ProgressCompleted();

	if (err != CPLErr::CE_None)
	{
		CallbackHelper::ErrorMsg("Failed to get default histogram.");
		return S_OK;
	}

	ComHelper::CreateInstance(idHistogram, (IDispatch**)retVal);

	((CHistogram*)*retVal)->Inject(numBuckets, minValue, maxValue, values, true);

	return S_OK;
}

// *************************************************************
//	  GetHistogram()
// *************************************************************
STDMETHODIMP CGdalRasterBand::GetHistogram(DOUBLE minValue, DOUBLE maxValue, LONG numBuckets, VARIANT_BOOL includeOutOfRange, 
							VARIANT_BOOL allowApproximate, IHistogram** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = NULL;
	if (!CheckBand()) return S_OK;

	if (numBuckets <= 0 || numBuckets > 65536)
	{
		ErrorMessage("Invalid number of buckets. Value between 1 and 65536 is expected.");
		return S_OK;
	}

	GUIntBig* values = new GUIntBig[numBuckets];

	CallbackParams params("Calculating histogram");

	CPLErr err = _band->GetHistogram(minValue, maxValue, numBuckets, values, includeOutOfRange ? 1: 0, allowApproximate ? 1 : 0, NULL, NULL);

	CallbackHelper::ProgressCompleted();

	if (err != CPLErr::CE_None)
	{
		delete[] values;
		CallbackHelper::ErrorMsg("Failed to calculate histogram.");
		return S_OK;
	}

	ComHelper::CreateInstance(idHistogram, (IDispatch**)retVal);

	((CHistogram*)*retVal)->Inject(numBuckets, minValue, maxValue, values, false);

	return S_OK;
}

// *************************************************************
//	  get_Overview()
// *************************************************************
STDMETHODIMP CGdalRasterBand::get_Overview(LONG overviewIndex, IGdalRasterBand** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*pVal = NULL;
	if (!CheckBand()) return S_OK;

	int count = _band->GetOverviewCount();

	// overview indices are 0 based
	if (overviewIndex < 0 || overviewIndex >= count)
	{
		ErrorMessage("Overview index is out of bounds.");
		return S_OK;
	}

	GDALRasterBand* overview = _band->GetOverview(overviewIndex);
	if (overview)
	{
		ComHelper::CreateInstance(idGdalRasterBand, (IDispatch**)pVal);
		RasterBandHelper::Cast(*pVal)->InjectBand(overview);
	}

	return S_OK;
}

// ********************************************************
//     GetStatistics
// ********************************************************
STDMETHODIMP CGdalRasterBand::GetStatistics(VARIANT_BOOL allowApproximate, VARIANT_BOOL forceCalculation, 
		DOUBLE* minimum, DOUBLE* maximum, DOUBLE* mean, DOUBLE* stdDev, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!CheckBand()) return S_OK;

	CPLErr err = _band->GetStatistics(allowApproximate ? 1 : 0, forceCalculation ? 1: 0, minimum, maximum, mean, stdDev);

	*retVal = err == CPLErr::CE_None;

	return S_OK;
}

// ********************************************************
//     GenerateColorScheme
// ********************************************************
STDMETHODIMP CGdalRasterBand::Classify(double minValue, double maxValue, tkClassificationType classification, LONG numBreaks, IGridColorScheme** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;

	if (!CheckBand()) return S_OK;

	switch (classification)
	{
		case ctStandardDeviation:
		case ctEqualSumOfValues:
		case ctNaturalBreaks:
			ErrorMessage("ctStandardDeviation and ctEqualSumOfValues aren't supported for raster classification.");
			return S_OK;	
		case ctUniqueValues:
			{
				GDALDataType dataType = RasterBandHelper::GetSimpleDataType(_band);
				if (dataType == GDT_CFloat32 || dataType == GDT_CFloat64)
				{
					*retVal = GenerateUniqueValuesColorScheme<double>(dataType);
				}
				else if (dataType == GDT_Int32)
				{
					*retVal = GenerateUniqueValuesColorScheme<int>(dataType);
				}
			}
			break;
		case ctEqualCount:
			{
				CComPtr<IHistogram> histogram = NULL;
				this->GetHistogram(minValue, maxValue, 512, VARIANT_FALSE, VARIANT_FALSE, &histogram);
				if (histogram)
				{
					histogram->GenerateColorScheme(numBreaks, retVal);
				}
			}
			break;
		case ctEqualIntervals:
			*retVal = GenerateEqualIntervalColorScheme(minValue, maxValue, numBreaks);
			break;
	}

	return S_OK;
}

// ********************************************************
//     GenerateEqualIntervalColorScheme
// ********************************************************
IGridColorScheme* CGdalRasterBand::GenerateEqualIntervalColorScheme(double minValue, double maxValue, int numBreaks)
{
	IGridColorScheme* scheme = NULL;
	ComHelper::CreateInstance(idGridColorScheme, (IDispatch**)&scheme);

	double step = (maxValue - minValue) / numBreaks;
	for (int i = 0; i < numBreaks; i++)
	{
		IGridColorBreak* br = NULL;
		ComHelper::CreateInstance(idGridColorBreak, (IDispatch**)&br);
		br->put_LowValue(minValue + i * step);
		br->put_HighValue(minValue + (i + 1) * step);
		scheme->InsertBreak(br);
	}

	return scheme;
}

// ********************************************************
//     GenerateUniqueValuesColorScheme
// ********************************************************
template <typename T>
IGridColorScheme* CGdalRasterBand::GenerateUniqueValuesColorScheme(GDALDataType dataType)
{
	VARIANT_BOOL clipped = VARIANT_FALSE;

	set<T> values;
	RasterBandHelper::GetUniqueValues(_band, dataType, values, 256, &clipped);
	
	if (clipped) {
		ErrorMessage("Unique value classification is possible for less than 256 values only.");
		return NULL;
	}

	IGridColorScheme* scheme = NULL;
	ComHelper::CreateInstance(idGridColorScheme, (IDispatch**)&scheme);

	set<T>::iterator it = values.begin();
	while (it != values.end())
	{
		IGridColorBreak* br = NULL;
		ComHelper::CreateInstance(idGridColorBreak, (IDispatch**)&br);
		double val = static_cast<double>(*it);
		br->put_LowValue(val);
		br->put_HighValue(val);
		scheme->InsertBreak(br);
		it++;
	}

	return scheme;
}

// ********************************************************
//     ComputeMinMax
// ********************************************************
STDMETHODIMP CGdalRasterBand::ComputeMinMax(VARIANT_BOOL allowApproximate, DOUBLE* minimum, DOUBLE* maximum, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (!CheckBand()) return S_OK;

	double values[2];
	CPLErr result = _band->ComputeRasterMinMax(allowApproximate ? 1 : 0, values);
	*minimum = values[0];
	*maximum = values[1];

	*retVal = result == CE_None ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// ********************************************************
//     get_Value
// ********************************************************
bool CGdalRasterBand::ValidateRowCol(LONG column, LONG row)
{
	int width = _band->GetXSize();
	int height = _band->GetYSize();

	if (column < 0 || row < 0 || column >= width || row >= height)
	{
		ErrorMessage("CGdalRasterBand::get_Value: invalid row or column index.");
		return false;
	}

	return true;
}

// ********************************************************
//     get_Value
// ********************************************************
STDMETHODIMP CGdalRasterBand::get_Value(LONG column, LONG row, double* pVal, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (!CheckBand()) return S_OK;

	if (!ValidateRowCol(column, row)) return S_OK;

	double val;
	CPLErr err = _band->RasterIO(GF_Read, column, row, 1, 1, &val, 1, 1, GDT_Float64, 0, 0);
	*pVal = val;

	*retVal = err == CE_None;

	return S_OK;
}

// ********************************************************
//     GetLocalStatistics
// ********************************************************
STDMETHODIMP CGdalRasterBand::ComputeLocalStatistics(LONG column, LONG row, LONG range, 
		double* minimum, double* maximum, double* mean, double* stdDev, LONG* count, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	// validation
	if (!CheckBand()) return S_OK;

	if (!ValidateRowCol(column, row)) return S_OK;

	if (range < 1 || range > 100)
	{
		ErrorMessage("GdalRasterBand.GetLocalStatistics: invalid range. Expected range is in [1; 100].");
		return S_OK;
	}

	// find the bounds of the region to be read
	long x = column - range;
	long y = row - range;
	long x2 = column + range;
	long y2 = row + range;

	long width = _band->GetXSize();
	long height = _band->GetYSize();

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x2 >= width) x2 = width - 1;
	if (y2 >= height) y2 = height - 1;

	width = x2 - x + 1;
	height = y2 - y + 1;

	// reading values
	double* values = new double[width * height];
	CPLErr err = _band->RasterIO(GF_Read, x, y, width, height, (void*)values, width, height, GDT_Float64, 0, 0);
	
	if (err != CE_None)	{
		goto cleaning;
	}

	*retVal = CalculateStats(values, width * height, *minimum, *maximum, *mean, *stdDev, *count) ? VARIANT_TRUE : VARIANT_FALSE;

cleaning:

	delete[] values;
	return S_OK;
}

// ********************************************************
//     CalculateStats
// ********************************************************
bool CGdalRasterBand::CalculateStats(double* values, int size, double& min, double& max, double& mean, double& stdDev, long& count)
{
	if (size <= 1) return false;

	min = DBL_MAX;
	max = DBL_MIN;

	double noDataValue = _band->GetNoDataValue();

	double squares = 0.0;
	double sum = 0.0;
	count = 0;

	for (int i = 0; i < size; i++)
	{
		if (values[i] == noDataValue) {
			continue;
		}
	
		if (values[i] > max) max = values[i];
		if (values[i] < min) min = values[i];

		sum += values[i];
		squares += values[i] * values[i];
		count++;
	}

	mean = sum / count;
	stdDev = sqrt(squares / count - pow(sum / count, 2));

	return count > 0;
}