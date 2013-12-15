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

// init static members
CString BaseProvider::m_proxyAddress = "";
short BaseProvider::m_proxyPort = 0;

#pragma region Load tile

// ************************************************************
//		GetTileImage()
// ************************************************************
TileCore* BaseProvider::GetTileImage(CPoint &pos, int zoom)
{
	TileCore* tile = new TileCore(this->Id, zoom, pos, this->Projection);
	for (size_t i = 0; i < subProviders.size(); i++)
	{
		CMemoryBitmap* bmp = subProviders[i]->DownloadBitmap(pos, zoom);
		if (bmp)
		{
			tile->AddBitmap(bmp);	
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
	CMemoryBitmap* bmp = this->GetTileImageUsingHttp(url);
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
//		GetTileImageUsingHttp()
// ************************************************************
CMemoryBitmap* BaseProvider::GetTileImageUsingHttp(CString m_urlStr, bool recursive)
{
	CAtlHttpClient* httpClient = new CAtlHttpClient();
	CAtlNavigateData navData;

	if (m_proxyAddress.GetLength() > 0)
	{
		httpClient->SetProxy(m_proxyAddress, m_proxyPort);
	}

	char* body = 0;
	int bodyLen = 0;
	bool imageData = false;
	
	if (httpClient->Navigate( m_urlStr, &navData ))
	{
		if (httpClient->GetStatus() == 200) // 200 = successful HTTP transaction
		{
			bodyLen = httpClient->GetBodyLength();
			if (bodyLen > 0)
			{
				body = new char[bodyLen+1];
				memcpy( body, httpClient->GetBody(), bodyLen );
				body[bodyLen] = 0;
				
				CString contentType;
				httpClient->GetHeaderValue( _T("Content-Type"), contentType );
				imageData = contentType.Left(5).CompareNoCase( _T("image") ) == 0;
				if (!imageData && this->Id == tkTileProvider::Rosreestr) {    // temporary fix
					imageData = true;
				}
			}
		}
	}
	else
	{
		this->httpStatus = httpClient->GetStatus();
		Debug::WriteLine("Request failed. Status code: %d; %s", httpStatus, m_urlStr);		
		if (httpStatus == -1 && !recursive)
		{
			// let's try one more time
			this->GetTileImageUsingHttp(m_urlStr, true);
		}
	}
	
	httpClient->Close();
	delete httpClient;

	CMemoryBitmap* bmp = NULL;
	if (imageData)
	{
		bmp = new CMemoryBitmap();
		bmp->LoadFromRawData(body, bodyLen);
		bmp->Provider = this->Id;
	}
	delete [] body;
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

		DWORD flags = Option[1].Value.dwValue;
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
#pragma endregion