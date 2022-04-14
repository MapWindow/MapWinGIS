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
// Paul Meems August 2018: Modernized the code as suggested by CLang and ReSharper

#include "StdAfx.h"
#include "SecureHttpClient.h"
#include "HttpProxyHelper.h"
#include "TileCore.h"

SecureHttpClient::SecureHttpClient(): file(nullptr)
{
    // create CURL handle
    curl = curl_easy_init();

    // set up write buffer
    chunk.memory = (char *)malloc(1); /* will be grown as needed */
    chunk.size = 0; /* no data yet */

	/* PM: Add user agent */
	curl_easy_setopt(curl, CURLOPT_USERAGENT, m_globalSettings.httpUserAgent);

    /* send all data to this function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
}

size_t SecureHttpClient::WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    const size_t realsize = size * nmemb;
    auto* mem = (struct MemoryStruct *)userp;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == nullptr)
    {
        /* out of memory! */
        CallbackHelper::ErrorMsg("not enough memory (realloc returned NULL)");
        return 0;
    }

    memcpy(&mem->memory[mem->size], contents, realsize);
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
bool SecureHttpClient::SetProxyAndAuthentication(const CString& userName, const CString& password,
                                                 const CString& domain) const
{
    // validate handle
    if (!curl) return false;

    CURLcode curlCode;
    //curlCode = curl_easy_setopt(curl, CURLOPT_SUPPRESS_CONNECT_HEADERS, 1L);
    //if (curlCode != CURLcode::CURLE_OK)
    //{
	   // /* what to do */
    //}

    if (HttpProxyHelper::m_proxyAddress.GetLength() > 0)
    {
	    if (!SetProxy((LPCTSTR)HttpProxyHelper::m_proxyAddress, (long)HttpProxyHelper::m_proxyPort))
		    return false;
    }

    //curlCode = curl_easy_setopt(curl, CURLOPT_PROXY, (LPCTSTR)domain);
    curlCode = curl_easy_setopt(curl, CURLOPT_USERNAME, (LPCTSTR)userName);
    curlCode = curl_easy_setopt(curl, CURLOPT_PASSWORD, (LPCTSTR)password);

    // TODO: this needs to be expanded to have more options, e.g. hidden auth is now impossible
    if (m_globalSettings.proxyAuthentication == tkProxyAuthentication::asNtlm)
    {
	    return (curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM) == CURLcode::CURLE_OK);
    }
    // Let CURL decide:
    return (curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY) == CURLcode::CURLE_OK);
}

// *************************************************************
//			GetBodyLength()
// *************************************************************
int SecureHttpClient::GetBodyLength() const
{
    return chunk.size;
}

// *************************************************************
//			GetBody()
// *************************************************************
BYTE* SecureHttpClient::GetBody() const
{
    return (BYTE*)chunk.memory;
}

// *************************************************************
//			ReadBody()
// *************************************************************
bool SecureHttpClient::ReadBody(char** body, int& length) const
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
TileHttpContentType SecureHttpClient::get_ContentType(int providerId) const
{
    char* szContentType;

    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &szContentType);
    CString contentType = szContentType;

    if (contentType.Left(5).CompareNoCase(_T("image")) == 0)
    {
        return TileHttpContentType::httpImage;
    }

    if (contentType.MakeLower().Find("xml") != -1)
    {
        return TileHttpContentType::httpXml;
    }

    if (providerId == (int)tkTileProvider::Rosreestr)
    {
        // ad-hoc fix
        return TileHttpContentType::httpImage;
    }

    return TileHttpContentType::httpUndefined;
}

// get request status
long SecureHttpClient::GetStatus() const
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
    // last error message should be in buffer
    CallbackHelper::ErrorMsg(Debug::Format("Failed to retrieve tile. cURL error: %s", errorString));
}

// *************************************************************
//			LogRequest()
// *************************************************************
void SecureHttpClient::LogRequest(int bodyLen, CString shortUrl, CString url) const
{
    if (tilesLogger.IsOpened() || Debug::LogTiles())
    {
        const bool hasError = GetStatus() != 200;
        // Not used: bool useShortUrl = false;

        if (!tilesLogger.errorsOnly || hasError)
        {
            CString err;
            err.Format("ERROR: %d; ", GetLastError());

            const CString tempUrl = m_globalSettings.useShortUrlForTiles ? shortUrl : url;

            CString s;
            s.Format("%sstatus %d size %6d b %s", !hasError ? "" : err, GetStatus(), bodyLen, tempUrl);

            tilesLogger.Log(s);
        }
    }
}

// ************************************************************
//		SetProxy()
// ************************************************************
bool SecureHttpClient::SetProxy(LPCTSTR address, long port) const
{
    return curl_easy_setopt(curl, CURLOPT_PROXY, address) == CURLcode::CURLE_OK &&
        curl_easy_setopt(curl, CURLOPT_PROXYPORT, port) == CURLcode::CURLE_OK;
}

// ************************************************************
//		Navigate()
// ************************************************************
bool SecureHttpClient::Navigate(LPCTSTR url) const
{
    if (curl_easy_setopt(curl, CURLOPT_URL, url) == CURLcode::CURLE_OK)
    {
        // the following call is currently required to avoid error CURLE_SSL_CACERT
        // not sure whether or not this is appropriate solution
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);

        // perform the operation
        const CURLcode result = curl_easy_perform(curl);
        return result == CURLcode::CURLE_OK;
    }
    // something went wrong
    return false;
}

// ************************************************************
//		CheckConnection()
// ************************************************************
bool SecureHttpClient::CheckConnection(const CString url)
{
    auto* httpClient = new SecureHttpClient();

    if (HttpProxyHelper::m_proxyAddress.GetLength() > 0)
    {
        httpClient->SetProxy(HttpProxyHelper::m_proxyAddress, (long)HttpProxyHelper::m_proxyPort);
    }

    const bool result = httpClient->Navigate((LPCTSTR)url);

    //httpClient->Close();
    delete httpClient;

    return result;
}
