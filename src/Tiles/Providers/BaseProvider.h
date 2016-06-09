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
#include "geopoint.h"
#include "baseprojection.h"
#include "TileCore.h"
#include "afxmt.h"

// ***************************************************************
//		BaseProvider
// ***************************************************************
// Downloads map tiles via HTTP; this is abstract class to inherit from
class BaseProvider
{
public:
	BaseProvider()
		: Id(-1), _minZoom(1), _maxZoom(18),
		_manager(NULL), _projection(NULL), _isStopped(false),
		_dynamicOverlay(false), _initAttemptCount(0)
	{
		_licenseUrl = "https://mapwingis.codeplex.com/wikipage?title=tiles";
		LanguageStr = "en";
	}

	virtual ~BaseProvider(void)
	{
		if (_projection) {
			delete _projection;
		}
	};

private:
	bool _dynamicOverlay;
	bool _isStopped;
	void* _manager;

protected:
	static ::CCriticalSection _clientLock;
	static CString _proxyUsername;
	static CString _proxyPassword;
	static CString _proxyDomain;

protected:
	vector<BaseProvider*> _subProviders;	// for complex providers with more than one source bitmap per tile
	BaseProjection* _projection;
	CStringW _copyright;
	CString _refererUrl; 
	CString _licenseUrl;
	CString _urlFormat;
	int _initAttemptCount;
	CString _serverLetters;
	int _minZoom;
	int _maxZoom;

public:
	int Id;
	CString Name;
	CString Version;
	CString LanguageStr;
 
private:
	CMemoryBitmap* GetTileHttpData(CString urlStr, CString shortUrl, bool recursive = false);
	void PreventParallelExecution();
	CMemoryBitmap* ProcessHttpRequest(void* client, CString url, CString shortUrl, bool success);
	CMemoryBitmap* DownloadBitmap(CPoint &pos, int zoom);
	void ParseServerException(CString s);

protected:
	virtual CString MakeTileImageUrl(CPoint &pos, int zoom) = 0;
	int GetServerNum(CPoint &pos, int max) 	{ return (pos.x + 2 * pos.y) % max; }
	CMemoryBitmap* ReadBitmap(char* body, int bodyLen);

public:
	// properties
	virtual bool IsWms() { return false; }
	virtual CStringW get_Copyright() { return _copyright; }
	virtual bool Initialize() { return true; };

	int get_MinZoom() { return _minZoom; }
	int get_MaxZoom() { return _maxZoom; }
	void* get_Manager() { return _manager; }
	void put_Manager(void* value) { _manager = value; }
	vector<BaseProvider*>* get_SubProviders() { return &_subProviders; }
	CString get_LicenseUrl() { return _licenseUrl; }
	BaseProjection* get_Projection() { return _projection; }
	CString get_UrlFormat() { return _urlFormat; }

public:
	// methods
	bool SetAuthorization(CString username, CString password, CString domain);
	void ClearAuthorization();

	void AddDynamicOverlay(BaseProvider* p);
	void ClearSubProviders();
	TileCore* GetTileImage(CPoint &pos, int zoom);	
};
