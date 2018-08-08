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
	struct MemoryStruct chunk;
	char errorString[CURL_ERROR_SIZE];
	static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
	//BasicAuth basicAuth;
	//CNTLMAuthObject ntlmAuth;

public:
	long GetStatus();
	void LogRequest(int bodyLen, CString shortUrl, CString url);
	void LogHttpError();
	bool SetProxy(LPCTSTR address, long port);
	bool Navigate(LPCTSTR url);
	TileHttpContentType get_ContentType(int providerId);
	int GetBodyLength();
	BYTE *GetBody();
	bool ReadBody(char** body, int& length);
public:
	// methods
	bool SetProxyAndAuthentication(CString userName, CString password, CString domain);

public:
	// static methods
	static bool CheckConnection(CString url);
	
};