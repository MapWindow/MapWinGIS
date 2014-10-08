/**************************************************************************************
 * File name: GlobalSettings.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Implementation of CGlobalSettings
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
 // Sergei Leschinski (lsu) 08 aug 2011 - created the file.

#include "stdafx.h"
#include "GlobalSettings.h"

// ****************************************************
//	    get_MinPolygonArea()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_MinPolygonArea(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = m_globalSettings.minPolygonArea;
	return S_OK;
}

// ****************************************************
//	    put_MinPolygonArea()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_MinPolygonArea(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_globalSettings.minPolygonArea = newVal;
	return S_OK;
}

// ****************************************************
//	    get_MinAreaToPerimeterRatio()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_MinAreaToPerimeterRatio(double* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = m_globalSettings.minAreaToPerimeterRatio;
	return S_OK;
}

// ****************************************************
//	    put_MinAreaToPerimeterRatio()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_MinAreaToPerimeterRatio(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_globalSettings.minAreaToPerimeterRatio = newVal;
	return S_OK;
}

// ****************************************************
//	    get_ClipperGcsMultiplicationFactor()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_ClipperGcsMultiplicationFactor(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.clipperGcsMultiplicationFactor;
	return S_OK;
}

// ****************************************************
//	    put_ClipperGcsMultiplicationFactor()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_ClipperGcsMultiplicationFactor(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.clipperGcsMultiplicationFactor = newVal;
	return S_OK;
}

// ****************************************************
//	    CreatePoint()
// ****************************************************
STDMETHODIMP CGlobalSettings::CreatePoint(IPoint** retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_factory.pointFactory->CreateInstance (NULL, IID_IPoint, (void**)retVal);
	return S_OK;
}

// ****************************************************
//	    get_ShapefileFastMode()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_ShapefileFastMode(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.shapefileFastMode ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ****************************************************
//	    put_ShapefileFastMode()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_ShapefileFastMode(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.shapefileFastMode = newVal == VARIANT_FALSE ? false: true;
	return S_OK;
}

// ****************************************************
//	    get_ShapefileFastMode()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_ZoomToFirstLayer(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.zoomToFirstLayer ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGlobalSettings::put_ZoomToFirstLayer(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.zoomToFirstLayer = newVal == VARIANT_FALSE ? false: true;
	return S_OK;
}

// ****************************************************
//	    get_LabelCollisionMode()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_LabelsCollisionMode(tkCollisionMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.labelsCollisionMode;
	return S_OK;
}

STDMETHODIMP CGlobalSettings::put_LabelsCollisionMode(tkCollisionMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.labelsCollisionMode = newVal;
	return S_OK;
}


// ****************************************************
//	    get_InvalidShapesBufferDistance()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_InvalidShapesBufferDistance(DOUBLE* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.invalidShapesBufferDistance;
	return S_OK;
}

// ****************************************************
//	    put_InvalidShapesBufferDistance()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_InvalidShapesBufferDistance(DOUBLE newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.invalidShapesBufferDistance = newVal;
	return S_OK;
}

// ****************************************************
//	    get_ShapefileFastUnion()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_ShapefileFastUnion(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.shapefileFastUnion ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

// ****************************************************
//	    put_ShapefileFastUnion()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_ShapefileFastUnion(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.shapefileFastUnion = newVal?true:false;
	return S_OK;
}

#pragma region "Gdal error handling"
// ****************************************************
//	    get_GdalLastErrorNo()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_GdalLastErrorNo(tkGdalError* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int err = CPLGetLastErrorNo();
	if (err < 0 || err >= (int)GdalErrorUndefined)
		err = (int)GdalErrorUndefined;
	*pVal = (tkGdalError)err;
	return S_OK;
}

// ****************************************************
//	    get_GdalLastErrorType()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_GdalLastErrorType(tkGdalErrorType* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int err = CPLGetLastErrorType();
	if (err < 0 || err >= (int)GdalUndefined)
		err = (int)GdalUndefined;
	*pVal = (tkGdalErrorType)err;
	return S_OK;
}

// ****************************************************
//	    get_GdalLastErrorMsg()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_GdalLastErrorMsg(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString s = CPLGetLastErrorMsg();
	*pVal = A2BSTR(s);
	return S_OK;
}

// ****************************************************
//	    get_GdalReprojectionErrorMsg()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_GdalReprojectionErrorMsg(BSTR* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = A2BSTR(m_globalSettings.gdalErrorMessage);
	return S_OK;
}

// ****************************************************
//	    ResetGdalError()
// ****************************************************
STDMETHODIMP CGlobalSettings::ResetGdalError(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPLErrorReset();
	m_globalSettings.gdalErrorMessage = "";
	return S_OK;
}
#pragma endregion

#pragma region Labels

// ****************************************************
//	    get_CompositingQuality()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_LabelsCompositingQuality(tkCompositingQuality* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.labelsCompositingQuality;
	return S_OK;
}

// ****************************************************
//	    put_CompositingQuality()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_LabelsCompositingQuality(tkCompositingQuality newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0 && newVal <= AssumeLinear)
		m_globalSettings.labelsCompositingQuality = newVal;
	return S_OK;
}

// ****************************************************
//	    get_SmoothingMode()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_LabelsSmoothingMode(tkSmoothingMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.labelsSmoothingMode;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_LabelsSmoothingMode(tkSmoothingMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (newVal >= 0 && newVal <= AntiAlias)
		m_globalSettings.labelsSmoothingMode = newVal;
	return S_OK;
}
#pragma endregion

// ****************************************************
//	    LocalizedString()
// ****************************************************
STDMETHODIMP CGlobalSettings::put_LocalizedString(tkLocalizedStrings unit, BSTR localizedString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	USES_CONVERSION;
	m_globalSettings.shortUnitStrings[unit] = OLE2W(localizedString);
	return S_OK;
}
STDMETHODIMP CGlobalSettings::get_LocalizedString(tkLocalizedStrings unit, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CStringW s = m_globalSettings.GetLocalizedString(unit);
	USES_CONVERSION;
	*retVal = W2BSTR(s);
	return S_OK;
}

// ****************************************************
//	    GridProxyFormat()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_GridProxyFormat(tkGridProxyFormat* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.gridProxyFormat;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_GridProxyFormat(tkGridProxyFormat newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.gridProxyFormat = newVal;
	return S_OK;
}

// ****************************************************
//	    MaxDirectGridSizeMb()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_MaxDirectGridSizeMb(double* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.MaxDirectGridSizeMb;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_MaxDirectGridSizeMb(double newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.MaxDirectGridSizeMb = newVal;
	return S_OK;
}

// ****************************************************
//	    get_AlwaysCreateProxyForGrids()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_GridProxyMode(tkGridProxyMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.gridProxyMode;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_GridProxyMode(tkGridProxyMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.gridProxyMode = newVal;
	return S_OK;
}

// ****************************************************
//	    get_MaxUniqueValuesCountForGridScheme()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_MaxUniqueValuesCountForGridScheme(int* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.maxUniqueValuesCount;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_MaxUniqueValuesCountForGridScheme(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.maxUniqueValuesCount = newVal;
	return S_OK;
}

// ****************************************************
//	    RandomColorSchemeForGrids()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_RandomColorSchemeForGrids(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.randomColorSchemeForGrids ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_RandomColorSchemeForGrids(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.randomColorSchemeForGrids = newVal ? true: false;
	return S_OK;
}
	
// ****************************************************
//	    DefaultColorSchemeForGrids()
// ****************************************************
STDMETHODIMP CGlobalSettings::get_DefaultColorSchemeForGrids(PredefinedColorScheme* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.defaultColorSchemeForGrids;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_DefaultColorSchemeForGrids(PredefinedColorScheme newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.defaultColorSchemeForGrids = newVal;
	return S_OK;
}

// ****************************************************
//	    ShapeOutputValidationMode
// ****************************************************
STDMETHODIMP CGlobalSettings::get_ShapeOutputValidationMode(tkShapeValidationMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.outputValidation;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_ShapeOutputValidationMode(tkShapeValidationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.outputValidation = newVal;
	return S_OK;
}

// ****************************************************
//	    ShapeInputValidationMode
// ****************************************************
STDMETHODIMP CGlobalSettings::get_ShapeInputValidationMode(tkShapeValidationMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.inputValidation;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_ShapeInputValidationMode(tkShapeValidationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.inputValidation = newVal;
	return S_OK;
}

// ****************************************************
//	    GeometryEngine
// ****************************************************
STDMETHODIMP CGlobalSettings::get_GeometryEngine(tkGeometryEngine* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.geometryEngine;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_GeometryEngine(tkGeometryEngine newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.geometryEngine = newVal;
	return S_OK;
}

// ****************************************************
//	    SaveGridColorSchemeToFile
// ****************************************************
STDMETHODIMP CGlobalSettings::get_SaveGridColorSchemeToFile( VARIANT_BOOL *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_globalSettings.saveGridColorSchemeToFile ? VARIANT_TRUE: VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CGlobalSettings::put_SaveGridColorSchemeToFile( VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.saveGridColorSchemeToFile = newVal ? true : false;
	return S_OK;
}

// ****************************************************
//	    TiffCompression
// ****************************************************
STDMETHODIMP CGlobalSettings::get_TiffCompression( tkTiffCompression *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_globalSettings.tiffCompression;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_TiffCompression( tkTiffCompression newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.tiffCompression = newVal;
	return S_OK;
}

// ****************************************************
//	    RasterOverviewCreation
// ****************************************************
STDMETHODIMP CGlobalSettings::get_RasterOverviewCreation( tkRasterOverviewCreation *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_globalSettings.rasterOverviewCreation;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_RasterOverviewCreation( tkRasterOverviewCreation newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.rasterOverviewCreation = newVal;
	return S_OK;
}

// ****************************************************
//	    MinOverviewWidth
// ****************************************************
STDMETHODIMP CGlobalSettings::get_MinOverviewWidth( int *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_globalSettings.minOverviewWidth;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_MinOverviewWidth( int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.minOverviewWidth = newVal;
	return S_OK;
}

// ****************************************************
//	    RasterOverviewResampling
// ****************************************************
STDMETHODIMP CGlobalSettings::get_RasterOverviewResampling( tkGDALResamplingMethod *retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_globalSettings.rasterOverviewResampling;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_RasterOverviewResampling( tkGDALResamplingMethod newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.rasterOverviewResampling = newVal;
	return S_OK;
}

// *********************************************************
//	     TilesThreadPoolSize
// *********************************************************
STDMETHODIMP CGlobalSettings::get_TilesThreadPoolSize(int* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.tilesThreadPoolSize;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_TilesThreadPoolSize(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.tilesThreadPoolSize = newVal;
	return S_OK;
}

// *********************************************************
//	     LoadSymbologyOnAddLayer
// *********************************************************
STDMETHODIMP CGlobalSettings::get_LoadSymbologyOnAddLayer(VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.loadSymbologyOnAddLayer;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_LoadSymbologyOnAddLayer(VARIANT_BOOL newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.loadSymbologyOnAddLayer = newVal ? true: false;
	return S_OK;
}

// *********************************************************
//	     TilesMinZoomOnProjectionMismatch
// *********************************************************
STDMETHODIMP CGlobalSettings::get_TilesMinZoomOnProjectionMismatch(int* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.tilesMaxZoomOnProjectionMismatch;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_TilesMinZoomOnProjectionMismatch(int newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.tilesMaxZoomOnProjectionMismatch = newVal;
	return S_OK;
}


// *********************************************************
//	     ImageUpsamplingMode
// *********************************************************
STDMETHODIMP CGlobalSettings::get_ImageUpsamplingMode(tkInterpolationMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.imageUpsamplingMode;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_ImageUpsamplingMode(tkInterpolationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.imageUpsamplingMode = newVal;
	return S_OK;
}

// *********************************************************
//	     ImageDownsamplingMode
// *********************************************************
STDMETHODIMP CGlobalSettings::get_ImageDownsamplingMode(tkInterpolationMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.imageDownsamplingMode;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_ImageDownsamplingMode(tkInterpolationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.imageDownsamplingMode = newVal;
	return S_OK;
}

// *********************************************************
//	     GridUpsamplingMode
// *********************************************************
STDMETHODIMP CGlobalSettings::get_GridUpsamplingMode(tkInterpolationMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.gridUpsamplingMode;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_GridUpsamplingMode(tkInterpolationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.gridUpsamplingMode = newVal;
	return S_OK;
}

// *********************************************************
//	     GridDownsamplingMode
// *********************************************************
STDMETHODIMP CGlobalSettings::get_GridDownsamplingMode(tkInterpolationMode* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*pVal = m_globalSettings.gridDownsamplingMode;
	return S_OK;
}
STDMETHODIMP CGlobalSettings::put_GridDownsamplingMode(tkInterpolationMode newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.gridDownsamplingMode = newVal;
	return S_OK;
}

// *********************************************************
//	     OgrStringEncoding
// *********************************************************
STDMETHODIMP CGlobalSettings::get_OgrStringEncoding(tkOgrEncoding* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	*retVal = m_globalSettings.ogrEncoding;
	return S_OK;
}

STDMETHODIMP CGlobalSettings::put_OgrStringEncoding(tkOgrEncoding pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_globalSettings.ogrEncoding = pVal;
	return S_OK;
}
