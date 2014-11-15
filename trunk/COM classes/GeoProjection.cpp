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

#include "stdafx.h"
#include "GeoProjection.h"

#pragma region "ErrorHandling"
// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CGeoProjection::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	*pVal = m_globalCallback;
	if( m_globalCallback != NULL )
		m_globalCallback->AddRef();
	return S_OK;
}

// ************************************************************
//		put_GlobalCallback()
// ************************************************************
STDMETHODIMP CGeoProjection::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&m_globalCallback);
	return S_OK;
}

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CGeoProjection::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CGeoProjection::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = m_lastErrorCode;
	m_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CGeoProjection::ErrorMessage(long ErrorCode)
{
	// onverting OGRErr code to MapWinGIS error code
	long code = tkNO_ERROR;
	switch(ErrorCode)
	{
		case OGRERR_CORRUPT_DATA:
			code = tkOGR_CORRUPT_DATA;
         break;
		case OGRERR_NOT_ENOUGH_MEMORY:
			code = OGRERR_NOT_ENOUGH_MEMORY;
         break;
		case OGRERR_UNSUPPORTED_GEOMETRY_TYPE:
			code = OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
         break;
		case OGRERR_UNSUPPORTED_OPERATION:
			code = OGRERR_UNSUPPORTED_OPERATION;
         break;
		case OGRERR_FAILURE:
			code = OGRERR_FAILURE;
         break;
		case OGRERR_UNSUPPORTED_SRS:
			code = OGRERR_UNSUPPORTED_SRS;
         break;
		case OGRERR_INVALID_HANDLE:
			code = OGRERR_INVALID_HANDLE;
         break;
	}
	
	m_lastErrorCode = code;
	if( m_globalCallback != NULL) 
		m_globalCallback->Error(OLE2BSTR(m_key),A2BSTR(ErrorMsg(m_lastErrorCode)));
	return;
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CGeoProjection::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(m_key);
	return S_OK;
}
STDMETHODIMP CGeoProjection::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(m_key);
	m_key = OLE2BSTR(newVal);
	return S_OK;
}

#pragma endregion

#pragma region "Conversion"
// *******************************************************
//		ExportToProj4()
// *******************************************************
STDMETHODIMP CGeoProjection::ExportToProj4(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	char* proj = NULL;
	OGRErr err = m_projection->exportToProj4(&proj);

	if (err == OGRERR_NONE)
	{
		*retVal = A2BSTR(proj);
	}
	else
	{
		ErrorMessage(err);
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
	USES_CONVERSION;
	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		CString str = OLE2CA(proj);
		OGRErr err = m_projection->importFromProj4(str);

		*retVal = err == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;
		if (err != OGRERR_NONE)
		{
			ErrorMessage(err);
		}
	}
	return S_OK;
}

// *******************************************************
//		Clear()
// *******************************************************
STDMETHODIMP CGeoProjection::Clear(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		this->StopTransform();
		m_projection->Clear();
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *******************************************************
//		Clone()
// *******************************************************
STDMETHODIMP CGeoProjection::Clone(IGeoProjection** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = NULL;
	GetUtils()->CreateInstance(tkInterface::idGeoProjection, (IDispatch**)retVal);
	VARIANT_BOOL vb;
	(*retVal)->CopyFrom(this, &vb);
	if (!vb)
	{
		(*retVal)->Release();
		(*retVal) = NULL;
	}
	return S_OK;
}

// *******************************************************
//		ImportFromESRI()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromESRI(BSTR proj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		USES_CONVERSION;
		char* str = OLE2A(proj);

		char** list = new char*[2];
		list[0] = str;
		list[1] = NULL;

		OGRErr err = m_projection->importFromESRI(list);
		// TODO: delete those 2 bytes; GDAL can change the pointer

		*retVal = err == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;
		if (err != OGRERR_NONE)
		{
			ErrorMessage(err);
		}
	}
	return S_OK;
}

