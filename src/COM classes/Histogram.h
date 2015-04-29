#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CHistogram :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CHistogram, &CLSID_Histogram>,
	public IDispatchImpl<IHistogram, &IID_IHistogram, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CHistogram()
	{
		_values = NULL;
		_minValue = 0.0;
		_maxValue = 0.0;
		_numBuckets = 0;
	}

	~CHistogram()
	{
		Clear();
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_HISTOGRAM)

	BEGIN_COM_MAP(CHistogram)
		COM_INTERFACE_ENTRY(IHistogram)
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
	STDMETHOD(get_NumBuckets)(LONG* pVal);
	STDMETHOD(get_MinValue)(DOUBLE* pVal);
	STDMETHOD(get_MaxValue)(DOUBLE* pVal);
	STDMETHOD(get_Count)(LONG bucketIndex, LONG* pVal);
	STDMETHOD(get_Value)(LONG bucketIndex, DOUBLE* pVal);

private:
	GUIntBig* _values;
	double _minValue;
	double _maxValue;
	int _numBuckets;	
	bool _allocatedByGdal;	

public:
	void Inject(int numBuckets, double minValue, double maxValue, GUIntBig* values, bool allocatedByGdal);
	void ErrorMessage(CString msg);
	bool ValidateBucketIndex(LONG bucketIndex);
	void Clear();
};

OBJECT_ENTRY_AUTO(__uuidof(Histogram), CHistogram)
