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
  
#pragma once
#include "geopoint.h"
#include "baseprojection.h"
#include "TileCore.h"
#include <atlhttp.h>		// ATL HTTP Client (CAtlHttpClient)
#include "afxmt.h"

extern Debug::Logger tilesLogger;

class MyHttpClient: public CAtlHttpClient
{
public:
	bool _inUse;

	MyHttpClient()
	{
		_inUse = false;
	}

	void SetDefaultUrl(CString url)
	{
		CAtlHttpClient::SetDefaultUrl(url);
	}
};

// Downloads map tiles via HTTP; this is abstract class to inherit from
class BaseProvider
{
private:
	bool m_initialized;

protected:
	static CString m_proxyAddress;
	static short m_proxyPort;
	std::vector<MyHttpClient*> _httpClients;
	::CCriticalSection _clientLock;
public:
	bool BaseProvider::CheckConnection(CString url);
	std::vector<BaseProvider*> subProviders;	// for complex providers with more than one source bitmap per tile
	BaseProjection* Projection;
	void *mapView;
	int Id;
	CString LanguageStr;
	CString RefererUrl;
	CString Copyright;
	CString ServerLetters;
	CString Name;
	CString UrlFormat;
	CString Version;
	CSize minOfTiles;
	CSize maxOfTiles;
	int minZoom;
    int maxZoom;
	int zoom;
	bool Selected;	// is used by clients only
	int httpStatus;
	bool IsStopped;

private:
	CMemoryBitmap* GetTileImageUsingHttp(CString urlStr, CString shortUrl, bool recursive = false);

protected:
	virtual CString MakeTileImageUrl(CPoint &pos, int zoom) = 0;

	int GetServerNum(CPoint &pos, int max)
	{
		return (pos.x + 2 * pos.y) % max;
	}
public:
	BaseProvider()
		: minZoom(1), maxZoom(18), zoom(0), httpStatus(200)
	{
		mapView = NULL;
		LanguageStr = "en";
		Projection = NULL;
		m_initialized = false;
		Selected = false;
		_clientLock.Unlock();
		IsStopped = false;
	}

	virtual ~BaseProvider(void)
	{
		if (Projection != NULL)
			delete Projection;
		for(size_t i = 0; i < _httpClients.size(); i++)
		{
			_httpClients[i]->Close();
			delete _httpClients[i];
		}
		_httpClients.clear();
	};

	MyHttpClient* GetHttpClient()
	{
		_clientLock.Lock();
		MyHttpClient* client = NULL;
		for(size_t i = 0; i < _httpClients.size(); i++)
		{
			if(!_httpClients[i]->_inUse)
			{
				_httpClients[i]->_inUse = true;
				client = _httpClients[i];
				break;
			}
		}
		_clientLock.Unlock();
		return client;
	}

	void ReleaseHttpClient(MyHttpClient* client)
	{
		_clientLock.Lock();
		client->_inUse = false;
		_clientLock.Unlock();
	}

	bool CanReuseConnections()
	{
		_clientLock.Lock();
		bool canReuse = _httpClients.size() > 0;
		_clientLock.Unlock();
		return canReuse;
	}

	// proxy support
	short get_ProxyPort() {return m_proxyPort;}
	CString get_ProxyAddress() {return m_proxyAddress;}
	bool SetProxy(CString address, int port);
	bool AutodetectProxy();

	CMemoryBitmap* DownloadBitmap(CPoint &pos, int zoom);
	TileCore* GetTileImage(CPoint &pos, int zoom);	
	

	
};