// *******************************************************
//		ImportFromEPSG()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromEPSG(LONG projCode, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else if (projCode == 3857)
	{
		SetGoogleMercator(retVal);
		return S_OK;
	}
	else if (projCode == 4326)
	{
		SetWgs84(retVal);
		return S_OK;
	}
	else
	{
		OGRErr err = m_projection->importFromEPSG(projCode);
		
		*retVal = (err == OGRERR_NONE) ? VARIANT_TRUE : VARIANT_FALSE;
		if (err != OGRERR_NONE)
		{
			ErrorMessage(err);
		}
	}
	return S_OK;
}

// *******************************************************
//		ExportToWKT()
// *******************************************************
STDMETHODIMP CGeoProjection::ExportToWKT(BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	char* proj = NULL;
	OGRErr err = m_projection->exportToWkt(&proj);
	if (err == OGRERR_NONE)
	{
		*retVal = A2BSTR(proj);
	}
	else
	{
		ErrorMessage(err);
		*retVal = A2BSTR("");
	}
	if (proj)
		CPLFree(proj);
	return S_OK;
}

// *******************************************************
//		ImportFromWKT()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromWKT(BSTR proj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		USES_CONVERSION;
		char* str = OLE2A(proj);
		
		char** list = new char*[2];
		list[0] = str;
		list[1] = NULL;
		OGRErr err = m_projection->importFromWkt(list);
		// TODO: delete those 2 bytes; GDAL can change the pointer

		*retVal = err == OGRERR_NONE ? VARIANT_TRUE : VARIANT_FALSE;
		if (err != OGRERR_NONE)
		{
			ErrorMessage(err);
		}
	}
	return S_OK;
}

// *******************************************************
//		ImportFromAutoDetect()
// *******************************************************
STDMETHODIMP CGeoProjection::ImportFromAutoDetect(BSTR proj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
	}
	else
	{
		USES_CONVERSION;
		CString s = OLE2A(proj);
		*retVal =VARIANT_FALSE;

		OGRErr err = m_projection->SetFromUserInput(s);
		
		if (err == OGRERR_NONE )
		{
			VARIANT_BOOL empty;
			this->get_IsEmpty(&empty);
			if (!empty)
				*retVal = VARIANT_TRUE;
		}
	}
	return S_OK;
}

// *******************************************************
//		SetWellKnownGeogCS()
// *******************************************************
STDMETHODIMP CGeoProjection::SetWellKnownGeogCS(tkCoordinateSystem newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_projection->importFromEPSG((int)newVal);
	return S_OK;
}
#pragma endregion

#pragma region "Querying"
// *******************************************************
//		get_IsGeographic()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsGeographic(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_projection->IsGeographic() == 0 ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		get_IsProjected()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsProjected(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_projection->IsProjected() == 0 ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		get_IsLocal()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsLocal(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_projection->IsLocal() == 0 ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

// *******************************************************
//		get_IsSame()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsSame(IGeoProjection* proj, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!proj)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		*pVal = VARIANT_FALSE;
	}
	else
	{
		OGRSpatialReference* ref = ((CGeoProjection*)proj)->get_SpatialReference();

		OGRSpatialReference ref1, ref2;
		
		char* s1 = NULL;
		ref->exportToProj4(&s1);
		ref1.importFromProj4(s1);

		char* s2 = NULL;
		m_projection->exportToProj4(&s2);
		ref2.importFromProj4(s2);

		*pVal = ref1.IsSame(&ref2) ? VARIANT_TRUE : VARIANT_FALSE;
		CPLFree(s1);
		CPLFree(s2);
	}
	return S_OK;
}

