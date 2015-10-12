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
  
#include "stdafx.h"
#include "Wininet.h"
#include "BaseProvider.h"

CString BaseProvider::m_proxyAddress = "";
CString BaseProvider::_proxyUsername = "";
CString BaseProvider::_proxyPassword = "";
CString BaseProvider::_proxyDomain = "";
short BaseProvider::m_proxyPort = 0;
::CCriticalSection BaseProvider::_clientLock;
Debug::Logger tilesLogger;

#pragma region Load tile

// ************************************************************
//		GetTileImage()
// ************************************************************
TileCore* BaseProvider::GetTileImage(CPoint &pos, int zoom)
{
	TileCore* tile = new TileCore(this->Id, zoom, pos, this->_projection);
	for (size_t i = 0; i < _subProviders.size(); i++)
	{
		CMemoryBitmap* bmp = _subProviders[i]->DownloadBitmap(pos, zoom);
		if (bmp)
		{
			tile->AddBitmap(bmp);	
		}
		else
		{
			tile->m_hasErrors = true;
		}
	}
	return tile;
}

// ************************************************************
//		DownloadBitmap()
// ************************************************************
CMemoryBitmap* BaseProvider::DownloadBitmap(CPoint &pos, int zoom)
{
	CString url = MakeTileImageUrl(pos, zoom);
	CString shortUrl;
	shortUrl.Format("\\zoom=%d\\x=%d\\y=%d", zoom, pos.x, pos.y);
	CMemoryBitmap* bmp = this->GetTileImageUsingHttp(url, shortUrl);
	return bmp;
}

// ************************************************************
//		CheckConnection()
// ************************************************************
bool BaseProvider::CheckConnection(CString url)
{
	CAtlHttpClient* httpClient = new CAtlHttpClient();
	CAtlNavigateData navData;

	if (m_proxyAddress.GetLength() > 0)	{
		httpClient->SetProxy(m_proxyAddress, m_proxyPort);
	}

	bool result = httpClient->Navigate(url);
	httpClient->Close();
	delete httpClient;
	return result;
}

// ************************************************************
//		InitHttpClient()
// ************************************************************
// passing authorization object in such way it ugly, but allocating them dynamically might 
// give even more troubles
bool BaseProvider::InitHttpClient(HttpClientEx& httpClient, BasicAuth& basicAuth, CNTLMAuthObject& ntlmAuth)
{
	// otherwise it will query zone settings in IE for that URL
	// if the setting is anything other than URLPOLICY_CREDENTIALS_SILENT_LOGON_OK it will fail
	httpClient.SetSilentLogonOk(true);
	
	if (m_proxyAddress.GetLength() > 0)
	{
		if (!httpClient.SetProxy(m_proxyAddress, m_proxyPort))
			return false;
	}

	basicAuth.SetCredentials(_proxyUsername, _proxyPassword, _proxyDomain);

	if (m_globalSettings.proxyAuthentication == tkProxyAuthentication::asNtlm)
	{
		// We're using the standard CNLMAuthObject class here because it automatically uses the current user's
		// credentials if no IAuthInfo implementation is given.
			return httpClient.AddAuthObj(_T("NTLM"), &ntlmAuth);
	}
	return httpClient.AddAuthObj("BASIC", &basicAuth, &basicAuth);
}

// ************************************************************
//		ReportHttpError()
// ************************************************************
void ReportHttpError(int httpStatus, DWORD socketError)
{
	LPVOID lpMsgBuf;
	// http_://msdn.microsoft.com/en-us/library/windows/desktop/ms680582(v=vs.85).aspx
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		socketError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	CString s = (char*)lpMsgBuf;
	LocalFree(lpMsgBuf);

	if (httpStatus == -1) {
		CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. WSA error: %d. %s", socketError, s));
	}
	else {
		CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. HTTP status: %d", httpStatus));
	}
}

