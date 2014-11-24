/**************************************************************************************
 * File name: Tiles.h
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: declaration of CTiles
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
 // lsu 21 aug 2011 - created the file

#pragma once
#include "TileCore.h"
#include "BaseProvider.h"
#include "TileProviders.h"
#include "TileLoader.h"
using namespace std;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CTiles :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTiles, &CLSID_Tiles>,
	public IDispatchImpl<ITiles, &IID_ITiles, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	#pragma region Constructor/destructor
	CTiles()
	{
		USES_CONVERSION;
		m_key = A2BSTR("");
		m_globalCallback = NULL;
		m_lastErrorCode = tkNO_ERROR;
		
		CoCreateInstance( CLSID_TileProviders, NULL, CLSCTX_INPROC_SERVER, IID_ITileProviders, (void**)&m_providers);
		((CTileProviders*)m_providers)->put_Tiles(this);
		this->SetDefaults();
		_lastZoom = -1;
		_lastProvider = tkTileProvider::ProviderNone;
		
	}

	~CTiles()
	{
		ClearAll();
	}
	
	void ClearAll()
	{
		this->Stop();
		this->Clear();
		if (m_providers)
		{
			m_providers->Release();
			m_providers = NULL;
		}
	}

	void SetDefaults()
	{
		m_provider = ((CTileProviders*)m_providers)->get_Provider((int)tkTileProvider::OpenStreetMap);
		m_visible = true;
		m_gridLinesVisible = false;
		m_doRamCaching = true;
		m_doDiskCaching = false;
		m_useRamCache = true;
		m_useDiskCache = true;
		m_useServer = true;
		m_minScaleToCache = 0;
		m_maxScaleToCache = 100;
		m_projExtentsNeedUpdate = true;
		_scalingRatio = 1.0;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TILES)

	BEGIN_COM_MAP(CTiles)
		COM_INTERFACE_ENTRY(ITiles)
		COM_INTERFACE_ENTRY(IDispatch)
		//COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
	#pragma endregion

public:
	#pragma region "COM interface"
	STDMETHOD(get_LastErrorCode)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ErrorMsg)(/*[in]*/ long ErrorCode, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_GlobalCallback)(/*[out, retval]*/ ICallback * *pVal);
	STDMETHOD(put_GlobalCallback)(/*[in]*/ ICallback * newVal);
	STDMETHOD(get_Key)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Key)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Visible)(VARIANT_BOOL* pVal);
	STDMETHOD(put_Visible)(VARIANT_BOOL newVal);
	STDMETHOD(get_GridLinesVisible)(VARIANT_BOOL* pVal);
	STDMETHOD(put_GridLinesVisible)(VARIANT_BOOL newVal);
	STDMETHOD(get_Provider)(tkTileProvider* pVal);
	STDMETHOD(put_Provider)(tkTileProvider newVal);
	STDMETHOD(get_DoCaching)(tkCacheType type, VARIANT_BOOL* pVal);
	STDMETHOD(put_DoCaching)(tkCacheType type, VARIANT_BOOL newVal);
	STDMETHOD(get_UseCache)(tkCacheType type, VARIANT_BOOL* pVal);
	STDMETHOD(put_UseCache)(tkCacheType type, VARIANT_BOOL newVal);
	STDMETHOD(get_UseServer)(VARIANT_BOOL* pVal);
	STDMETHOD(put_UseServer)(VARIANT_BOOL newVal);
	STDMETHOD(get_MaxCacheSize)(tkCacheType type, double* pVal);
	STDMETHOD(put_MaxCacheSize)(tkCacheType type, double newVal);
	STDMETHOD(get_MinScaleToCache)(int* pVal);
	STDMETHOD(put_MinScaleToCache)(int newVal);
	STDMETHOD(get_MaxScaleToCache)(int* pVal);
	STDMETHOD(put_MaxScaleToCache)(int newVal);
	STDMETHOD(ClearCache)(tkCacheType type);
	STDMETHOD(ClearCache2)(tkCacheType type, tkTileProvider provider, int fromScale = 0, int toScale = 100);
	STDMETHOD(get_CacheSize)(tkCacheType type, double* retVal);
	STDMETHOD(get_CacheSize2)(tkCacheType type, tkTileProvider provider, int scale, double* retVal);
	STDMETHOD(Serialize)(BSTR* retVal);
	STDMETHOD(Deserialize)(BSTR newVal);
	STDMETHOD(SetProxy)(BSTR address, int port, VARIANT_BOOL* retVal);
	STDMETHOD(get_Proxy)(BSTR* retVal);
	STDMETHOD(AutodetectProxy)(VARIANT_BOOL* retVal);
	STDMETHOD(get_DiskCacheFilename)(BSTR* retVal);
	STDMETHOD(put_DiskCacheFilename)(BSTR pVal);
	STDMETHOD(get_Providers)(ITileProviders** retVal);
	STDMETHOD(get_ProviderId)(int* retVal);
	STDMETHOD(put_ProviderId)(int newVal);
	STDMETHOD(GetTilesIndices)(IExtents* boundsDegrees, int zoom, int provider, IExtents** retVal);
	STDMETHOD(Prefetch)(double minLat, double maxLat, double minLng, double maxLng, int zoom, int provider, IStopExecution* stop, LONG* retVal);
	STDMETHOD(Prefetch2)(int minX, int maxX, int minY, int maxY, int zoom, int provider, IStopExecution* stop, LONG* retVal);
	STDMETHOD(get_DiskCacheCount)(int provider, int zoom, int xMin, int xMax, int yMin, int yMax, LONG* retVal);
	STDMETHOD(get_ProviderName)(BSTR* retVal);
	STDMETHOD(CheckConnection)(BSTR url, VARIANT_BOOL* retVal);
	STDMETHOD(GetTileBounds)(int provider, int zoom, int tileX, int tileY, IExtents** retVal);
	STDMETHOD(get_CurrentZoom)(int* retVal);
	STDMETHOD(PrefetchToFolder)(IExtents* ext, int zoom, int providerId, BSTR savePath, BSTR fileExt, IStopExecution* stop, LONG* retVal);
	STDMETHOD(get_PrefetchErrorCount)(int* retVal);
	STDMETHOD(get_PrefetchTotalCount)(int* retVal);
	STDMETHOD(ClearPrefetchErrors)();
	STDMETHOD(StartLogRequests)(BSTR filename, VARIANT_BOOL errorsOnly, VARIANT_BOOL* retVal);
	STDMETHOD(StopLogRequests)();
	STDMETHOD(get_LogFilename)(BSTR* retVal);
	STDMETHOD(get_LogIsOpened)(VARIANT_BOOL* retVal);
	STDMETHOD(get_LogErrorsOnly)(VARIANT_BOOL* retVal);
	STDMETHOD(put_LogErrorsOnly)(VARIANT_BOOL pVal);
	STDMETHOD(get_SleepBeforeRequestTimeout)(long* retVal);
	STDMETHOD(put_SleepBeforeRequestTimeout)(long pVal);
	STDMETHOD(get_MaxZoom)(int* retVal);
	STDMETHOD(get_MinZoom)(int* pVal);
	STDMETHOD(get_ServerProjection)(tkTileProjection* retVal);
	STDMETHOD(get_ProjectionStatus)(tkTilesProjectionStatus* retVal);
	STDMETHOD(get_ScalingRatio)(double* pVal);
	STDMETHOD(put_ScalingRatio)(double newVal);
	STDMETHOD(SetProxyAuthorization)(BSTR username, BSTR password, BSTR domain, VARIANT_BOOL* retVal);
	STDMETHOD(ClearProxyAuthorization)();
	#pragma endregion

