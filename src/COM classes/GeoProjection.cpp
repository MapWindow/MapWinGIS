/**************************************************************************************
 * File name: GeoProjection.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control)
 * Description: Implementation of the CGeoProjection
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

// ReSharper disable CppTooWideScopeInitStatement
#include "StdAfx.h"
#include "GeoProjection.h"

//// ************************************************************
//		InjectSpatialReference()
// ************************************************************
void CGeoProjection::InjectSpatialReference(const gsl::not_null<OGRSpatialReference*> sr)
{
	if (_projection)
	{
		_projection->Clear();
		OGRSpatialReference::DestroySpatialReference(_projection);
	}
	_projection = sr->Clone();
}

#pragma region "ErrorHandling"
// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CGeoProjection::get_GlobalCallback(ICallback** pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _globalCallback;
	if (_globalCallback != nullptr)
		_globalCallback->AddRef();
	return S_OK;
}

// ************************************************************
//		put_GlobalCallback()
// ************************************************************
STDMETHODIMP CGeoProjection::put_GlobalCallback(ICallback* newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CGeoProjection::get_ErrorMsg(long ErrorCode, BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CGeoProjection::get_LastErrorCode(long* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// **************************************************************
//		ReportOgrError()
// **************************************************************
void CGeoProjection::ReportOgrError(long errorCode, tkCallbackVerbosity verbosity)
{
	if (verbosity < m_globalSettings.callbackVerbosity) return;

	// converting OGRErr code to MapWinGIS error code
	long code = tkNO_ERROR;
	switch (errorCode)
	{
	case OGRERR_CORRUPT_DATA:
		code = tkOGR_CORRUPT_DATA;
		break;
	case OGRERR_NOT_ENOUGH_MEMORY:
		code = tkOGR_NOT_ENOUGH_MEMORY;
		break;
	case OGRERR_UNSUPPORTED_GEOMETRY_TYPE:
		code = tkOGR_UNSUPPORTED_GEOMETRY_TYPE;
		break;
	case OGRERR_UNSUPPORTED_OPERATION:
		code = tkOGR_UNSUPPORTED_OPERATION;
		break;
	case OGRERR_FAILURE:
		code = tkOGR_FAILURE;
		break;
	case OGRERR_UNSUPPORTED_SRS:
		code = tkOGR_UNSUPPORTED_SRS;
		break;
	case OGRERR_INVALID_HANDLE:
		code = tkOGR_INVALID_HANDLE;
		break;
	case OGRERR_NOT_ENOUGH_DATA:
		code = tkOGR_NOT_ENOUGH_DATA;
		break;
	case OGRERR_NON_EXISTING_FEATURE:
		code = tkOGR_NON_EXISTING_FEATURE;
		break;
	default:;
	}

	if (code != tkNO_ERROR)
	{
		ErrorMessage(code);
	}
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CGeoProjection::ErrorMessage(const long errorCode)
{
	_lastErrorCode = errorCode;
	const CString msg = ErrorMsg(_lastErrorCode);
	CallbackHelper::ErrorMsg("GeoProjection", _globalCallback, _key, msg);
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CGeoProjection::get_Key(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CGeoProjection::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

#pragma endregion

#pragma region "Conversion"
// *******************************************************
//		ExportToProj4()
// Use of this function is discouraged. Its behavior in GDAL >= 3 / PROJ >= 6 is
// significantly different from earlier versions. In particular +datum will only
// encode WGS84, NAD27 and NAD83, and +towgs84/+nadgrids terms will be missing most of the time.
// PROJ strings to encode CRS should be considered as a a legacy solution.
// Using a AUTHORITY:CODE or WKT representation is the recommended way.
// *******************************************************
STDMETHODIMP CGeoProjection::ExportToProj4(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const OGR_SRSNode* node = _projection->GetRoot();		// no need to generate GDAL errors, if know that it's empty
	if (!node) {
		*retVal = A2BSTR("");
		return S_OK;
	}

	char* proj = nullptr;
	const OGRErr err = _projection->exportToProj4(&proj);

	if (err == OGRERR_NONE)
	{
		*retVal = A2BSTR(proj);
	}
	else
	{
		ReportOgrError(err);
		*retVal = A2BSTR("");
	}
	if (proj)
		CPLFree(proj);
	return S_OK;
}

// *******************************************************
//		ImportFromProj4()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromProj4(BSTR proj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
		return S_OK;
	}

	const CString str(proj);
	if (str.GetLength() == 0)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	const OGRErr err = _projection->importFromProj4(str);

	*retVal = err == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;
	if (err != OGRERR_NONE)
	{
		ReportOgrError(err, cvAll);
	}

	return S_OK;
}

// *******************************************************
//		Clear()
// *******************************************************
STDMETHODIMP CGeoProjection::Clear(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		this->StopTransform(); // precautionary, prior to clear
		_projection->Clear();
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *******************************************************
//		Clone()
//   GDAL3+ implementation
// *******************************************************
STDMETHODIMP CGeoProjection::Clone(IGeoProjection** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IGeoProjection* gp;
	ComHelper::CreateInstance(idGeoProjection, reinterpret_cast<IDispatch**>(&gp));
	// inject our spatial reference (calls GDAL Clone)
	dynamic_cast<CGeoProjection*>(gp)->InjectSpatialReference(_projection);
	// set into return value
	*retVal = gp;

	return S_OK;
}

// *******************************************************
//		ImportFromESRI()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromESRI(const BSTR proj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		// convert BSTR to CString
		CString strProj(proj);

		// Parameters: papszPrj	NULL terminated list of strings containing the definition.
		char** papszPrj{}; 
		papszPrj = CSLAddString(papszPrj, LPCSTR(strProj));
		const OGRErr err = _projection->importFromESRI(papszPrj);
		// destroy string list
		CSLDestroy(papszPrj);

		*retVal = err == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;
		if (err != OGRERR_NONE)
		{
			ReportOgrError(err);
		}
	}
	return S_OK;
}

// *******************************************************
//		ImportFromEPSG()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromEPSG(const LONG projCode, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		const OGRErr err = _projection->importFromEPSG(projCode);

		*retVal = (err == OGRERR_NONE) ? VARIANT_TRUE : VARIANT_FALSE;
		if (err != OGRERR_NONE)
		{
			ReportOgrError(err);
		}
	}
	return S_OK;
}

// *******************************************************
//		ExportToWKT()
// *******************************************************
STDMETHODIMP CGeoProjection::ExportToWKT(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	const OGR_SRSNode* node = _projection->GetRoot();		// no need to generate GDAL errors, if know that it's empty
	if (!node) 
	{
		*retVal = A2BSTR("");
		return S_OK;
	}

	char* s = nullptr;
	const OGRErr err = _projection->exportToWkt(&s);
	// copy into CString and then free the buffer
	CString proj(s);
	CPLFree(s);

	if (err == OGRERR_NONE)
	{
		*retVal = A2BSTR((LPCSTR)proj);
	}
	else
	{
		ReportOgrError(err);
		*retVal = A2BSTR("");
	}

	return S_OK;
}

// *******************************************************
//		ExportToWktEx()
// Starting with GDAL 3.0, the OGRSpatialReference::exportToWkt() method accepts options
// *******************************************************
STDMETHODIMP CGeoProjection::ExportToWktEx(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const OGR_SRSNode* node = _projection->GetRoot();		// no need to generate GDAL errors, if know that it's empty
	if (!node) 
	{
		*retVal = A2BSTR("");
		return S_OK;
	}

	char* s = nullptr;
	char** papszOptions{};
	// build null-terminated array of strings
	papszOptions = CSLAddString(papszOptions, "FORMAT=WKT2_2019");
	papszOptions = CSLAddString(papszOptions, "MULTILINE=YES");
	const OGRErr err = _projection->exportToWkt(&s, papszOptions);
	// copy into CString and then free the buffer
	CString proj(s);
	CPLFree(s);
	CSLDestroy(papszOptions);

	if (err == OGRERR_NONE)
	{
		*retVal = A2BSTR((LPCSTR)proj);
	}
	else
	{
		ReportOgrError(err);
		*retVal = A2BSTR("");
	}

	return S_OK;
}

// *******************************************************
//		ImportFromWKT()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromWKT(const BSTR proj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		CString strProj(proj);

		OGRErr result = OGRERR_NONE;
		// use newer method importFromWkt(const char*)
		if ((result = _projection->importFromWkt(LPCSTR(strProj))) == OGRERR_NONE)
		{
			// then Validate (https://gdal.org/api/ogrspatialref.html#_CPPv4NK19OGRSpatialReference8ValidateEv)
			result = _projection->Validate();
		}

		*retVal = result == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;
		if (result != OGRERR_NONE)
		{
			ReportOgrError(result);
		}
	}
	return S_OK;
}

// *******************************************************
//		ImportFromAutoDetect()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromAutoDetect(BSTR proj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		const CString s(proj);
		*retVal = VARIANT_FALSE;

		const OGRErr err = _projection->SetFromUserInput(s);

		if (err == OGRERR_NONE)
		{
			// jf: Based on GDAL documentation, I don't think an empty test is necessary
			VARIANT_BOOL empty;
			this->get_IsEmpty(&empty);
			if (empty == VARIANT_FALSE)
				*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// *******************************************************
//		SetWellKnownGeogCS()
// *******************************************************
STDMETHODIMP CGeoProjection::SetWellKnownGeogCS(tkCoordinateSystem newVal, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	// pass on to import function
	return ImportFromEPSG(newVal, retVal);
}
#pragma endregion

#pragma region "Querying"
// *******************************************************
//		get_IsGeographic()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsGeographic(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = (_projection->IsGeographic() == 0) ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		get_IsProjected()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsProjected(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = (_projection->IsProjected() == 0) ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		get_IsLocal()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsLocal(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = (_projection->IsLocal() == 0) ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		get_IsSame()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsSame(IGeoProjection* proj, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = VARIANT_FALSE;

	if (!proj)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	VARIANT_BOOL vb, vb2;
	this->get_IsEmpty(&vb);
	proj->get_IsEmpty(&vb2);
	if (vb || vb2 )
	{
		ErrorMessage(tkPROJECTION_NOT_INITIALIZED);
		return S_OK;
	}

	const OGRSpatialReference* const sr = dynamic_cast<CGeoProjection*>(proj)->get_SpatialReference();
	// use OGRSpatialReference to test same-ness
	*pVal = (_projection->IsSame(sr) == 0) ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		IsSameProjection()
// *******************************************************
bool CGeoProjection::IsSameProjection(OGRCoordinateTransformation* transf, double x, double y, bool projected)
{
	double xNew = x;
	double yNew = y;

	const BOOL res = transf->Transform(1, &xNew, &yNew);
	{
		const int tolerance = projected == true ? 3 : 7;

		// probably it will be needed to think more over the comparison, 
		// let's try an existing function so far
		const double x1 = Utility::FloatRound(xNew, tolerance);
		const double x2 = Utility::FloatRound(x, tolerance);
		const double y1 = Utility::FloatRound(yNew, tolerance);
		const double y2 = Utility::FloatRound(y, tolerance);

		if (x1 == x2 || y1 == y2)
		{
			return true;
		}
	}
	return false;
}

// *******************************************************
//		get_IsSameExt()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsSameExt(IGeoProjection* proj, IExtents* bounds, int numSamplingPoints, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = VARIANT_FALSE;

	if (!proj || !bounds)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	VARIANT_BOOL vb, vb2;
	this->get_IsEmpty(&vb);
	proj->get_IsEmpty(&vb2);
	if (vb || vb2 )
	{
		return S_OK;
	}

	// first let's try standard approach
	this->get_IsSame(proj, pVal);
	if (*pVal == VARIANT_TRUE)
	{
		return S_OK;
	}

	double xMin, xMax, yMin, yMax, zMin, zMax;
	bounds->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

	const gsl::not_null<OGRSpatialReference*> projSource = _projection;
	const OGRSpatialReference* projTarget = dynamic_cast<CGeoProjection*>(proj)->get_SpatialReference();

	OGRCoordinateTransformation* transf = OGRCreateCoordinateTransformation(projSource, projTarget);
	if (!transf)
	{
		*pVal = VARIANT_FALSE;
		return S_OK;
	}

	const bool projected = projSource->IsProjected() != 0;

	*pVal = VARIANT_TRUE;
	double xs[4]{};
	double ys[4]{};

	// bounds first
	xs[0] = xMin;
	ys[0] = yMin;
	xs[1] = xMin;
	ys[1] = yMax;
	xs[2] = xMax;
	ys[2] = yMax;
	xs[3] = xMax;
	ys[3] = yMin;

	// coordinates being tested may be outside bounds of projection; no need to report errors; we only testing after all
	m_globalSettings.suppressGdalErrors = true;

	for (int i = 0; i < 4; i++)
	{
		if (!this->IsSameProjection(transf, gsl::at(xs, i), gsl::at(ys, i), projected))
		{
			*pVal = VARIANT_FALSE;
			break;
		}
	}

	// let's take some more random points
	if (numSamplingPoints < 0)
		numSamplingPoints = 0;
	else if (numSamplingPoints > 100)
		numSamplingPoints = 100;

	if (*pVal != VARIANT_FALSE && numSamplingPoints > 0)
	{
		for (int i = 0; i < numSamplingPoints; i++)
		{
			// random samplings (not really) will be internal points along the diagonal of the extents
			const double x = xMin + ((xMax - xMin) / (static_cast<double>(numSamplingPoints) + 1) * (static_cast<double>(i) + 1));
			const double y = yMin + ((yMax - yMin) / (static_cast<double>(numSamplingPoints) + 1) * (static_cast<double>(i) + 1));

			if (!this->IsSameProjection(transf, x, y, projected))
			{
				*pVal = VARIANT_FALSE;
				break;
			}
		}
	}
	m_globalSettings.suppressGdalErrors = false;

	OGRCoordinateTransformation::DestroyCT(transf);

	return S_OK;
}

// *******************************************************
//		get_IsSameGeogCS()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsSameGeogCS(IGeoProjection* proj, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!proj)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		*pVal = VARIANT_FALSE;
	}
	else
	{
		const OGRSpatialReference* ref = dynamic_cast<CGeoProjection*>(proj)->get_SpatialReference();
		*pVal = (_projection->IsSameGeogCS(ref) == 0) ? VARIANT_FALSE : VARIANT_TRUE;
	}
	return S_OK;
}

// *******************************************************
//		get_InverseFlattening()
// *******************************************************
STDMETHODIMP CGeoProjection::get_InverseFlattening(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OGRErr err = OGRERR_NONE;
	*pVal = _projection->GetInvFlattening(&err);
	if (err != OGRERR_NONE)
	{
		*pVal = 0.0;
	}
	return S_OK;
}

// *******************************************************
//		get_SemiMajor()
// *******************************************************
STDMETHODIMP CGeoProjection::get_SemiMajor(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OGRErr err = OGRERR_NONE;
	*pVal = _projection->GetSemiMajor(&err);
	if (err != OGRERR_NONE)
	{
		*pVal = 0.0;
	}
	return S_OK;
}

// *******************************************************
//		get_SemiMinor()
// *******************************************************
STDMETHODIMP CGeoProjection::get_SemiMinor(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OGRErr err = OGRERR_NONE;
	*pVal = _projection->GetSemiMinor(&err);
	if (err != OGRERR_NONE)
	{
		*pVal = 0.0;
	}
	return S_OK;
}

// *******************************************************
//		get_ProjectionParam()
// *******************************************************
STDMETHODIMP CGeoProjection::get_ProjectionParam(tkProjectionParameter name, double* value, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = VARIANT_FALSE;

	CString s;
	switch (name)
	{
	case LatitudeOfOrigin:
		s = "latitude_of_origin";
		break;
	case LongitudeOfOrigin:
		s = "longitude_of_origin";
		break;
	case CentralMeridian:
		s = "central_meridian";
		break;
	case ScaleFactor:
		s = "scale_factor";
		break;
	case FalseEasting:
		s = "false_easting";
		break;
	case FalseNorthing:
		s = "false_northing";
		break;
	}

	if (s == "")
	{
		return S_OK;
	}

	OGRErr err = OGRERR_NONE;
	const double res = _projection->GetProjParm(s, *value, &err);
	if (err == OGRERR_NONE)
	{
		*value = res;
		*pVal = VARIANT_TRUE;
	}
	else
	{
		*value = 0.0;
	}
	return S_OK;
}

#pragma endregion

STDMETHODIMP CGeoProjection::get_IsEmpty(VARIANT_BOOL* retVal)
{
	// https://gdal.org/api/ogrspatialref.html#_CPPv4NK19OGRSpatialReference7IsEmptyEv
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _projection->IsEmpty() ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// *******************************************************
//		CopyFrom()
// *******************************************************
STDMETHODIMP CGeoProjection::CopyFrom(IGeoProjection* sourceProj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		return S_OK;
	}

	if (!sourceProj)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	VARIANT_BOOL bEmpty;
	if (((sourceProj->get_IsEmpty(&bEmpty)) == S_OK) && bEmpty)
	{
		// source is empty, so clear local
		Clear(retVal);
		return S_OK;
	}

	CComBSTR bstr;
	sourceProj->ExportToWKT(&bstr);
	CW2A w2a(bstr);
	char* prj = LPSTR(w2a);

	const OGRErr err = _projection->importFromWkt(&prj);
	if (err != OGRERR_NONE)
	{
		CallbackHelper::ErrorMsg("Failed to copy projection");
	}

	*retVal = err == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// *******************************************************
//		ReadFromFile()
// *******************************************************
STDMETHODIMP CGeoProjection::ReadFromFile(BSTR filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const CStringW filenameW(filename);

	*retVal = ReadFromFileCore(filenameW, false) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// ************************************************************
//		ReadFromFileEx
// ************************************************************
STDMETHODIMP CGeoProjection::ReadFromFileEx(BSTR filename, VARIANT_BOOL esri, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const CStringW filenameW = OLE2W(filename);

	*retVal = ReadFromFileCore(filenameW, esri ? true : false) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// ************************************************************
//		ReadFromFileCore
// ************************************************************
bool CGeoProjection::ReadFromFileCore(CStringW filename, bool esri)
{
	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		return false;
	}

	if (!Utility::FileExistsW(filename))
		return false;

	OGRErr err = OGRERR_NONE;

	if (esri)
	{
		// importFromEsri requires lines of file as null-terminated list of strings
		char** papszPrj = GdalHelper::ReadFile(filename);
		if (!papszPrj) return false;

		// passing the first string only
		// to keep safe the initial pointer to array
		char* pszWKT = CPLStrdup(*papszPrj);
		// do the import
		err = _projection->importFromESRI(&pszWKT);
		// clean up strings
		CSLDestroy(papszPrj);
		CPLFree(pszWKT);
	}
	else
	{
		// SetFromUserInput requires entire file as a single string
		FILE* prjFile = _wfopen(filename, L"r");
		if (!prjFile) {
			return false;
		}
		// determine file length
		fseek(prjFile, 0L, SEEK_END);
		const int fileLen = ftell(prjFile);
		fseek(prjFile, 0L, SEEK_SET);
		// allocate buffer for file
		vector<char> pszWKT = vector<char>(fileLen, 0);
		// read the file
		fread(pszWKT.data(), sizeof(char), fileLen, prjFile);
		fclose(prjFile);
		// do the import
		err = _projection->SetFromUserInput(pszWKT.data());
		// clean up
		//delete pszWKT;
	}

	if (err != OGRERR_NONE)
	{
		ReportOgrError(err);
		return false;
	}

	return true;
}

// *******************************************************
//		WriteToFile()
// *******************************************************
STDMETHODIMP CGeoProjection::WriteToFile(BSTR filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const CStringW filenameW = OLE2W(filename);

	*retVal = WriteToFileCore(filenameW, false) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// ************************************************************
//		WriteToFileEx
// ************************************************************
STDMETHODIMP CGeoProjection::WriteToFileEx(BSTR filename, VARIANT_BOOL esri, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const CStringW filenameW = OLE2W(filename);

	*retVal = WriteToFileCore(filenameW, esri ? true : false) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

// ************************************************************
//		WriteToFileCore
// ************************************************************
bool CGeoProjection::WriteToFileCore(CStringW filename, bool esri)
{
	if (filename.CompareNoCase(L"") == 0)
		return false;

	FILE* prjFile = _wfopen(filename, L"wb");
	if (!prjFile) {
		return false;
	}

	CString proj;
	CComBSTR bstr;
	if (esri)
	{
		this->ExportToEsri(&bstr);
	}
	else 
	{
		// Use new ExportWktEx 
		this->ExportToWktEx(&bstr);
	}
	// set into CString
	proj = bstr;

	if (proj.GetLength() != 0)
	{
		fputs((LPCSTR)proj, prjFile);
		//fprintf(prjFile, "%s", (LPCSTR)proj);
	}

	fclose(prjFile);
	prjFile = nullptr;

	return true;
}

// *******************************************************
//		get_Name()
// *******************************************************
STDMETHODIMP CGeoProjection::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_projection->IsGeographic())
	{
		this->get_GeogCSName(pVal);
	}
	else if (_projection->IsProjected())
	{
		this->get_ProjectionName(pVal);
	}
	else
	{
		*pVal = A2BSTR("");
	}
	return S_OK;
}

// *******************************************************
//		get_ProjectionName()
// *******************************************************
STDMETHODIMP CGeoProjection::get_ProjectionName(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const char* name = _projection->GetAttrValue("PROJCS");
	if (name)
		*pVal = A2BSTR(name);
	else
		*pVal = A2BSTR("");

	return S_OK;
}

// *******************************************************
//		get_GeogCSName()
// *******************************************************
STDMETHODIMP CGeoProjection::get_GeogCSName(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	const char* name = _projection->GetAttrValue("GEOGCS");
	if (name)
		*pVal = A2BSTR(name);
	else
		*pVal = A2BSTR("");

	return S_OK;
}

// *******************************************************
//		get_GeogCSParam()
// *******************************************************
STDMETHODIMP CGeoProjection::get_GeogCSParam(tkGeogCSParameter name, DOUBLE* pVal, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OGRErr err = OGRERR_NONE;

	switch (name)
	{
	case AngularUnit:
		*pVal = _projection->GetAngularUnits();
		break;
	case PrimeMeridian:
		*pVal = _projection->GetPrimeMeridian();
		break;
	case InverseFlattening:
		*pVal = _projection->GetInvFlattening(&err);
		break;
	case SemiMajor:
		*pVal = _projection->GetSemiMinor(&err);
		break;
	case SemiMinor:
		*pVal = _projection->GetSemiMinor(&err);
		break;
	default:
		*pVal = 0.0;
		err = OGRERR_UNSUPPORTED_OPERATION;
		break;
	}

	*retVal = (err == OGRERR_NONE) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ***********************************************************
//		SetGeographicCS()
// ***********************************************************
STDMETHODIMP CGeoProjection::SetGeographicCS(tkCoordinateSystem coordinateSystem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
#if GDAL_VERSION_MAJOR >= 3
		VARIANT_BOOL bRet = VARIANT_FALSE;
		ImportFromEPSG(coordinateSystem, &bRet);
#else
		_projection->importFromEPSG((int)coordinateSystem);
#endif
	return S_OK;
}

// ***********************************************************
//		SetWgs84Projection()
// ***********************************************************
STDMETHODIMP CGeoProjection::SetWgs84Projection(tkWgs84Projection projection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
#if GDAL_VERSION_MAJOR >= 3
		VARIANT_BOOL bRet = VARIANT_FALSE;
		ImportFromEPSG(projection, &bRet);
#else
		_projection->importFromEPSG((int)projection);
#endif	
	return S_OK;
}

// ***********************************************************
//		SetNad83Projection()
// ***********************************************************
STDMETHODIMP CGeoProjection::SetNad83Projection(tkNad83Projection projection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
#if GDAL_VERSION_MAJOR >= 3
		VARIANT_BOOL bRet = VARIANT_FALSE;
		ImportFromEPSG(projection, &bRet);
#else
		_projection->importFromEPSG((int)projection);
#endif	
	return S_OK;
}

STDMETHODIMP CGeoProjection::get_HasTransformation(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = (_transformation != nullptr) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ***********************************************************
//		StartTransform()
// ***********************************************************
STDMETHODIMP CGeoProjection::StartTransform(IGeoProjection* target, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retval = VARIANT_FALSE;

	if (_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		return S_OK;
	}

	if (!target)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		return S_OK;
	}

	StopTransform();

	VARIANT_BOOL vb1, vb2;
	this->get_IsEmpty(&vb1);
	target->get_IsEmpty(&vb2);
	if (vb1 || vb2 )
	{
		ErrorMessage(tkPROJECTION_NOT_INITIALIZED);
		return S_OK;
	}

	const OGRSpatialReference* projTarget = dynamic_cast<CGeoProjection*>(target)->get_SpatialReference();

	_transformation = OGRCreateCoordinateTransformation(_projection, projTarget);

	if (!_transformation)
	{
		ErrorMessage(tkFAILED_TO_REPROJECT);
		return S_OK;
	}

	*retval = VARIANT_TRUE;

	return S_OK;
}

// ***********************************************************
//		Transform()
// ***********************************************************
STDMETHODIMP CGeoProjection::Transform(double* x, double* y, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!_transformation)
	{
		*retval = VARIANT_FALSE;
		this->ErrorMessage(tkTRANSFORMATION_NOT_INITIALIZED);
	}
	else
	{
		const BOOL res = _transformation->Transform(1, x, y);
		*retval = res ? VARIANT_TRUE : VARIANT_FALSE;
	}
	return S_OK;
}

// ***********************************************************
//		StopTransform()
// ***********************************************************
STDMETHODIMP CGeoProjection::StopTransform()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (_transformation)
	{
		OGRCoordinateTransformation::DestroyCT(_transformation);
		_transformation = nullptr;
	}
	return S_OK;
}

// ************************************************************
//		SetGoogleMercator
// ************************************************************
STDMETHODIMP CGeoProjection::SetGoogleMercator(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: For GDAL3 Shouldn't this not be this->ImportFromEPSG(3857, retVal);

	CComBSTR bstr = L"PROJCS[\"WGS 84 / Pseudo-Mercator\","
		L"GEOGCS[\"WGS 84\","
		L"DATUM[\"WGS_1984\","
		L"SPHEROID[\"WGS 84\",6378137,298.257223563,"
		L"AUTHORITY[\"EPSG\",\"7030\"]],"
		L"AUTHORITY[\"EPSG\",\"6326\"]],"
		L"PRIMEM[\"Greenwich\",0,"
		L"AUTHORITY[\"EPSG\",\"8901\"]],"
		L"UNIT[\"degree\",0.0174532925199433,"
		L"AUTHORITY[\"EPSG\",\"9122\"]],"
		L"AUTHORITY[\"EPSG\",\"4326\"]],"
		L"PROJECTION[\"Mercator_1SP\"],"
		L"PARAMETER[\"central_meridian\",0],"
		L"PARAMETER[\"scale_factor\",1],"
		L"PARAMETER[\"false_easting\",0],"
		L"PARAMETER[\"false_northing\",0],"
		L"UNIT[\"metre\",1,"
		L"AUTHORITY[\"EPSG\",\"9001\"]],"
		L"AXIS[\"X\",EAST],"
		L"AXIS[\"Y\",NORTH],"
		L"EXTENSION[\"PROJ4\",\"+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs\"],"
		L"AUTHORITY[\"EPSG\",\"3857\"]]";

	this->ImportFromWKT(bstr, retVal);

	return S_OK;
}

// ************************************************************
//		SetWgs84
// ************************************************************
STDMETHODIMP CGeoProjection::SetWgs84(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

#if GDAL_VERSION_MAJOR >= 3

	// TODO: Shouldn't this not be this->ImportFromEPSG(4326, retVal);

	// --- new value: SRS_WKT_WGS84_LAT_LONG
	CComBSTR bstr = L"GEOGCS[\"WGS 84\","
		L"DATUM[\"WGS_1984\","
		L"SPHEROID[\"WGS 84\",6378137,298.257223563,"
		L"AUTHORITY[\"EPSG\",\"7030\"]],"
		L"AUTHORITY[\"EPSG\",\"6326\"]],"
		L"PRIMEM[\"Greenwich\",0,"
		L"AUTHORITY[\"EPSG\",\"8901\"]],"
		L"UNIT[\"degree\",0.0174532925199433,"
		L"AUTHORITY[\"EPSG\",\"9122\"]],"
		L"AXIS[\"Latitude\",NORTH],"
		L"AXIS[\"Longitude\",EAST],"
		L"AUTHORITY[\"EPSG\",\"4326\"]]";
#else
	// original value = depricated SRS_WKT_WGS84
	CComBSTR bstr = L"GEOGCS[\"WGS 84\","
		L"DATUM[\"WGS_1984\","
		L"SPHEROID[\"WGS 84\", 6378137, 298.257223563,"
		L"AUTHORITY[\"EPSG\", \"7030\"]],"
		L"AUTHORITY[\"EPSG\", \"6326\"]],"
		L"PRIMEM[\"Greenwich\", 0,"
		L"AUTHORITY[\"EPSG\", \"8901\"]],"
		L"UNIT[\"degree\", 0.01745329251994328,"
		L"AUTHORITY[\"EPSG\", \"9122\"]],"
		L"AUTHORITY[\"EPSG\", \"4326\"]]";
#endif

	this->ImportFromWKT(bstr, retVal);
	return S_OK;
}

// ************************************************************
//		get_IsFrozen
// ************************************************************
STDMETHODIMP CGeoProjection::get_IsFrozen(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = _isFrozen ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ************************************************************
//		TryAutoDetectEpsg
// ************************************************************
STDMETHODIMP CGeoProjection::TryAutoDetectEpsg(int* epsgCode, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*epsgCode = -1;
	if (!_isFrozen) {

		// Copied from https://github.com/OSGeo/gdal/blob/master/frmts/sigdem/sigdemdataset.cpp
		if (_projection->morphFromESRI() != OGRERR_NONE)
		{
			*retVal = VARIANT_FALSE;
			return S_OK;
		}

		//  Set EPSG authority info if possible.
		// https://gdal.org/api/ogrspatialref.html#_CPPv4N19OGRSpatialReference16AutoIdentifyEPSGEv
		//_projection->AutoIdentifyEPSG();	// it changes the underlying object
		if (_projection->AutoIdentifyEPSG() != OGRERR_NONE)
		{
			int nEntries = 0;
			int* panConfidence = nullptr;

			// Try to identify a match between the passed SRS and a related SRS in a catalog.
			// Starting with GDAL 3.0, it relies on PROJ’ proj_identify() function.
			// https://gdal.org/api/ogrspatialref.html?highlight=autoidentifyepsg#_CPPv4NK19OGRSpatialReference11FindMatchesEPPcPiPPi
			OGRSpatialReferenceH* pahSrs = _projection->FindMatches(nullptr, &nEntries, &panConfidence);
			// create a span over the arrays
			const gsl::span<OGRSpatialReferenceH> spahSrs(pahSrs, nEntries);
			const gsl::span<int> spanConfidence(panConfidence, nEntries);
			if (nEntries > 0 && spanConfidence[0] > 80)
			{
				_projection->Release();
				_projection = static_cast<OGRSpatialReference*>(spahSrs[0]);
				CPLFree(pahSrs);
			}
			else
			{
				OSRFreeSRSArray(pahSrs);
			}
			CPLFree(panConfidence);
		}
	}

	if (_projection->IsGeographic())
	{
		*epsgCode = _projection->GetEPSGGeogCS();
	}
	else
	{
		const char* pszAuthName = _projection->GetAuthorityName("PROJCS");
		if (pszAuthName != nullptr && !_strnicmp(pszAuthName, "epsg", 4))
			*epsgCode = atoi(_projection->GetAuthorityCode("PROJCS"));
	}
	*retVal = *epsgCode != -1 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}


// ************************************************************
//		ExportToEsri
// ************************************************************
STDMETHODIMP CGeoProjection::ExportToEsri(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	const OGR_SRSNode* node = _projection->GetRoot();
	if (!node) {
		*retVal = m_globalSettings.CreateEmptyBSTR();
		return S_OK;
	}

	// TODO: Deprecated
	// Since GDAL 3.0, this function has only user-visible effects at exportToWkt() time.
	// It is recommended to use instead exportToWkt(char**, const char* const char*) const with options having FORMAT=WKT1_ESRI.

	// we can't morph the initial instance, 
	// since the reverse operation doesn't return the exact same projection;
	// let's create a copy instead
	char* szProj = nullptr;
	OGRErr err = _projection->exportToWkt(&szProj);
	if (err == OGRERR_NONE)
	{
		OGRSpatialReference* projTemp = reinterpret_cast<OGRSpatialReference*>(OSRNewSpatialReference(nullptr));
		err = projTemp->importFromWkt(szProj);
		CPLFree(szProj);

		if (err == OGRERR_NONE)
		{
			projTemp->morphToESRI();
			const OGRErr err2 = projTemp->exportToWkt(&szProj);

			if (err2 == OGRERR_NONE)
			{
				*retVal = A2BSTR(szProj);
				CPLFree(szProj);
			}
			else
			{
				ReportOgrError(err2);
				*retVal = m_globalSettings.CreateEmptyBSTR();
			}
		}
		// destroy spatial reference
		OGRSpatialReference::DestroySpatialReference(projTemp);
	}

	return S_OK;
}

// ************************************************************
//		get_LinearUnits
// ************************************************************
STDMETHODIMP CGeoProjection::get_LinearUnits(tkUnitsOfMeasure* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	char* s = nullptr;
	const double ratio = _projection->GetLinearUnits(&s);	  // don't free the memory, the string is internal to OGRSpatialReference

	CString units(s);

	if (!ParseLinearUnits(units, *pVal))
	{
		*pVal = _projection->IsGeographic() ? umDecimalDegrees : umMeters;
	}

	return S_OK;
}

// ************************************************************
//		ParseLinearUnits
// ************************************************************
bool CGeoProjection::ParseLinearUnits(CString s, tkUnitsOfMeasure& units)
{
	if (s.GetLength() == 0) return false;

	if (s.CompareNoCase("unknown") == 0)
	{
		return false;
	}
	else if (s.CompareNoCase("meter") == 0 || s.CompareNoCase("metre") == 0 || s.CompareNoCase("meters") == 0 || s.CompareNoCase("metres") == 0)
	{   // jf: based on input on GitHub from ultraTCS, I'm adding the variations of 'meter(s)';
		// although the WKT UNIT references are singular, the plural 'Meters' will be returned
		// by GDAL GetLinearUnits, so we leave that (and the 'metres' variation for completeness)
		units = umMeters;
		return true;
	}
	else if (s.CompareNoCase("degrees") == 0)
	{
		units = umDecimalDegrees;
		return true;
	}
	else if (s.CompareNoCase("feet") == 0 || s.CompareNoCase("us survey foot") == 0 || s.CompareNoCase("foot_us") == 0)
	{
		units = umFeets;
		return true;
	}

	Debug::WriteLine("Unrecognized linear units: %s", s);
	return false;
}