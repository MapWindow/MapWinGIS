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
#include "SecureHttpClient.h"
#include "HttpProxyHelper.h"
#include "TileCore.h"



// ************************************************************
//		SetAuthentication()
// ************************************************************
// passing authorization object in such way it ugly, but allocating them dynamically might 
// give even more troubles
bool SecureHttpClient::SetProxyAndAuthentication(CString userName, CString password, CString domain)
{
	// otherwise it will query zone settings in IE for that URL
	// if the setting is anything other than URLPOLICY_CREDENTIALS_SILENT_LOGON_OK it will fail
	SetSilentLogonOk(true);

	if (HttpProxyHelper::m_proxyAddress.GetLength() > 0)
	{
		if (!SetProxy(HttpProxyHelper::m_proxyAddress, HttpProxyHelper::m_proxyPort))
			return false;
	}

	basicAuth.SetCredentials(userName, password, domain);

	if (m_globalSettings.proxyAuthentication == tkProxyAuthentication::asNtlm)
	{
		// We're using the standard CNLMAuthObject class here because it automatically uses the current user's
		// credentials if no IAuthInfo implementation is given.
		return AddAuthObj(_T("NTLM"), &ntlmAuth);
	}

	return AddAuthObj("BASIC", &basicAuth, &basicAuth);
}

// *************************************************************
//			ReadBody()
// *************************************************************
bool SecureHttpClient::ReadBody(char** body, int& length)
{
	length = GetBodyLength();

	if (length > 0)
	{
		*body = new char[length + 1];
		memcpy(*body, GetBody(), length);
		(*body)[length] = 0;
		return true;
	}

	return false;
}

// *************************************************************
//			get_ContentType()
// *************************************************************
TileHttpContentType SecureHttpClient::get_ContentType(int providerId)
{
	CString contentType;
	GetHeaderValue(_T("Content-Type"), contentType);

	if (contentType.Left(5).CompareNoCase(_T("image")) == 0)
	{
		return httpImage;
	}

	if (contentType.MakeLower().Find("xml") != -1)
	{
		return httpXml;
	}

	if (providerId == (int)tkTileProvider::Rosreestr) {    // ad-hoc fix
		return httpImage;
	}

	return httpUndefined;
}

// ************************************************************
//		LogHttpError()
// ************************************************************
void SecureHttpClient::LogHttpError()
{
	int status = GetStatus();
	DWORD socketError = GetLastError();

	if (status == -1 && socketError == 0)
	{
		CallbackHelper::ErrorMsg("Failed to retrieve tile: undefined error.");
		return;
	}

	if (status == -1)
	{
		CString msg = Utility::GetSocketErrorMessage(socketError);
		CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. WSA error: %d. %s", socketError, msg));
	}
	else
	{
		CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. HTTP status: %d", status));
	}
}

// *************************************************************
//			LogRequest()
// *************************************************************
void SecureHttpClient::LogRequest(int bodyLen, CString shortUrl, CString url)
{
	if (tilesLogger.IsOpened() || Debug::LogTiles())
	{
		bool hasError = GetStatus() != 200;
		bool useShortUrl = false;

		if (!tilesLogger.errorsOnly || hasError)
		{
			CString err;
			err.Format("ERROR: %d; ", GetLastError());

			CString tempUrl = m_globalSettings.useShortUrlForTiles ? shortUrl : url;

			CString s;
			s.Format("%sstatus %d size %6d b %s", (!hasError ? "" : err), GetStatus(), bodyLen, tempUrl);

			tilesLogger.Log(s);
		}
	}
}

// ************************************************************
//		CheckConnection()
// ************************************************************
bool SecureHttpClient::CheckConnection(CString url)
{
	CAtlHttpClient* httpClient = new CAtlHttpClient();
	CAtlNavigateData navData;

	if (HttpProxyHelper::m_proxyAddress.GetLength() > 0)	
	{
		httpClient->SetProxy(HttpProxyHelper::m_proxyAddress, HttpProxyHelper::m_proxyPort);
	}

	bool result = httpClient->Navigate(url);
	httpClient->Close();
	delete httpClient;

	return result;
}