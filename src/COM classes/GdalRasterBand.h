// GdalRasterBand.h : Declaration of the CGdalRasterBand
#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CGdalRasterBand :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CGdalRasterBand, &CLSID_GdalRasterBand>,
	public IDispatchImpl<IGdalRasterBand, &IID_IGdalRasterBand, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGdalRasterBand()
	{
		_band = NULL;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GDALRASTERBAND)


	BEGIN_COM_MAP(CGdalRasterBand)
		COM_INTERFACE_ENTRY(IGdalRasterBand)
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
	STDMETHOD(get_NoDataValue)(DOUBLE* pVal);
	STDMETHOD(get_Minimum)(DOUBLE* pVal);
	STDMETHOD(get_Maximum)(DOUBLE* pVal);
	STDMETHOD(get_OverviewCount)(LONG* pVal);
	STDMETHOD(get_ColorInterpretation)(tkColorInterpretation* pVal);
	STDMETHOD(put_ColorInterpretation)(tkColorInterpretation newVal);
	STDMETHOD(get_DataType)(tkGdalDataType* pVal);
	STDMETHOD(get_XSize)(LONG* pVal);
	STDMETHOD(get_YSize)(LONG* pVal);
	STDMETHOD(get_BlockSizeX)(LONG* pVal);
	STDMETHOD(get_BlockSizeY)(LONG* pVal);
	STDMETHOD(get_UnitType)(BSTR* pVal);
	STDMETHOD(get_Scale)(DOUBLE* pVal);
	STDMETHOD(get_Offset)(DOUBLE* pVal);
	STDMETHOD(get_HasColorTable)(VARIANT_BOOL* pVal);
	STDMETHOD(get_MetadataCount)(LONG* pVal);
	STDMETHOD(get_MetadataItem)(LONG itemIndex, BSTR* pVal);
	STDMETHOD(get_ColorTable)(IGridColorScheme** pVal);
	STDMETHOD(GetUniqueValues)(LONG maxCount, VARIANT* arr, VARIANT_BOOL* result);
	STDMETHOD(GetDefaultHistogram)(VARIANT_BOOL forceCalculate, IHistogram** retVal);
	STDMETHOD(GetHistogram)(DOUBLE minValue, DOUBLE maxValue, LONG numBuckets, VARIANT_BOOL includeOutOfRange, VARIANT_BOOL allowApproximate, IHistogram** retVal);
	STDMETHOD(get_Overview)(LONG bandIndex, IGdalRasterBand** pVal);
	STDMETHOD(GetStatistics)(VARIANT_BOOL allowApproximate, VARIANT_BOOL forceCalculation, DOUBLE* minimum, DOUBLE* maximum, DOUBLE* mean, DOUBLE* stdDev, VARIANT_BOOL* retVal);
	STDMETHOD(GenerateColorScheme)(tkClassificationType classification, LONG numBreaks, IGridColorScheme** retVal);

private:
	GDALRasterBand* _band;

private:
	bool CheckBand();

	template <typename T>
	IGridColorScheme* GenerateUniqueValuesColorScheme(GDALDataType dataType);

public:
	void InjectBand(GDALRasterBand* band) { _band = band; }
	void ErrorMessage(CString msg);
};

OBJECT_ENTRY_AUTO(__uuidof(GdalRasterBand), CGdalRasterBand)
