/**************************************************************************************
 * File name: Tiles.cpp
 *
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
 * Description: implementation of CTiles
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
// Paul Meems August 2018: Modernized the code as suggested by CLang and ReSharper

#include "StdAfx.h"
#include "Tiles.h"
#include "DiskCache.h"
#include "TileHelper.h"
#include "CustomTileProvider.h"
#include "TileCacheManager.h"
#include "HttpProxyHelper.h"
#include "SecureHttpClient.h"

// ************************************************************
//		get_Provider()
// ************************************************************
BaseProvider* CTiles::get_Provider(int providerId) const
{
    return ((CTileProviders*)_providers)->get_Provider(providerId);
}

// ************************************************************
//		get_ReloadNeeded()
// ************************************************************
bool CTiles::get_ReloadNeeded()
{
    const bool val = _reloadNeeded;
    _reloadNeeded = false;
    return val;
}

// ************************************************************
//		Stop()
// ************************************************************
void CTiles::Stop()
{
    _manager.get_Loader()->Stop();

    put_Visible(VARIANT_FALSE); // will prevent reloading tiles after remove all layers in map destructor
}

// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CTiles::get_GlobalCallback(ICallback** pVal)
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
STDMETHODIMP CTiles::put_GlobalCallback(ICallback* newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
    return S_OK;
}

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CTiles::get_ErrorMsg(long ErrorCode, BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    USES_CONVERSION;
    *pVal = A2BSTR(ErrorMsg(ErrorCode));

    return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CTiles::get_LastErrorCode(long* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    *pVal = _lastErrorCode;
    _lastErrorCode = tkNO_ERROR;

    return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CTiles::ErrorMessage(long ErrorCode)
{
    _lastErrorCode = ErrorCode;
    CallbackHelper::ErrorMsg("Tiles", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CTiles::get_Key(BSTR* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    USES_CONVERSION;
    *pVal = OLE2BSTR(_key);

    return S_OK;
}

STDMETHODIMP CTiles::put_Key(BSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    SysFreeString(_key);
    _key = OLE2BSTR(newVal);

    return S_OK;
}

// *********************************************************
//	     SleepBeforeRequestTimeout()
// *********************************************************
STDMETHODIMP CTiles::get_DelayRequestTimeout(long* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    *pVal = _manager.get_Loader()->get_DelayRequestTimeout();

    return S_OK;
}

STDMETHODIMP CTiles::put_DelayRequestTimeout(long newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (newVal > 10000) newVal = 10000;
    if (newVal < 0) newVal = 0;

    _manager.get_Loader()->set_DelayRequestTimeout(newVal);

    return S_OK;
}

// *********************************************************
//	     ScalingRatio()
// *********************************************************
STDMETHODIMP CTiles::get_ScalingRatio(double* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    *pVal = _manager.scalingRatio();;
    return S_OK;
}

STDMETHODIMP CTiles::put_ScalingRatio(double newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())

    if (newVal < 0.5 || newVal > 4.0)
    {
        ErrorMessage(tkINVALID_PARAMETER_VALUE);
        return S_OK;
    }

    _manager.scalingRatio(newVal);

    return S_OK;
}

// *********************************************************
//	     AutodetectProxy()
// *********************************************************
STDMETHODIMP CTiles::AutodetectProxy(VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = HttpProxyHelper::AutodetectProxy();
    return S_OK;
}

// *********************************************************
//	     SetProxy()
// *********************************************************
STDMETHODIMP CTiles::SetProxy(BSTR address, int port, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;
    *retVal = HttpProxyHelper::SetProxy(OLE2A(address), port);
    return S_OK;
}

// *********************************************************
//	     SetProxyAuthorization()
// *********************************************************
STDMETHODIMP CTiles::SetProxyAuthentication(BSTR username, BSTR password, BSTR domain, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    USES_CONVERSION;
    *retVal = BaseProvider::SetAuthorization(username, password, domain) ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

// *********************************************************
//	     ClearProxyAuthorization()
// *********************************************************
STDMETHODIMP CTiles::ClearProxyAuthorization()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    BaseProvider::ClearAuthorization();
    return S_OK;
}

// *********************************************************
//	     get_Proxy()
// *********************************************************
STDMETHODIMP CTiles::get_Proxy(BSTR* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    USES_CONVERSION;
    CString s = HttpProxyHelper::m_proxyAddress;
    if (s.GetLength() == 0)
    {
        *retVal = A2BSTR("");
    }
    else
    {
        const CString format = s + ":%d";
        const short num = HttpProxyHelper::m_proxyPort;
        s.Format(format, num);
        *retVal = A2BSTR(s);
    }

    return S_OK;
}

// ************************************************************
//		get_CurrentZoom()
// ************************************************************
STDMETHODIMP CTiles::get_CurrentZoom(int* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = -1;

    IMapViewCallback* map = _manager.get_MapCallback();
    if (map)
    {
        *retVal = map->_ChooseZoom(_provider, _manager.scalingRatio(), false);
    }

    return S_OK;
}

// *********************************************************
//	     TilesAreInCache()
// *********************************************************
// checks whether all the tiles are present in cache
bool CTiles::TilesAreInCache(IMapViewCallback* map, tkTileProvider providerId)
{
    BaseProvider* provider = providerId == ProviderNone ? _provider : get_Provider(providerId);

    if (!_visible || !provider)
    {
        // there is no valid provider, so no need to schedule download
        return true;
    }

    return _manager.TilesAreInCache(provider);
}

// *********************************************************
//	     LoadTiles
// *********************************************************
void CTiles::LoadTiles(bool isSnapshot, const CString& key)
{
    if (!_visible)
    {
        _manager.Clear();
        return;
    }

    _manager.LoadTiles(_provider, isSnapshot, key);
}

// *********************************************************
//	     Provider
// *********************************************************
STDMETHODIMP CTiles::get_Provider(tkTileProvider* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    auto* p = dynamic_cast<CustomTileProvider*>(_provider);
    *pVal = p ? tkTileProvider::ProviderCustom : (tkTileProvider)_provider->Id;

    return S_OK;
}

STDMETHODIMP CTiles::put_Provider(tkTileProvider newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (newVal < 0 || newVal >= tkTileProvider::ProviderCustom)
    {
        return S_OK;
    }

    if (_provider->Id != newVal && newVal != tkTileProvider::ProviderCustom)
    {
        put_ProviderId((int)newVal);
    }

    return S_OK;
}

// *********************************************************
//	     get_ProviderName
// *********************************************************
STDMETHODIMP CTiles::get_ProviderName(BSTR* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    USES_CONVERSION;
    *retVal = _provider ? A2BSTR(_provider->Name) : A2BSTR("");

    return S_OK;
}

// *********************************************************
//	     GridLinesVisible
// *********************************************************
STDMETHODIMP CTiles::get_GridLinesVisible(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _manager.get_GridLinesVisible() ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CTiles::put_GridLinesVisible(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _manager.set_GridLinesVisible(newVal ? true : false);
    return S_OK;
}

// *********************************************************
//	     MinScaleToCache
// *********************************************************
STDMETHODIMP CTiles::get_MinScaleToCache(int* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _minScaleToCache; // TODO: use in caching process
    return S_OK;
}

STDMETHODIMP CTiles::put_MinScaleToCache(int newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _minScaleToCache = newVal;
    return S_OK;
}

// *********************************************************
//	     MaxScaleToCache
// *********************************************************
STDMETHODIMP CTiles::get_MaxScaleToCache(int* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _maxScaleToCache; // TODO: use in caching process
    return S_OK;
}

STDMETHODIMP CTiles::put_MaxScaleToCache(int newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _maxScaleToCache = newVal;
    return S_OK;
}

// *********************************************************
//	     Visible
// *********************************************************
STDMETHODIMP CTiles::get_Visible(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _visible;
    return S_OK;
}

STDMETHODIMP CTiles::put_Visible(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _visible = newVal != 0;
    return S_OK;
}

// *********************************************************
//	     CustomProviders
// *********************************************************
STDMETHODIMP CTiles::get_Providers(ITileProviders** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (_providers)
        _providers->AddRef();
    *retVal = _providers;
    return S_OK;
}

// *********************************************************
//	     DoCaching
// *********************************************************
STDMETHODIMP CTiles::get_DoCaching(tkCacheType type, VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *pVal = _manager.get_Cache(type)->doCaching ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CTiles::put_DoCaching(tkCacheType type, VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    _manager.get_Cache(type)->doCaching = newVal ? true : false;

    return S_OK;
}

// *********************************************************
//	     UseCache
// *********************************************************
STDMETHODIMP CTiles::get_UseCache(tkCacheType type, VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *pVal = _manager.get_Cache(type)->useCache ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

STDMETHODIMP CTiles::put_UseCache(tkCacheType type, VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    _manager.get_Cache(type)->useCache = newVal ? true : false;

    return S_OK;
}

// *********************************************************
//	     UseServer
// *********************************************************
STDMETHODIMP CTiles::get_UseServer(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = _manager.useServer();
    return S_OK;
}

STDMETHODIMP CTiles::put_UseServer(VARIANT_BOOL newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    _manager.useServer(newVal != 0);
    return S_OK;
}

// *********************************************************
//	     get_DiskCacheFilename
// *********************************************************
STDMETHODIMP CTiles::get_DiskCacheFilename(BSTR* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    USES_CONVERSION;
    *retVal = W2BSTR(_manager.get_DiskCache()->cache->get_Filename());

    return S_OK;
}

// *********************************************************
//	     put_DiskCacheFilename
// *********************************************************
STDMETHODIMP CTiles::put_DiskCacheFilename(BSTR pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    USES_CONVERSION;
    _manager.get_DiskCache()->cache->set_Filename(OLE2W(pVal));

    return S_OK;
}

// *********************************************************
//	     MaxCacheSize
// *********************************************************
STDMETHODIMP CTiles::get_MaxCacheSize(tkCacheType type, double* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ITileCache* cache = TileCacheManager::get_Cache((CacheType)type);
    *pVal = cache ? cache->get_MaxSize() : 0.0;

    return S_OK;
}

STDMETHODIMP CTiles::put_MaxCacheSize(tkCacheType type, double newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ITileCache* cache = TileCacheManager::get_Cache((CacheType)type);
    if (cache)
    {
        cache->set_MaxSize(newVal);
    }

    return S_OK;
}

// *********************************************************
//	     ClearCache()
// *********************************************************
STDMETHODIMP CTiles::ClearCache(tkCacheType type)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ITileCache* cache = TileCacheManager::get_Cache((CacheType)type);
    if (cache)
    {
        cache->Clear(tkTileProvider::ProviderNone, 0, 100);
    }

    return S_OK;
}

// *********************************************************
//	     ClearCache2()
// *********************************************************
STDMETHODIMP CTiles::ClearCache2(tkCacheType type, int providerId, int fromScale, int toScale)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ITileCache* cache = TileCacheManager::get_Cache((CacheType)type);
    if (cache)
    {
        cache->Clear(providerId, fromScale, toScale);
    }

    return S_OK;
}

// *********************************************************
//	     get_CacheSize()
// *********************************************************
STDMETHODIMP CTiles::get_CacheSize(tkCacheType type, double* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = 0.0;

    ITileCache* cache = TileCacheManager::get_Cache((CacheType)type);
    if (cache)
    {
        *retVal = cache->get_SizeMB();
    }

    return S_OK;
}

// *********************************************************
//	     get_CacheSize3()
// *********************************************************
STDMETHODIMP CTiles::get_CacheSize2(tkCacheType type, int providerId, int scale, double* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = _manager.get_Cache(type)->cache->get_SizeMB(providerId, scale);

    return S_OK;
}

// ********************************************************
//     Serialize()
// ********************************************************
STDMETHODIMP CTiles::Serialize(BSTR* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    CPLXMLNode* psTree = this->SerializeCore("TilesClass");
    Utility::SerializeAndDestroyXmlTree(psTree, retVal);
    return S_OK;
}

// ********************************************************
//     SerializeCore()
// ********************************************************
CPLXMLNode* CTiles::SerializeCore(const CString& elementName)
{
    USES_CONVERSION;
    CPLXMLNode* psTree = CPLCreateXMLNode(nullptr, CXT_Element, elementName);

    if (!_visible)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "Visible", CPLString().Printf("%d", (int)_visible));

    if (_manager.get_GridLinesVisible())
        Utility::CPLCreateXMLAttributeAndValue(psTree, "GridLinesVisible",
                                               CPLString().Printf("%d", (int)_manager.get_GridLinesVisible()));

    if (_provider->Id != 0)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "Provider", CPLString().Printf("%d", (int)_provider->Id));

    bool value = _manager.get_RamCache()->doCaching;
    if (!value)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "DoRamCaching", CPLString().Printf("%d", (int)value));

    value = _manager.get_DiskCache()->doCaching;
    if (value)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "DoDiskCaching", CPLString().Printf("%d", (int)value));

    value = _manager.get_RamCache()->useCache;
    if (!value)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "UseRamCache", CPLString().Printf("%d", (int)value));

    value = _manager.get_DiskCache()->useCache;
    if (!value)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "UseDiskCache", CPLString().Printf("%d", (int)value));

    if (!_manager.useServer())
        Utility::CPLCreateXMLAttributeAndValue(psTree, "UseServer",
                                               CPLString().Printf("%d", (int)_manager.useServer()));

    double dbl = _manager.get_RamCache()->cache->get_MaxSize();
    if (dbl != 100.0)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxRamCacheSize", CPLString().Printf("%f", dbl));

    dbl = _manager.get_DiskCache()->cache->get_MaxSize();
    if (dbl != 100.0)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxDiskCacheSize", CPLString().Printf("%f", dbl));

    if (_minScaleToCache != 0)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "MinScaleToCache", CPLString().Printf("%d", _minScaleToCache));

    if (_maxScaleToCache != 100)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "MaxScaleToCache", CPLString().Printf("%d", _maxScaleToCache));

    CStringW dbName = _manager.get_DiskCache()->cache->get_Filename();
    if (dbName.GetLength() != 0)
        Utility::CPLCreateXMLAttributeAndValue(psTree, "DiskCacheFilename", Utility::ConvertToUtf8(dbName));

    // serialization of custom providers
    CPLXMLNode* psProviders = CPLCreateXMLNode(nullptr, CXT_Element, "TileProviders");
    if (psProviders)
    {
        vector<BaseProvider*>* providers = ((CTileProviders*)_providers)->GetList();
        for (size_t i = 0; i < providers->size(); i++)
        {
            CustomTileProvider* cp = dynamic_cast<CustomTileProvider*>(providers->at(i));
            if (cp)
            {
                CPLXMLNode* psCustom = CPLCreateXMLNode(nullptr, CXT_Element, "TileProvider");
                Utility::CPLCreateXMLAttributeAndValue(psCustom, "Id", CPLString().Printf("%d", cp->Id));
                Utility::CPLCreateXMLAttributeAndValue(psCustom, "Name", cp->Name);
                Utility::CPLCreateXMLAttributeAndValue(psCustom, "Url", cp->get_UrlFormat());
                Utility::CPLCreateXMLAttributeAndValue(psCustom, "Projection",
                                                       CPLString().Printf("%d", (int)cp->get_Projection()));
                Utility::CPLCreateXMLAttributeAndValue(psCustom, "MinZoom",
                                                       CPLString().Printf("%d", cp->get_MinZoom()));
                Utility::CPLCreateXMLAttributeAndValue(psCustom, "MaxZoom",
                                                       CPLString().Printf("%d", cp->get_MaxZoom()));
                Utility::CPLCreateXMLAttributeAndValue(psCustom, "Copyright", cp->get_Copyright());
                CPLAddXMLChild(psProviders, psCustom);
            }
        }
        CPLAddXMLChild(psTree, psProviders);
    }
    return psTree;
}

// ********************************************************
//     Deserialize()
// ********************************************************
STDMETHODIMP CTiles::Deserialize(BSTR newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState())
    USES_CONVERSION;

    CString s = OLE2CA(newVal);
    CPLXMLNode* node = CPLParseXMLString(s.GetString());
    if (node)
    {
        CPLXMLNode* nodeTiles = CPLGetXMLNode(node, "=TilesClass");
        if (nodeTiles)
        {
            DeserializeCore(nodeTiles);
        }
        CPLDestroyXMLNode(node);
    }
    return S_OK;
}

void setBoolean(CPLXMLNode* node, const CString& name, bool& value)
{
    const CString s = CPLGetXMLValue(node, name, nullptr);
    if (s != "") value = atoi(s) == 0 ? false : true;
}

void setInteger(CPLXMLNode* node, const CString& name, int& value)
{
    const CString s = CPLGetXMLValue(node, name, nullptr);
    if (s != "") value = atoi(s);
}

void setDouble(CPLXMLNode* node, const CString& name, double& value)
{
    const CString s = CPLGetXMLValue(node, name, nullptr);
    if (s != "") value = Utility::atof_custom(s);
}

// ********************************************************
//     DeserializeCore()
// ********************************************************
bool CTiles::DeserializeCore(CPLXMLNode* node)
{
    if (!node)
        return false;

    SetDefaults();

    setBoolean(node, "Visible", _visible);

    bool temp;
    setBoolean(node, "GridLinesVisible", temp);
    _manager.set_GridLinesVisible(temp);

    setBoolean(node, "DoRamCaching", temp);
    _manager.get_RamCache()->doCaching = temp;

    setBoolean(node, "DoDiskCaching", temp);
    _manager.get_DiskCache()->doCaching = temp;

    setBoolean(node, "UseRamCache", temp);
    _manager.get_RamCache()->useCache = temp;

    setBoolean(node, "UseDiskCache", temp);
    _manager.get_DiskCache()->useCache = temp;

    setBoolean(node, "UseServer", temp);
    _manager.useServer(temp);

    CString s = CPLGetXMLValue(node, "Provider", nullptr);
    if (s != "") this->put_ProviderId(atoi(s));

    double dbl;
    setDouble(node, "MaxRamCacheSize", dbl);
    _manager.get_RamCache()->cache->set_MaxSize(dbl);

    setDouble(node, "MaxDiskCacheSize", dbl);
    _manager.get_DiskCache()->cache->set_MaxSize(dbl);

    setInteger(node, "MinScaleToCache", _minScaleToCache);
    setInteger(node, "MaxScaleToCache", _maxScaleToCache);

    USES_CONVERSION;
    s = CPLGetXMLValue(node, "DiskCacheFilename", nullptr);
    if (s != "")
    {
        _manager.get_DiskCache()->cache->set_Filename(Utility::ConvertFromUtf8(s));
    }

    // custom providers
    CPLXMLNode* nodeProviders = CPLGetXMLNode(node, "TileProviders");
    if (nodeProviders)
    {
        // don't clear providers as it will clear the cache as well,
        // if provider with certain id exists, it simply won't be added twice
        // Not used: vector<BaseProvider*>* providers = ((CTileProviders*)_providers)->GetList();

        CPLXMLNode* nodeProvider = nodeProviders->psChild;
        while (nodeProvider)
        {
            if (strcmp(nodeProvider->pszValue, "TileProvider") == 0)
            {
                int id = 0, minZoom = 0, maxZoom = 0, projection = 0;
                CString url, name, copyright;

                s = CPLGetXMLValue(nodeProvider, "Id", nullptr);
                if (s != "") id = atoi(s);

                s = CPLGetXMLValue(nodeProvider, "MinZoom", nullptr);
                if (s != "") minZoom = atoi(s);

                s = CPLGetXMLValue(nodeProvider, "MaxZoom", nullptr);
                if (s != "") maxZoom = atoi(s);

                s = CPLGetXMLValue(nodeProvider, "Projection", nullptr);
                if (s != "") projection = atoi(s);

                s = CPLGetXMLValue(nodeProvider, "Url", nullptr);
                if (s != "") url = s;

                s = CPLGetXMLValue(nodeProvider, "Name", nullptr);
                if (s != "") name = s;

                s = CPLGetXMLValue(nodeProvider, "Copyright", nullptr);
                if (s != "") copyright = s;

                VARIANT_BOOL vb;
                CComBSTR bstrName(name);
                CComBSTR bstrUrl(url);
                CComBSTR bstrCopyright(copyright);

                _providers->Add(id, bstrName, bstrUrl, (tkTileProjection)projection, minZoom, maxZoom, bstrCopyright,
                                &vb);
            }
            nodeProvider = nodeProvider->psNext;
        }
    }
    return true;
}

// *********************************************************
//	     CustomProviderId
// *********************************************************
STDMETHODIMP CTiles::get_ProviderId(int* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    if (!_visible)
    {
        *retVal = -1;
        return S_OK;
    }

    *retVal = _provider->Id;
    return S_OK;
}

STDMETHODIMP CTiles::put_ProviderId(int providerId)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    const bool visibleOld = _visible;
    put_Visible(providerId == -1 ? VARIANT_FALSE : VARIANT_TRUE);

    BaseProvider* provider = get_Provider(providerId);

    if (_provider != provider || visibleOld != _visible)
    {
        _reloadNeeded = true;
    }

    if (provider)
    {
        _provider = provider;
    }

    return S_OK;
}

// *********************************************************
//	     GetTilesIndices
// *********************************************************
STDMETHODIMP CTiles::GetTilesIndices(IExtents* boundsDegrees, int zoom, int providerId, IExtents** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = nullptr;

    if (!boundsDegrees)
    {
        ErrorMessage(tkUNEXPECTED_NULL_PARAMETER);
        return S_OK;
    }

    double xMin, xMax, yMin, yMax, zMin, zMax;
    boundsDegrees->GetBounds(&xMin, &yMin, &zMin, &xMax, &yMax, &zMax);

    BaseProvider* provider = get_Provider(providerId);
    if (provider)
    {
        CPoint p1;
        provider->get_Projection()->FromLatLngToXY(PointLatLng(yMax, xMin), zoom, p1);

        CPoint p2;
        provider->get_Projection()->FromLatLngToXY(PointLatLng(yMin, xMax), zoom, p2);

        IExtents* ext = nullptr;
        ComHelper::CreateExtents(&ext);
        ext->SetBounds(p1.x, p1.y, 0.0, p2.x, p2.y, 0.0);
        *retVal = ext;
    }

    return S_OK;
}

// *********************************************************
//	     Prefetch
// *********************************************************
STDMETHODIMP CTiles::Prefetch(double minLat, double maxLat, double minLng, double maxLng, int zoom, int providerId,
                              IStopExecution* stop, LONG* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = -1;

    BaseProvider* p = get_Provider(providerId);
    if (!p)
    {
        ErrorMessage(tkINVALID_PROVIDER_ID);
        return S_OK;
    }

    CRect indices;
    const Extent ext(minLng, maxLng, minLat, maxLat);
    p->get_Projection()->getTileRectXY(ext, zoom, indices);

    Prefetch2(indices.left, indices.right, indices.bottom, indices.top, zoom, providerId, stop, retVal);

    return S_OK;
}

// *********************************************************
//	     Prefetch2
// *********************************************************
STDMETHODIMP CTiles::Prefetch2(int minX, int maxX, int minY, int maxY, int zoom, int providerId, IStopExecution* stop,
                               LONG* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = -1;

    BaseProvider* provider = get_Provider(providerId);
    if (!provider)
    {
        ErrorMessage(tkINVALID_PROVIDER_ID);
        return S_OK;
    }

    PrefetchManager* manager = PrefetchManagerFactory::Create(TileCacheManager::get_Cache(tctSqliteCache));
    if (manager)
    {
        USES_CONVERSION;
        const CRect rect(minX, minY, maxX, maxY);
        *retVal = manager->Prefetch(provider, rect, zoom, _globalCallback, stop);
    }

    return S_OK;
}

// *********************************************************
//	     PrefetchToFolder()
// *********************************************************
// Writes tiles to the specified folder
STDMETHODIMP CTiles::PrefetchToFolder(IExtents* ext, int zoom, int providerId, BSTR savePath, BSTR fileExt,
                                      IStopExecution* stop, LONG* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    BaseProvider* provider = get_Provider(providerId);
    if (!provider)
    {
        ErrorMessage(tkINVALID_PROVIDER_ID);
        *retVal = -1;
        return S_OK;
    }

    LogBulkDownloadStarted(ext, zoom, provider, savePath, fileExt);

    USES_CONVERSION;
    CStringW path = OLE2W(savePath);
    if (path.GetLength() > 0 && path.GetAt(path.GetLength() - 1) != L'\\')
    {
        path += L"\\";
    }

    if (!Utility::DirExists(path))
    {
        CallbackHelper::ErrorMsg("PrefetchManager", nullptr, "", "Folder doesn't exist: ", W2A(path));
        return -1;
    }

    DiskCache* cache = new DiskCache(path, fileExt);

    PrefetchManager* manager = PrefetchManagerFactory::Create(cache);
    if (manager)
    {
        CRect indices;
        const Extent extents(ext);
        provider->get_Projection()->getTileRectXY(extents, zoom, indices);

        USES_CONVERSION;
        *retVal = manager->Prefetch(provider, indices, zoom, _globalCallback, stop);
    }

    return S_OK;
}

// *********************************************************
//	     get_DiskCacheCount
// *********************************************************
STDMETHODIMP CTiles::get_DiskCacheCount(int providerId, int zoom, int xMin, int xMax, int yMin, int yMax, LONG* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ITileCache* cache = TileCacheManager::get_Cache(tctSqliteCache);
    if (cache)
    {
        cache->get_TileCount(providerId, zoom, CRect(xMin, yMax, xMax, yMin));
    }

    return S_OK;
}

// *********************************************************
//	     CheckConnection
// *********************************************************
STDMETHODIMP CTiles::CheckConnection(BSTR url, VARIANT_BOOL* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    if (_provider != nullptr)
    {
        USES_CONVERSION;
        *retVal = SecureHttpClient::CheckConnection(OLE2A(url)) ? VARIANT_TRUE : VARIANT_FALSE;
    }
    else
    {
        *retVal = VARIANT_FALSE;
    }
    return S_OK;
}

// *********************************************************
//	     get_TileBounds
// *********************************************************
STDMETHODIMP CTiles::GetTileBounds(int providerId, int zoom, int tileX, int tileY, IExtents** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = VARIANT_FALSE;

    BaseProvider* provider = get_Provider(providerId);
    if (!provider)
    {
        ErrorMessage(tkINVALID_PROVIDER_ID);
        return S_OK;
    }

    CSize size;
    provider->get_Projection()->GetTileMatrixSizeXY(zoom, size);

    if (tileX < 0 || tileX > size.cx - 1 || tileY < 0 || tileY > size.cy - 1)
    {
        ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
        return S_OK;
    }

    const CPoint pnt1(tileX, tileY);
    const CPoint pnt2(tileX + 1, tileY + 1);
    PointLatLng p1, p2;

    provider->get_Projection()->FromXYToLatLng(pnt1, zoom, p1);
    provider->get_Projection()->FromXYToLatLng(pnt2, zoom, p2);

    IExtents* ext = nullptr;
    ComHelper::CreateExtents(&ext);

    ext->SetBounds(p1.Lng, p1.Lat, 0.0, p2.Lng, p2.Lat, 0.0);

    *retVal = ext;

    return S_OK;
}

// ************************************************************
//		get_MaxZoom
// ************************************************************
STDMETHODIMP CTiles::get_MaxZoom(int* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retVal = _provider->get_MaxZoom();
    return S_OK;
}

// ************************************************************
//		put_MinZoom
// ************************************************************
STDMETHODIMP CTiles::get_MinZoom(int* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *retVal = _provider->get_MinZoom();
    return S_OK;
}

// ************************************************************
//		ServerProjection
// ************************************************************
STDMETHODIMP CTiles::get_ServerProjection(IGeoProjection** retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = nullptr;

    BaseProjection* p = _provider->get_Projection();
    if (p)
    {
        *retVal = p->get_ServerProjection();

        if (*retVal)
        {
            (*retVal)->AddRef();
        }
    }

    return S_OK;
}

// ************************************************************
//		ProjectionStatus
// ************************************************************
STDMETHODIMP CTiles::get_ProjectionStatus(tkTilesProjectionStatus* retVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *retVal = tkTilesProjectionStatus::tpsEmptyOrInvalid;

    IMapViewCallback* map = _manager.get_MapCallback();
    if (!map)
    {
        return S_OK;
    }

    IGeoProjection* gp = map->_GetMapProjection();

    IGeoProjection* gpServer = _provider->get_Projection()->get_ServerProjection();

    if (gp && gpServer)
    {
        VARIANT_BOOL vb;
        gp->get_IsSame(gpServer, &vb);

        if (vb)
        {
            *retVal = tkTilesProjectionStatus::tpsNative;
        }
        else
        {
            gpServer->StartTransform(gp, &vb);
            if (vb)
            {
                *retVal = tkTilesProjectionStatus::tpsCompatible;
                gpServer->StopTransform();
            }
        }
    }

    return S_OK;
}

// ************************************************************
//		ProxyAuthenticationScheme
// ************************************************************
STDMETHODIMP CTiles::get_ProxyAuthenticationScheme(tkProxyAuthentication* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    *pVal = m_globalSettings.proxyAuthentication;
    return S_OK;
}

STDMETHODIMP CTiles::put_ProxyAuthenticationScheme(tkProxyAuthentication newVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    m_globalSettings.proxyAuthentication = newVal;
    return S_OK;
}

// ************************************************************
//		ProjectionIsSphericalMercator
// ************************************************************
STDMETHODIMP CTiles::get_ProjectionIsSphericalMercator(VARIANT_BOOL* pVal)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    *pVal = _provider->get_Projection()->IsSphericalMercator() ? VARIANT_TRUE : VARIANT_FALSE;

    return S_OK;
}

// *********************************************************
//	     LogBulkDownloadStarted
// *********************************************************
void CTiles::LogBulkDownloadStarted(IExtents* ext, const int zoom, BaseProvider* provider, BSTR savePath, BSTR fileExt)
{
    if (tilesLogger.IsOpened())
    {
        CComBSTR extentString;
        ext->ToDebugString(&extentString);

        tilesLogger.out() << "\n";
        tilesLogger.out() << "Start PrefetchToFolder:\n";
        tilesLogger.out() << "Extent " << CString(extentString) << endl;
        tilesLogger.out() << "Zoom " << zoom << endl;
        tilesLogger.out() << "Provider " << provider->Name << endl;
        tilesLogger.out() << "SavePath " << CString(savePath) << endl;
        tilesLogger.out() << "FileExt " << CString(fileExt) << endl;
        tilesLogger.out() << "---------------------" << endl;
    }
}
