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

// ***************************************************************
//		HttpClientEx
// ***************************************************************
class HttpClientEx: public CAtlHttpClient
{
public:
	bool _inUse;

	HttpClientEx()
	{
		_inUse = false;
	}

	void SetDefaultUrl(CString url)
	{
		CAtlHttpClient::SetDefaultUrl(url);
	}
};

// ***************************************************************
//		BasicAuth
// ***************************************************************
class BasicAuth : public CBasicAuthObject, public ATL::IAuthInfo
{
private:
	CString _username; 
	CString _password;
	CString _domain;
public:
	void SetCredentials(CString username, CString password, CString domain)
	{
		_username = username;
		_password = password;
		_domain = domain;
	}

	void Init(CAtlHttpClient *pSocket, IAuthInfo *pAuthInfo)
	{
		CBasicAuthObject::Init(pSocket, pAuthInfo);
	}

	bool Authenticate(LPCTSTR szAuthTypes, bool bProxy)
	{
		bool result = CBasicAuthObject::Authenticate(szAuthTypes, bProxy);
		if (!result) {
			CallbackHelper::ErrorMsg("Tiles proxy authentication failed. Check if proper credentials are set for Tiles.SetProxyAuthentication.");
		}
		return result;
	}

	HRESULT GetPassword(LPTSTR szPwd, DWORD* dwBuffSize)
	{
		if (CopyCString(_password, szPwd, dwBuffSize))
			return S_OK;
		return E_FAIL;
	}

	HRESULT GetUsername(LPTSTR szUid, DWORD* dwBuffSize)
	{
		if (CopyCString(_username, szUid, dwBuffSize))
			return S_OK;
		return E_FAIL;
	}
	HRESULT GetDomain(LPTSTR szDomain, DWORD* dwBuffSize)
	{
		// MS sample indicates that it's not expected to be called at all
		// http_://msdn.microsoft.com/en-us/library/f3wxbf3f%28v=vs.80%29.aspx
		return S_OK;
	}
};

// ***************************************************************
//		BaseProvider
// ***************************************************************
// Downloads map tiles via HTTP; this is abstract class to inherit from
class BaseProvider
{
public:
	BaseProvider()
		: _minZoom(1), _maxZoom(18), _httpStatus(200),
		_manager(NULL), _projection(NULL), _isStopped(false),
		_dynamicOverlay(false), _initAttemptCount(0), Id(-1)
	{
		_licenseUrl = "https://mapwingis.codeplex.com/wikipage?title=tiles";
		LanguageStr = "en";
	}

	virtual ~BaseProvider(void)
	{
		if (_projection != NULL) {
			delete _projection;
		}

		for (size_t i = 0; i < _httpClients.size(); i++)
		{
			_httpClients[i]->Close();
			delete _httpClients[i];
		}

		_httpClients.clear();
	};

private:
	bool _dynamicOverlay;
	bool _isStopped;
	int _httpStatus;
	void* _manager;

protected:
	static CString m_proxyAddress;
	static short m_proxyPort;
	static CString _proxyUsername;
	static CString _proxyPassword;
	static CString _proxyDomain;
	static ::CCriticalSection _clientLock;

protected:
	vector<HttpClientEx*> _httpClients;
	vector<BaseProvider*> _subProviders;	// for complex providers with more than one source bitmap per tile
	BaseProjection* _projection;
	CStringW _copyright;
	CString _refererUrl; 
	CString _licenseUrl;
	CString _urlFormat;
	int _initAttemptCount;
	CString _serverLetters;
	
	int _minZoom;
	int _maxZoom;

public:
	int Id;
	CString Name;
	CString Version;
	CString LanguageStr;

private:
	CMemoryBitmap* GetTileImageUsingHttp(CString urlStr, CString shortUrl, bool recursive = false);

protected:
	virtual CString MakeTileImageUrl(CPoint &pos, int zoom) = 0;
	int GetServerNum(CPoint &pos, int max) 	{ return (pos.x + 2 * pos.y) % max; }

public:
	virtual CStringW get_Copyright() { return _copyright; }
	virtual bool Initialize() { return true; };

	int get_MinZoom() { return _minZoom; }
	int get_MaxZoom() { return _maxZoom; }
	void* get_Manager() { return _manager; }
	void put_Manager(void* value) { _manager = value; }
	vector<BaseProvider*>* get_SubProviders() { return &_subProviders; }
	CString get_LicenseUrl() { return _licenseUrl; }
	BaseProjection* get_Projection() { return _projection; }
	CString get_UrlFormat() { return _urlFormat; }

	bool CheckConnection(CString url);
	bool InitProxy(HttpClientEx& httpClient, BasicAuth& basicAuth, CNTLMAuthObject& ntlmAuth);

	// proxy support
	short get_ProxyPort() {return m_proxyPort;}
	CString get_ProxyAddress() {return m_proxyAddress;}
	bool SetProxy(CString address, int port);
	bool SetProxyAuthorization(CString username, CString password, CString domain);
	void ClearProxyAuthorization();
	bool AutodetectProxy();

	void AddDynamicOverlay(BaseProvider* p);
	void ClearSubProviders();

	CMemoryBitmap* DownloadBitmap(CPoint &pos, int zoom);
	TileCore* GetTileImage(CPoint &pos, int zoom);	
};