// *******************************************************
//		IsSameProjection()
// *******************************************************
bool CGeoProjection::IsSameProjection(OGRCoordinateTransformation* transf, double x, double y, bool projected)
{
	double xNew = x;
	double yNew = y;
	
	BOOL res = transf->Transform( 1, &xNew, &yNew);
	{
		int tolerance = projected == true ? 3 : 7;
		
		// probably it will be needed to think more over the comparison, 
		// let's try an existing function so far
		double x1 = Utility::FloatRound(xNew, tolerance);
		double x2 = Utility::FloatRound(x, tolerance);
		double y1 = Utility::FloatRound(yNew, tolerance);
		double y2 = Utility::FloatRound(y, tolerance);

		if ( x1 == x2 || y1 == y2)
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
	if (!proj || !bounds)
	{
		ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
		*pVal = VARIANT_FALSE;
		return S_FALSE;
	}
	
	// first let's try standard aproach
	this->get_IsSame(proj, pVal);
	if (*pVal == VARIANT_TRUE)
	{
		return S_OK;
	}

	double xMin, xMax, yMin, yMax, zMin, zMax;
	bounds->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

	OGRSpatialReference* projSource = m_projection;
	OGRSpatialReference* projTarget = ((CGeoProjection*)proj)->get_SpatialReference();

	OGRCoordinateTransformation* transf = OGRCreateCoordinateTransformation( projSource, projTarget );
	if (!transf)
	{
		ErrorMessage(tkFAILED_TO_REPROJECT);
		*pVal = VARIANT_FALSE;
	}
	else
	{
		bool projected = projSource->IsProjected() != 0;
		
		*pVal = VARIANT_TRUE;
		double* xs = new double[4];
		double* ys = new double[4];
		
		// bounds first
		xs[0] = xMin;
		ys[0] = yMin;
		xs[1] = xMin;
		ys[1] = yMax;
		xs[2] = xMax;
		ys[2] = yMax;
		xs[3] = xMax;
		ys[3] = yMin;
		
		for (int i = 0; i< 4; i++)
		{
			if (!this->IsSameProjection(transf, xs[i], ys[i], projected))
			{
				*pVal = VARIANT_FALSE;
				break;
			}
		}
		delete[] xs;
		delete[] ys;

		// let's take some more random points
		if (numSamplingPoints < 0)
			numSamplingPoints = 0;
		else if (numSamplingPoints > 100)
			numSamplingPoints = 100;
		
		if (*pVal != VARIANT_FALSE && numSamplingPoints > 0)
		{
			double dx = xMax - xMin;
			double dy = yMax - yMin;
			
			for (int i =0; i< numSamplingPoints; i++)
			{
				if (!this->IsSameProjection(transf, xMin, yMin, projected))
				{
					*pVal = VARIANT_FALSE;
					break;
				}
			}
		}

		//*pVal is set to VARIANT_TRUE above;
	}
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
		OGRSpatialReference* ref = ((CGeoProjection*)proj)->get_SpatialReference();
		*pVal = (m_projection->IsSameGeogCS(ref) == 0) ? VARIANT_FALSE : VARIANT_TRUE;
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
	*pVal =  m_projection->GetInvFlattening(&err);
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
	*pVal =  m_projection->GetSemiMajor(&err);
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
	*pVal =  m_projection->GetSemiMinor(&err);
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
	switch(name)
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
	
	if (s = "")
	{
		return S_OK;
	}

	OGRErr err= OGRERR_NONE;
	double res = m_projection->GetProjParm(s, *value, &err);
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

// *******************************************************
//		get_IsEmpty()
// *******************************************************
STDMETHODIMP CGeoProjection::get_IsEmpty(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	OGR_SRSNode* node = m_projection->GetRoot();
	if (node)
	{
		const char* name = m_projection->GetAttrValue("GEOGCS");
		*retVal = name ? VARIANT_FALSE  : VARIANT_TRUE;
	}
	else
	{
		*retVal = VARIANT_TRUE;
	}
	return S_OK;
}

// *******************************************************
//		CopyFrom()
// *******************************************************
STDMETHODIMP CGeoProjection::CopyFrom(IGeoProjection* sourceProj, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retVal = VARIANT_FALSE;
		return S_FALSE;
	}
	else
	{
		if (!sourceProj)
		{
			ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
			*retVal = VARIANT_FALSE;
			return S_FALSE;
		}
		else
		{
			BSTR s;
			sourceProj->ExportToWKT(&s);
			USES_CONVERSION;
			char* prj = OLE2A(s);
			m_projection->importFromWkt(&prj);
			*retVal = VARIANT_TRUE;
			return S_OK;
		}		
	}
}

// *******************************************************
//		ReadFromFile()
// *******************************************************
STDMETHODIMP CGeoProjection::ReadFromFile(BSTR filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = VARIANT_FALSE;

	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		return S_FALSE;
	}
	else
	{
		USES_CONVERSION;
		CString name = OLE2A(filename);		// TODO!!!: use Unicode

		if (!Utility::fileExists(name))
		{
			return S_FALSE;
		}
		else
		{
			char** papszPrj = CSLLoad(name);
			if (papszPrj == NULL)
				return S_FALSE;

			// passing the first string only
			// to keep safe the initial pointer to array
			char* pszWKT = CPLStrdup(papszPrj[0]);

			OGRErr err = m_projection->SetFromUserInput(pszWKT);
		
			CSLDestroy(papszPrj);
			CPLFree(pszWKT);

			if (err != OGRERR_NONE)
			{
				ErrorMessage(err);
				return S_FALSE;
			}

			*retVal  = VARIANT_TRUE;
			return S_OK;
		}
	}
}

