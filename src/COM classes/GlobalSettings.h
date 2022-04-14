/**************************************************************************************
 * File name: GlobalSettings.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: Declaration of the CGlobalSettings
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

#pragma once

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CGlobalSettings
class ATL_NO_VTABLE CGlobalSettings :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<CGlobalSettings, &CLSID_GlobalSettings>,
	public IDispatchImpl<IGlobalSettings, &IID_IGlobalSettings, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CGlobalSettings()
	{
		_pUnkMarshaler = NULL;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_GLOBALSETTINGS)

	BEGIN_COM_MAP(CGlobalSettings)
		COM_INTERFACE_ENTRY(IGlobalSettings)
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
	STDMETHOD(get_MinPolygonArea)(double* retVal);
	STDMETHOD(put_MinPolygonArea)(double newVal);
	STDMETHOD(get_MinAreaToPerimeterRatio)(double* retVal);
	STDMETHOD(put_MinAreaToPerimeterRatio)(double newVal);
	STDMETHOD(get_ClipperGcsMultiplicationFactor)(DOUBLE* pVal);
	STDMETHOD(put_ClipperGcsMultiplicationFactor)(DOUBLE newVal);

	STDMETHOD(CreatePoint)(IPoint** retVal);
	STDMETHOD(get_ShapefileFastMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapefileFastMode)(VARIANT_BOOL newVal);
	STDMETHOD(get_InvalidShapesBufferDistance)(DOUBLE* pVal);
	STDMETHOD(put_InvalidShapesBufferDistance)(DOUBLE newVal);
	STDMETHOD(get_ShapefileFastUnion)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ShapefileFastUnion)(VARIANT_BOOL newVal);

	STDMETHOD(get_GdalLastErrorNo)(tkGdalError* pVal);
	STDMETHOD(get_GdalLastErrorType)(tkGdalErrorType* pVal);
	STDMETHOD(get_GdalLastErrorMsg)(BSTR* pVal);
	STDMETHOD(ResetGdalError)(void);
	STDMETHOD(get_GdalReprojectionErrorMsg)(BSTR* pVal);

	STDMETHOD(get_LabelsSmoothingMode)(tkSmoothingMode* pVal);
	STDMETHOD(put_LabelsSmoothingMode)(tkSmoothingMode newVal);
	STDMETHOD(get_LabelsCompositingQuality)(tkCompositingQuality* pVal);
	STDMETHOD(put_LabelsCompositingQuality)(tkCompositingQuality newVal);

	STDMETHOD(put_LocalizedString)(tkLocalizedStrings unit, BSTR localizedString);
	STDMETHOD(get_LocalizedString)(tkLocalizedStrings unit, BSTR* retVal);

	STDMETHOD(put_ZoomToFirstLayer)(VARIANT_BOOL newVal);
	STDMETHOD(get_ZoomToFirstLayer)(VARIANT_BOOL* pVal);

	STDMETHOD(put_LabelsCollisionMode)(tkCollisionMode newVal);
	STDMETHOD(get_LabelsCollisionMode)(tkCollisionMode* pVal);

	STDMETHOD(get_GridProxyFormat)(tkGridProxyFormat* pVal);
	STDMETHOD(put_GridProxyFormat)(tkGridProxyFormat newVal);
	STDMETHOD(get_MaxDirectGridSizeMb)(double* pVal);
	STDMETHOD(put_MaxDirectGridSizeMb)(double newVal);
	STDMETHOD(put_GridProxyMode)(tkGridProxyMode newVal);
	STDMETHOD(get_GridProxyMode)(tkGridProxyMode* pVal);

	STDMETHOD(put_MaxUniqueValuesCountForGridScheme)(int newVal);
	STDMETHOD(get_MaxUniqueValuesCountForGridScheme)(int* pVal);
	STDMETHOD(get_RandomColorSchemeForGrids)( VARIANT_BOOL *retVal);
	STDMETHOD(put_RandomColorSchemeForGrids)( VARIANT_BOOL newVal);
	STDMETHOD(get_DefaultColorSchemeForGrids)( PredefinedColorScheme *retVal);
	STDMETHOD(put_DefaultColorSchemeForGrids)( PredefinedColorScheme newVal);

	STDMETHOD(get_ShapeOutputValidationMode)( tkShapeValidationMode *retVal);
	STDMETHOD(put_ShapeOutputValidationMode)( tkShapeValidationMode newVal);
	STDMETHOD(get_ShapeInputValidationMode)( tkShapeValidationMode *retVal);
	STDMETHOD(put_ShapeInputValidationMode)( tkShapeValidationMode newVal);

	STDMETHOD(get_GeometryEngine)( tkGeometryEngine *retVal);
	STDMETHOD(put_GeometryEngine)( tkGeometryEngine newVal);

	STDMETHOD(get_SaveGridColorSchemeToFile)( VARIANT_BOOL *retVal);
	STDMETHOD(put_SaveGridColorSchemeToFile)( VARIANT_BOOL newVal);
	STDMETHOD(get_TiffCompression)( tkTiffCompression *retVal);
	STDMETHOD(put_TiffCompression)( tkTiffCompression newVal);

	STDMETHOD(get_RasterOverviewCreation)( tkRasterOverviewCreation *retVal);
	STDMETHOD(put_RasterOverviewCreation)( tkRasterOverviewCreation newVal);
	STDMETHOD(get_MinOverviewWidth)( int *retVal);
	STDMETHOD(put_MinOverviewWidth)( int newVal);
	STDMETHOD(get_RasterOverviewResampling)( tkGDALResamplingMethod *retVal);
	STDMETHOD(put_RasterOverviewResampling)( tkGDALResamplingMethod newVal);

	STDMETHOD(put_TilesThreadPoolSize)(int newVal);
	STDMETHOD(get_TilesThreadPoolSize)(int* pVal);

	STDMETHOD(put_LoadSymbologyOnAddLayer)(VARIANT_BOOL newVal);
	STDMETHOD(get_LoadSymbologyOnAddLayer)(VARIANT_BOOL* pVal);
	STDMETHOD(put_TilesMinZoomOnProjectionMismatch)(int newVal);
	STDMETHOD(get_TilesMinZoomOnProjectionMismatch)(int* pVal);
	STDMETHOD(get_ImageUpsamplingMode)(tkInterpolationMode* newVal);
	STDMETHOD(put_ImageUpsamplingMode)(tkInterpolationMode newVal);
	STDMETHOD(get_ImageDownsamplingMode)(tkInterpolationMode* newVal);
	STDMETHOD(put_ImageDownsamplingMode)(tkInterpolationMode newVal);
	STDMETHOD(get_GridUpsamplingMode)(tkInterpolationMode* newVal);
	STDMETHOD(put_GridUpsamplingMode)(tkInterpolationMode newVal);
	STDMETHOD(get_GridDownsamplingMode)(tkInterpolationMode* newVal);
	STDMETHOD(put_GridDownsamplingMode)(tkInterpolationMode newVal);

	STDMETHOD(get_OgrStringEncoding)(tkOgrEncoding* newVal);
	STDMETHOD(put_OgrStringEncoding)(tkOgrEncoding pVal);
	STDMETHOD(get_OgrLayerMaxFeatureCount)(LONG* pVal);
	STDMETHOD(put_OgrLayerMaxFeatureCount)(LONG newVal);
	STDMETHOD(get_AutoChooseOgrLoadingMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_AutoChooseOgrLoadingMode)(VARIANT_BOOL newVal);

	STDMETHOD(get_AttachMapCallbackToLayers)(VARIANT_BOOL* pVal);
	STDMETHOD(put_AttachMapCallbackToLayers)(VARIANT_BOOL newVal);
	STDMETHOD(get_HotTrackingMaxShapeCount)(LONG* pVal);
	STDMETHOD(put_HotTrackingMaxShapeCount)(LONG newVal);
	STDMETHOD(get_AllowLayersWithoutProjections)(VARIANT_BOOL* pVal);
	STDMETHOD(put_AllowLayersWithoutProjections)(VARIANT_BOOL newVal);
	STDMETHOD(get_AllowProjectionMismatch)(VARIANT_BOOL* pVal);
	STDMETHOD(put_AllowProjectionMismatch)(VARIANT_BOOL newVal);
	STDMETHOD(get_ReprojectLayersOnAdding)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ReprojectLayersOnAdding)(VARIANT_BOOL newVal);
	STDMETHOD(get_MouseTolerance)(DOUBLE* pVal);
	STDMETHOD(put_MouseTolerance)(DOUBLE newVal);

	STDMETHOD(get_ApplicationCallback)(ICallback** pVal);
	STDMETHOD(put_ApplicationCallback)(ICallback* newVal);
	STDMETHOD(get_MaxReprojectionShapeCount)(LONG* pVal);
	STDMETHOD(put_MaxReprojectionShapeCount)(LONG newVal);
	STDMETHOD(get_PixelOffsetMode)(tkPixelOffsetMode* pVal);
	STDMETHOD(put_PixelOffsetMode)(tkPixelOffsetMode newVal);
	STDMETHOD(get_AutoChooseRenderingHintForLabels)(VARIANT_BOOL* pVal);
	STDMETHOD(put_AutoChooseRenderingHintForLabels)(VARIANT_BOOL newVal);
	STDMETHOD(get_GdalVersion)(BSTR* retVal);
	STDMETHOD(get_OgrLayerForceUpdateMode)(VARIANT_BOOL* pVal);
	STDMETHOD(put_OgrLayerForceUpdateMode)(VARIANT_BOOL newVal);
	STDMETHOD(get_ForceHideLabels)(VARIANT_BOOL* pVal);
	STDMETHOD(put_ForceHideLabels)(VARIANT_BOOL newVal);
	STDMETHOD(get_GdalPluginPath)(BSTR* pVal);
	STDMETHOD(put_GdalPluginPath)(BSTR newVal);
	STDMETHOD(get_GdalDataPath)(BSTR* pVal);
	STDMETHOD(put_GdalDataPath)(BSTR newVal);
	STDMETHOD(get_ProjPath)(BSTR* pVal);
	STDMETHOD(put_ProjPath)(BSTR newVal);

	// Tiles:
	STDMETHOD(get_BingApiKey)(BSTR* pVal);
	STDMETHOD(put_BingApiKey)(BSTR newVal);
	STDMETHOD(TestBingApiKey)(BSTR apiKey, VARIANT_BOOL* retVal);
	STDMETHOD(SetHereMapsApiKey)(BSTR appId, BSTR appCode);
	STDMETHOD(SetHttpUserAgent)(BSTR userAgent);
	STDMETHOD(StartLogTileRequests)(BSTR filename, VARIANT_BOOL errorsOnly, VARIANT_BOOL* retVal);
	STDMETHOD(StopLogTileRequests)();
	STDMETHOD(get_TileLogFilename)(BSTR* retVal);	
	STDMETHOD(get_TileLogIsOpened)(VARIANT_BOOL* retVal);
	STDMETHOD(get_LogTileErrorsOnly)(VARIANT_BOOL* retVal);
	STDMETHOD(put_LogTileErrorsOnly)(VARIANT_BOOL newVal);
	STDMETHOD(get_WmsDiskCaching)(VARIANT_BOOL* pVal);
	STDMETHOD(put_WmsDiskCaching)(VARIANT_BOOL newVal);

	STDMETHOD(get_CompressOverviews)(tkTiffCompression* pVal);
	STDMETHOD(put_CompressOverviews)(tkTiffCompression newVal);
	STDMETHOD(get_GridFavorGreyscale)(VARIANT_BOOL* pVal);
	STDMETHOD(put_GridFavorGreyscale)(VARIANT_BOOL newVal);
	STDMETHOD(get_GridUseHistogram)(VARIANT_BOOL* pVal);
	STDMETHOD(put_GridUseHistogram)(VARIANT_BOOL newVal);

	STDMETHOD(get_OverrideLocalCallback)(VARIANT_BOOL* pVal);
	STDMETHOD(put_OverrideLocalCallback)(VARIANT_BOOL newVal);

	STDMETHOD(get_CacheDbfRecords)(VARIANT_BOOL* pVal);
	STDMETHOD(put_CacheDbfRecords)(VARIANT_BOOL newVal);
	STDMETHOD(get_CacheShapeRenderingData)(VARIANT_BOOL* pVal);
	STDMETHOD(put_CacheShapeRenderingData)(VARIANT_BOOL newVal);

	STDMETHOD(get_CallbackVerbosity)(tkCallbackVerbosity* pVal);
	STDMETHOD(put_CallbackVerbosity)(tkCallbackVerbosity newVal);
	STDMETHOD(get_OgrShareConnection)(VARIANT_BOOL* pVal);
	STDMETHOD(put_OgrShareConnection)(VARIANT_BOOL newVal);
	STDMETHOD(get_OgrInterpretYNStringAsBoolean)(VARIANT_BOOL* pVal);
	STDMETHOD(put_OgrInterpretYNStringAsBoolean)(VARIANT_BOOL newVal);
    STDMETHOD(get_AllowLayersWithIncompleteReprojection)(VARIANT_BOOL* pVal);
    STDMETHOD(put_AllowLayersWithIncompleteReprojection)(VARIANT_BOOL newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(GlobalSettings), CGlobalSettings)
