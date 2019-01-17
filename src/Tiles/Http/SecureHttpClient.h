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
// jf, 8/2018: Replace ATL Http library usage with libCurl so as to support SSL/HTTPS

#pragma once

#include "curl.h"

struct MemoryStruct
{
	char *memory;
	size_t size;
};

class SecureHttpClient
{
public:
	SecureHttpClient();
	~SecureHttpClient();

private:
	CURL *curl;
	FILE *file;
	struct MemoryStruct chunk{};
	char errorString[CURL_ERROR_SIZE]{};
	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

public:
	long GetStatus() const;
	void LogRequest(int bodyLen, CString shortUrl, CString url) const;
	void LogHttpError();
	bool SetProxy(LPCTSTR address, long port) const;
	bool Navigate(LPCTSTR url) const;
	TileHttpContentType get_ContentType(int providerId) const;
	int GetBodyLength() const;
	BYTE *GetBody() const;
	bool ReadBody(char** body, int& length) const;
public:
	// methods
	bool SetProxyAndAuthentication(const CString& userName, const CString& password, const CString& domain) const;

public:
	// static methods
	static bool CheckConnection(CString url);
	
};