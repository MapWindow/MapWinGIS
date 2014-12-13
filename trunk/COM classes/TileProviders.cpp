/**************************************************************************************
 * Project: MapWindow Open Source (MapWinGis ActiveX control) 
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
 // Sergei Leschinski (lsu) 8 june 2012 - created the file.

#include "stdafx.h"
#include "TileProviders.h"
#include "BingMapProvider.h"
#include "OpenStreetMapProvider.h"
#include "OviProvider.h"
#include "YahooProvider.h"
#include "YandexProvider.h"
#include "GoogleMapProvider.h"
#include "TopomapperProvider.h"
#include "RosreestrProvider.h"
#include "SQLiteCache.h"
#include "RAMCache.h"

// ************************************************************
//		get_Provider()
// ************************************************************
BaseProvider* CTileProviders::get_Provider(int providerId)
{
	for (size_t i = 0; i < _providers.size(); i++)
	{
		if (_providers[i]->Id == providerId)
		{
			return _providers[i];
		}
	}
	return NULL;
}

#pragma region "ErrorHandling"
// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CTileProviders::get_GlobalCallback(ICallback **pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _globalCallback;
	if( _globalCallback != NULL )
		_globalCallback->AddRef();
	return S_OK;
}

// ************************************************************
//		put_GlobalCallback()
// ************************************************************
STDMETHODIMP CTileProviders::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	ComHelper::SetRef(newVal, (IDispatch**)&_globalCallback);
	return S_OK;
}

// *****************************************************************
//	   get_ErrorMsg()
// *****************************************************************
STDMETHODIMP CTileProviders::get_ErrorMsg(long ErrorCode, BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = A2BSTR(ErrorMsg(ErrorCode));
	return S_OK;
}

// ************************************************************
//		get_LastErrorCode()
// ************************************************************
STDMETHODIMP CTileProviders::get_LastErrorCode(long *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _lastErrorCode;
	_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CTileProviders::ErrorMessage(long ErrorCode)
{
	_lastErrorCode = ErrorCode;
	CallbackHelper::ErrorMsg("TileProviders", _globalCallback, _key, ErrorMsg(_lastErrorCode));
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CTileProviders::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(_key);
	return S_OK;
}
STDMETHODIMP CTileProviders::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(_key);
	_key = OLE2BSTR(newVal);
	return S_OK;
}

#pragma endregion

#pragma region Default providers
// *****************************************************
//		getProvider()
// *****************************************************
// Instantiate default providers
BaseProvider* CTileProviders::getProviderCore(tkTileProvider provider )
{
	for (size_t i = 0; i < _providers.size(); i++)
	{
		if (_providers[i]->Id == provider)
			return _providers[i];
	}

	BaseProvider* p = NULL;

	switch (provider)
	{
		case BingMaps:
			p = (BaseProvider*)new BingMapProvider();
			break;
		case BingSatellite:
			p = (BaseProvider*)new BingSatelliteProvider();
			break;
		case BingHybrid:
			p = (BaseProvider*)new BingHybridProvider();
			break;
		case OpenStreetMap:
			p = (BaseProvider*)new OpenStreetMapProvider();
			break;
		case OviMaps:
			p = (BaseProvider*)new OviMapProvider();
			break;
		case OviSatellite:
			p = (BaseProvider*)new OviSatelliteProvider();
			break;
		case OviHybrid:			
			p = (BaseProvider*)new OviHybridProvider();
			break;
		case OviTerrain:			
			p = (BaseProvider*)new OviTerrainProvider();
			break;
		case YahooMaps:
			p = (BaseProvider*)new YahooMapProvider();
			break;
		case YahooSatellite:
			p = (BaseProvider*)new YahooSatelliteProvider();
			break;
		case YahooHybrid:
			p = (BaseProvider*)new YahooHybridProvider(this);
			break;
		case YandexMaps:
			p = (BaseProvider*)new YandexMapProvider();
			break;
		case YandexSatellite:
			p = (BaseProvider*)new YandexSatelliteProvider();
			break;
		case YandexHybrid:
			p = (BaseProvider*)new YandexHybridProvider(this);
			break;
		case GoogleMaps:
			p = (BaseProvider*)new GoogleMapProvider();
			break;
		case GoogleSatellite:
			p = (BaseProvider*)new GoogleSatelliteProvider();
			break;
		case GoogleHybrid:
			p = (BaseProvider*)new GoogleHybridProvider(this);
			break;
		case GoogleTerrain:
			p = (BaseProvider*)new GoogleTerrainProvider();
			break;
		case OpenTransportMap:
			p = (BaseProvider*)new OpenTransportMapProvider();
			break;
		case OpenCycleMap:
			p = (BaseProvider*)new OpenCycleMapProvider();
			break;
		case Topomapper:
			p = (BaseProvider*)new TopomapperProvider();
			break;
		case Rosreestr:
			{
				RosreestrProvider* pr1 = new RosreestrProvider(false);
				pr1->subProviders.push_back(pr1);
				pr1->AddDynamicOverlay(new RosreestrProvider(true));
				pr1->AddDynamicOverlay(new RosreestrBordersProvider());
				p = (BaseProvider*)pr1;
			}
			break;
		case OpenHumanitarianMap:
			p = (BaseProvider*)new OpenHumanitarianMapProvider();
			break;
	}

	if (p) 
	{
		_providers.push_back(p);
	}
	else
	{
		CallbackHelper::AssertionFailed(Debug::Format("Provider wasn't found: %d.", (int)provider));
	}
	return p;
}
#pragma endregion

// ******************************************************
//		Remove()
// ******************************************************
STDMETHODIMP CTileProviders::Remove(LONG providerId, VARIANT_BOOL clearCache, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	BaseProvider* p = this->get_Provider(providerId);
	if (!p)
	{
		ErrorMessage(tkINVALID_PROVIDER_ID);
		return S_FALSE;
	}
	else
	{
		CustomProvider* custom = dynamic_cast<CustomProvider*>(p);
		if (!custom)
		{
			ErrorMessage(tkCANT_DELETE_DEFAULT_PROVIDER);
			return S_FALSE;
		}
		else
		{
			if (_tiles != NULL)
			{
				// check, probably the provider is currently in use 
				int id = -1;
				_tiles->get_ProviderId(&id);
				if (p->Id == id)
					_tiles->put_Provider(tkTileProvider::OpenStreetMap);
			}
			
			for (size_t i = 0; i < _providers.size(); i++)
			{
				if (_providers[i]->Id == providerId)
				{
					delete _providers[i];
					_providers.erase(_providers.begin() + i);
					break;
				}
			}

			if (clearCache)
			{
				SQLiteCache::Clear(providerId);
				RamCache::ClearByProvider(providerId);
			}
			return S_OK;
		}
	}
}

// *******************************************************
//			Clear()
// *******************************************************
STDMETHODIMP CTileProviders::Clear(VARIANT_BOOL clearCache)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	// make sure no provider is currently in use
	if (_tiles != NULL) {
		tkTileProvider provider;
		_tiles->get_Provider(&provider);
		if(provider == tkTileProvider::ProviderCustom)
			_tiles->put_Provider(tkTileProvider::OpenStreetMap);
	}
	
	// default providers should remain untouched
	std::vector<BaseProvider*>::iterator it = _providers.begin();
	while (it < _providers.end())
	{
		CustomProvider* p = dynamic_cast<CustomProvider*>(*it);
		if (p)
		{
			if (clearCache)
			{
				SQLiteCache::Clear(p->Id);
				RamCache::ClearByProvider(p->Id);
			}

			delete *it;
			it = _providers.erase(it);
		}
		else
		{
			++it;
		}
	}
	return S_OK;
}

// *******************************************************
//			get_Count()
// *******************************************************
STDMETHODIMP CTileProviders::get_Count(LONG* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pVal = _providers.size();
	return S_OK;
}

// *******************************************************
//			Add()
// *******************************************************
STDMETHODIMP CTileProviders::Add(int Id, BSTR name, BSTR urlPattern, tkTileProjection projection, int minZoom, int maxZoom, VARIANT_BOOL* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = VARIANT_FALSE;

	if (minZoom < 1 )
		minZoom = 1;

	if (Id < (int)tkTileProvider::ProviderCustom) {
		this->ErrorMessage(tkPROVIDER_ID_RESERVED);
		return S_FALSE;
	}

	BaseProvider* p = this->get_Provider(Id);
	if (p) {
		this->ErrorMessage(tkPROVIDER_ID_IN_USE);
		return S_FALSE;
	}

	try
	{
		CustomProvider* provider = new CustomProvider(Id, name, urlPattern, projection, minZoom, maxZoom);
		_providers.push_back(provider);
		*retVal = VARIANT_TRUE;
	}
	catch(int val)
	{
		switch (val)
		{
			case 1:
				this->ErrorMessage(tkINVALID_PROJECTION);
				break;
			case 2:
				this->ErrorMessage(tkINVALID_URL);
				break;
		}
		return S_FALSE;
	}
	return S_OK;
}

#pragma region Provider properties
// *******************************************************
//			get_Id()
// *******************************************************
STDMETHODIMP CTileProviders::get_Id(int Index, LONG* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size()){
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = -1;
	}
	else {
		*retVal = (LONG)_providers[Index]->Id;
	}
	return S_OK;
}

// *******************************************************
//			get_Name()
// *******************************************************
STDMETHODIMP CTileProviders::get_Name(int Index, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else {
		*retVal = A2BSTR(_providers[Index]->Name);
	}	
	return S_OK;
}

STDMETHODIMP CTileProviders::put_Name(int Index, BSTR pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	else
	{
		USES_CONVERSION;
		_providers[Index]->Name = OLE2A(pVal);
	}
	return S_OK;
}

// *******************************************************
//			Language()
// *******************************************************
STDMETHODIMP CTileProviders::get_Language(int Index, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else {
		*retVal = A2BSTR(_providers[Index]->LanguageStr);
	}	
	return S_OK;
}

STDMETHODIMP CTileProviders::put_Language(int Index, BSTR pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	else
	{
		USES_CONVERSION;
		_providers[Index]->LanguageStr = OLE2A(pVal);
	}
	return S_OK;
}

// ******************************************************
//		get_IsCustom()
// ******************************************************
STDMETHODIMP CTileProviders::get_IsCustom(int Index, VARIANT_BOOL* pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	return S_OK;
}

// *******************************************************
//			get_Version()
// *******************************************************
STDMETHODIMP CTileProviders::get_Version(int Index, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else {
		*retVal = A2BSTR(_providers[Index]->Version);
	}	
	return S_OK;
}

// *******************************************************
//			put_Version()
// *******************************************************
STDMETHODIMP CTileProviders::put_Version(int Index, BSTR pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	else
	{
		USES_CONVERSION;
		_providers[Index]->Version = OLE2A(pVal);
	}
	return S_OK;
}

// *******************************************************
//			get_UrlPattern()
// *******************************************************
STDMETHODIMP CTileProviders::get_UrlPattern(int Index, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else {
		*retVal = A2BSTR(_providers[Index]->UrlFormat);
	}	
	return S_OK;
}

// *******************************************************
//			get_Projection()
// *******************************************************
STDMETHODIMP CTileProviders::get_Projection(int Index, tkTileProjection* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = (tkTileProjection)-1;
	}
	else {
		CustomProvider* p = dynamic_cast<CustomProvider*>(_providers[Index]);
		*retVal = p ? p->m_projectionId :(tkTileProjection)-1;
	}	
	return S_OK;
}

// *******************************************************
//			get_MinZoom()
// *******************************************************
STDMETHODIMP CTileProviders::get_MinZoom(int Index, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = -1;
	}
	else {
		*retVal = _providers[Index]->minZoom;
	}	
	return S_OK;
}

// *******************************************************
//			get_MaxZoom()
// *******************************************************
STDMETHODIMP CTileProviders::get_MaxZoom(int Index, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = -1;
	}
	else {
		*retVal = _providers[Index]->maxZoom;
	}	
	return S_OK;
}
#pragma endregion

// *******************************************************
//			get_IndexByProvider()
// *******************************************************
STDMETHODIMP CTileProviders::get_IndexByProvider(tkTileProvider provider, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	this->get_IndexByProviderId((int)provider, retVal);
	return S_OK;
}

STDMETHODIMP CTileProviders::get_IndexByProviderId(int providerId, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*retVal = -1;
	for (size_t i = 0; i < _providers.size(); i++)
	{
		if (providerId == _providers[i]->Id)
		{
			*retVal = i;
			break;
		}
	}
	return S_OK;
}

