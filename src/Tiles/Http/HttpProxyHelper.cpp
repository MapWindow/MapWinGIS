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
#include "stdafx.h"
#include "HttpProxyHelper.h"
#include <Wininet.h>
#include "atlhttp.h"

CString HttpProxyHelper::m_proxyAddress = "";
short HttpProxyHelper::m_proxyPort = -1;

// *************************************************************
//			GetProxyServer()
// *************************************************************
CString HttpProxyHelper::GetProxyServer()
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

	if (!InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize))
	{
		// failed to retrieve
	}
	else
	{
		if (Option[0].Value.pszValue != NULL)
		{
			s = Option[0].Value.pszValue;
			GlobalFree(Option[0].Value.pszValue);
		}

		if ((Option[1].Value.dwValue & PROXY_TYPE_PROXY) != PROXY_TYPE_PROXY)
			return "";
	}
	return s;
}

// *************************************************************
//			AutodetectProxy()
// *************************************************************
bool HttpProxyHelper::AutodetectProxy()
{
	bool ret = false;
	try
	{
		CString s = GetProxyServer();
		if (s.GetLength() > 0)
		{
			CString ip = "";
			CString port = "";
			AfxExtractSubString(ip, s, 0, ':');
			AfxExtractSubString(port, s, 1, ':');
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
	catch (...)
	{
		// do nothing
	}
	return ret;
}

// *************************************************************
//			SetProxy()
// *************************************************************
bool HttpProxyHelper::SetProxy(CString address, int port)
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