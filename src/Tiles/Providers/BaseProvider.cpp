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
//#include "Wininet.h"
#include "BaseProvider.h"
#include "SecureHttpClient.h"

CString BaseProvider::_proxyUsername = "";
CString BaseProvider::_proxyPassword = "";
CString BaseProvider::_proxyDomain = "";
CCriticalSection BaseProvider::_clientLock;
const CString filePrefix = "file:///";

// ************************************************************
//		GetTileImage()
// ************************************************************
TileCore* BaseProvider::GetTileImage(CPoint& pos, const int zoom)
{
    auto* tile = new TileCore(this->Id, zoom, pos, this->_projection);

    for (size_t i = 0; i < _subProviders.size(); i++)
    {
        CMemoryBitmap* bmp = _subProviders[i]->DownloadBitmap(pos, zoom);
        if (bmp)
        {
            tile->AddOverlay(bmp);
        }
        else
        {
            tile->hasErrors(true);
        }
    }

    return tile;
}

// ************************************************************
//		GetTileHttpData()
// ************************************************************
CMemoryBitmap* BaseProvider::GetTileHttpData(CString url, CString shortUrl, bool recursive)
{
    // single-file access to the tile load
    // MWGIS-207; allow multiple thread access
    //CSingleLock lock(&_clientLock, TRUE);

    // stack-based instance
    SecureHttpClient client;

    client.SetProxyAndAuthentication(_proxyUsername, _proxyPassword, _proxyDomain);

    // jf: Likewise, don't think we need this work-around.
    // if we observe multiple retransmissions, we can reconsider.
    //PreventParallelExecution();

    const bool success = client.Navigate(url);
    if (!success)
    {
        client.LogHttpError();
        return nullptr;
    }

    CMemoryBitmap* bmp = ProcessHttpRequest(reinterpret_cast<void*>(&client), url, shortUrl, success);

    // this is a leftover from the Atl Http code; it remains to be seen if it is necessary
    if (!success && !recursive && client.GetStatus() == -1)
    {
        // it's a socket error, so let's try one more time
        Sleep(20);
        tilesLogger.Log("Reloading attempt: " + m_globalSettings.useShortUrlForTiles ? shortUrl : url);
        bmp = GetTileHttpData(url, shortUrl, true);
    }

    return bmp;
}

// ************************************************************
//		GetTileFileData()
// ************************************************************
CMemoryBitmap* BaseProvider::GetTileFileData(CString url)
{
	url.Delete(0, filePrefix.GetLength());
	url.Replace("|", ":");
	url.Replace("/", "\\");

	std::ifstream fl = std::ifstream(url.GetBuffer(), std::ofstream::binary);
	if (!fl)
		return nullptr;

	fl.seekg(0, std::ios::end);
	int sz = fl.tellg();
	if (sz == 0)
		return nullptr;

	std::vector<char> buf(sz);

	fl.seekg(0, std::ios::beg);
	fl.read(buf.data(), buf.size());
	if (!fl)
		return nullptr;

	return ReadBitmap(buf.data(), buf.size());
}

// ************************************************************
//		DownloadBitmap()
// ************************************************************
CMemoryBitmap* BaseProvider::DownloadBitmap(CPoint& pos, int zoom)
{
    const CString url = MakeTileImageUrl(pos, zoom);
    CString shortUrl;

    shortUrl.Format(R"(\zoom=%d\x=%d\y=%d)", zoom, pos.x, pos.y);

	if (url.Find(filePrefix) == 0)
		return GetTileFileData(url);
	else
		return GetTileHttpData(url, shortUrl);
}

// ************************************************************
//		ProcessResults()
// ************************************************************
CMemoryBitmap* BaseProvider::ProcessHttpRequest(void* secureHttpClient, const CString& url, const CString& shortUrl,
                                                bool success)
{
    auto* client = reinterpret_cast<SecureHttpClient*>(secureHttpClient);

    if (_isStopped) return nullptr;

    if (!success) client->LogHttpError();

    const TileHttpContentType contentType = client->get_ContentType(Id);

    char* body = nullptr;
    int length = 0;

    if (success && client->GetStatus() == 200)
    {
        client->ReadBody(&body, length);
    }

    client->LogRequest(length, shortUrl, url);

    CMemoryBitmap* bmp = nullptr;
    switch (contentType)
    {
    case httpImage:
        bmp = ReadBitmap(body, length);
        break;
    case httpXml:
        if (IsWms())
        {
            const CString s(body);
            ParseServerException(s);
        }
        break;
    default:
        break;
    }

    delete[] body;

    return bmp;
}

// ************************************************************
//		ParseServerException()
// ************************************************************
void BaseProvider::ParseServerException(const CString& s) const
{
    CPLXMLNode* node = CPLParseXMLString(s);
    if (node)
    {
        while (node)
        {
            CPLXMLNode* nodeException = CPLGetXMLNode(node, "ServiceException");
            if (nodeException)
            {
                const CString msg = CPLGetXMLValue(nodeException, "", "");
                CallbackHelper::ErrorMsg(Debug::Format("WMS Server exception (%s): %s", Name, msg));
            }
            node = node->psNext;
        }

        CPLDestroyXMLNode(node);
    }
}

// ************************************************************
//		PreventParallelExecution()
// ************************************************************
void BaseProvider::PreventParallelExecution()
{
    CSingleLock lock(&_clientLock, TRUE);

    // there is info that ARP might have problems with simultaneous connections, 
    // discarding the next connection if the previous one is under resolution; 
    // since we experience periodic rertransmissions let's introduce
    // a small delay between connections http://stackoverflow.com/questions/1875151/delay-in-multiple-tcp-connections-from-java-to-the-same-machine
    Sleep(5);
}

// *************************************************************
//			ReadBitmap()
// *************************************************************
CMemoryBitmap* BaseProvider::ReadBitmap(char* body, int bodyLen) const
{
    auto* bmp = new CMemoryBitmap();
    bmp->LoadFromRawData(body, bodyLen);
    bmp->Provider = this->Id;
    return bmp;
}

// *************************************************************
//			SetProxyAuthorization()
// *************************************************************
bool BaseProvider::SetAuthorization(const CString& username, const CString& password, const CString& domain)
{
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
void BaseProvider::ClearAuthorization()
{
    _proxyUsername = "";
    _proxyDomain = "";
    _proxyPassword = "";
}

// *************************************************************
//			AddDynamicOverlay()
// *************************************************************
void BaseProvider::AddDynamicOverlay(BaseProvider* p)
{
    if (p)
    {
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