// *******************************************************
//		WriteToFile()
// *******************************************************
STDMETHODIMP CGeoProjection::WriteToFile(BSTR filename, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	*retVal = VARIANT_FALSE;

	USES_CONVERSION;
	CString name = OLE2CA(filename);

	if (name.CompareNoCase("") == 0)
		return S_FALSE;
	
	FILE * prjFile = fopen(name, "wb");
	if (prjFile)
	{
		char* proj = NULL;
		m_projection->exportToWkt(&proj);
		if (proj != NULL && strlen(proj) > 0)
		{
			fprintf(prjFile, "%s", proj);
			CPLFree(proj);
		}

		fclose(prjFile);
		prjFile = NULL;
		
		*retVal  = VARIANT_TRUE;
		return S_OK;
	}
	else
		return S_FALSE;
}

// *******************************************************
//		get_Name()
// *******************************************************
STDMETHODIMP CGeoProjection::get_Name(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_projection->IsGeographic())
	{
		 this->get_GeogCSName(pVal);
	}
	else if (m_projection->IsProjected())
	{
		this->get_ProjectionName(pVal);
	}
	else
	{
		USES_CONVERSION;
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
	USES_CONVERSION;
	
	const char* name = m_projection->GetAttrValue("PROJCS");
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
	USES_CONVERSION;
	
	const char* name = m_projection->GetAttrValue("GEOGCS");
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
	
	switch(name)
	{
		case AngularUnit:
			*pVal =  m_projection->GetAngularUnits();
			break;
		case PrimeMeridian:
			*pVal =  m_projection->GetPrimeMeridian();
			break;
		case InverseFlattening:
			*pVal =  m_projection->GetInvFlattening(&err);
			break;
		case SemiMajor:
			*pVal =  m_projection->GetSemiMinor(&err);
			break;
		case SemiMinor:
			*pVal =  m_projection->GetSemiMinor(&err);
			break;
		default:
			*pVal = 0.0;
			err = OGRERR_UNSUPPORTED_OPERATION;
			break;
	}
	
	*retVal = (err == OGRERR_NONE)? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ***********************************************************
//		SetGeographicCS()
// ***********************************************************
STDMETHODIMP CGeoProjection::SetGeographicCS(tkCoordinateSystem coordinateSystem)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_projection->importFromEPSG((int)coordinateSystem);
	return S_OK;
}

// ***********************************************************
//		SetWgs84Projection()
// ***********************************************************
STDMETHODIMP CGeoProjection::SetWgs84Projection(tkWgs84Projection projection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_projection->importFromEPSG((int)projection);
	return S_OK;
}

// ***********************************************************
//		SetNad83Projection()
// ***********************************************************
STDMETHODIMP CGeoProjection::SetNad83Projection(tkNad83Projection projection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_projection->importFromEPSG((int)projection);
	return S_OK;
}

STDMETHODIMP CGeoProjection::get_HasTransformation(VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retval = m_transformation != NULL;
	return S_OK;
}

// ***********************************************************
//		StartTransform()
// ***********************************************************
STDMETHODIMP CGeoProjection::StartTransform(IGeoProjection* target, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (m_isFrozen)
	{
		ErrorMessage(tkPROJECTION_IS_FROZEN);
		*retval = VARIANT_FALSE;
	}
	else
	{
		*retval = VARIANT_FALSE;

#ifdef _DEBUG
	gMemLeakDetect.stopped = true;
#endif
		if (!target)
		{
			this->ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
			return S_FALSE;
		}
		
		VARIANT_BOOL vbretval;
		target->get_IsEmpty(&vbretval);
		
		if (vbretval)
		{
			this->ErrorMessage(tkPROJECTION_NOT_INITIALIZED);
			return S_FALSE;
		}

		OGRSpatialReference* projTarget = ((CGeoProjection*)target)->get_SpatialReference();

		StopTransform();

		m_transformation = OGRCreateCoordinateTransformation ( m_projection, projTarget );

		if (!m_transformation)
		{
			ErrorMessage(tkFAILED_TO_REPROJECT);
			return S_FALSE;
		}
		else
		{
			*retval = VARIANT_TRUE;
		}
#ifdef _DEBUG
	gMemLeakDetect.stopped = false;
#endif
	}
	return S_OK;
}

