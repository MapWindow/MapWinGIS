/**************************************************************************************
 * File name: GeoProjection.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control)
 * Description: Declaration of the CGeoProjection
 *
 **************************************************************************************
 * The contents of this file are subject to the Mozilla Public License Version 1.1
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at http://www.mozilla.org/mpl/
 * See the License for the specific language governing rights and limitations
 * under the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 **************************************************************************************
 * Contributor(s):
 * (Open source contributors should list themselves and their modifications here). */
 // Sergei Leschinski (lsu) 14 may 2011 - created the file.

#pragma once

#include <gsl/gsl>
#define GSL_THROW_ON_CONTRACT_VIOLATION 1

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CGeoProjection
class ATL_NO_VTABLE CGeoProjection :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CGeoProjection, &CLSID_GeoProjection>,
	public IDispatchImpl<IGeoProjection, &IID_IGeoProjection, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGeoProjection()
	{
		_pUnkMarshaler = nullptr;
		_key = A2BSTR("");
		_globalCallback = nullptr;
		_lastErrorCode = tkNO_ERROR;
		_projection = reinterpret_cast<OGRSpatialReference*>(OSRNewSpatialReference(nullptr));
#if GDAL_VERSION_MAJOR >= 3
		// TODO: This should be a temporarily fix??
		_projection->SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
#endif
		_transformation = nullptr;
		gReferenceCounter.AddRef(tkInterface::idGeoProjection);
		_isFrozen = false;
	}

	~CGeoProjection() noexcept(false)
	{
		SysFreeString(_key);
		StopTransform();
		if (_projection)
		{
			_projection->Clear();
			OGRSpatialReference::DestroySpatialReference(_projection);
		}
		gReferenceCounter.Release(tkInterface::idGeoProjection);
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GEOPROJECTION)

	BEGIN_COM_MAP(CGeoProjection)
		COM_INTERFACE_ENTRY(IGeoProjection)
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
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long* pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR* pVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback** pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback* newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR* pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(ImportFromProj4)(BSTR proj, VARIANT_BOOL* retVal);
	STDMETHOD(ImportFromESRI)(BSTR proj, VARIANT_BOOL* retVal);
	STDMETHOD(ImportFromEPSG)(LONG projCode, VARIANT_BOOL* retVal);
	STDMETHOD(ImportFromWKT)(BSTR proj, VARIANT_BOOL* retVal);
	STDMETHOD(ImportFromAutoDetect)(BSTR proj, VARIANT_BOOL* retVal);
	STDMETHOD(ExportToProj4)(BSTR* retVal);
	STDMETHOD(ExportToWKT)(BSTR* retVal);
	STDMETHOD(ExportToWktEx)(BSTR* retVal);
	STDMETHOD(SetWellKnownGeogCS)(tkCoordinateSystem newVal, VARIANT_BOOL* retVal);
	STDMETHOD(get_IsGeographic)(VARIANT_BOOL* pVal);
	STDMETHOD(get_IsProjected)(VARIANT_BOOL* pVal);
	STDMETHOD(get_IsLocal)(VARIANT_BOOL* pVal);
	STDMETHOD(get_IsSame)(IGeoProjection* proj, VARIANT_BOOL* pVal);
	STDMETHOD(get_IsSameGeogCS)(IGeoProjection* proj, VARIANT_BOOL* pVal);
	STDMETHOD(get_InverseFlattening)(DOUBLE* pVal);
	STDMETHOD(get_SemiMajor)(DOUBLE* pVal);
	STDMETHOD(get_SemiMinor)(DOUBLE* pVal);
	STDMETHOD(get_ProjectionParam)(tkProjectionParameter name, double* value, VARIANT_BOOL* pVal);
	STDMETHOD(get_IsEmpty)(VARIANT_BOOL* retVal);
	STDMETHOD(CopyFrom)(IGeoProjection* sourceProj, VARIANT_BOOL* pVal);
	STDMETHOD(get_GeogCSParam)(tkGeogCSParameter name, DOUBLE* pVal, VARIANT_BOOL* retVal);
	STDMETHOD(get_Name)(BSTR* pVal);
	STDMETHOD(get_ProjectionName)(BSTR* pVal);
	STDMETHOD(get_GeogCSName)(BSTR* pVal);
	STDMETHOD(SetGeographicCS)(tkCoordinateSystem coordinateSystem);
	STDMETHOD(SetWgs84Projection)(tkWgs84Projection projection);
	STDMETHOD(SetNad83Projection)(tkNad83Projection projection);
	STDMETHOD(get_IsSameExt)(IGeoProjection* proj, IExtents* bounds, int numSamplingPoints, VARIANT_BOOL* pVal);
	STDMETHOD(ReadFromFile)(BSTR filename, VARIANT_BOOL* retVal);
	STDMETHOD(WriteToFile)(BSTR filename, VARIANT_BOOL* retVal);
	STDMETHOD(StartTransform)(IGeoProjection* target, VARIANT_BOOL* retval);
	STDMETHOD(Transform)(double* x, double* y, VARIANT_BOOL* retval);
	STDMETHOD(StopTransform)();
	STDMETHOD(get_HasTransformation)(VARIANT_BOOL* retval);
	STDMETHOD(SetGoogleMercator)(VARIANT_BOOL* retVal);
	STDMETHOD(SetWgs84)(VARIANT_BOOL* retVal);
	STDMETHOD(Clear)(VARIANT_BOOL* retVal);
	STDMETHOD(Clone)(IGeoProjection** retVal);
	STDMETHOD(get_IsFrozen)(VARIANT_BOOL* retVal);
	STDMETHOD(TryAutoDetectEpsg)(int* epsgCode, VARIANT_BOOL* retVal);
	STDMETHOD(WriteToFileEx)(BSTR filename, VARIANT_BOOL esri, VARIANT_BOOL* retVal);
	STDMETHOD(ReadFromFileEx)(BSTR filename, VARIANT_BOOL esri, VARIANT_BOOL* retVal);
	STDMETHOD(ExportToEsri)(BSTR* retVal);
	STDMETHOD(get_LinearUnits)(tkUnitsOfMeasure* pVal);

private:
	OGRSpatialReference* _projection;
	long _lastErrorCode;
	ICallback* _globalCallback;
	BSTR _key;
	bool _isFrozen;
	OGRCoordinateTransformation* _transformation;

private:
	void ErrorMessage(long errorCode);
	bool IsSameProjection(OGRCoordinateTransformation* transf, double x, double y, bool projected);
	bool ReadFromFileCore(CStringW filename, bool esri);
	bool WriteToFileCore(CStringW filename, bool esri);
	void ReportOgrError(long errorCode, tkCallbackVerbosity verbosity = cvLimited);
	bool ParseLinearUnits(CString s, tkUnitsOfMeasure& units);

public:
	OGRSpatialReference* get_SpatialReference() noexcept { return _projection; }
	void SetIsFrozen(bool frozen) noexcept { _isFrozen = frozen; }
	void InjectSpatialReference(const gsl::not_null<OGRSpatialReference*> sr);
};

OBJECT_ENTRY_AUTO(__uuidof(GeoProjection), CGeoProjection)
