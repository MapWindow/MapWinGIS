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

SecureHttpClient::SecureHttpClient()
{
	// create CURL handle
	curl = curl_easy_init();

	// set up write buffer
	chunk.memory = (char *)malloc(1);	/* will be grown as needed */
	chunk.size = 0;						/* no data yet */

	/* send all data to this function */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
}

size_t SecureHttpClient::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		CallbackHelper::ErrorMsg("not enough memory (realloc returned NULL)");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

SecureHttpClient::~SecureHttpClient()
{
	free(chunk.memory);
	// CURL cleanup
	curl_easy_cleanup(curl);
}

// ************************************************************
//		SetAuthentication()
// ************************************************************
// passing authorization object in such way it ugly, but allocating them dynamically might 
// give even more troubles
bool SecureHttpClient::SetProxyAndAuthentication(CString userName, CString password, CString domain)
{
	// otherwise it will query zone settings in IE for that URL
	// if the setting is anything other than URLPOLICY_CREDENTIALS_SILENT_LOGON_OK it will fail
	//SetSilentLogonOk(true);

	// validate handle
	if (!curl) return false;

	CURLcode curlCode;
	curlCode = curl_easy_setopt(curl, CURLOPT_SUPPRESS_CONNECT_HEADERS, 1L);
	if (curlCode != CURLcode::CURLE_OK) /* what to do */;

	if (HttpProxyHelper::m_proxyAddress.GetLength() > 0)
	{
		//if (!SetProxy(HttpProxyHelper::m_proxyAddress, HttpProxyHelper::m_proxyPort))
		//	return false;

		if (!SetProxy((LPCTSTR)HttpProxyHelper::m_proxyAddress, (long)HttpProxyHelper::m_proxyPort))
			return false;
	}

	//basicAuth.SetCredentials(userName, password, domain);

	//curlCode = curl_easy_setopt(curl, CURLOPT_PROXY, (LPCTSTR)domain);
	curlCode = curl_easy_setopt(curl, CURLOPT_USERNAME, (LPCTSTR)userName);
	curlCode = curl_easy_setopt(curl, CURLOPT_PASSWORD, (LPCTSTR)password);

	if (m_globalSettings.proxyAuthentication == tkProxyAuthentication::asNtlm)
	{
		// We're using the standard CNLMAuthObject class here because it automatically uses the current user's
		// credentials if no IAuthInfo implementation is given.
		//return AddAuthObj(_T("NTLM"), &ntlmAuth);

		return (curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM) == CURLcode::CURLE_OK);
	}

	//return AddAuthObj("BASIC", &basicAuth, &basicAuth);

	return (curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC) == CURLcode::CURLE_OK);
}

// *************************************************************
//			GetBodyLength()
// *************************************************************
int SecureHttpClient::GetBodyLength()
{
	//curl_off_t byteCount = 0;
	//curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &byteCount);
	//
	return chunk.size; // (int)byteCount;
}

// *************************************************************
//			GetBody()
// *************************************************************
BYTE *SecureHttpClient::GetBody()
{
	//void *buffer;
	//fread(buffer, 1, GetBodyLength(), file);
	return (BYTE*)chunk.memory; // buffer;
}

// *************************************************************
//			ReadBody()
// *************************************************************
bool SecureHttpClient::ReadBody(char** body, int& length)
{
	length = GetBodyLength();

	//if (length > 0)
	//{
	//	*body = new char[length + 1];
	//	memcpy(*body, GetBody(), length);
	//	(*body)[length] = 0;
	//	return true;
	//}

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
	char* szContentType;
	CString contentType;
	//GetHeaderValue(_T("Content-Type"), contentType);

	curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &szContentType);
	contentType = szContentType;

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

// get request status
long SecureHttpClient::GetStatus()
{
	long responseCode = -1;
	if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode) != CURLcode::CURLE_OK)
		responseCode = -1;
	//
	return responseCode;
}

// ************************************************************
//		LogHttpError()
// ************************************************************
void SecureHttpClient::LogHttpError()
{
	//int status = GetStatus();
	//DWORD socketError = GetLastError();

	//if (status == -1 && socketError == 0)
	//{
	//	CallbackHelper::ErrorMsg("Failed to retrieve tile: undefined error.");
	//	return;
	//}

	//if (status == -1)
	//{
	//	CString msg = Utility::GetSocketErrorMessage(socketError);
	//	CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. WSA error: %d. %s", socketError, msg));
	//}
	//else
	//{
	//	CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. HTTP status: %d", status));
	//}

	// last error message should be in buffer
	CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. cURL error: %s", errorString));
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
//		SetProxy()
// ************************************************************
bool SecureHttpClient::SetProxy(LPCTSTR address, long port)
{
	return (curl_easy_setopt(curl, CURLOPT_PROXY, address) == CURLcode::CURLE_OK &&
			curl_easy_setopt(curl, CURLOPT_PROXYPORT, port) == CURLcode::CURLE_OK);
}

// ************************************************************
//		Navigate()
// ************************************************************
bool SecureHttpClient::Navigate(LPCTSTR url)
{
	if (curl_easy_setopt(curl, CURLOPT_URL, url) == CURLcode::CURLE_OK)
	{
		// the following call is currently required to avoid error CURLE_SSL_CACERT
		// not sure whether or not this is appropriate solution
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
		// perform the operation
		CURLcode result = curl_easy_perform(curl);
		return (result == CURLcode::CURLE_OK);
	}
	// something went wrong
	return false;
}

// ************************************************************
//		CheckConnection()
// ************************************************************
bool SecureHttpClient::CheckConnection(CString url)
{
	SecureHttpClient* httpClient = new SecureHttpClient();

	if (HttpProxyHelper::m_proxyAddress.GetLength() > 0)	
	{
		httpClient->SetProxy(HttpProxyHelper::m_proxyAddress, (long)HttpProxyHelper::m_proxyPort);
	}

	bool result = httpClient->Navigate((LPCTSTR)url);

	//httpClient->Close();
	delete httpClient;

	return result;
}

