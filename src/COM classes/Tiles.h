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
 
#pragma once
#include "TileCore.h"
#include "BaseProvider.h"
#include "TileProviders.h"
#include "TileLoader.h"
#include "WmsCustomProvider.h"
#include "TileManager.h"
using namespace std;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class ATL_NO_VTABLE CTiles :
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<CTiles, &CLSID_Tiles>,
	public IDispatchImpl<ITiles, &IID_ITiles, &LIBID_MapWinGIS, /*wMajor =*/ VERSION_MAJOR, /*wMinor =*/ VERSION_MINOR>
{
public:
	CTiles()
		: _manager(true)
	{
		_pUnkMarshaler = NULL;
		_key = SysAllocString(L"");
		_globalCallback = NULL;
		_lastErrorCode = tkNO_ERROR;
		
		ComHelper::CreateInstance(idWmsProviders, (IDispatch**)&_wmsProviders);
		ComHelper::CreateInstance(idTileProviders, (IDispatch**)&_providers);
		((CTileProviders*)_providers)->put_Tiles(this);

		SetDefaults();

		gReferenceCounter.AddRef(tkInterface::idTiles);
	}

	~CTiles()
	{
		SysFreeString(_key);

		ClearAll();

		gReferenceCounter.Release(tkInterface::idTiles);
	}
	
	void ClearAll()
	{
		Stop();

		_manager.Clear();

		if (_providers)
		{
			_providers->Release();
			_providers = NULL;
		}

		if (_wmsProviders) {
			_wmsProviders->Release();
			_wmsProviders = NULL;
		}
	}

	void SetDefaults()
	{
		_provider = ((CTileProviders*)_providers)->get_Provider((int)tkTileProvider::OpenStreetMap);
		_visible = true;
		_gridLinesVisible = false;
		
		_minScaleToCache = 0;
		_maxScaleToCache = 100;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TILES)

	BEGIN_COM_MAP(CTiles)
		COM_INTERFACE_ENTRY(ITiles)
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
	STDMETHOD(SetProxyAuthentication)(BSTR username, BSTR password, BSTR domain, VARIANT_BOOL* retVal);
	STDMETHOD(ClearProxyAuthorization)();
	STDMETHOD(get_ProxyAuthenticationScheme)(tkProxyAuthentication* pVal);
	STDMETHOD(put_ProxyAuthenticationScheme)(tkProxyAuthentication newVal);
	STDMETHOD(get_WmsProviders)(IWmsProviders** pVal);

private:
	long _lastErrorCode;
	ICallback * _globalCallback;
	BSTR _key;
	
	bool _visible;
	bool _gridLinesVisible;
	int _minScaleToCache;
	int _maxScaleToCache;
	ITileProviders* _providers;
	IWmsProviders* _wmsProviders;
	CStringW _logPath;

public:
	TileManager _manager;
	BaseProvider* _provider;

private:
	void ErrorMessage(long ErrorCode);
	void SetAuthorization(BSTR userName, BSTR password, BSTR domain);
	TileLoader* get_Prefetcher() { return _manager.get_Prefetcher(); }

public:
	// properties
	TileManager* get_Manager() { return &_manager; }
	BaseProvider* get_Provider() { return _provider; }

public:
	void Init(IMapViewCallback* map) {_manager.set_MapCallback(map); }
	long PrefetchCore(int minX, int maxX, int minY, int maxY, int zoom, int providerId, BSTR savePath, BSTR fileExt, IStopExecution* stop);
	void LoadTiles(bool isSnapshot = false, CString key = "");
	void LoadTiles(bool isSnapshot, int providerId, CString key = "");
	bool TilesAreInCache(IMapViewCallback* map, tkTileProvider providerId = ProviderNone);
	bool TilesAreInScreenBuffer(IMapViewCallback* map);
	bool DeserializeCore(CPLXMLNode* node);
	CPLXMLNode* SerializeCore(CString ElementName);
	BaseProjection* get_Projection(){ return _provider->get_Projection(); }
	void Stop();
};

OBJECT_ENTRY_AUTO(__uuidof(Tiles), CTiles)