// ***********************************************************
//		Transform()
// ***********************************************************
STDMETHODIMP CGeoProjection::Transform(double* x, double* y, VARIANT_BOOL* retval)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!m_transformation)
	{
		*retval = VARIANT_FALSE;
		this->ErrorMessage(tkTRANSFORMATION_NOT_INITIALIZED);
	}
	else
	{
		BOOL res = m_transformation->Transform( 1, x, y);
		*retval = res ? VARIANT_TRUE: VARIANT_FALSE;
	}
	return S_OK;
}

// ***********************************************************
//		StopTransform()
// ***********************************************************
STDMETHODIMP CGeoProjection::StopTransform()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_transformation)
	{
		//delete m_transformation;		// better to do it using Gdal API
		OGRCoordinateTransformation::DestroyCT(m_transformation);
		m_transformation = NULL;
	}
	return S_OK;
}

// ************************************************************
//		SetGoogleMercator
// ************************************************************
STDMETHODIMP CGeoProjection::SetGoogleMercator(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->ImportFromWKT(A2BSTR(
		"PROJCS[\"WGS 84 / Pseudo-Mercator\","
			"GEOGCS[\"WGS 84\","
				"DATUM[\"WGS_1984\","
					"SPHEROID[\"WGS 84\",6378137,298.257223563,"
						"AUTHORITY[\"EPSG\",\"7030\"]],"
					"AUTHORITY[\"EPSG\",\"6326\"]],"
				"PRIMEM[\"Greenwich\",0,"
					"AUTHORITY[\"EPSG\",\"8901\"]],"
				"UNIT[\"degree\",0.0174532925199433,"
					"AUTHORITY[\"EPSG\",\"9122\"]],"
				"AUTHORITY[\"EPSG\",\"4326\"]],"
			"PROJECTION[\"Mercator_1SP\"],"
			"PARAMETER[\"central_meridian\",0],"
			"PARAMETER[\"scale_factor\",1],"
			"PARAMETER[\"false_easting\",0],"
			"PARAMETER[\"false_northing\",0],"
			"UNIT[\"metre\",1,"
				"AUTHORITY[\"EPSG\",\"9001\"]],"
			"AXIS[\"X\",EAST],"
			"AXIS[\"Y\",NORTH],"
			"EXTENSION[\"PROJ4\",\"+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs\"],"
			"AUTHORITY[\"EPSG\",\"3857\"]]"), retVal);

	return S_OK;
}

// ************************************************************
//		SetWgs84
// ************************************************************
STDMETHODIMP CGeoProjection::SetWgs84(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

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

	this->ImportFromWKT(bstr, retVal);
	return S_OK;
}

// ************************************************************
//		get_IsFrozen
// ************************************************************
STDMETHODIMP CGeoProjection::get_IsFrozen(VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_isFrozen ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

// ************************************************************
//		TryAutoDetectEpsg
// ************************************************************
STDMETHODIMP CGeoProjection::TryAutoDetectEpsg(int* epsgCode, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*epsgCode = -1;
	if (!m_isFrozen) {
		m_projection->AutoIdentifyEPSG();	// it changes the underlying object
	}
	if (m_projection->IsGeographic())
	{
		*epsgCode = m_projection->GetEPSGGeogCS();
	}
	else
	{
		const char *pszAuthName = m_projection->GetAuthorityName( "PROJCS" );
		if( pszAuthName != NULL && !_strnicmp(pszAuthName, "epsg", 4))
		    *epsgCode = atoi(m_projection->GetAuthorityCode( "PROJCS" ));
	}
	*retVal = *epsgCode != -1 ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}