private:
	
	long m_lastErrorCode;
	ICallback * m_globalCallback;
	BSTR m_key;

	double _scalingRatio;

	bool m_visible;
	bool m_gridLinesVisible;
	bool m_doRamCaching;
	bool m_doDiskCaching;
	bool m_useRamCache;
	bool m_useDiskCache;
	bool m_useServer;
	
	int m_minScaleToCache;
	int m_maxScaleToCache;
	
	ITileProviders* m_providers;
	
	TileLoader m_tileLoader;
	TileLoader m_prefetchLoader;
	
	Extent m_projExtents;			// extents of the world under current projection; in WGS84 it'll be (-180, 180, -90, 90)
	bool m_projExtentsNeedUpdate;	// do we need to update bounds in m_projExtents on the next request?
	CStringW m_logPath;
	void* mapView;

	// to avoid duplicate consecutive requests
	int _lastZoom;
	int _lastProvider;
	Extent _lastTileExtents;	// in tile coordinates
	Extent _lastMapExtents;

public:
	::CCriticalSection _tilesBufferLock;
	std::vector<TileCore*> m_tiles;
	BaseProvider* m_provider;
	
public:	
	void Init(void* map) {	mapView = map; }
	void MarkUndrawn();
	long PrefetchCore(int minX, int maxX, int minY, int maxY, int zoom, int providerId, 
								  BSTR savePath, BSTR fileExt, IStopExecution* stop);
	
	void AddTileWithCaching(TileCore* tile);
	void AddTileNoCaching(TileCore* tile);
	void AddTileOnlyCaching(TileCore* tile);
	void AddTileToRamCache(TileCore* tile);
	void Clear();
	void LoadTiles(void* mapView, bool isSnapshot = false, CString key = "");
	void LoadTiles(void* mapView, bool isSnapshot, int providerId, CString key = "");
	bool TilesAreInCache(void* mapView, tkTileProvider providerId = ProviderNone);
	bool TilesAreInScreenBuffer(void* mapView);
	bool GetTilesForMap(void* mapView, int providerId, int& xMin, int& xMax, int& yMin, int& yMax, int& zoom);
	bool GetTilesForMap(void* mapView, int& xMin, int& xMax, int& yMin, int& yMax, int& zoom);
	bool DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);
	bool UndrawnTilesExist();
	bool DrawnTilesExist();
	BaseProjection* get_Projection(){return m_provider->Projection;}
	bool ProjectionBounds(BaseProvider* provider, IGeoProjection* mapProjection, IGeoProjection* wgsProjection,tkTransformationMode transformationMode, Extent& retVal);
	void HandleOnTilesLoaded(bool isSnapshot, CString key, bool nothingToLoad);
	void UpdateProjection();
	void Stop() {
		m_tileLoader.Stop(); 
		this->m_visible = false;	// will prevent reloading tiles after remove all layers in map destructor
	}
private:	
	void ErrorMessage(long ErrorCode);
	int ChooseZoom(double xMin, double xMax, double yMin, double yMax, double pixelPerDegree, bool limitByProvider, BaseProvider* provider);
	int ChooseZoom(IExtents* ext, double pixelPerDegree, bool limitByProvider, BaseProvider* provider);
	void getRectangleXY(double xMinD, double xMaxD, double yMinD, double yMaxD, int zoom, CRect &rect, BaseProvider* provider);
	bool ProjectionSupportsWorldWideTransform( IGeoProjection* mapProjection, IGeoProjection* wgs84Projection );
	void SetAuthorization(BSTR userName, BSTR password, BSTR domain);
public:
	
};

OBJECT_ENTRY_AUTO(__uuidof(Tiles), CTiles)
