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

#pragma region "ErrorHandling"
// ************************************************************
//		get_GlobalCallback()
// ************************************************************
STDMETHODIMP CTileProviders::get_GlobalCallback(ICallback **pVal)
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
STDMETHODIMP CTileProviders::put_GlobalCallback(ICallback *newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	Utility::put_ComReference(newVal, (IDispatch**)&m_globalCallback);
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
	*pVal = m_lastErrorCode;
	m_lastErrorCode = tkNO_ERROR;
	return S_OK;
}

// **************************************************************
//		ErrorMessage()
// **************************************************************
void CTileProviders::ErrorMessage(long ErrorCode)
{
	m_lastErrorCode = ErrorCode;
	if( m_globalCallback != NULL) 
		m_globalCallback->Error(OLE2BSTR(m_key),A2BSTR(ErrorMsg(m_lastErrorCode)));
	return;
}

// ************************************************************
//		get/put_Key()
// ************************************************************
STDMETHODIMP CTileProviders::get_Key(BSTR *pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	USES_CONVERSION;
	*pVal = OLE2BSTR(m_key);
	return S_OK;
}
STDMETHODIMP CTileProviders::put_Key(BSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SysFreeString(m_key);
	m_key = OLE2BSTR(newVal);
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
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		if (m_providers[i]->Id == provider)
			return m_providers[i];
	}

#ifdef _DEBUG
	gMemLeakDetect.stopped = true;
#endif

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
			RosreestrProvider* pr1 = new RosreestrProvider(false);
			pr1->subProviders.push_back(pr1);
			pr1->subProviders.push_back(new RosreestrProvider(true));
			pr1->subProviders.push_back(new RosreestrBordersProvider());
			p = pr1;
			break;
	}

	if (p) 
	{
		m_providers.push_back(p);
	}
	else
	{
		Debug::WriteLine("Provider wasn't found: %d", (int)provider);
	}
#ifdef _DEBUG
	gMemLeakDetect.stopped = false;
#endif

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
			if (m_tiles != NULL)
			{
				// check, probably the provider is currently in use 
				int id = -1;
				m_tiles->get_ProviderId(&id);
				if (p->Id == id)
					m_tiles->put_Provider(tkTileProvider::OpenStreetMap);
			}
			
			for (size_t i = 0; i < m_providers.size(); i++)
			{
				if (m_providers[i]->Id == providerId)
				{
					delete m_providers[i];
					m_providers.erase(m_providers.begin() + i);
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
	if (m_tiles != NULL) {
		tkTileProvider provider;
		m_tiles->get_Provider(&provider);
		if(provider == tkTileProvider::ProviderCustom)
			m_tiles->put_Provider(tkTileProvider::OpenStreetMap);
	}
	
	// default providers should remain untouched
	std::vector<BaseProvider*>::iterator it = m_providers.begin();
	while (it < m_providers.end())
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
			it = m_providers.erase(it);
		}
		else
		{
			it++;
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
	*pVal = m_providers.size();
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
		m_providers.push_back(provider);
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
	if (Index < 0 || Index >= (int)m_providers.size()){
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = -1;
	}
	else {
		*retVal = (LONG)m_providers[Index]->Id;
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
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else {
		*retVal = A2BSTR(m_providers[Index]->Name);
	}	
	return S_OK;
}

STDMETHODIMP CTileProviders::put_Name(int Index, BSTR pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	else
	{
		USES_CONVERSION;
		m_providers[Index]->Name = OLE2A(pVal);
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
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else {
		*retVal = A2BSTR(m_providers[Index]->Version);
	}	
	return S_OK;
}

// *******************************************************
//			put_Version()
// *******************************************************
STDMETHODIMP CTileProviders::put_Version(int Index, BSTR pVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		return S_FALSE;
	}
	else
	{
		USES_CONVERSION;
		m_providers[Index]->Version = OLE2A(pVal);
	}
	return S_OK;
}

// *******************************************************
//			get_UrlPattern()
// *******************************************************
STDMETHODIMP CTileProviders::get_UrlPattern(int Index, BSTR* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = A2BSTR("");
	}
	else {
		*retVal = A2BSTR(m_providers[Index]->UrlFormat);
	}	
	return S_OK;
}

// *******************************************************
//			get_Projection()
// *******************************************************
// TODO: add separate property to return GeoProjection object
STDMETHODIMP CTileProviders::get_Projection(int Index, tkTileProjection* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = (tkTileProjection)-1;
	}
	else {
		CustomProvider* p = dynamic_cast<CustomProvider*>(m_providers[Index]);
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
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = -1;
	}
	else {
		*retVal = m_providers[Index]->minZoom;
	}	
	return S_OK;
}

// *******************************************************
//			get_MaxZoom()
// *******************************************************
STDMETHODIMP CTileProviders::get_MaxZoom(int Index, int* retVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	if (Index < 0 || Index >= (int)m_providers.size())
	{
		ErrorMessage(tkINDEX_OUT_OF_BOUNDS);
		*retVal = -1;
	}
	else {
		*retVal = m_providers[Index]->maxZoom;
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
	for (size_t i = 0; i < m_providers.size(); i++)
	{
		if (providerId == m_providers[i]->Id)
		{
			*retVal = i;
			break;
		}
	}
	return S_OK;
}

