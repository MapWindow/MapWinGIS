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
#include <atlhttp.h>		// ATL HTTP Client (CAtlHttpClient)
#include <stdio.h>
#include "geopoint.h"
#include "baseprojection.h"
#include "TileCore.h"

// Downloads map tiles via HTTP; this is abstract class to inherit from
class BaseProvider
{
private:
	CAtlHttpClient* httpClient;
	bool m_initialized;

protected:
	static CString m_proxyAddress;
	static short m_proxyPort;

public:
	bool BaseProvider::CheckConnection(CString url);
	std::vector<BaseProvider*> subProviders;	// for complex providers with more than one source bitmap per tile
	BaseProjection* Projection;
	void *mapView;
	int Id;
	CString LanguageStr;
	CString RefererUrl;
	CString Copyright;
	CString ServerLetters;
	CString Name;
	CString UrlFormat;
	CString Version;
	CSize minOfTiles;
	CSize maxOfTiles;
	int minZoom;
    int maxZoom;
	int zoom;
	bool Selected;	// is used by clients only
	int httpStatus;

private:
	CMemoryBitmap* GetTileImageUsingHttp(CString m_urlStr, bool recursive = false);

protected:
	// abstract members
	virtual CString MakeTileImageUrl(CPoint &pos, int zoom) = 0;

	int GetServerNum(CPoint &pos, int max)
	{
		return (pos.x + 2 * pos.y) % max;
	}
public:
	BaseProvider()
		: minZoom(1), maxZoom(18), zoom(0), httpStatus(200)
	{
		mapView = NULL;
		LanguageStr = "en";
		Projection = NULL;
	}

	virtual ~BaseProvider(void)
	{
		if (Projection != NULL)
			delete Projection;
	};

	// proxy suport
	short get_ProxyPort() {return m_proxyPort;}
	CString get_ProxyAddress() {return m_proxyAddress;}
	bool SetProxy(CString address, int port);
	bool AutodetectProxy();

	CMemoryBitmap* DownloadBitmap(CPoint &pos, int zoom);
	TileCore* GetTileImage(CPoint &pos, int zoom);	
};