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
#include "atlhttp.h"
#include "CallbackHelper.h"

class SecureHttpClient;

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