// ************************************************************
//		GetTileImageUsingHttp()
// ************************************************************
CMemoryBitmap* BaseProvider::GetTileImageUsingHttp(CString urlStr, CString shortUrl, bool recursive)
{
	HttpClientEx httpClient; //this->GetHttpClient();
	CAtlNavigateData navData;

	BasicAuth basicAuth; 
	CNTLMAuthObject ntlmAuth;
	InitHttpClient(httpClient, basicAuth, ntlmAuth);      // rename

	char* body = NULL;
	int bodyLen = 0;
	bool imageData = false;

	CSingleLock lock(&_clientLock);
	lock.Lock();
	Sleep(5);      // there is info that ARP might have problems with simultaneous connections, 
				   // discarding the next connection if the previous one is under resolution; 
				   // since we experience periodic rertransmissions let's introduce
				   // a small delay between connections http://stackoverflow.com/questions/1875151/delay-in-multiple-tcp-connections-from-java-to-the-same-machine

	lock.Unlock();

	bool result = httpClient.Navigate( urlStr, &navData );

	if (_isStopped) {
		return NULL;
	}
	_httpStatus = httpClient.GetStatus();

	if (!result) 
	{
		DWORD socketError = httpClient.GetLastError();
		ReportHttpError(_httpStatus, socketError);
	}

	if (result)
	{
		if (_httpStatus == 200) // 200 = successful HTTP transaction
		{
			bodyLen = httpClient.GetBodyLength();
			if (bodyLen > 0)
			{
				body = new char[bodyLen+1];
				memcpy( body, httpClient.GetBody(), bodyLen );
				body[bodyLen] = 0;
				
				CString contentType;
				httpClient.GetHeaderValue( _T("Content-Type"), contentType );
				imageData = contentType.Left(5).CompareNoCase( _T("image") ) == 0;
				
				if (!imageData && this->Id == tkTileProvider::Rosreestr) {    // ad-hoc fix
					imageData = true;
				}
			}
		}
	}
	
	if (tilesLogger.IsOpened() || Debug::LogTiles())
	{
		bool hasError = _httpStatus != 200;
		bool useShortUrl = false;
		
		if (tilesLogger.errorsOnly && !hasError)
		{
			// do nothing
		}
		else
		{
			CString err;
			err.Format("ERROR: %d; ", httpClient.GetLastError());
			CString s;
			s.Format("%sstatus %d size %6d b %s", (!hasError ? "": err), _httpStatus, bodyLen, 
					m_globalSettings.useShortUrlForTiles ? shortUrl : urlStr);
			tilesLogger.Log(s);
		}
	}
	
	CMemoryBitmap* bmp = NULL;
	if (imageData)
	{
		bmp = new CMemoryBitmap();
		bmp->LoadFromRawData(body, bodyLen);
		bmp->Provider = this->Id;
	}
	if (body)
		delete [] body;
	
	if (!result && !recursive && (_httpStatus == -1 || bodyLen == 0))
	{
		// let's try one more time
		Sleep(20);
		tilesLogger.Log("Reloading attempt: " + m_globalSettings.useShortUrlForTiles ? shortUrl : urlStr);
		bmp = this->GetTileImageUsingHttp(urlStr, shortUrl, true);
	}
	return bmp;
}
#pragma endregion

#pragma region Proxy
// *************************************************************
//			SetProxy()
// *************************************************************
bool BaseProvider::SetProxy(CString address, int port)
{
	bool ret = false;
	CAtlHttpClient* httpClient = new CAtlHttpClient();
	if (httpClient)
	{
		ret = httpClient->SetProxy(address, port);
		if (ret)
		{
			m_proxyAddress = address;
			m_proxyPort = port;
		}
		httpClient->Close();
		delete httpClient;
	}
	return ret;
}

// *************************************************************
//			SetProxyAuthorization()
// *************************************************************
bool BaseProvider::SetProxyAuthorization(CString username, CString password, CString domain)
{
	HttpClientEx httpClient;

	CString oldProxy = _proxyUsername;
	CString oldDomain = _proxyDomain;
	CString oldPassword = _proxyPassword;

	_proxyUsername = username;
	_proxyDomain = domain;
	_proxyPassword = password;

	return true;
}

// *************************************************************
//			ClearProxyAuthorization()
// *************************************************************
void BaseProvider::ClearProxyAuthorization()
{
	_proxyUsername = "";
	_proxyDomain = "";
	_proxyPassword = "";
}

// *************************************************************
//			GetProxyServer()
// *************************************************************
CString GetProxyServer()
{
	CString s = "";
	INTERNET_PER_CONN_OPTION_LIST    List;
	INTERNET_PER_CONN_OPTION         Option[2];
	unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

	Option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
	Option[1].dwOption = INTERNET_PER_CONN_FLAGS;

	List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
	List.pszConnection = NULL;
	List.dwOptionCount = 2;
	List.dwOptionError = 0;
	List.pOptions = Option;

	if(!InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize))
	{
		// failed to retrieve
	}
	else
	{
		if(Option[0].Value.pszValue != NULL)
		{
			s = Option[0].Value.pszValue;
			GlobalFree(Option[0].Value.pszValue);
		}

		if((Option[1].Value.dwValue & PROXY_TYPE_PROXY) != PROXY_TYPE_PROXY)
			return "";
	}
	return s;
}

// *************************************************************
//			AutodetectProxy()
// *************************************************************
bool BaseProvider::AutodetectProxy()
{
	bool ret = false;
	try
	{
		CString s = GetProxyServer();
		if (s.GetLength() > 0)
		{
			CString ip = "";
			CString port = "";
			AfxExtractSubString(ip, s, 0,':');
			AfxExtractSubString(port, s, 1,':');
			int portNumber = atoi(port);
			
			CAtlHttpClient* httpClient = new CAtlHttpClient();
			ret = httpClient->SetProxy(ip, portNumber); 
			if (ret)
			{
				m_proxyAddress = ip;
				m_proxyPort = portNumber;
			}
			httpClient->Close();
			delete httpClient;
		}
		else 
		{
			m_proxyAddress = "";
			m_proxyPort = 0;
		}
	}
	catch(...)
	{
		// do nothing
	}
	return ret;
}

// *************************************************************
//			AddDynamicOverlay()
// *************************************************************
void BaseProvider::AddDynamicOverlay(BaseProvider* p)
{
	if (p) {
		p->_dynamicOverlay = true;
	}
	_subProviders.push_back(p);
}

// *************************************************************
//			ClearSubProviders()
// *************************************************************
void BaseProvider::ClearSubProviders()
{
	for (size_t i = 0; i < _subProviders.size(); i++)
	{
		if (_subProviders[i]->_dynamicOverlay)
			delete _subProviders[i];
	}
	_subProviders.clear();
}


#pragma endregion
